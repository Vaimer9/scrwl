#include "../include/mthread.hpp"
#include "../include/scrwl.hpp"

#include <mutex>
#include <optional>

scrwl::ThreadPool::ThreadPool(std::size_t size)
{
    for (std::size_t i = 0; i < size; i++)
    {
        this->threads.emplace_back(/* per-thread function -> */ [this] (std::stop_token) {
            auto s_token = this->stop_source.get_token();

            // Keep doing tasks until you need to stop
            while (!s_token.stop_requested())
            {
                std::function<void()> task;

                {
                    std::unique_lock lock(this->task_mutex);

                    // This releases the lock we made above
                    this->task_condition.wait(lock, s_token, [this] {
                        // This will run when the main thread will try to wake ts up
                        // Will only truly wake up when this returns true
                        return this->stop_source.get_token().stop_requested() || !this->task_list.empty();
                    });

                    // Die if stop is requested
                    if (s_token.stop_requested()) return;

                    // Or run the least recent given task 
                    task = std::move(this->task_list.back());
                    this->task_list.pop_back();

                    this->active_tasks += 1; // Ongoing task now
                }

                task();

                {
                    std::unique_lock lock(this->task_mutex);
                    this->active_tasks -= 1; // Just completed a task

                    if (this->active_tasks == 0 && this->task_list.empty())
                    {
                        // Tell the destructor its okay to die (existentialism)
                        this->drain_condition.notify_all();
                    }
                }
            }
        });
    }
}

// nq = Enqueue
template <typename F> requires std::invocable<F&, scrwl::TaskCtx>
auto scrwl::ThreadPool::nq(F&& f, scrwl::TaskCtx ctx) -> std::future<std::invoke_result_t<F&, scrwl::TaskCtx>>
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

scrwl::ThreadPool::~ThreadPool()
{
    {
        std::unique_lock lock(this->task_mutex);

        // Drain gang
        this->drain_condition.wait(lock, [this] {
            return this->task_list.empty() && this->active_tasks == 0;
        });
    }
    this->stop_source.request_stop();
}
