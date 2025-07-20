#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include "order_book_variants.hpp"
#include "event_generator.hpp"

static uint64_t nano_time() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

int main(int argc, char** argv) {
    std::string variant = "map";
    size_t events = 1'000'00; // 100k default
    for (int i=1;i<argc;i++){
        std::string a = argv[i];
        if (a.rfind("--variant=",0)==0) variant = a.substr(10);
        else if (a.rfind("--events=",0)==0) events = std::stoull(a.substr(9));
    }

    std::unique_ptr<IOrderBook> book;
    if (variant=="map") book = std::make_unique<MapOrderBook>();
    else if (variant=="vector") book = std::make_unique<VectorOrderBook>();
    else if (variant=="hash") book = std::make_unique<HashOrderBook>();
    else {
        std::cerr << "Unknown variant\n";
        return 1;
    }

    EventGenerator gen(42);

    std::vector<uint64_t> samples; samples.reserve(events/100);
    // Warm-up
    for (size_t i=0;i<10'000;i++){
        auto ev = gen.next();
        if (ev.type==Event::ADD_LIMIT) book->add_limit(ev.is_buy, ev.price, ev.qty);
        else if (ev.type==Event::MARKET) book->add_market(ev.is_buy, ev.qty);
        else book->cancel(ev.is_buy, ev.price, ev.qty);
    }

    uint64_t t_start = nano_time();
    for (size_t i=0;i<events;i++){
        auto ev = gen.next();
        uint64_t t0=0;
        if ((i % 100)==0) t0 = nano_time();
        if (ev.type==Event::ADD_LIMIT) book->add_limit(ev.is_buy, ev.price, ev.qty);
        else if (ev.type==Event::MARKET) book->add_market(ev.is_buy, ev.qty);
        else book->cancel(ev.is_buy, ev.price, ev.qty);
        if ((i % 100)==0) {
            uint64_t t1 = nano_time();
            samples.push_back(t1 - t0);
        }
    }
    uint64_t t_end = nano_time();

    double elapsed_sec = (t_end - t_start) / 1e9;
    double throughput = events / elapsed_sec;

    std::sort(samples.begin(), samples.end());
    auto pct = [&](double q){
        size_t idx = (size_t)(q * samples.size());
        if (idx >= samples.size()) idx = samples.size()-1;
        return samples[idx];
    };

    std::cout << "variant=" << variant
              << " events=" << events
              << " throughput_ev_s=" << throughput
              << " median_ns=" << pct(0.50)
              << " p95_ns=" << pct(0.95)
              << " p99_ns=" << pct(0.99)
              << " best_bid=" << book->best_bid()
              << " best_ask=" << book->best_ask()
              << "\n";
}