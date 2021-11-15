//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "Defines.hpp"

NS_BEGIN
class Position;
class Statistics;
class Context;
struct Report;

class Formatter {
public: // static
    static Formatter title(const Symbol& symbol);
    static Formatter update(const Position& position, const Context& context);
    static Formatter order(const OrderBase& order);
    static Formatter profit(const Report& report, const Symbol& symbol);
    static Formatter report(const Report& report, const Symbol& symbol);
    static Formatter stats(const Statistics& statistics, const Symbol& symbol);

public: // methods
    Formatter() = default;
    Formatter(std::string value);

    std::string terminal() const;
    std::string markdown() const;

private: // methods
    static void replace(std::string& text, const std::string& from, const std::string& to);

private: // vars
    std::string _text;
};
NS_END