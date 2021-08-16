#include "BookWrapper.hpp"
#include "OrderWrapper.hpp"

BookWrapper* BookWrapper::create() {
    BookWrapper* wrapper = new BookWrapper();
    return wrapper;
}

const OrderWrapper* BookWrapper::add(const Order& data) {
    OrderWrapper* wrapper = OrderWrapper::create();
    wrapper->set(data);
    return wrapper;
}

const std::vector<const OrderWrapper*>& BookWrapper::get() const {
    return _orders;
}
