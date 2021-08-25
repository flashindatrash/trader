#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"

NS_BEGIN
class Position : public db::Object {
public: // methods
    Position(const db::Key& key);

    const Price price() const;
    void setPrice(Price value);
};

class Positions : public db::ArrayAbstract<Position> {
public: // static
    static Positions* create(const Symbol& pair, bool sync);

public: // methods
    const Position* findProfitableFor(Price price) const;

    void max_element();

protected: // methods
    Positions(const db::Key& key, bool sync);

    virtual bool proceed_push(Position& value) const override;
    virtual bool proceed_erase(Position& value) const override;
    virtual bool proceed_load() const override;

protected: // vars
    const bool _sync;
};
NS_END

