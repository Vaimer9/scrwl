#include <iostream>
#include <print>
#include <stdlib.h>
#include <httplib.h>
#include <utility>

#include "../include/scrwl.hpp"

int main(int argc, char** argv) {
    scrwl::Scrawl sc("https://crawler-test.com/", 50);
    sc.start();

    return 0;
}
