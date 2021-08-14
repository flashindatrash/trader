#pragma once

class Order {
public: // static
    static Order* create();

protected: // methods
    Order() = default;
};

