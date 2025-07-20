#pragma once
#include "iorder_book.hpp"
#include <map>
#include <vector>
#include <unordered_map>

class MapOrderBook : public IOrderBook {
    std::map<int,int> bids_; // price -> qty, sell orders
    std::map<int,int> asks_; // buy orders. Price -> qty
public:
    void add_limit(bool is_buy, int price, int qty) override;
    void add_market(bool is_buy, int qty) override;
    void cancel(bool is_buy, int price, int qty) override;
    int best_bid() const override;
    int best_ask() const override;
};

class VectorOrderBook : public IOrderBook {
    std::vector<Level> bids_; // descending prices
    std::vector<Level> asks_; // ascending prices
public:
    void add_limit(bool is_buy, int price, int qty) override;
    void add_market(bool is_buy, int qty) override;
    void cancel(bool is_buy, int price, int qty) override;
    int best_bid() const override;
    int best_ask() const override;
};

// defalt two HashMaps order book performed way worse than Map and Vector order book
// so, optimizing it using two heaps with lazy removal
#include <queue>
class HashOrderBook : public IOrderBook {
    std::unordered_map<int,int> bids_;
    std::unordered_map<int,int> asks_;

    // Max-heap for bids (highest price first)
    std::priority_queue<int> bid_heap_;
    // Min-heap for asks (lowest price first) via greater<>
    std::priority_queue<int, std::vector<int>, std::greater<int>> ask_heap_;

    // Helpers: lazily pop stale (price not present or qty <= 0)
    void clean_best_bid();
    void clean_best_ask();

public:
    void add_limit(bool is_buy, int price, int qty) override;
    void add_market(bool is_buy, int qty) override;
    void cancel(bool is_buy, int price, int qty) override;
    int best_bid() const override;
    int best_ask() const override;
};