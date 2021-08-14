#pragma once

#include <unordered_map>
#include <string>

class Symbol;

template<class T> class SymbolSet : protected std::unordered_map<std::string, T*> {
    typedef std::unordered_map<std::string, T*> BaseClass;
public: // methods
    SymbolSet() = default;
    virtual ~SymbolSet() {
        for (auto& pair : *this)
            SAFE_DELETE(pair.second);
        BaseClass::clear();
    }

public: // methods
    const T* get(const std::string& key) const {
        auto it = BaseClass::find(key);
        if (it == BaseClass::end())
            return nullptr;
        return it->second;
    }

    T* get_mutable(const std::string& key) {
        T* wrapper = nullptr;
        auto it = BaseClass::find(key);
        if (it == BaseClass::end()) {
            wrapper = T::create();
            BaseClass::insert(std::make_pair(key, wrapper));
        } else
            wrapper = it->second;
        return wrapper;
    }
};

#define CONTAINER(Key, Class, Name) \
    public: \
    inline const Class* Name(const Key& key) const { return _##Name##s.get(key); } \
    inline Class* Name##_mutable(const Key& key) { return _##Name##s.get_mutable(key); } \
    protected: \
    SymbolSet<Class> _##Name##s;


