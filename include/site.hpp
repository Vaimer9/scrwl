#pragma once

#include <optional>
#include <string>
#include <httplib.h>
#include <vector>
#include <regex>
#include <print>

namespace scrwl
{
    struct Site
    {
        std::string url;
        httplib::Client client;
        std::optional<std::string> data;

        Site(std::string url): url(url), client(url) {}

        std::string get_data();
    };
}
