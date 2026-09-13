#pragma once

#include "site.hpp"
#include <optional>
#include <string>
#include <vector>
#include <regex>
#include <print>

namespace scrwl
{
    namespace HtmlParser
    {
        std::vector<std::string> extract_raw_outlinks(std::string data);
        std::vector<std::string> extract_outlinks(std::string data, std::string url);
    };
}
