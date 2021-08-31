#pragma once

#include "Object.hpp"
#include "Database.hpp"
#include <algorithm>

namespace db {
template<class T>
class ArrayAbstract {
private: // static
    typedef std::vector<T> VectorT;

    static bool find(const VectorValues& vector, const Value& value) {
        // todo: refactor to predicate
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

        if (not proceed_sync() || DB().rpush(_key, value) == size() + 1)
            _values.push_back(value);
        else load();

        proceed_sort();
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

        if (proceed_sync() && count != DB().lrem(_key, value))
            load();

        if (count > 0) {
            proceed_sync();
            return true;
        }

        return false;
    }

    bool has(const T& value) const {
        for (const T& it : _values)
            if (it == value)
                return true;
        return false;
    }

protected: //
    void load() {
        if (not proceed_sync() || _key.empty())
            return;

        const VectorValues upd = DB().lrange(_key);

        // if added or removed changed
        bool changed = false;

        // remove
        for (auto it = _values.begin(); it < _values.end(); ++it) {
            if (not find(upd, *it) && proceed_erase(*it)) {
                it = _values.erase(it);
                changed = true;
            }
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
            changed = true;
        }

        if (changed)
            proceed_sort();
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

    typedef std::function<bool(const T&)> Predicate;
    typedef std::function<bool(const T&, const T&)> Compare;

    const size_t count_if(Predicate predicate) const {
        return std::count_if(cbegin(), cend(), predicate);
    };

    const const_iterator find_if(Predicate predicate) const {
        return std::find_if(cbegin(), cend(), predicate);
    };

    const const_iterator compare(Compare comparator) const {
        const_iterator result = cend();
        for (const_iterator it = cbegin(); it < cend(); ++it) {
            if (result == cend() || comparator(*result, *it))
                result = it;
        }
        return result;
    }

    const const_iterator compare_if(Predicate predicate, Compare comparator) const {
        const_iterator result = cend();
        for (const_iterator it = cbegin(); it < cend(); ++it) {
            if (not predicate(*it))
                continue;
            if (result == cend() || comparator(*result, *it))
                result = it;
        }
        return result;
    }

    template<typename V>
    const V summarize(std::function<V(const T&)> summarizator) const {
        V result = V();
        for (const_iterator it = cbegin(); it < cend(); ++it) {
            result += summarizator(*it);
        }
        return result;
    }

protected: // virtual methods (todo: rename)
    virtual bool proceed_push(T& value) const { return true; }
    virtual bool proceed_erase(T& value) const { return true; }
    virtual bool proceed_sync() const { return true; }
    virtual void proceed_sort() {}

protected:
    Key _key;
    VectorT _values;
};

// Primitive array
typedef ArrayAbstract<Value> Array;
}

