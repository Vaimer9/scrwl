#pragma once

#include <cstdio>
#include <print>
#include <format>
#include <utility>

namespace scrwl
{
    template<typename... Args>
    void log_info(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println(
            "\033[1;32m[INFO]: \033[0m{}",
            std::format(fmt, std::forward<Args>(args)...)
        );
    }

    template<typename... Args>
    void log_warn(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println(
            "\033[1;33m[WARN]: \033[0m{}",
            std::format(fmt, std::forward<Args>(args)...)
        );
    }

    template<typename... Args>
    void log_err(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println(
            stderr,
            "\033[1;31m[ERR]: \033[0m{}",
            std::format(fmt, std::forward<Args>(args)...)
        );
    }

    template<typename... Args>
    void log_panic(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println(
            stderr,
            "\033[1;31m[PANIC]: \033[0m{}",
            std::format(fmt, std::forward<Args>(args)...)
        );
        std::exit(-1);
    }
}
