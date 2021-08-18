#pragma once

#define STORAGE_PUBLIC(Class, Name) \
    public: \
    typedef StorageMap<Class> Type_##Name; \
    inline const Class* Name(const std::string& key) const { return _##Name##s.get(key); } \
    inline Class* Name(const std::string& key) { return _##Name##s.get(key); } \
    protected: \
    inline const Type_##Name & Name##s() const { return _##Name##s; } \
    inline Type_##Name & Name##s() { return _##Name##s; } \
    Type_##Name _##Name##s;

#define STORAGE_CONNECTOR(Class, Name) \
    public: \
    typedef StorageMap<Class> Type_##Name; \
    inline const Class* Name(const std::string& key) const { return _##Name##s.get(key); } \
    inline Class* Name(const std::string& key) { return _##Name##s.get(key); } \
    protected: \
    inline const Type_##Name & Name##s() const { return _##Name##s; } \
    inline Type_##Name & Name##s() { return _##Name##s; } \
    Type_##Name _##Name##s;
