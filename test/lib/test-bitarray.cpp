#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <container/bitarray.hpp>

#include "../test-util.hpp"

TEST_CASE("all bits are initially 0", "[bitarray]")
{
    bitarray<80> set;
    for (auto i = 0UL; i < set.size(); i++)
    {
        REQUIRE(set[i] == false);
    }
}

TEST_CASE("all bits are initially 1", "[bitarray]")
{
    bitarray<80> set(true);
    for (auto i = 0UL; i < set.size(); i++)
    {
        REQUIRE(set[i] == true);
    }
}

TEST_CASE("can set a bit using set method", "[bitarray]")
{
    bitarray<200> set;
    set.set(100);
    REQUIRE(set[100] == true);
}

TEST_CASE("can set a bit using index", "[bitarray]")
{
    bitarray<200> set;
    set[100] = true;
    REQUIRE(set[100] == true);
}

TEST_CASE("can clear a bit using clear method", "[bitarray]")
{
    bitarray<200> set;
    set.clear(100);
    REQUIRE(set[100] == false);
}

TEST_CASE("can clear a bit using index", "[bitarray]")
{
    bitarray<200> set(true);
    set[100] = false;
    REQUIRE(set[100] == false);
}

TEST_CASE("can toggle a bit using toggle method", "[bitarray]")
{
    bitarray<200> set;
    set.toggle(100);
    REQUIRE(set[100] == true);
    set.toggle(100);
    REQUIRE(set[100] == false);
}

TEST_CASE("can get popcount", "[bitarray]")
{
    bitarray<200> set;
    set.set(1);
    set.set(10);
    set.set(100);
    REQUIRE(set.popcount() == 3);
}

TEST_CASE("can find first set bit", "[bitarray]")
{
    bitarray<200> set;
    set.set(100);
    REQUIRE(*set.find() == 100);
}

TEST_CASE("can find second set bit", "[bitarray]")
{
    bitarray<200> set;
    set.set(100);
    set.set(150);
    REQUIRE(*set.find(set.find()) == 100);
}

TEST_CASE("can find first clear bit", "[bitarray]")
{
    bitarray<200> set;
    set.clear(100);
    REQUIRE(*set.find(false) == 100);
}

TEST_CASE("can find second clear bit", "[bitarray]")
{
    bitarray<200> set(true);
    set.clear(100);
    set.clear(150);
    REQUIRE(*set.find(set.find(false), false) == 150);
}

TEST_CASE("can set multiple bits", "[bitarray]")
{
    bitarray<100> set;
    set.set(7, 90);
    REQUIRE(set.popcount() == 90);              // There should be 90 set bits.
    REQUIRE(*set.find() == 7);                  // The first set bit should be at position 7.
    REQUIRE(*set.find(set.at(7), false) == 97); // All the bits up to position 97 should be set.
}

TEST_CASE("can clear multiple bits", "[bitarray]")
{
    bitarray<100> set(true);
    REQUIRE(set.popcount() == 100);
    set.clear(7, 90);
    REQUIRE(set.popcount() == 10);       // There should be 10 set/90 clear bits.
    REQUIRE(*set.find(false) == 7);      // The first clear bit should be at position 7.
    REQUIRE(*set.find(set.at(7)) == 97); // All the bits up to position 97 should be clear.
}
