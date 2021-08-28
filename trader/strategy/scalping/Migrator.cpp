//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Migrator.h"
#include "Positions.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "database/Database.hpp"

NS_USE

void Migrator::migrate(Positions* positions, const Symbol& symbol) {
    if (Exchanger().loadOrders(symbol)) {
        const std::vector<const OrderWrapper*>& orders = Exchanger().book(symbol)->get();
        std::vector<std::string> keys = DB().keys("order:*");
        for (const OrderWrapper* order : orders) {
            std::string id = "order:" + order->id();
            if (std::find(keys.begin(), keys.end(), id) == keys.end())
                continue;

            if (positions->copy(order) && positions->proceed_sync())
                DB().del(id);
        }
    }
}