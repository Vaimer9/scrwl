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
        inline static std::size_t MAX_DEPTH = 0;
        std::size_t MAX_BREADTH = 0;

        // TODO: Change this to a variable later on
        scrwl::ClientPool client_p;
        scrwl::UrlQueue url_q;
        scrwl::ThreadPool tp = scrwl::ThreadPool(4);

        Scrawl(std::string seed, std::size_t breadth, std::size_t depth);
        void start(); // Main loop
    };
}
