#include "../include/scrwl.hpp"
#include "../include/htmlparser.hpp"
#include <httplib.h>

static const std::regex href_regex(
    R"(<a\b[^>]*?\s+href\s*=\s*["']([^"']*)["'][^>]*>)",
    std::regex::icase | std::regex::optimize
);

void scrwl::HtmlParser::extract_outlinks(
    const std::string& data,
    const scrwl::Url& url,
    scrwl::UrlQueue& url_queue
) {
    auto begin = std::sregex_iterator(data.begin(), data.end(), href_regex);
    auto end   = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        const std::smatch& m = *iter;
        std::string value = m[1].str();

        // Skip empty matches so value.front() is safe
        if (value.empty())
            continue;

        // Skip relativeurl: entries
        if (value.starts_with("relativeurl:"))
            continue;

        // Expand root-relative links
        if (value.front() == '/')
        {
            // Strip the last char of the original url so we don't double up slashes
            auto [origin, _] = scrwl::HtmlParser::split_path(url.link);
            value = origin + value;
        }

        // TODO
        if (value.empty()) continue;
        if (value.starts_with('#')) continue;
        if (value.starts_with("mailto:")) continue;
        if (value.starts_with("javascript:")) continue;
        if (value.starts_with("data:")) continue;
        if (value.starts_with("relativeurl:")) continue;

        auto created = scrwl::Url(value, url.link, url.depth + 1);

        url_queue.push(std::move(created));
    }
}

std::pair<std::string, std::string> scrwl::HtmlParser::split_path(const std::string& url)
{
    auto scheme_end = url.find("://");
    auto host_start = scheme_end + 3;
    auto path_start = url.find('/', host_start);

    if (path_start == std::string::npos)
        return { url, "/" };
    else
        return { url.substr(0, path_start), url.substr(path_start) };
}
