#pragma once

#include <functional>

namespace core {
template<class T> class Proxy {
public:
    virtual ~Proxy() = default;

    static T& getInstance() {
        static T instance;
        return instance;
    }
};
}
