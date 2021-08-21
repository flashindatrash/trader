#pragma once

#include "Database.hpp"

namespace database {
class BaseWrapper {
public: // methods
    bool load();
    bool save();
    bool remove();

protected: // methods
    BaseWrapper(const Key key, const Value id);

private: // methods
    Key internalKey() const;

protected: // vars
    const Key _key;
    const Value _id;

    Object _obj;
};
}
