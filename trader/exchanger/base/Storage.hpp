#pragma once

#include <unordered_map>
#include <string>
#include "exchanger/base/Storage-macros.hpp"
#include "Signal.hpp"

template<class T> class StorageMap : protected std::unordered_map<std::string, T*> {
    typedef std::unordered_map<std::string, T*> BaseClass;
public: // methods
    StorageMap() = default;
    virtual ~StorageMap();

public: // methods
    const T* get(const std::string& key) const;
    T* get(const std::string& key);

public: // signals
    Signal<std::string&> onChanged;
};

class ExchangeWrapper;
class PriceWrapper;
class BalanceWrapper;
class CandlestickWrapper;
class ChartWrapper;
class BookWrapper;

class Storage {
    STORAGE_PUBLIC(ExchangeWrapper, info)
    STORAGE_PUBLIC(PriceWrapper, price)
    STORAGE_PUBLIC(BalanceWrapper, balance)
    STORAGE_PRIVATE(CandlestickWrapper, stat)
    STORAGE_PRIVATE(ChartWrapper, chart)
    STORAGE_PRIVATE(BookWrapper, book)
};

