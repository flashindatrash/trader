#pragma once

#include <vector>
#include "exchanger/base/Identifier.hpp"

struct OrderDetail;
class OrderWrapper;

class BookWrapper : public Identifier {
public: // static
    static BookWrapper* create();

public: // methods
    virtual ~BookWrapper();

    const OrderWrapper* add(const OrderDetail& data);
    const std::vector<const OrderWrapper*>& get() const;

protected: // methods
    BookWrapper() = default;

protected: // vars
    std::vector<const OrderWrapper*> _orders;
};

