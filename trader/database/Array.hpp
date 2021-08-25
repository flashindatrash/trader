#pragma once

#include "Object.hpp"
#include "Database.hpp"

namespace db {
template<class T>
class ArrayAbstract {
private: // static
    typedef std::vector<T> VectorT;

    static bool find(const VectorValues& vector, const Value& value) {
        for (const Value& v : vector)
            if (v == value)
                return true;
        return false;
    }

public: // methods
    ArrayAbstract(const Key& key) : _key(key) {}
    ArrayAbstract() = default;

    bool push(T value) {
        if (has(value) || not proceed_push(value))
            return false;

        if (not proceed_load() || DB().rpush(_key, value) == size() + 1)
            _values.push_back(value);
        else load();
        return true;
    }

    bool remove(const Value& value) {
        size_t count = 0;
        for (auto it = _values.begin(); it < _values.end(); ++it) {
            if (value == *it && proceed_erase(*it)) {
                ++count;
                it = _values.erase(it);
            }
        }

        if (count != DB().lrem(_key, value))
            load();

        return count > 0;
    }

    bool has(const T& value) const {
        for (const T& it : _values)
            if (it == value)
                return true;
        return false;
    }

protected: //
    void load() {
        if (not proceed_load() || _key.empty())
            return;

        const VectorValues upd = DB().lrange(_key);

        // remove
        for (auto it = _values.begin(); it < _values.end(); ++it) {
            if (not find(upd, *it) && proceed_erase(*it))
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

            if (not add)
                continue;

            T interpolated = T((*it).asString());
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

public: //
    const const_iterator find_if(std::function<bool(const T&)> predicate) const {
        return std::find_if(cbegin(), cend(), predicate);
    };

protected: // methods
    virtual bool proceed_push(T& value) const { return true; }
    virtual bool proceed_erase(T& value) const { return true; }
    virtual bool proceed_load() const { return true; }

protected:
    Key _key;
    VectorT _values;
};

// Primitive array
typedef ArrayAbstract<Value> Array;
}

