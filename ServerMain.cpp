#include <atomic>
#include <iostream>
#include <thread>

#include "Net.h"
#include "Protocol.h"
#include "ServerStub.h"
#include "ServerExpert.h"

static void handle_client(int cfd, int engineer_id, ExpertQueue &xq)
{
	try
	{
		ServerStub stub(cfd);
		::Order o{};

		while (stub.ReceiveOrder(o))
		{
			RobotInfo r{};
			r.customer_id = o.customer_id;
			r.order_number = o.order_number;
			r.robot_type = o.robot_type;
			r.engineer_id = engineer_id;
			r.expert_id = -1;

			if (o.robot_type == 0)
			{
				// ship immediately for regular
				if (!stub.ShipRobot(r))
					break;
			}
			else
			{
				// for special robot
				ExpertRequest req;
				req.robot = r;
				std::future<RobotInfo> fut = req.prom.get_future();
				xq.push(std::move(req));
				RobotInfo completed = fut.get();
				if (!stub.ShipRobot(completed))
					break;
			}
		}
	}
	catch (...)
	{
	}
}

int main(int argc, char *argv[])
{

	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <#experts>\n";
		return 2;
	}

	const uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));
	const int num_experts = std::stoi(argv[2]);
	if (num_experts <= 0)
	{
		std::cerr << "#experts must be positive\n";
		return 2;
	}

	int listen_fd = -1;
	try
	{
		listen_fd = create_server_socket(port, 128);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Server setup error: " << e.what() << "\n";
		return 2;
	}

	std::cout << "Server listening on port " << port
			  << " with " << num_experts << " expert(s) ... (Ctrl+C to stop)\n";

	ExpertQueue expert_queue;
	const int expert_id_base = 1000000; // prevents overlap with regular engg.
	ExpertPool experts(static_cast<std::size_t>(num_experts), expert_id_base, expert_queue);

	std::atomic<int> next_engineer_id{1};

	while (true)
	{
		const int cfd = accept_client(listen_fd);
		if (cfd < 0)
			continue;
		const int engineer_id = next_engineer_id.fetch_add(1);
		std::thread(handle_client, cfd, engineer_id, std::ref(expert_queue)).detach();
	}

	return 0;
}
