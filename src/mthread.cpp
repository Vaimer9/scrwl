#include "../include/mthread.hpp"
#include "../include/scrwl.hpp"
#include <mutex>
#include <optional>

scrwl::ThreadPool::ThreadPool(std::size_t size)
{
    for (std::size_t i = 0; i < size; i++)
    {
        this->threads.emplace_back([this] (std::stop_token /* Not needed */) {
            auto stop_token = this->stop_source.get_token();

            while (!stop_token.stop_requested())
            {
                std::function<void()> task;
                {
                    // Lock the tasklist
                    std::unique_lock lock(this->task_mutex);

                    // Lock is released here and the thread goes to sleep
                    // Gets woken up when notified && this lambda returns true
                    this->cond_var.wait(lock, stop_token, [this] {
                        return this->stop_source.get_token().stop_requested()
                                || !this->task_list.empty();
                    });

                    if (stop_token.stop_requested()) break; // Don't even wait

                    // Get the next task to do now that the thread is woken up
                    task = std::move(this->task_list.front());
                    this->task_list.pop_front();

                    // Release the lock here
                }

                task();
            }
        });
    }
}

template <typename F>
auto scrwl::ThreadPool::nq(F&& f) -> std::future<decltype(f())>
{
    using RetType = decltype(f());

    auto task = std::make_shared<std::packaged_task<RetType()>>(
        // We be perfect forwarding
        std::forward<F>(f)
    );

    std::future<RetType> res = task->get_future();

    {
        std::unique_lock lock(this->task_mutex);

        // Package that task into a function<void()>
        // TODO: Maybe add parameters? Overkill maybe
        this->task_list.emplace([task]() { (*task)(); });
    }

    this->cond_var.notify_one();

    return res;
}
