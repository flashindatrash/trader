#include "Asset.hpp"
#include "proxy/Exchanger.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "util/StringUtil.hpp"

Asset::Asset(const std::string& asset) {
    setId(util::uppercase(asset.c_str()));
}

const char* Asset::c_str() const {
    return _identifier.c_str();
}

Asset::operator std::string() const {
    return _identifier;
}

const Quantity& Asset::getBalance() const {
    return Exchanger().balance(*this)->get();
}

const std::string Asset::operator+(const Asset& quote) const {
    return id() + quote.id();
}
