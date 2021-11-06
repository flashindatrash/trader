//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "Defines.hpp"

NS_BEGIN
class Position;
class Context;
struct Report;

class EventFormatter {
public: // static
    static std::string title(const Symbol& symbol);
    static std::string update(const Position& position, const Context& context);
    static std::string order(const OrderBase& order);
    static std::string profit(const Report& report, const Asset& asset);
    static std::string report(const Report& report, const Symbol& symbol);
};
NS_END