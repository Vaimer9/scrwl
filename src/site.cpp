#include "../include/scrwl.hpp"

static const std::regex href_regex(
    R"(<a\b[^>]*?\s+href\s*=\s*["']([^"']*)["'][^>]*>)",
    std::regex::icase | std::regex::optimize
);

std::string scrwl::Site::get_data()
{
    if (!this->data.has_value())
    {
        if (auto result = this->client.Get("/"))
        {
            scrwl::log_info("Recieved data from {}", this->url);
            this->data = result->body;
        }
    }
    return this->data.value_or("NULL");
}

std::vector<std::string> scrwl::Site::extract_raw_outlinks()
{
    if (!this->outlinks.has_value())
    {
        if (!this->data.has_value())
        {
            scrwl::log_info("Couldn't find data, pinging now");
            this->get_data();
        }

        auto begin = std::sregex_iterator(
            this->data.value().begin(),
            this->data.value().end(),
            href_regex
        );
        auto end = std::sregex_iterator();
        scrwl::log_info("Running regex patterns");

        this->outlinks.emplace();
        for (auto iter = begin; iter != end; ++iter)
        {
            const std::smatch& m = *iter;
            this->outlinks.value().emplace_back(m[1].str());
        }
    }

    return this->outlinks.value_or({});
}

std::vector<std::string> scrwl::Site::extract_outlinks()
{
    if (!this->outlinks.has_value())
    {
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
