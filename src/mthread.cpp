#include "../include/mthread.hpp"
#include "../include/scrwl.hpp"
#include <mutex>
#include <optional>

template <typename F>
scrwl::ThreadPool::ThreadPool(std::size_t n, F func)
{
    this->threads.reserve(n);
    for (int i = 0; i < n; i++)
        this->threads.emplace_back(func);
}

std::optional<scrwl::QueueType> scrwl::Queue::pop()
{

    // Lock the queue
    std::unique_lock<std::mutex> lock(this->mutex);

    // Sit back while the queue is empty
    c_var.wait(lock, [&] { return !this->queue.empty(); });

    if (queue.empty())
    {
        return std::nullopt;    
    } else
    {
        // Get and delete the first element
        scrwl::QueueType item = std::move(this->queue.front());
        this->queue.pop_front(); 

        return item; // Implicit conversion my beloved <3
    }
}

void scrwl::Queue::push(scrwl::QueueType item)
{
    // Lock the queue
    std::unique_lock<std::mutex> lock(this->mutex);
    this->queue.push_back(std::move(item));
    lock.unlock();

    this->c_var.notify_one(); // Let a thread know there is more stuff
}
