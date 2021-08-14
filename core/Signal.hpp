#pragma once

#include <functional>

template<class T>class Signal {
    typedef std::function<void(const T&)> Fn;

public:
    void connect(Fn listener) {
        _listeners.push_back(listener);
    }

protected:
    void emmit(const T& data) {
        for (Fn& listener : _listeners)
            listener(data);
    }

protected:
    std::vector<Fn> _listeners;
};
