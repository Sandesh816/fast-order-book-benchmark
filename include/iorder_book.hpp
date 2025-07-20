#pragma once
#include "types.hpp"

class IOrderBook {
public:
    virtual ~IOrderBook() = default;
    virtual void add_limit(bool is_buy, int price, int qty) = 0;
    virtual void add_market(bool is_buy, int qty) = 0;
    virtual void cancel(bool is_buy, int price, int qty) = 0;
    virtual int  best_bid() const = 0;  // -1 if none
    virtual int  best_ask() const = 0;  // -1 if none
};