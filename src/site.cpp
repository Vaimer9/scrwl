#include "../include/site.hpp"

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
            this->data = result->body;
        }
    }
    return this->data.value_or("NULL");
}

std::vector<std::string> scrwl::Site::extract_outlinks()
{
    if (!this->outlinks.has_value())
    {
        if (!this->data.has_value())
        {
            this->get_data();
        }

        auto begin = std::sregex_iterator(
            this->data.value().begin(),
            this->data.value().end(),
            href_regex
        );
        auto end = std::sregex_iterator();
        std::println("Works");

        this->outlinks.emplace();
        for (auto iter = begin; iter != end; ++iter)
        {
            const std::smatch& m = *iter;
            this->outlinks.value().emplace_back(m[1].str());
        }
    }

    return this->outlinks.value_or({});
}
