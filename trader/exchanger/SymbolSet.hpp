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
    const T* get(const Symbol& symbol) const {
        auto it = BaseClass::find(symbol);
        if (it == BaseClass::end())
            return nullptr;
        return it->second;
    }

    T* get_mutable(const Symbol& symbol) {
        T* wrapper = nullptr;
        auto it = BaseClass::find(symbol);
        if (it == BaseClass::end()) {
            wrapper = T::create();
            BaseClass::insert(std::make_pair(symbol, wrapper));
        } else
            wrapper = it->second;
        return wrapper;
    }
};

#define CONTAINER(Class, Name) \
    public: \
    inline const Class* Name(const Symbol& symbol) const { return _##Name##s.get(symbol); } \
    inline Class* Name##_mutable(const Symbol& symbol) { return _##Name##s.get_mutable(symbol); } \
    protected: \
    BaseSymbolContainer<Class> _##Name##s;


