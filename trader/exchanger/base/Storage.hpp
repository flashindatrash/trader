#pragma once

#include <unordered_map>
#include "Signal.hpp"

template<class K> class MapIdentifier {
public:
    typedef K sTypeKey;

    void setIdentifier(K identifier) {
        _identifier  = identifier;
    }

    const K& getIdentifier() const {
        return _identifier;
    }

protected:
    K _identifier;
};

template<class K, class T> class StorageMap : protected std::unordered_map<K, T*> {
    typedef std::unordered_map<K, T*> BaseClass;
public: // methods
    StorageMap() = default;
    virtual ~StorageMap() {
        for (auto& pair : *this)
            delete pair.second;
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
            wrapper->setIdentifier(key);
            BaseClass::insert(std::make_pair(key, wrapper));
        } else
            wrapper = it->second;
        return wrapper;
    }

public: // signals
    Signal<K&> onChanged;
};
