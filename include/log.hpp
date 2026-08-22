#pragma once

#include <string>

namespace scrwl
{
    void log_success(std::string msg);
    void log_warn(std::string msg);
    void log_err(std::string msg);
    void log_panic(std::string msg);
}
