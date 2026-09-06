#include <iostream>
#include <print>
#include <stdlib.h>
#include <httplib.h>

#include "../include/scrwl.hpp"

int main(int argc, char** argv) {
    scrwl::Site site("https://crawler-test.com/");
    std::vector<std::string> links = site.extract_outlinks();

    scrwl::log_info("Got {} outlinks", links.size());

    for (auto&& x : links)
    {
        std::println("{}", x);
    }

    return 0;
}
