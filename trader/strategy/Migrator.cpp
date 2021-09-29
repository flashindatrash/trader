//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Migrator.hpp"
#include "Position.hpp"
#include "Statistics.hpp"
#include "TraderApp.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

NS_USE

bool Migrator::migrate(Position* position, Statistics* statistics, const Symbol& symbol) {
    const std::string& current_version = TraderApp::sVersion.toString();
    const std::string& migrate_version = statistics->version();

    if (migrate_version == "1.2.2") {
        // todo: проверить миграцию очищения
        if (not statistics->remove())
            return false;
    }

    if (migrate_version != current_version) {
        statistics->setVersion(current_version);
        return statistics->save();
    }

    return true;
}