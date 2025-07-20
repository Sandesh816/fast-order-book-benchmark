#include "order_book_variants.hpp"
#include <algorithm>
#include <queue>

void HashOrderBook::clean_best_bid() {
    while (!bid_heap_.empty()) {
        int p = bid_heap_.top();
        auto it = bids_.find(p);
        if (it == bids_.end() || it->second <= 0) {
            bid_heap_.pop(); // stale
        } else {
            break; // top is valid
        }
    }
}

void HashOrderBook::clean_best_ask() {
    while (!ask_heap_.empty()) {
        int p = ask_heap_.top();
        auto it = asks_.find(p);
        if (it == asks_.end() || it->second <= 0) {
            ask_heap_.pop(); // stale
        } else {
            break;
        }
    }
}

void HashOrderBook::add_limit(bool is_buy, int price, int qty) {
    if (qty <= 0) return;
    if (is_buy) {
        int &ref = bids_[price];
        if (ref == 0) {
            // new live level -> push to heap
            bid_heap_.push(price);
        }
        ref += qty;
    } else {
        int &ref = asks_[price];
        if (ref == 0) {
            ask_heap_.push(price);
        }
        ref += qty;
    }
}

void HashOrderBook::add_market(bool is_buy, int qty) {
    if (qty <= 0) return;
    if (is_buy) {
        while (qty > 0) {
            clean_best_ask();
            if (ask_heap_.empty()) break;
            int best = ask_heap_.top();
            int &level_qty = asks_[best];
            int take = std::min(qty, level_qty);
            level_qty -= take;
            qty -= take;
            if (level_qty <= 0) {
                asks_.erase(best);      // heap entry lazily removed later
                // we won't pop here
            }
        }
    } else {
        while (qty > 0) {
            clean_best_bid();
            if (bid_heap_.empty()) break;
            int best = bid_heap_.top();
            int &level_qty = bids_[best];
            int take = std::min(qty, level_qty);
            level_qty -= take;
            qty -= take;
            if (level_qty <= 0) {
                bids_.erase(best);
            }
        }
    }
}

void HashOrderBook::cancel(bool is_buy, int price, int qty) {
    if (qty <= 0) return;
    if (is_buy) {
        auto it = bids_.find(price);
        if (it == bids_.end()) return;
        it->second -= qty;
        if (it->second <= 0) {
            bids_.erase(it);
            // lazy: heap entry remains; will be skipped later
        }
    } else {
        auto it = asks_.find(price);
        if (it == asks_.end()) return;
        it->second -= qty;
        if (it->second <= 0) {
            asks_.erase(it);
        }
    }
}

int HashOrderBook::best_bid() const {
    // const_cast to reuse cleanup logic (or duplicate a const-safe version)
    auto *self = const_cast<HashOrderBook*>(this);
    self->clean_best_bid();
    if (self->bid_heap_.empty()) return -1;
    return self->bid_heap_.top();
}

int HashOrderBook::best_ask() const {
    auto *self = const_cast<HashOrderBook*>(this);
    self->clean_best_ask();
    if (self->ask_heap_.empty()) return -1;
    return self->ask_heap_.top();
}