//
// Created by Вадим Проскурин on 31.07.2022.
//

#include "Wallet.hpp"

bool WalletRequest::mask(Policy bit) const {
    return 0 != (policy & bit);
}