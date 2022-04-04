//
// Created by Вадим Проскурин on 05.04.2022.
//

#include "Processor.hpp"
#include "processor/trader/Trader.hpp"
#include "processor/listing/Listing.hpp"
#include "processor/staking/Staking.hpp"

Processor* Processor::create(const std::string& type) {
    if (type == "pair")
        return new trader::Trader();
    else if (type == "listing")
        return new listing::Listing();
    else if (type == "staking")
        return new staking::Staking();

    return nullptr;
}