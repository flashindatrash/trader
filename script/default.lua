--[[
    # Algorithm methods:
        - (side, pip) open(): open new position
        - bool close(position): close active position
        - bool average(position): average active position

    # API methods:
        - void print(string.format(text)): print log
        - number balance(string asset): get account balance
        - trend, signal = dema(int fast, int slow): get trend side and signal side of double ema

    # Structures
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
]]

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

    if position.change < 0.003 then
        return false;
    end

    return true;
end

function average(position)
    if position.side == 1 then
        balance_quantity = balance(position.quoteAsset);
        position_quantity = position.quoteQuantity;
    elseif position.side == 2 then
        balance_quantity = balance(position.baseAsset);
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