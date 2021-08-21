#include "BaseWrapper.hpp"

using namespace database;

BaseWrapper::BaseWrapper(const Key key, Value id)
    : _key(key)
    , _id(id)
{
}

Key BaseWrapper::internalKey() const {
    return _key + ":" + _id.asString();
}

bool BaseWrapper::load() {
    _obj = DB().hgetall(internalKey());
    return not _obj.empty();
}

bool BaseWrapper::save() {
    return true;
}

bool BaseWrapper::remove() {
    DB().del(internalKey());
    return true;
}
