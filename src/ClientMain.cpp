#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

#include "ClientStub.h"
#include "Protocol.h"

using Clock = std::chrono::high_resolution_clock;
using Usec = std::chrono::microseconds;

int main(int argc, char *argv[])
{
	//for args of client main.
	if (argc != 6)
	{
		std::cerr << "Usage: " << argv[0]
				  << " <server_ip> <port> <#customers> <#orders> <robot_type>\n";
		return 2;
	}

	const std::string ip = argv[1];
	const int port = std::stoi(argv[2]);
	const int customers = std::stoi(argv[3]);
	const int orders = std::stoi(argv[4]);
	const int robotType = std::stoi(argv[5]); 

	if (customers <= 0 || orders <= 0)
	{
		std::cerr << "Both #customers and #orders must be positive.\n";
		return 2;
	}

	
	std::vector<std::vector<long long>> per_thread_lat_us(customers);
	std::vector<std::thread> threads;
	threads.reserve(customers);

	const auto t_start = Clock::now();

	for (int i = 0; i < customers; ++i)
	{
		threads.emplace_back([&, i]
							 {
      const int customer_id = i + 1;  // customer id
      per_thread_lat_us[i].reserve(orders);

      ClientStub stub;
      try {
        stub.Init(ip, port);

        for (int j = 0; j < orders; ++j) {
          ::Order o{customer_id, j, robotType};  // one order at a time
          const auto s = Clock::now();
          RobotInfo r  = stub.Order(o);
          const auto e = Clock::now();
          const auto us = std::chrono::duration_cast<Usec>(e - s).count();

          per_thread_lat_us[i].push_back(us);
          (void)r;
        }

        stub.Close();
      } catch (const std::exception& ex) {
        std::cerr << "[customer " << customer_id << "] error: " << ex.what() << "\n";
      } });
	}

	for (auto &th : threads)
		th.join();

	const auto t_end = Clock::now();
	const double wall_seconds = std::chrono::duration<double>(t_end - t_start).count();

	
	long long total_orders = 0;
	long double sum_us = 0.0L;
	long long min_us = std::numeric_limits<long long>::max();
	long long max_us = 0;

	for (const auto &v : per_thread_lat_us)
	{
		total_orders += static_cast<long long>(v.size());
		for (auto us : v)
		{
			sum_us += us;
			if (us < min_us)
				min_us = us;
			if (us > max_us)
				max_us = us;
		}
	}

	if (total_orders == 0)
	{
		std::cerr << "No completed orders; cannot compute stats.\n";
		return 1;
	}

	const long long avg_us = static_cast<long long>(sum_us / total_orders + 0.5L);
	const double throughput = static_cast<double>(total_orders) / wall_seconds;

	std::cout << avg_us << "\t" << min_us << "\t" << max_us << "\t" << throughput << "\n";
	return 0;
}
