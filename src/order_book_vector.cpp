#include "order_book_variants.hpp"
#include <algorithm>

// sorted array, using binary search for insertion
static void add_level(std::vector<Level>& levels, int price, int qty, bool descending) {
    auto cmp = [descending](const Level& a, const Level& b){
        return descending ? a.price > b.price : a.price < b.price;
    };
    // Binary search find position
    int lo=0, hi=(int)levels.size();
    while(lo<hi){
        int mid=(lo+hi)/2;
        if ((descending && levels[mid].price > price) ||
            (!descending && levels[mid].price < price)) lo=mid+1;
        else hi=mid;
    }
    if (lo < (int)levels.size() && levels[lo].price == price) {
        levels[lo].total_qty += qty;
    } else {
        levels.insert(levels.begin()+lo, Level{price, qty});
    }
}

static void match_vector(std::vector<Level>& opp, bool is_buy) {
    // no-op: real matching handled in add_market
    (void)opp; (void)is_buy;
}

void VectorOrderBook::add_limit(bool is_buy, int price, int qty) {
    if (is_buy) add_level(bids_, price, qty, /*descending=*/true);
    else        add_level(asks_, price, qty, /*descending=*/false);
}

void VectorOrderBook::add_market(bool is_buy, int qty){
    auto &opp = is_buy ? asks_ : bids_;
    while(qty>0 && !opp.empty()){
        Level &best = is_buy ? opp.front() : opp.front(); // asks_ ascending, bids_ descending (front chosen)
        int take = std::min(qty, best.total_qty);
        best.total_qty -= take;
        qty -= take;
        if (best.total_qty == 0) opp.erase(opp.begin());
    }
}

void VectorOrderBook::cancel(bool is_buy, int price, int qty){
    auto &side = is_buy ? bids_ : asks_;
    for (size_t i=0;i<side.size();++i){
        if (side[i].price == price){
            side[i].total_qty -= qty;
            if (side[i].total_qty <=0) side.erase(side.begin()+i);
            return;
        }
    }
}

int VectorOrderBook::best_bid() const {
    return bids_.empty() ? -1 : bids_.front().price;
}
int VectorOrderBook::best_ask() const {
    return asks_.empty() ? -1 : asks_.front().price;
}