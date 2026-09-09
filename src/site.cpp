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
