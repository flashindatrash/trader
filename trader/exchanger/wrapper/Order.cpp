#include "Order.hpp"

Order* Order::create()
{
    Order* wrapper = new Order();
    return wrapper;
}
