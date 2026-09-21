#pragma once

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <future>
#include <mutex>
#include <string_view>
#include <thread>
#include <vector>
#include <functional>
#include <concepts>
#include <unordered_set>
#include <type_traits>

#include "./site.hpp"

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

        // nq = Enqueue
        template <typename F> requires std::invocable<F&, scrwl::TaskCtx>
        auto nq(F&& f, TaskCtx ctx) -> std::future<std::invoke_result_t<F&, scrwl::TaskCtx>>
        {
            using RetType = std::invoke_result_t<F&, scrwl::TaskCtx>;

            auto task = std::make_shared<std::packaged_task<RetType()>>(
                /* lambda -> */ [f = std::forward<F>(f), ctx]() mutable { return f(ctx); }
            );

            std::future<RetType> res = task->get_future();

            {
                std::unique_lock lock(this->task_mutex);

                // Package that task into a function<void()>
                // TODO: Maybe add parameters? Overkill maybe
                this->task_list.emplace_back([task]() { (*task)(); });
            }

            this->task_condition.notify_one();

            return res;
        }
    };

    struct UrlQueue
    {
        std::mutex mutex;
        std::deque<scrwl::Url> site_list; // TODO: Make this a priority queue
        std::unordered_set<std::uint64_t> visited_hash;

        bool check_visited(std::string_view link);
        std::optional<scrwl::Url> pop();
        void push(scrwl::Url);
    };
}
