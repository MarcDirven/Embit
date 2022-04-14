#define CATCH_CONFIG_MAIN

#include <array>
#include <catch2/catch.hpp>
#include <embit/CString.hpp>
#include <embit/Chain.hpp>
#include <embit/Concat.hpp>
#include <embit/Core.hpp>
#include <embit/Filter.hpp>
#include <embit/Map.hpp>
#include <embit/Take.hpp>
#include <embit/Join.hpp>

TEST_CASE("test") {
    const char* str = "helloworld!";
    auto strView = embit::cstring(str);
    auto joined = embit::join(strView, " ");
    for (char c : joined) {
        std::cout << c << std::flush;
    }
}