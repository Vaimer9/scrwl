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
        std::string link;
        std::string referrer;
        int depth;

        Url(std::string l):
            link(l), referrer(""), depth(0) {}
        Url(std::string l, std::string r, int d):
            link(l), referrer(r), depth(d) {}
    };

    struct HostClient
    {
        httplib::Client client;
        // Multiple threads may hold a host client at once ! 
        // Must make sure that only one of them is able to make a request
        std::mutex mutex; 
        HostClient(const std::string& base_url): client(base_url) {}

        std::optional<std::string> get_data(const std::string);
    };

    struct ClientPool
    {
        std::mutex mutex;
        std::unordered_map<std::string, std::shared_ptr<scrwl::HostClient>> clients;

        // This either gets a host from the map or creates one
        std::shared_ptr<scrwl::HostClient> acquire(const std::string& host);
    };

}
