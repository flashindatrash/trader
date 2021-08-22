#pragma once

#include "Types.hpp"
#include "Value.hpp"

namespace db {
class ArrayHelper {
public:
    static size_t rpush(const Key& key, const Value& value);
    static VectorValues lrange(const Key& key, int start = 0, int stop = -1);
    static size_t lrem(const Key& key, const Value& value, int count = 0);
};

template<class T>
class ArrayAbstract {
    typedef std::vector<T> VectorT;

public: // methods
    ArrayAbstract(const Key& key) : _key(key) {}
    ArrayAbstract() = default;

    bool push(T& value) {
        if (has(value) || not proceed_push(value))
            return false;

        size_t db_size = ArrayHelper::rpush(_key, value);
        if (db_size == 0)
            return false;

        size_t local_size = size() + 1;
        if (db_size == local_size)
            _values.push_back(value);
        else load();
        return true;
    }

    size_t remove(const Value& value) {
        size_t db_removed = ArrayHelper::lrem(_key, value);
        size_t local_removed = 0;
        for (auto it = _values.begin(); it < _values.end(); ++it) {
            if (value == *it && proceed_erase(*it)) {
                ++local_removed;
                it = _values.erase(it);
            }
        }

        if (db_removed != local_removed)
            load();

        return db_removed;
    }

    bool has(const T& value) const {
        for (const T& it : _values) {
            if (it == value)
                return true;
        }
        return false;
    }

protected: //
    void load() {
        if (_key.empty())
            return;

        const VectorValues upd = ArrayHelper::lrange(_key);

        // remove
        for (auto it = _values.begin(); it < _values.end(); ++it) {
            bool erase = false;
            for (const Value& n : upd) {
                if (n == *it) {
                    erase = true;
                    break;
                }
            }
            if (erase && proceed_erase(*it))
                it = _values.erase(it);
        }

        // add
        for (auto it = upd.begin(); it < upd.end(); ++it) {
            bool add = true;
            for (const Value& n : _values) {
                if (n == *it) {
                    add = false;
                    break;
                }
            }

            T interpolated = interpolate(*it);
            if (add && proceed_push(interpolated))
                _values.push_back(interpolated);
        }
    }

public: // iterator
    inline typename VectorT::reference front();
    inline typename VectorT::const_reference front() const;
    inline typename VectorT::reference back();
    inline typename VectorT::const_reference back() const;

    typedef typename VectorT::iterator iterator;
    typedef typename VectorT::const_iterator const_iterator;

    inline iterator begin() { return _values.begin(); }
    inline const_iterator begin() const { return _values.begin(); }
    inline const_iterator cbegin() const { return _values.cbegin(); }

    inline iterator end() { return _values.end(); }
    inline const_iterator end() const { return _values.end(); }
    inline const_iterator cend() const { return _values.cend(); }

    inline void resize(size_t size) { return _values.resize(size); }
    inline void reserve(size_t size) { return _values.reserve(size); }

    inline size_t size() const { return _values.size(); }

protected: // methods
    virtual bool proceed_push(T& value) const { return true; }
    virtual bool proceed_erase(T& value) const { return true; }
    virtual T interpolate(const Value& value) const = 0;

protected:
    Key _key;
    VectorT _values;
};
}
