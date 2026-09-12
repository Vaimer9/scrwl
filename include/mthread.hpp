#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <future>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <vector>

#include "./scrwl.hpp"

namespace scrwl
{
    using QueueType = scrwl::Site;
    using FuncType = void();

    struct ThreadPool
    {
        std::stop_source stop_source;
        std::mutex task_mutex;
        std::condition_variable_any task_condition;
        std::condition_variable_any drain_condition;
        std::vector<std::jthread> threads;
        std::deque<std::function<void()>> task_list;
        unsigned int active_tasks;

        ThreadPool(std::size_t);
        ~ThreadPool();

        // TODO: Make this more constrained
        template <typename F>
        auto nq(F&& f) -> std::future<decltype(f())>;
    };
}
