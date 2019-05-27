#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <agate_pris/bitonic_sort.hpp>

#include <range/v3/algorithm/is_sorted.hpp>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <vector>

struct s
{
    int a;
    float b;
};

SCENARIO("")
{
    GIVEN("")
    {
        using v_t = std::vector< int >;
        v_t v(v_t::size_type(std::rand() % 10));
        std::generate(std::begin(v), std::end(v), [] {
            return std::rand() % 10;
        });
        auto f = std::less<> {};
        WHEN("")
        {
            agate_pris::bitonic_sort_by(v, f);
            THEN("")
            {
                REQUIRE(ranges::is_sorted(v, f));
            }
        }
    }

    GIVEN("")
    {
        using v_t = std::vector< s >;
        v_t v(v_t::size_type(std::rand() % 10));
        std::generate(std::begin(v), std::end(v), [] {
            s a;
            a.a = std::rand() % 10;
            a.b = std::rand() % 10;
            return a;
        });

        WHEN("")
        {
            auto f = [](auto& lhs, auto& rhs) {
                if (lhs.a < rhs.a)
                    return true;
                if (lhs.a > rhs.a)
                    return false;
                return lhs.b < rhs.b;
            };
            agate_pris::bitonic_sort_by(v, f);
            THEN("")
            {
                REQUIRE(ranges::is_sorted(v, f));
            }
        }
    }
}
