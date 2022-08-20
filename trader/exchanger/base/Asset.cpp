#include "Asset.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "util/StringUtil.hpp"

const Asset Asset::Empty = Asset("");
const Asset Asset::USDT = Asset("USDT");
const Asset Asset::BUSD = Asset("BUSD");
const Asset Asset::LD = Asset("LD");

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
    static Quantity zero;

    const BalanceWrapper* wrapper = Exchanger().balance(*this);
    if (wrapper == nullptr)
        return zero;

    return wrapper->get();
}

Quantity Asset::balance(const Asset& asset) const {
    return convert(balance(), asset);
}

Quantity Asset::convert(Quantity quantity, const Asset& asset/* = Asset::USDT*/) const {
    if (quantity == Decimal::Zero || id() == asset.id() || (isUSD() && asset.isUSD()))
        return quantity;

    if (const PriceWrapper* price = Exchanger().price(id() + asset.id()))
        return price->get() * quantity;

    return {};
}

bool Asset::isUSD() const {
    return id() == USDT.id() || id() == BUSD.id();
}

std::string Asset::operator+(const Asset& quote) const {
    return id() + quote.id();
}

Asset Asset::origin() const {
    if (id().rfind(LD.id(), 0) == 0)
        return id().substr(LD.id().size());

    return *this;
}

Asset Asset::ld() const {
    const std::string& ticker = LD.id() + id();

    if (const BalanceWrapper* wrapper = Exchanger().balance(ticker))
        return ticker;

    for (auto& balance : Exchanger().balances()) {
        if (balance.first.rfind(ticker, 0) == 0)
            return balance.first;
    }

    return Empty;
}