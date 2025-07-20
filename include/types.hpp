#pragma once
#include <cstdint>

struct Order {
    uint64_t id;
    bool     is_buy;
    int      price;     // integer ticks
    int      qty;
};

struct Level {
    int price;
    int total_qty;
};