#include <iostream>
#include <print>
#include <stdlib.h>
#include <httplib.h>
#include <utility>

#include "../include/scrwl.hpp"

int main(int argc, char** argv) {
    scrwl::Site site("https://crawler-test.com/");
    scrwl::HtmlParser parser(site);

    auto links = parser.extract_outlinks();

    scrwl::log_info("Got {} links from {}", links.size(), site.url);

    return 0;
}
