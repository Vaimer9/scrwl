#include "../include/mthread.hpp"
#include "../include/scrwl.hpp"
#include <mutex>
#include <optional>
#include <contracts>

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

                    // Or run the latest given task 
                    task = std::move(this->task_list.front());
                    this->task_list.pop_back();

                    this->active_tasks += 1; // Ongoing task now
                }

                task();

                {
                    std::unique_lock lock(this->task_mutex);
                    this->active_tasks -= 1; // Just completed a task

                    if (this->active_tasks == 0 && this->task_list.empty())
                    {
                        this->drain_condition.notify_all(); // Tell the destructor its okay to die (existentialism)
                    }
                }
            }
        });
    }
}

template <typename F>
auto scrwl::ThreadPool::nq(F&& f) -> std::future<decltype(f())>
{
    using RetType = decltype(f());

    auto task = std::make_shared<std::packaged_task<RetType()>>(
        // Perfect forwarding!
        std::forward<F>(f)
    );

    std::future<RetType> res = task->get_future();

    {
        std::unique_lock lock(this->task_mutex);

        // Package that task into a function<void()>
        // TODO: Maybe add parameters? Overkill maybe
        this->task_list.emplace([task]() { (*task)(); });
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
