#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <algorithm.hpp>

#include "../test-util.hpp"

// Should be an even number for overlap tests to work.
constexpr size_t ARRAY_SIZE = 100;

TEST_CASE("should copy forwards", "[copy]")
{
    char* a = new char[ARRAY_SIZE];
    char* b = new char[ARRAY_SIZE];
    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        a[i] = i;
    }

    copy_forwards(a, a + ARRAY_SIZE, b);

    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        REQUIRE(a[i] == b[i]);
    }

    delete[] a;
    delete[] b;
}

TEST_CASE("should copy backwards", "[copy]")
{
    char* a = new char[ARRAY_SIZE];
    char* b = new char[ARRAY_SIZE];
    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        a[i] = i;
    }

    copy_backwards(a, a + ARRAY_SIZE, b);

    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        REQUIRE(a[i] == b[i]);
    }

    delete[] a;
    delete[] b;
}

TEST_CASE("should copy with no overlap", "[copy]")
{
    char* a = new char[ARRAY_SIZE];
    char* b = new char[ARRAY_SIZE];
    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        a[i] = i;
    }

    safe_copy(a, a + ARRAY_SIZE, b);

    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        REQUIRE(a[i] == b[i]);
    }

    delete[] a;
    delete[] b;
}

TEST_CASE("should copy with overlap at beginning", "[copy]")
{
    char* a = new char[ARRAY_SIZE];
    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        a[i] = i;
    }

    safe_copy(a, a + ARRAY_SIZE/2, a + ARRAY_SIZE/2);

    for (auto i = 0UL; i < ARRAY_SIZE/2; i++) {
        REQUIRE(a[i] == a[i + ARRAY_SIZE/2]);
    }

    delete[] a;
}

TEST_CASE("should copy with overlap at end", "[copy]")
{
    char* a = new char[ARRAY_SIZE];
    for (auto i = 0UL; i < ARRAY_SIZE; i++) {
        a[i] = i;
    }

    safe_copy(a + ARRAY_SIZE/2, a + ARRAY_SIZE, a);

    for (auto i = 0UL; i < ARRAY_SIZE/2; i++) {
        REQUIRE(a[i] == a[i + ARRAY_SIZE/2]);
    }

    delete[] a;
}
