#include "order_book_variants.hpp"
#include <algorithm>

// Using a balanced map (std::map) for the order book (red-black tree)
void MapOrderBook::add_limit(bool is_buy, int price, int qty) {
    auto &side = is_buy ? bids_ : asks_;
    side[price] += qty;
}

void MapOrderBook::add_market(bool is_buy, int qty) {
    auto &opp = is_buy ? asks_ : bids_;
    while (qty > 0 && !opp.empty()) {
        auto it = is_buy ? opp.begin() : std::prev(opp.end());
        int take = std::min(qty, it->second);
        it->second -= take;
        qty -= take;
        if (it->second == 0) opp.erase(it);
    }
}

void MapOrderBook::cancel(bool is_buy, int price, int qty) {
    auto &side = is_buy ? bids_ : asks_;
    auto it = side.find(price);
    if (it == side.end()) return;
    it->second -= qty;
    if (it->second <= 0) side.erase(it);
}

int MapOrderBook::best_bid() const {
    return bids_.empty() ? -1 : bids_.rbegin()->first;
}
int MapOrderBook::best_ask() const {
    return asks_.empty() ? -1 : asks_.begin()->first;
}