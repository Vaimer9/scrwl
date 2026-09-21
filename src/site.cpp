#include "../include/scrwl.hpp"

// TODO: Implement LRU-cache deletion
std::shared_ptr<scrwl::HostClient> scrwl::ClientPool::acquire(const std::string& key)
{
    std::unique_lock lock(this->mutex);
    
    // We don't directly emplace the full shared pointer here
    // That is done later inside the if-statement
    // Makes cache hits way more cheaper
    auto [iterator, inserted] = this->clients.try_emplace(key, nullptr);

    if (inserted)
    {
        iterator->second = std::make_shared<scrwl::HostClient>(key);
    }

    return iterator->second;
}

std::string scrwl::HostClient::get_data(std::string url)
{
    std::unique_lock lock(this->mutex);
    return (this->client.Get(url))->body;
}
