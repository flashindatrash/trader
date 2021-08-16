#pragma once

#include <global.hpp>

namespace core {

template<class T> class Proxy {
public:
    virtual ~Proxy() = default;

    static T& getInstance() {
        static T instance;
        return instance;
    }
};

template<class T>class Emitter {
    typedef std::function<void(const T&)> Fn;

public:
    void addListener(Fn listener) {
        _listeners.push_back(listener);
    }

protected:
    void invoke(const T& data) {
        for (Fn& listener : _listeners)
            listener(data);
    }

protected:
    std::vector<Fn> _listeners;
};

}
