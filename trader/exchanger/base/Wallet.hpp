//
// Created by Вадим Проскурин on 31.07.2022.
//


#pragma once

struct WalletRequest {
    enum Policy : unsigned int {
        None = 0,
        TestMode = 1,
        CheckBalance = 2,
        RedeemSavings = 4
    };

    unsigned int policy = CheckBalance | RedeemSavings;

    bool mask(Policy policy) const;
};


