//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Migrator.hpp"
#include "Positions.hpp"
#include "Statistics.hpp"
#include "TraderApp.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "database/Database.hpp"

NS_USE

void Migrator::migrate(Positions* positions, Statistics* statistics, const Symbol& symbol, bool test) {
    if (statistics->version().empty()) {
        if (Exchanger().loadOrders(symbol)) {
            const std::vector<const OrderWrapper *> &orders = Exchanger().book(symbol)->get();
            std::vector<std::string> keys = DB().keys("order:*");
            for (const OrderWrapper *order: orders) {
                std::string id = "order:" + order->id();
                if (std::find(keys.begin(), keys.end(), id) == keys.end())
                    continue;

                if (positions->copy(order) && not test)
                    DB().del(id);
            }
        }

        double profit = DB().get("stats:profit:" + symbol.id()).asDouble();
        if (profit > 0.0) {
            if (not test)
                DB().del("stats:profit:" + symbol.id());

            statistics->addProfit(profit);
        }
    }

    statistics->setVersion(TraderApp::sVersion.toString());
}