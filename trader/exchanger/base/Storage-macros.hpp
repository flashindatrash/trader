#pragma once

#define MAP(Key, Class, Name) \
    public: \
    typedef StorageMap<Key, Class> Type_##Name; \
    inline const Type_##Name & Name##s() const { return _##Name##s; } \
    inline Type_##Name & Name##s() { return _##Name##s; } \
    inline const Class* Name(const Key& key) const { return _##Name##s.get(key); } \
    inline Class* Name(const Key& key) { return _##Name##s.get(key); } \
    protected: \
    Type_##Name _##Name##s;

#define MAP_INTERNAL(Key, Class, Name) \
    protected: \
    StorageMap<Key, Class> _##Name##s;



