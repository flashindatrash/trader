#pragma once

#include <unordered_map>
#include <string>
#include <cassert>
#include "exchanger/base/Storage-macros.hpp"

template<class T> class StorageMap : protected std::unordered_map<std::string, T*> {
    typedef std::unordered_map<std::string, T*> BaseClass;

public: // methods
    StorageMap() = default;
    virtual ~StorageMap();

public: // methods
    const T* get(const std::string& key) const;
    T* get(const std::string& key);
};

class Symbol;
class PriceWrapper;
class BalanceWrapper;
class CandlestickWrapper;
class ChartWrapper;
class BookWrapper;

class Storage {
    STORAGE_PUBLIC(Symbol, pair)
    STORAGE_PUBLIC(PriceWrapper, price)
    STORAGE_PUBLIC(BalanceWrapper, balance)
    STORAGE_PUBLIC(CandlestickWrapper, stat)
    STORAGE_PUBLIC(ChartWrapper, chart)
    STORAGE_PUBLIC(BookWrapper, book)
};

