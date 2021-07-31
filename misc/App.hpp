#pragma once

namespace Core {

class App
{
public: // methods
    App();

    void run();

protected: // methods
    virtual void init() {}
    virtual void tick() {};
};

}
