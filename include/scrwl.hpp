#pragma once

#include "log.hpp"
#include "site.hpp"
#include "htmlparser.hpp"
#include "mthread.hpp"
#include <condition_variable>
#include <thread>

namespace scrwl
{
    // Orchestrator
    struct Scrawl
    {
        // TODO: Change this to a variable later on
        scrwl::ThreadPool tp = scrwl::ThreadPool(4);
        scrwl::ClientPool client_p;
        scrwl::UrlQueue url_q;

        Scrawl(std::string seed);
    };
}
