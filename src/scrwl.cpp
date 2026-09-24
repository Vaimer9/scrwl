#include "../include/scrwl.hpp"

std::vector<std::string> callback(scrwl::TaskCtx ctx)
{
    if (auto res = ctx.host->get_data("/"))
    {
        return scrwl::HtmlParser::extract_outlinks(res.value(), ctx.url.link);
    } else {
        return {};
    }
}

scrwl::Scrawl::Scrawl(std::string seed)
{
    while (true) {
        if (auto url = this->url_q.wait_and_pop())
        {
            this->tp.nq(
                callback,
                scrwl::TaskCtx { url.value(), this->client_p.acquire(url.value().link) }
            );
        }
    }
}
