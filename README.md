# Fast Order Book Benchmark (C++17)

FastOrderBookBenchmark is a C++17 micro-benchmark comparing three limit order book data structure layouts: a red-black tree (`std::map`), a sorted dynamic array using binary-search for O(log n) random order retrieval and O(1) best-order retrieval( `std::vector`), and a two HashMap layout (`std::unordered_map`) + **lazy min/max heaps** for best bid/ask tracking.

## Workload & Methodology

- **Events:** Synthetic stream (default 500,000 per run in these measurements)
- **Mix:** 60% limit orders, 25% market orders, 15% cancels
- **Price generation:** Normal distribution N(10,000, 80)
- **Quantity:** Uniform 1–100
- **Sampling:** Every 100th event latency (ns) recorded; warm-up of 10,000 events excluded
- **Build:** C++17, `-O3 -march=native -DNDEBUG`
- **Seed:** 42 (deterministic RNG)
- **Platform:** macOS (CPU: M1 Pro 8 core)

## Aggregate Multi-Trial Results (500k events, 5 trials each)

| Variant | Mean Throughput (events/sec) | Std Dev | Median Latency (ns) | p95 (ns) | p99 (ns) |
|---------|------------------------------|---------|---------------------|----------|----------|
| `map` (RB-tree)    | 8.85M | 0.74M | 42 | 92 | 175 |
| `vector` (sorted)  | 8.58M | 0.14M | 42 | 125 | 167 |
| `hash` + lazy heaps| **10.39M** | 0.11M | 42 | 125 | 233 |


> **Key Observation:** After introducing *lazy min/max heap tracking with active price sets*, the hash layout not only removed the original tail latency blow-up (naive p99 1.29 µs) but also delivered the highest throughput (~10.4M events/sec) with a p99 ≤ 250 ns—comparable to tree/vector variants.

## Raw Trial Data

| Variant | Trial | Throughput (events/sec) | Median ns | p95 ns | p99 ns |
|---------|------|-------------------------|-----------|--------|--------|
| map | 1 | 7.39332e+06 | 42 | 125 | 208 |
| map | 2 | 9.34371e+06 | 42 | 84  | 167 |
| map | 3 | 9.38439e+06 | 42 | 84  | 166 |
| map | 4 | 9.15934e+06 | 42 | 84  | 166 |
| map | 5 | 8.95129e+06 | 42 | 84  | 167 |
| vector | 1 | 8.66860e+06 | 42 | 125 | 167 |
| vector | 2 | 8.38934e+06 | 42 | 125 | 167 |
| vector | 3 | 8.63170e+06 | 42 | 125 | 167 |
| vector | 4 | 8.44210e+06 | 42 | 125 | 167 |
| vector | 5 | 8.78762e+06 | 42 | 125 | 167 |
| hash | 1 | 1.04255e+07 | 41 | 125 | 209 |
| hash | 2 | 1.04557e+07 | 41 | 125 | 209 |
| hash | 3 | 1.04215e+07 | 42 | 125 | 250 |
| hash | 4 | 1.02026e+07 | 42 | 125 | 250 |
| hash | 5 | 1.04581e+07 | 42 | 125 | 250 |

## Performance Summary

Baseline `std::map` and sorted `std::vector` implementations both reached ~8.6–8.9M events/sec with 42 ns median and ≤ 0.25 µs p99 latency. The **initial naive hash design** (not included in the table) achieved only ~5.2M events/sec with a 1.29 µs p99 because it performed full scans to recompute best bid/ask after deletions. Replacing those rescans with *lazy min/max heaps plus active price sets* produced a hash variant averaging **~10.4M events/sec**, while shrinking tail latency to ≤ 0.25 µs—demonstrating that data structure *ancillary maintenance strategy* (not the hashing itself) was the real bottleneck.

## Build & Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/fastob_bench --variant=map --events=500000
./build/fastob_bench --variant=vector --events=500000
./build/fastob_bench --variant=hash --events=500000
