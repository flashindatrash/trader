#include "BookWrapper.hpp"
#include "OrderWrapper.hpp"

BookWrapper* BookWrapper::create() {
    BookWrapper* wrapper = new BookWrapper();
    return wrapper;
}

BookWrapper::~BookWrapper() {
    for (const OrderWrapper* order : _orders)
        delete order;
    _orders.clear();
}

const OrderWrapper* BookWrapper::add(const Order& data) {
    OrderWrapper* wrapper = OrderWrapper::create();
    wrapper->set(data);
    _orders.push_back(wrapper);
    return wrapper;
}

const std::vector<const OrderWrapper*>& BookWrapper::get() const {
    return _orders;
}
