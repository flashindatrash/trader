//
// Created by Вадим Проскурин on 07.08.2022.
//


#pragma once

struct OrderRequest;
class Position;

class OrderController {
public: // methods
    static bool create(OrderRequest& request, Position& result, bool release);

};


