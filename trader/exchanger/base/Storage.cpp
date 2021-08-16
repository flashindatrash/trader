#include "Storage.hpp"

template <class T>
StorageMap<T>::~StorageMap() {
    for (auto& pair : *this)
        delete pair.second;
    BaseClass::clear();
}

template <class T>
const T* StorageMap<T>::get(const std::string& key) const {
    auto it = BaseClass::find(key);
    if (it == BaseClass::end())
        return nullptr;
    return it->second;
}

template <class T>
T* StorageMap<T>::get(const std::string& key) {
    T* wrapper = nullptr;
    auto it = BaseClass::find(key);
    if (it == BaseClass::end()) {
        wrapper = T::create();
        wrapper->setIdentifier(key);
        BaseClass::insert(std::make_pair(key, wrapper));
    } else
        wrapper = it->second;
    return wrapper;
}

#include "exchanger/wrapper/ExchangeWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"

template class StorageMap<ExchangeWrapper>;
template class StorageMap<PriceWrapper>;
template class StorageMap<BalanceWrapper>;
template class StorageMap<CandlestickWrapper>;
template class StorageMap<ChartWrapper>;
template class StorageMap<BookWrapper>;
