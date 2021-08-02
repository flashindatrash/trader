#pragma once

namespace Core {

template<class T> class Proxy {
public:
    static T& getInstance() {
        static T instance;
        static_cast<Proxy*>(&instance)->retain();
        return instance;
    }

protected:
    virtual bool init() { return true; }

private:
    void retain() {
        if (not _inited)
            _inited = init();
    }

    bool _inited = false;
};

}
