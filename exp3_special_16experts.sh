#!/usr/bin/env bash
set -euo pipefail
SERVER_IP="10.200.125.80"  
PORT=20003
TOTAL_ORDERS=10000
CUSTOMER_SET="1 2 4 8 16 32 64 128 256"

out="exp3_special_16experts.csv"
echo "customers,avg_us,min_us,max_us,throughput" > "$out"

for c in $CUSTOMER_SET; do
  ORD=$(( (TOTAL_ORDERS + c - 1) / c ))
  line=$(./client "$SERVER_IP" "$PORT" "$c" "$ORD" 1 | tail -n1)
  echo "$c,$(echo $line | tr ' ' ',')" | tee -a "$out"
done
echo "Saved: $out"
