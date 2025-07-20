#include "event_generator.hpp"
#include <random>

Event EventGenerator::next() {
    std::uniform_int_distribution<int> tdist(0, 99);
    int pick = tdist(rng_);
    Event ev{};
    if (pick < 60) ev.type = Event::ADD_LIMIT;
    else if (pick < 85) ev.type = Event::MARKET;
    else ev.type = Event::CANCEL;

    std::bernoulli_distribution bdist(0.5);
    ev.is_buy = bdist(rng_);

    std::normal_distribution<double> pdist(mid_, 80.0);
    ev.price = (int)pdist(rng_);
    if (ev.price <= 0) ev.price = mid_;

    std::uniform_int_distribution<int> qdist(1, 100);
    ev.qty = qdist(rng_);

    return ev;
}