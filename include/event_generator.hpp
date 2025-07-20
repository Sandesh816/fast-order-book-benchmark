#pragma once
#include <random>

struct Event {
    enum Type { ADD_LIMIT, MARKET, CANCEL } type;
    bool is_buy;
    int price;
    int qty;
};

class EventGenerator {
    std::mt19937_64 rng_;
    int mid_ = 10'000;  // base price
public:
    explicit EventGenerator(uint64_t seed=42) : rng_(seed) {}
    Event next();
};