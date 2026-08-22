#include <iostream>
#include <stdlib.h>
#include <httplib.h>

#include "../include/site.hpp"

int main(int argc, char** argv) {
    scrwl::Site site("https://crawler-test.com/");

    // std::cout << site.get_data() << std::endl;
    
    site.get_data();
    for (auto&& x : site.extract_outlinks())
    {
        std::cout << x << std::endl;
    }

    return 0;
}
