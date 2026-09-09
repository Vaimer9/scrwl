#pragma once

#include "site.hpp"
#include <optional>
#include <string>
#include <vector>
#include <regex>
#include <print>

namespace scrwl
{
    struct HtmlParser
    {
        std::string raw, url;
        std::optional<std::vector<std::string>> outlinks;
        
        HtmlParser(std::string raw, std::string url):
            raw(raw), url(url) {};
        HtmlParser(scrwl::Site& site);
        HtmlParser();
        std::vector<std::string> extract_raw_outlinks();
        std::vector<std::string> extract_outlinks();
    };
}
