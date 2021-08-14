#pragma once

#include <unordered_map>
#include <string>
#include "Signal.hpp"

template<class K, class T> class SymbolSet : protected std::unordered_map<K, T*> {
    typedef std::unordered_map<K, T*> BaseClass;
public: // methods
    SymbolSet() = default;
    virtual ~SymbolSet() {
        for (auto& pair : *this)
            SAFE_DELETE(pair.second);
        BaseClass::clear();
    }

public: // methods
    const T* get(const K& key) const {
        auto it = BaseClass::find(key);
        if (it == BaseClass::end())
            return nullptr;
        return it->second;
    }

    T* get(const K& key) {
        T* wrapper = nullptr;
        auto it = BaseClass::find(key);
        if (it == BaseClass::end()) {
            wrapper = T::create();
            BaseClass::insert(std::make_pair(key, wrapper));
        } else
            wrapper = it->second;
        return wrapper;
    }

public: // signals
    Signal<K&> onChanged;
};

#define MAP(Key, Class, Name) \
    public: \
    typedef SymbolSet<Key, Class> Type_##Name; \
    inline const Type_##Name & Name##s() const { return _##Name##s; } \
    inline Type_##Name & Name##s() { return _##Name##s; } \
    inline const Class* Name(const Key& key) const { return _##Name##s.get(key); } \
    inline Class* Name(const Key& key) { return _##Name##s.get(key); } \
    protected: \
    Type_##Name _##Name##s;


