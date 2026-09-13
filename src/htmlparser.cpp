#include "../include/scrwl.hpp"
#include "../include/htmlparser.hpp"
#include <httplib.h>

static const std::regex href_regex(
    R"(<a\b[^>]*?\s+href\s*=\s*["']([^"']*)["'][^>]*>)",
    std::regex::icase | std::regex::optimize
);

std::vector<std::string> scrwl::HtmlParser::extract_raw_outlinks(std::string data)
{
    std::vector<std::string> outlinks;

    auto begin = std::sregex_iterator(
        data.begin(), data.end(), href_regex
    );
    auto end = std::sregex_iterator();

    scrwl::log_info("Running regex patterns");

    for (auto iter = begin; iter != end; ++iter)
    {
        const std::smatch& m = *iter;
        outlinks.emplace_back(m[1].str());
    }

    return outlinks;
}


std::vector<std::string> scrwl::HtmlParser::extract_outlinks(std::string data, std::string url)
{
    std::vector<std::string> raw = scrwl::HtmlParser::extract_raw_outlinks(data);

    // TODO: Handle relativeurl

    for (int i = 0; i < raw.size(); i++)
    {
        auto& value = raw[i];
        // Replace all inlinks with actual usable links
        if (value[0] == '/')
        {
            // Remove the last slash from the original url
            value = url.substr(0, url.length() - 1) + value;
        }

        // Remove relativeurl in O(1) time
        // I hope this doesn't leak memory lmao
        if (value.starts_with("relativeurl:"))
        {
            value = std::move(raw.back());
            raw.pop_back();
        }
    }

    return raw;
}
