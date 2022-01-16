//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "exchanger/base/Symbol.hpp"

class Position;
struct Settings;
struct Report;

namespace pair {
class Context;

class Formatter {
public: // static
    static Formatter title(const Symbol& symbol);
    static Formatter update(const Position& position, const Context& context, const Settings& settings);
    static Formatter order(const OrderBase& order);
    static Formatter profit(const Report& report, const Symbol& symbol);
    static Formatter report(const Report& report, const Symbol& symbol);

private: // static
    static std::string asset(Quantity quantity, const Asset& asset = Asset::Empty, bool change = false);

public: // methods
    Formatter() = default;
    Formatter(std::string value);

    std::string terminal() const;
    std::string html() const;

private: // methods
    static void replace(std::string& text, const std::string& from, const std::string& to);

private: // vars
    std::string _text;
};
}