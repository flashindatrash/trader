#pragma once

namespace Core {

template<class T> class Proxy {
public:
    static T& getInstance() {
        static T instance;
        return instance;
    }
};

}
