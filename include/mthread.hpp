#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#include "./scrwl.hpp"

namespace scrwl
{
    using QueueType = scrwl::Site;

    struct ThreadPool
    {
        std::vector<std::jthread> threads;

        template <typename F>
        ThreadPool(std::size_t, F);
        ~ThreadPool() = default;
    };

    struct Queue
    {
        std::deque<QueueType> queue;
        mutable std::mutex mutex;
        std::condition_variable_any c_var;

        Queue() = default;

        std::optional<QueueType> pop();
        size_t size() const;
        void push(QueueType);
    };
}
