#include "../include/scrwl.hpp"

void callback(scrwl::TaskCtx ctx)
{
    scrwl::log_info("Started the task with {}", ctx.url.link);
    if (auto res = ctx.host->get_data("/"))
    {
         scrwl::HtmlParser::extract_outlinks(
             res.value(),
             std::ref(ctx.url),
             std::ref(ctx.url_q)
         );
    } else {
        scrwl::log_err("Didn't run with {}", ctx.url.link);
    }

    scrwl::log_info("Did the task with {}", ctx.url.link);
}

scrwl::Scrawl::Scrawl(std::string seed, std::size_t depth)
{
    scrwl::Scrawl::MAX_DEPTH = depth;
    this->url_q.push(scrwl::Url(seed));
}

void scrwl::Scrawl::start()
{
    auto i = 0;
    while (true) {
        if (auto url = this->url_q.wait_and_pop())
        {
            auto [host, link] = scrwl::HtmlParser::split_path(url.value().link);

            this->tp.nq(
                callback,
                scrwl::TaskCtx {
                    url.value(),
                    this->client_p.acquire(host),
                    std::ref(this->url_q)
                }
            );

            if (url.value().depth > scrwl::Scrawl::MAX_DEPTH) break;
        }
    }
}
