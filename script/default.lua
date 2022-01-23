--[[
    # Algorithm methods:
        - bool __main__(settings): initialize
        - (side, pip) open(): open new position
        - bool close(position): close active position
        - bool average(position): average active position

    # API methods:
        - void print(string.format(text)): print log
        - number balance(string asset): get account balance
        - number price(): get current price
        - void topup(string asset, number quantity): top up test balance
        - trend, signal = dema(int fast, int slow): get trend side and signal side of double ema
        - bool chart(string baseAsset, string quoteAsset, Enum interval, int days = 1): load & listen chart

    # Structures
        - settings (Object)
            .baseAsset: first asset
            .quoteAsset: second asset
            .mode: mode
        - position (Object)
            .baseAsset: first asset
            .quoteAsset: second asset
            .side: buy or sell
            .baseQuantity: total base quantity
            .quoteQuantity: total quote quantity
            .price: average price position
            .distance: different between current price
            .change: change to current price in percent
            .profit: profit includes fee
        - side (Enum)
            0: Invalid
            1: Buy
            2: Sell
        - interval (Enum)
            0: 5 min
            1: 15 min
            2: 1 hour
            3: 1 day
]]

-- variables
function indicator() return dema(20, 30); end
percent_lot = 0.25;
percent_profit = 0.01;
percent_average = 0.25;

function __main__(settings)
    local days = 1;

    if settings.mode == "backtest" then
        days = 7;
        -- topup(settings.baseAsset, 100);
        -- topup(settings.quoteAsset, 100);
    end

    baseAsset = settings.baseAsset;
    quoteAsset = settings.quoteAsset;

    return chart(settings.baseAsset, settings.quoteAsset, 0, days);
end

function open()
    local trend, signal = indicator();

    if signal == 0 or signal == 2 then
        return 0, 0;
    end

    local balance = balance(quoteAsset) + balance("LD" .. quoteAsset);
    local lot = balance * percent_lot / price();
    return signal, lot;
end

function close(position)
    local trend, signal = indicator();

    if trend == 0 or trend == position.side then
        return false;
    end

    if position.change < percent_profit then
        return false;
    end

    return true;
end

function average(position)
    if position.side == 1 then
        balance_quantity = balance(position.quoteAsset) + balance("LD" .. position.quoteAsset);
        position_quantity = position.quoteQuantity;
    elseif position.side == 2 then
        balance_quantity = balance(position.baseAsset) + balance("LD" .. position.baseAsset);
        position_quantity = position.baseQuantity;
    else
        return false;
    end

    local averages = 0;
    while balance_quantity >= position_quantity do
      averages = averages + 1;
      balance_quantity = balance_quantity - position_quantity;
      position_quantity = position_quantity * 2.0;
    end

    if averages == 0 then
        return false;
    end

    local percent = -percent_average;
    for i = 1, averages do
        percent = percent / 2.0;
    end

    return position.change <= percent;
end