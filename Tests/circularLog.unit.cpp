#include <catch2/catch.hpp>

#include <CircularLogs.hpp>

// FIXME Fails to compile with clang-9.0.0 (x86-pc-windows-msvc)
TEMPLATE_TEST_CASE("KeyComparasion", "[KeyComparasion][template]", uint8_t, int8_t)
{
  const auto i = GENERATE(take(5, random(std::numeric_limits<TestType>::min(), std::numeric_limits<TestType>::max())));

  SECTION("LessThen")
  {
    const auto f = GENERATE(
        take(5, random(static_cast<TestType>(1),
                       static_cast<TestType>(
                           (std::numeric_limits<TestType>::max() - std::numeric_limits<TestType>::min()) / 2))));

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
    const auto f = GENERATE(range(
        static_cast<TestType>(1),
        static_cast<TestType>((std::numeric_limits<TestType>::max() - std::numeric_limits<TestType>::min()) / 2)));

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

TEST_CASE("IndexedCircularLogs", "[IndexedCircularLogs]")
{
  IndexedCircularLogs<int, 10> logs;

  SECTION("PushBack 1 element")
  {
    logs.push_back(10);
    REQUIRE(logs.at(0).second == 10);
    REQUIRE(logs.r_at(0).second == 10);
    REQUIRE(logs.size() == 1);
    REQUIRE(logs.m_wrappingIndex == 0);
  }
  SECTION("PushBack 2 element")
  {
    logs.push_back(10);
    logs.push_back(20);

    REQUIRE(logs.at(0).second == 10);
    REQUIRE(logs.r_at(0).second == 20);
    REQUIRE(logs.size() == 2);
    REQUIRE(logs.m_wrappingIndex == 1);
  }
  SECTION("PushBack 10 element")
  {
    logs.push_back(10);
    logs.push_back(20);
    logs.push_back(30);
    logs.push_back(40);
    logs.push_back(50);
    logs.push_back(60);
    logs.push_back(70);
    logs.push_back(80);
    logs.push_back(90);
    logs.push_back(100);

    REQUIRE(logs.at(0).second == 10);
    REQUIRE(logs.r_at(0).second == 100);
    REQUIRE(logs.size() == 10);
    REQUIRE(logs.m_wrappingIndex == 9);
  }
  SECTION("PushBack 15 element")
  {
    logs.push_back(10);
    logs.push_back(20);
    logs.push_back(30);
    logs.push_back(40);
    logs.push_back(50);
    logs.push_back(60);
    logs.push_back(70);
    logs.push_back(80);
    logs.push_back(90);
    logs.push_back(100);
    logs.push_back(110);
    logs.push_back(120);
    logs.push_back(130);
    logs.push_back(140);
    logs.push_back(150);

    REQUIRE(logs.at(0).second == 60);
    REQUIRE(logs.r_at(0).second == 150);
    REQUIRE(logs.size() == 10);
    REQUIRE(logs.m_wrappingIndex == 14);
  }

  SECTION("get_index")
  {
    logs.push_back(10);
    logs.push_back(20);
    logs.push_back(30);
    logs.push_back(40);
    logs.push_back(50);
    logs.push_back(60);
    logs.push_back(70);
    logs.push_back(80);
    logs.push_back(90);
    logs.push_back(100);
    logs.push_back(110);
    logs.push_back(120);
    logs.push_back(130);
    logs.push_back(140);
    logs.push_back(150);

    REQUIRE(logs.get_index(0) == std::end(logs));
    const auto IndexIt_7 = logs.get_index(7);
    REQUIRE(IndexIt_7 != std::end(logs));
    REQUIRE(IndexIt_7->first == 7);
    REQUIRE(IndexIt_7->second == 80);
  }

  SECTION("get_index distance")
  {
    logs.push_back(10);
    logs.push_back(20);
    logs.push_back(30);
    logs.push_back(40);
    logs.push_back(50);
    logs.push_back(60);
    logs.push_back(70);
    logs.push_back(80);
    logs.push_back(90);
    logs.push_back(100);
    logs.push_back(110);
    logs.push_back(120);
    logs.push_back(130);
    logs.push_back(140);
    logs.push_back(150);

    const auto indexIt = logs.get_index(7);
    REQUIRE(indexIt != std::end(logs));
    const auto dist = std::distance(std::begin(logs), indexIt);
    REQUIRE(logs.at(dist).first == 7);
  }

  SECTION("get_index distance invalid")
  {
    logs.push_back(10);
    logs.push_back(20);
    logs.push_back(30);
    logs.push_back(40);
    logs.push_back(50);
    logs.push_back(60);
    logs.push_back(70);
    logs.push_back(80);
    logs.push_back(90);
    logs.push_back(100);
    logs.push_back(110);
    logs.push_back(120);
    logs.push_back(130);
    logs.push_back(140);
    logs.push_back(150);

    const auto indexIt = logs.get_index(20);
    REQUIRE(indexIt == std::end(logs));
    const auto dist = std::distance(std::begin(logs), indexIt);
    REQUIRE(logs.at(dist).first == 7);
  }
}