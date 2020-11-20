#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <container/array.hpp>

#include "../test-util.hpp"

TEST_CASE("can initialise array with a value", "[array]")
{
    auto x = random<int>();
    array<int, 100> a(x);
    for (auto i = 0UL; i < a.size(); i++)
    {
        REQUIRE(x == a[i]);
    }
}

TEST_CASE("can set element by index", "[array]")
{
    auto x = random<int>();
    array<int, 100> a(x);
    // Make sure x and y are different values.
    int y;
    do {
        y = random<int>();
    } while (x == y);

    auto i = random<int>(0, static_cast<int>(a.size()));
    a[i] = y;

    REQUIRE(y == a[i]);
}

TEST_CASE("can get array size", "[array]")
{
    array<int, 100> a;
    REQUIRE(100 == a.size());
}

TEST_CASE("can iterate array using iterators", "[array]")
{
    auto x = random<int>(10, 100);
    array<int, 100> a(x);
    auto it = a.begin();
    auto end = a.end();

    for (; it < end; it++)
    {
        REQUIRE(x == *it);
    }

    REQUIRE(it == end);
}

TEST_CASE("can iterate array using range-based for loop", "[array]")
{
    auto x = random<int>();
    array<int, 100> a(x);
    for (auto y : a)
    {
        REQUIRE(x == y);
    }
}

TEST_CASE("memory is stored contiguously", "[array]")
{
    auto x = random<int>();
    array<int, 100> a(x);
    auto p = &a[0];
    auto end = &a[a.size()];
    for (; p < end; p++)
    {
        REQUIRE(x == *p);
    }

    REQUIRE(p == end);
}

TEST_CASE("can copy an array", "[array]")
{
    array<int, 100> a;
    for (auto i = 0UL; i < a.size(); i++)
    {
        a[i] = random<int>();
    }

    auto b = a;
    for (auto i = 0UL; i < a.size(); i++)
    {
        REQUIRE(a[i] == b[i]);
    }
}

TEST_CASE("can find an element using linear search", "[array]")
{
    array<size_t, 100> a;
    for (auto i = 0UL; i < a.size(); i++)
    {
        a[i] = i;
    }

    auto x = random<size_t>(0, a.size());
    auto y = linear_search(x, a.begin(), a.end());
    REQUIRE(x == *y);
}

TEST_CASE("can find an element using binary search", "[array]")
{
    array<size_t, 100> a;
    for (auto i = 0UL; i < a.size(); i++)
    {
        a[i] = i;
    }

    auto x = random<size_t>(0, a.size());
    auto y = binary_search(x, a.begin(), a.end());
    REQUIRE(x == *y);
}
