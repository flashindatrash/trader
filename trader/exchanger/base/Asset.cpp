#include "Asset.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "util/StringUtil.hpp"

const Asset Asset::Empty = Asset();
const Asset Asset::USDT = Asset("USDT");

Asset::Asset(const std::string& asset) {
    setId(util::uppercase(asset.c_str()));
}

const char* Asset::c_str() const {
    return _identifier.c_str();
}

Asset::operator std::string() const {
    return _identifier;
}

const Quantity& Asset::balance() const {
    return Exchanger().balance(*this)->get();
}

Quantity Asset::balance(const Asset& asset) const {
    if (id() == asset.id())
        return balance();

    if (const PriceWrapper* price = Exchanger().price(id() + asset.id()))
        return price->get() * balance();

    return 0.0;
}

const std::string Asset::operator+(const Asset& quote) const {
    return id() + quote.id();
}
