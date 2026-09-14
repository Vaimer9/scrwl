#pragma once

#include <optional>
#include <string>
#include <httplib.h>
#include <unordered_map>
#include <vector>
#include <regex>
#include <print>

namespace scrwl
{
    struct Url
    {
        std::string url;
        std::string referrer;
        int depth = 0;
    };

    struct HostClient
    {
        httplib::Client client;
        // Multiple threads may hold a host client at once ! 
        // Must make sure that only one of them is able to make a request
        std::mutex mutex; 
        HostClient(const std::string& base_url): client(base_url) {}
    };

    struct ClientPool
    {
        std::mutex mutex;
        std::unordered_map<std::string, std::shared_ptr<HostClient>> clients;

        // This either gets a host from the map or creates one
        std::shared_ptr<HostClient> acquire(const std::string& host);
    };
}
