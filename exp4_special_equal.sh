#!/usr/bin/env bash
set -euo pipefail
SERVER_IP="10.200.125.80"  
PORT=20004
TOTAL_ORDERS=10000
CUSTOMER_SET="1 2 4 8 16 32 64 128 256"

out="exp4_special_equal.csv"
echo "customers,avg_us,min_us,max_us,throughput" > "$out"

for c in $CUSTOMER_SET; do
  echo "=== Restart server on host A: ./server $PORT $c  ==="
  read -p "Press Enter once server $c-experts is running..."
  ORD=$(( (TOTAL_ORDERS + c - 1) / c ))
  line=$(./client "$SERVER_IP" "$PORT" "$c" "$ORD" 1 | tail -n1)
  echo "$c,$(echo $line | tr ' ' ',')" | tee -a "$out"
done
echo "Saved: $out"
