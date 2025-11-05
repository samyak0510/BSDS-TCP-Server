# Multithreaded TCP Server – Robot Factory

This project contains the C++ implementation and benchmarking artifacts for a high-concurrency TCP server used to simulate a robot factory order pipeline. The implementation targets Khoury College Linux servers and combines a thread-per-client networking front-end with a background pool of specialist engineers that fulfill "special" robot requests.

## Features
- **Thread-per-client networking core** that accepts hundreds of simultaneous TCP clients while keeping per-request latency flat.
- **RPC-inspired protocol stubs** shared by the server and client to simplify message exchange over sockets.
- **Expert worker pool** backed by a thread-safe FIFO queue using `std::mutex` and `std::condition_variable` to process special robot builds.
- **Benchmark automation scripts** that replay the experiments highlighted in the accompanying report and collect CSV summaries.

## Repository layout
```
.
├── include/            # Public headers shared by the server and client
├── src/                # C++ source files for the server, client, and common utilities
├── scripts/            # Benchmark helpers that gather throughput and latency data
├── reports/
│   ├── BSDS.pdf        # Project report
│   ├── exp*.csv        # Captured benchmark outputs (Excel-friendly)
│   └── plots/          # Throughput and latency plots for each experiment
├── Makefile            # Build rules for the server and client binaries
└── README.md           # This file
```

## Build instructions
1. Ensure a C++11-capable toolchain is available (tested with `g++`).
2. From the repository root, run:
   ```sh
   make          # builds ./server and ./client
   ```
   Use `make debug` to compile with debug symbols and `-DDEBUG` enabled.
3. Clean generated binaries and objects with:
   ```sh
   make clean
   ```

The Makefile creates all object files under `build/` and links the final binaries in the repository root.

## Running the server and client
1. Launch the server with the listening port and number of expert engineers:
   ```sh
   ./server <port> <num_experts>
   ```
2. Start the client with the server address, port, number of concurrent customers, orders per customer, and robot type (0 = regular, 1 = special):
   ```sh
   ./client <server_ip> <port> <customers> <orders_per_customer> <robot_type>
   ```

The client prints average, minimum, and maximum latency (microseconds) plus throughput (ops/s) for the requested run.

## Reproducing benchmarks
Automation helpers in `scripts/` can be used to replay the throughput and latency experiments from the report:

```sh
./scripts/exp1_regular.sh
./scripts/exp2_special_1expert.sh
./scripts/exp3_special_16experts.sh
./scripts/exp4_special_equal.sh
```

Each script saves its CSV output to the `reports/` directory (creating it if necessary). The generated CSV files can be opened directly in Excel or plotted via the Python notebooks used to create the published graphs under `reports/plots/`.

## Additional resources
- **Detailed report:** `reports/BSDS.pdf`
- **Benchmark plots:** `reports/plots/*.png`

These documents capture the linear throughput scaling results cited in the accompanying resume entry.
