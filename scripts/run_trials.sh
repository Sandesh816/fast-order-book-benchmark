#!/usr/bin/env bash
set -e
variants=("map" "vector" "hash")
events=500000
trials=5
echo "variant,trial,throughput_ev_s,median_ns,p95_ns,p99_ns" > results.csv

for v in "${variants[@]}"; do
  for t in $(seq 1 $trials); do
    line=$(./build/fastob_bench --variant=$v --events=$events)
    # parse simple key=val fields
    throughput=$(echo "$line" | sed -n 's/.*throughput_ev_s=\([^ ]*\).*/\1/p')
    median=$(echo "$line" | sed -n 's/.*median_ns=\([^ ]*\).*/\1/p')
    p95=$(echo "$line" | sed -n 's/.*p95_ns=\([^ ]*\).*/\1/p')
    p99=$(echo "$line" | sed -n 's/.*p99_ns=\([^ ]*\).*/\1/p')
    echo "$v,$t,$throughput,$median,$p95,$p99" >> results.csv
  done
done