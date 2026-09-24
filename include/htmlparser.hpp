#pragma once

#include "mthread.hpp"
#include "site.hpp"
#include <deque>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <regex>
#include <print>

namespace scrwl
{
    namespace HtmlParser
    {
        void extract_outlinks(const std::string& data, const scrwl::Url& url, scrwl::UrlQueue& url_queue);
        std::pair<std::string, std::string> split_path(const std::string& url);
    };
}
