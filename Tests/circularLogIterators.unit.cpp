#include <catch2/catch.hpp>
#include <array>

#include <CircularLogsIterator.hpp>

TEST_CASE("iterator--", "[ITERATOR][operator--]"){
    SECTION("TEST 0-9"){
        std::array<int, 10> arr  {0,1,2,3,4,5,6,7,8,9};
        auto it = LogIterator<int, 10>(arr.data(), 9, false);

        REQUIRE(*it-- == 9);
        REQUIRE(*it-- == 8);
        REQUIRE(*it-- == 7);
        REQUIRE(*it-- == 6);
        REQUIRE(*it-- == 5);
        REQUIRE(*it-- == 4);
        REQUIRE(*it-- == 3);
        REQUIRE(*it-- == 2);
        REQUIRE(*it-- == 1);
        REQUIRE(*it-- == 0);

        REQUIRE(*it-- == 9);
        REQUIRE(*it-- == 8);
        REQUIRE(*it-- == 7);
        REQUIRE(*it-- == 6);
        REQUIRE(*it-- == 5);
        REQUIRE(*it-- == 4);
        REQUIRE(*it-- == 3);
        REQUIRE(*it-- == 2);
        REQUIRE(*it-- == 1);
        REQUIRE(*it-- == 0);

        REQUIRE(*it-- == 9);
        REQUIRE(*it-- == 8);
        REQUIRE(*it-- == 7);
        REQUIRE(*it-- == 6);
        REQUIRE(*it-- == 5);
        REQUIRE(*it-- == 4);
        REQUIRE(*it-- == 3);
        REQUIRE(*it-- == 2);
        REQUIRE(*it-- == 1);
        REQUIRE(*it-- == 0);
    }
}