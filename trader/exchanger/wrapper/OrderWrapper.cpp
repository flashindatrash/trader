#include "OrderWrapper.hpp"

OrderWrapper* OrderWrapper::create()
{
    OrderWrapper* wrapper = new OrderWrapper();
    return wrapper;
}
