#include <iostream>
#include <print>
#include <stdlib.h>
#include <httplib.h>
#include <utility>

#include "../include/scrwl.hpp"

std::string callback(scrwl::TaskCtx ctx)
{
    
}

int main(int argc, char** argv) {
    scrwl::UrlQueue q;    

    // Simulate queue
    q.push(scrwl::Url("one"));
    q.push(scrwl::Url("two"));
    q.push(scrwl::Url("three"));
    q.push(scrwl::Url("one"));

    scrwl::ThreadPool tp(4);
    scrwl::ClientPool cp;

    scrwl::Url url("https://crawler-test.com/");
    std::shared_ptr<scrwl::HostClient> hc = cp.acquire(url.link);

    std::future<std::string> ft = tp.nq(callback, scrwl::TaskCtx { scrwl::Url {""}, scrwl::HostClient { "" } });
    
    ft.get();

    return 0;
}
