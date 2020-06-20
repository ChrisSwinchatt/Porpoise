#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <container/vector.hpp>

#include "../test-util.hpp"

TEST_CASE("can initialise vector with a value", "[vector]")
{
    auto x = random<int>();
    test_allocator<int> allocator;
    vector<int> a(100, x, allocator);
    for (auto i = 0UL; i < a.size(); i++)
    {
        REQUIRE(x == a[i]);
    }
}

TEST_CASE("can set element by index", "[vector]")
{
    auto x = random<int>();
    test_allocator<int> allocator;
    vector<int> a(100, x, allocator);
    // Make sure x and y are different values.
    int y;
    do {
        y = random<int>();
    } while (x == y);

    auto i = random<int>(0, static_cast<int>(a.size()));
    a[i] = y;

    REQUIRE(y == a[i]);
}

TEST_CASE("can get vector size", "[vector]")
{
    test_allocator<int> allocator;
    vector<int> a(100, 0, allocator);
    REQUIRE(100 == a.size());
}

TEST_CASE("can iterate vector using iterators", "[vector]")
{
    auto x = random<int>(10, 100);
    test_allocator<int> allocator;
    vector<int> a(100, x, allocator);
    auto it = a.begin();
    auto end = a.end();

    for (; it < end; it++)
    {
        REQUIRE(x == *it);
    }

    REQUIRE(it == end);
}

TEST_CASE("can iterate vector using range-based for loop", "[vector]")
{
    auto x = random<int>();
    test_allocator<int> allocator;
    vector<int> a(100, x, allocator);
    for (auto y : a)
    {
        REQUIRE(x == y);
    }
}

TEST_CASE("memory is stored contiguously", "[vector]")
{
    auto x = random<int>();
    test_allocator<int> allocator;
    vector<int> a(100, x, allocator);
    auto p = &a[0];
    auto end = p + a.size();
    for (; p < end; p++)
    {
        REQUIRE(x == *p);
    }

    REQUIRE(p == end);
}

TEST_CASE("can copy a vector", "[vector]")
{
    test_allocator<int> allocator;
    vector<int> a(100, allocator);
    for (auto i = 0UL; i < a.size(); i++)
    {
        a[i] = random<int>();
    }

    auto b = a;
    for (auto i = 0UL; i < b.size(); i++)
    {
        REQUIRE(a[i] == b[i]);
    }
}

TEST_CASE("can find an element using linear search", "[vector]")
{
    test_allocator<size_t> allocator;
    vector<size_t> a(100, allocator);
    for (auto i = 0UL; i < a.size(); i++)
    {
        a[i] = i;
    }

    auto x = random<size_t>(0, a.size());
    auto y = linear_search(x, a.begin(), a.end());
    REQUIRE(x == *y);
}

TEST_CASE("can find an element using binary search", "[vector]")
{
    test_allocator<size_t> allocator;
    vector<size_t> a(100, allocator);
    for (auto i = 0UL; i < a.size(); i++)
    {
        a[i] = i;
    }

    auto x = random<size_t>(0, a.size());
    auto y = binary_search(x, a.begin(), a.end());
    REQUIRE(x == *y);
}
