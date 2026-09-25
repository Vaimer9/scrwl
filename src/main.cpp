#include <iostream>
#include <print>
#include <stdlib.h>
#include <httplib.h>
#include <utility>

#include "../include/scrwl.hpp"

int main(int argc, char** argv) {

    if (argc >= 4) {
        scrwl::Scrawl sc(
            argv[1],
            std::stoi(argv[2]),
            std::stoi(argv[3])
        );
        sc.start();
    }

    return 0;
}
