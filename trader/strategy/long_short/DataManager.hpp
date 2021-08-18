#pragma once

#include <global.hpp>

class DataManager
{
public: // vars
    static const std::string& sDbKeyOrder;
    static const std::string& sDbKeyProfit;

public: // methods
    static std::vector<std::string> getPositionIds();
    static void openPosition(const std::string& order_id);
    static void closePosition(const std::string& order_id);
    static time_t getLastOrderTime(const std::string& symbol);
    static double addProfit(const std::string& asset, double profit);
};

