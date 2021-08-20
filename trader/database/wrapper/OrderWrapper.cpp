#include "OrderWrapper.hpp"

using namespace database;

OrderWrapper* OrderWrapper::create(Id id) {
    OrderWrapper* order = new OrderWrapper();
    return order;
}
