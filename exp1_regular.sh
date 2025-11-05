#!/usr/bin/env bash
set -euo pipefail

SERVER_IP="10.200.125.80"  
PORT=20001                     

TOTAL_ORDERS=10000             
CUSTOMER_SET="1 2 4 8 16 32 64 128 256"

out="exp1_regular.csv"
echo "customers,avg_us,min_us,max_us,throughput" > "$out"

for c in $CUSTOMER_SET; do
  
  ORD=$(( (TOTAL_ORDERS + c - 1) / c ))
  echo "Running client with $c customers and $ORD orders each..."
  line=$(./client "$SERVER_IP" "$PORT" "$c" "$ORD" 0 | tail -n1)

  avg=$(awk '{print $1}' <<<"$line")
  min=$(awk '{print $2}' <<<"$line")
  max=$(awk '{print $3}' <<<"$line")
  thp=$(awk '{print $4}' <<<"$line")

  echo "$c,$avg,$min,$max,$thp" | tee -a "$out"
done

echo "Saved results to: $out"
