#pragma once

class OrderManager;

class Migrator
{
public: // static
    static bool migrate(OrderManager& orders);

public: // methods
    Migrator() = default;
};

