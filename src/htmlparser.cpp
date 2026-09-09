#include "../include/scrwl.hpp"
#include "../include/htmlparser.hpp"
#include <httplib.h>

static const std::regex href_regex(
    R"(<a\b[^>]*?\s+href\s*=\s*["']([^"']*)["'][^>]*>)",
    std::regex::icase | std::regex::optimize
);

scrwl::HtmlParser::HtmlParser(scrwl::Site& site)
{
    this->url = site.url;
    this->raw = site.get_data();
}

std::vector<std::string> scrwl::HtmlParser::extract_raw_outlinks()
{
    if (!this->outlinks.has_value())
    {
        auto begin = std::sregex_iterator(
            this->raw.begin(), this->raw.end(), href_regex
        );
        auto end = std::sregex_iterator();

        scrwl::log_info("Running regex patterns");

        this->outlinks.emplace(); // Initialize the array if its not already
        for (auto iter = begin; iter != end; ++iter)
        {
            const std::smatch& m = *iter;
            this->outlinks.value().emplace_back(m[1].str());
        }
    }

    return this->outlinks.value_or({});
}


std::vector<std::string> scrwl::HtmlParser::extract_outlinks()
{
    if (!this->outlinks.has_value())
    {
        scrwl::log_warn("No outlinks to sanitize, requesting now.");
        this->extract_raw_outlinks();
    }

    // TODO: Handle relativeurl

    for (int i = 0; i < this->outlinks->size(); i++)
    {
        auto& value = (*this->outlinks)[i];
        // Replace all inlinks with actual usable links
        if (value[0] == '/')
        {
            // Remove the last slash from the original url
            value = this->url.substr(0, this->url.length() - 1) + value;
        }

        // Remove relativeurl in O(1) time
        // I hope this doesn't leak memory lmao
        if (value.starts_with("relativeurl:"))
        {
            value = std::move(this->outlinks->back());
            this->outlinks->pop_back();
        }
    }

    return this->outlinks.value_or({});
}
