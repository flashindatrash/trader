#pragma once

#include "exchanger/base/Symbol.hpp"

#define NS_BEGIN namespace strategy { namespace pair {
#define NS_END } }
#define NS_USE using namespace strategy::pair;

#define WRAPPER_CREATE(Class) \
    static Class* create(const Id id) { \
        Class* wrapper = new Class(id); \
        return wrapper; \
    }

#define WRAPPER_BYID(Class) \
    static Class* byId(const Id id) { \
        Class* wrapper = create(id); \
        wrapper->load(); \
        return wrapper; \
    }
