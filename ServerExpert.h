#pragma once
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include "Protocol.h"

// for a expert engineer.
struct ExpertRequest
{
    RobotInfo robot;
    std::promise<RobotInfo> prom;
};

// Threadsafe FIFO
class ExpertQueue
{
public:
    void push(ExpertRequest &&req)
    {
        {
            std::lock_guard<std::mutex> lk(mu_);
            q_.push_back(std::move(req));
        }
        cv_.notify_one();
    }

    ExpertRequest pop()
    {
        std::unique_lock<std::mutex> lk(mu_);
        cv_.wait(lk, [&]
                 { return !q_.empty(); });
        ExpertRequest r = std::move(q_.front());
        q_.pop_front();
        return r;
    }

private:
    std::mutex mu_;
    std::condition_variable cv_;
    std::deque<ExpertRequest> q_;
};

class ExpertPool
{
public:
    // prevents overlap
    ExpertPool(std::size_t num_experts, int expert_id_base, ExpertQueue &queue)
        : q_(queue)
    {
        threads_.reserve(num_experts);
        for (std::size_t i = 0; i < num_experts; ++i)
        {
            const int my_id = expert_id_base + static_cast<int>(i);
            threads_.emplace_back([this, my_id]
                                  { run_expert(my_id); });
            threads_.back().detach();
        }
    }

private:
    void run_expert(int expert_id)
    {
        while (true)
        {
            ExpertRequest req = q_.pop();

            std::this_thread::sleep_for(std::chrono::microseconds(100));
            req.robot.expert_id = expert_id;
            try
            {
                req.prom.set_value(req.robot);
            }
            catch (...)
            {
            }
        }
    }

    ExpertQueue &q_;
    std::vector<std::thread> threads_;
};
