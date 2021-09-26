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

bool Migrator::migrate(Positions* positions, Statistics* statistics, const Symbol& symbol, bool test) {
    const std::string& current_version = TraderApp::sVersion.toString();
    const std::string& migrate_version = statistics->version();

    if (migrate_version == "1.2.2") {
        // todo: проверить миграцию очищения
        if (not statistics->remove() || not positions->clear())
            return false;
    }

    if (migrate_version != current_version) {
        statistics->setVersion(current_version);
        return statistics->save();
    }

    return true;
}