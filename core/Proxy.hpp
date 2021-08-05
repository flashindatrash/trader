#pragma once

namespace Core {

template<class T> class Proxy {
public:
    virtual ~Proxy() {}

    static T& getInstance() {
        static T instance;
        return instance;
    }
};

}
