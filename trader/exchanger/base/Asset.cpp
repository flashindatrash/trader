#include "Asset.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "util/StringUtil.hpp"

const Asset Asset::Empty = Asset("");
const Asset Asset::USDT = Asset("USDT");
const Asset Asset::BUSD = Asset("BUSD");

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
    return convert(balance(), asset);
}

Quantity Asset::convert(Quantity quantity, const Asset& asset/* = Asset::USDT*/) const {
    if (id() == asset.id() || (isUSD() && asset.isUSD()))
        return quantity;

    if (const PriceWrapper* price = Exchanger().price(id() + asset.id()))
        return price->get() * quantity;

    return 0.0;
}

bool Asset::isUSD() const {
    return id() == USDT.id() || id() == BUSD.id();
}

std::string Asset::operator+(const Asset& quote) const {
    return id() + quote.id();
}
