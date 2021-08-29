//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Status.hpp"
#include "Logger.hpp"

NS_USE

Status* Status::create(const Symbol& symbol) {
    auto* status = new Status();
    status->setSymbol(symbol);
    return status;
}

void Status::setSymbol(const Symbol& symbol) {
    Logger::title("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}