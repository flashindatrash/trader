--[[
    # Algorithm methods:
        - bool __main__(settings): initialize
        - (side, pip) open(): open new position
        - bool close(position): close active position
        - bool average(position): average active position

    # API methods:
        - void print(string.format(text)): print log
        - number balance(string asset): get account balance
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

function __main__(settings)
    days = 1;

    if settings.mode == "backtest" then
        days = 31;
        topup(settings.baseAsset, 100);
        topup(settings.quoteAsset, 100);
    end

    return chart(settings.baseAsset, settings.quoteAsset, 0, days);
end

function open()
    trend, signal = dema(20, 30);

    if signal == 0 or signal == 2 then
        return 0, 0;
    end

    return signal, 2.0;
end

function close(position)
    trend, signal = dema(20, 30);

    if trend == 0 or trend == position.side then
        return false;
    end

    if position.change < 0.01 then
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

    percent = -0.25;
    for i = 1, averages do
        percent = percent / 2.0;
    end

    return position.change <= percent;
end