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
        template <typename F> requires std::invocable<F&>
        auto nq(F&& f) -> std::future<std::invoke_result_t<F&>>;
    };

    struct UrlQueue
    {
        std::mutex list_mutex;
        std::deque<scrwl::Url> site_list; // TODO: Make this a priority queue
        std::unordered_set<std::string> visited_url;

        std::optional<scrwl::Url> pop();
    };
}
