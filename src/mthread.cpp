#include "../include/mthread.hpp"

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
    this->task_condition.notify_all();
}

bool scrwl::UrlQueue::check_visited(std::string_view link)
{
    auto hash = std::hash<std::string_view>{}(link);
    auto [_, inserted] = this->visited_hash.insert(hash); // Try inserting and check
                                                          // if it inserted or nah

    return !inserted;
}

void scrwl::UrlQueue::push(scrwl::Url url)
{
    std::unique_lock lock(this->mutex);

    if (!this->check_visited(url.link))
    {
        this->site_list.push_back(url);
    }

    // Tell the main listener thread its time to wake up
    this->subscriber_cv.notify_all(); 
}

// THERE IS NO LOCKING HERE
// CALL wait_and_pop TO AVOID DATA RACE!
std::optional<scrwl::Url> scrwl::UrlQueue::pop()
{
    if (!this->site_list.empty())
    {
        scrwl::Url ret = this->site_list.front();
        this->site_list.pop_front();

        return ret;
    } else {
        return std::nullopt;
    }
}

std::optional<scrwl::Url> scrwl::UrlQueue::wait_and_pop()
{
    std::unique_lock lock(this->mutex);
    this->subscriber_cv.wait(lock, [this]() { return !this->site_list.empty(); });
    return this->pop();
}
