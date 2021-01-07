#include <catch2/catch.hpp>

#include <CircularLogs.hpp>

// FIXME Fails to compile with clang-9.0.0 (x86-pc-windows-msvc)
TEMPLATE_TEST_CASE("KeyComparasion", "[KeyComparasion][template]", uint8_t, int8_t)
{
  const auto i = GENERATE(take(5, random(std::numeric_limits<TestType>::min(), std::numeric_limits<TestType>::max())));

  SECTION("LessThen")
  {
    const auto f =
        GENERATE(take(5, random(static_cast<TestType>(1), static_cast<TestType>((std::numeric_limits<TestType>::max() - std::numeric_limits<TestType>::min()) / 2))));

    REQUIRE(ns_CircularLogs::Details::key_lesserThen(static_cast<TestType>(i + f), i)
            != ns_CircularLogs::Details::key_lesserThen(i, static_cast<TestType>(i + f)));
  }

  SECTION("EQUALS") { REQUIRE(false == ns_CircularLogs::Details::key_lesserThen(i, i)); }

  SECTION("LessThen false")
  {
    REQUIRE(ns_CircularLogs::Details::key_lesserThen(i, static_cast<TestType>(i + 1))
            != ns_CircularLogs::Details::key_lesserThen(static_cast<TestType>(i + 1), i));
  }
}

TEMPLATE_TEST_CASE("KeyComparasion - All", "[.KeyComparasion][template][All]", uint8_t, int8_t)
{
  const auto i = GENERATE(range(std::numeric_limits<TestType>::min(), std::numeric_limits<TestType>::max()));

  SECTION("LessThen")
  {
    const auto f =
        GENERATE(range(static_cast<TestType>(1), static_cast<TestType>((std::numeric_limits<TestType>::max() - std::numeric_limits<TestType>::min()) / 2)));

    REQUIRE(ns_CircularLogs::Details::key_lesserThen(static_cast<TestType>(i + f), i)
            != ns_CircularLogs::Details::key_lesserThen(i, static_cast<TestType>(i + f)));
  }

  SECTION("EQUALS") { REQUIRE(false == ns_CircularLogs::Details::key_lesserThen(i, i)); }

  SECTION("LessThen false")
  {
    REQUIRE(ns_CircularLogs::Details::key_lesserThen(i, static_cast<TestType>(i + 1))
            != ns_CircularLogs::Details::key_lesserThen(static_cast<TestType>(i + 1), i));
  }
}