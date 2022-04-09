#pragma once

#include <vector>
#include <functional>

template<class T>class Signal {
public:
    typedef std::function<void(const T&)> Fn;

public:
    ~Signal() {
        _listeners.clear();
    }

    size_t connect(Fn listener) {
        _listeners.push_back(listener);
        return _listeners.size() - 1;
    }

    size_t disconnect(size_t index) {
        // TODO: багоопасно, index смещается
        if (index > 0 && index < _listeners.size())
            _listeners.erase(_listeners.begin() + index);
        return -1;
    }

    void emmit(const T& data) {
        for (Fn& listener : _listeners)
            listener(data);
    }

protected:
    std::vector<Fn> _listeners;
};
