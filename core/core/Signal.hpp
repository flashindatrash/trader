#pragma once

#include <vector>
#include <functional>

template<class T>class Signal {
public:
    typedef std::function<void(const T&)> Fn;

public:
    void connect(Fn listener) {
        _listeners.push_back(listener);
    }

    void emmit(const T& data) {
        for (Fn& listener : _listeners)
            listener(data);
    }

protected:
    std::vector<Fn> _listeners;
};
