#include <catch2/catch.hpp>

#include <CircularMemory.hpp>

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

    REQUIRE(ns_CircularMemory::Details::key_lesserThen(static_cast<TestType>(i + f), i)
            != ns_CircularMemory::Details::key_lesserThen(i, static_cast<TestType>(i + f)));
  }

  SECTION("EQUALS") { REQUIRE(false == ns_CircularMemory::Details::key_lesserThen(i, i)); }

  SECTION("LessThen false")
  {
    REQUIRE(ns_CircularMemory::Details::key_lesserThen(i, static_cast<TestType>(i + 1))
            != ns_CircularMemory::Details::key_lesserThen(static_cast<TestType>(i + 1), i));
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

    REQUIRE(ns_CircularMemory::Details::key_lesserThen(static_cast<TestType>(i + f), i)
            != ns_CircularMemory::Details::key_lesserThen(i, static_cast<TestType>(i + f)));
  }

  SECTION("EQUALS") { REQUIRE(false == ns_CircularMemory::Details::key_lesserThen(i, i)); }

  SECTION("LessThen false")
  {
    REQUIRE(ns_CircularMemory::Details::key_lesserThen(i, static_cast<TestType>(i + 1))
            != ns_CircularMemory::Details::key_lesserThen(static_cast<TestType>(i + 1), i));
  }
}

TEST_CASE("IndexedCircularMemory - push_back", "[IndexedCircularMemory][push_back]")
{
  SECTION("push_back 1 element")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(10);
    REQUIRE(logs.at(0).second == 10);
    REQUIRE(logs.r_at(0).second == 10);
    REQUIRE(logs.size() == 1);
    REQUIRE(logs.m_wrappingIndex == 0);
  }
  SECTION("push_back 2 element")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(10);
    logs.push_back(20);

    REQUIRE(logs.at(0).second == 10);
    REQUIRE(logs.r_at(0).second == 20);
    REQUIRE(logs.size() == 2);
    REQUIRE(logs.m_wrappingIndex == 1);
  }
  SECTION("push_back 10 element")
  {
    IndexedCircularMemory<int, 10> logs;
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
  SECTION("push_back 15 element")
  {
    IndexedCircularMemory<int, 10> logs;
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
}

TEST_CASE("IndexedCircularMemory - iterator", "[IndexedCircularMemory][iterator]")
{
  SECTION("0 element -> begin == end")
  {
    IndexedCircularMemory<int, 10> logs;

    REQUIRE(std::begin(logs) == std::end(logs));
    REQUIRE(0 == std::distance(std::begin(logs), std::end(logs)));
  }
  SECTION("0 element -> rbegin == rend")
  {
    IndexedCircularMemory<int, 10> logs;

    REQUIRE(std::rbegin(logs) == std::rend(logs));
    REQUIRE(0 == std::distance(std::rbegin(logs), std::rend(logs)));
  }
  SECTION("1 element -> begin != end")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);

    REQUIRE(std::begin(logs) != std::end(logs));
    REQUIRE(std::next(std::begin(logs), 1) == std::end(logs));
    REQUIRE(1 == std::distance(std::begin(logs), std::end(logs)));
  }
  SECTION("1 element -> rbegin != rend")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);

    REQUIRE(std::rbegin(logs) != std::rend(logs));
    REQUIRE(std::next(std::rbegin(logs), 1) == std::rend(logs));
    REQUIRE(1 == std::distance(std::rbegin(logs), std::rend(logs)));
  }
  SECTION("10 element -> begin != end")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);
    logs.push_back(1);
    logs.push_back(2);
    logs.push_back(3);
    logs.push_back(4);
    logs.push_back(5);
    logs.push_back(6);
    logs.push_back(7);
    logs.push_back(8);
    logs.push_back(9);

    REQUIRE(std::begin(logs) != std::end(logs));
    REQUIRE(std::next(std::begin(logs), 9) != std::end(logs));
    REQUIRE(std::next(std::begin(logs), 10) == std::end(logs));
    REQUIRE(10 == std::distance(std::begin(logs), std::end(logs)));
  }
  SECTION("10 element -> begin != end")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);
    logs.push_back(1);
    logs.push_back(2);
    logs.push_back(3);
    logs.push_back(4);
    logs.push_back(5);
    logs.push_back(6);
    logs.push_back(7);
    logs.push_back(8);
    logs.push_back(9);

    REQUIRE(std::rbegin(logs) != std::rend(logs));
    REQUIRE(std::next(std::rbegin(logs), 9) != std::rend(logs));
    REQUIRE(std::next(std::rbegin(logs), 10) == std::rend(logs));
    REQUIRE(10 == std::distance(std::rbegin(logs), std::rend(logs)));
  }
  SECTION("15 element -> begin != end")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);
    logs.push_back(1);
    logs.push_back(2);
    logs.push_back(3);
    logs.push_back(4);
    logs.push_back(5);
    logs.push_back(6);
    logs.push_back(7);
    logs.push_back(8);
    logs.push_back(9);
    logs.push_back(10);
    logs.push_back(11);
    logs.push_back(12);
    logs.push_back(13);
    logs.push_back(14);

    REQUIRE(std::begin(logs) != std::end(logs));
    REQUIRE(std::next(std::begin(logs), 9) != std::end(logs));
    REQUIRE(std::next(std::begin(logs), 10) == std::end(logs));
    REQUIRE(10 == std::distance(std::begin(logs), std::end(logs)));
  }
  SECTION("15 element -> begin != end")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);
    logs.push_back(1);
    logs.push_back(2);
    logs.push_back(3);
    logs.push_back(4);
    logs.push_back(5);
    logs.push_back(6);
    logs.push_back(7);
    logs.push_back(8);
    logs.push_back(9);
    logs.push_back(10);
    logs.push_back(11);
    logs.push_back(12);
    logs.push_back(13);
    logs.push_back(14);

    REQUIRE(std::rbegin(logs) != std::rend(logs));
    REQUIRE(std::next(std::rbegin(logs), 9) != std::rend(logs));
    REQUIRE(std::next(std::rbegin(logs), 10) == std::rend(logs));
    REQUIRE(10 == std::distance(std::rbegin(logs), std::rend(logs)));
  }
  SECTION("GetDistance from start")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);
    logs.push_back(1);
    logs.push_back(2);
    logs.push_back(3);
    logs.push_back(4);
    logs.push_back(5);
    logs.push_back(6);
    logs.push_back(7);
    logs.push_back(8);
    logs.push_back(9);
    logs.push_back(10);
    logs.push_back(11);
    logs.push_back(12);
    logs.push_back(13);
    logs.push_back(14);

    const auto f = GENERATE(range(0, 20));
    const auto GetDistance = [&logs](const uint8_t t_wantedIndex) ->IndexedCircularMemory<int, 10>::difference_type
    {
      const auto it = logs.get_index(t_wantedIndex);
      if (it != std::end(logs)) {
          return std::distance(std::begin(logs), it) + 1;
      }
      return -1;
    };

    if (f<5) {
      REQUIRE(GetDistance(f) == -1);
    } else if (f>14) {
      REQUIRE(GetDistance(f) == -1);
    } else {
      REQUIRE(GetDistance(f) == f-4);
    }
    REQUIRE(GetDistance(std::begin(logs)->first) == 1);
    REQUIRE(GetDistance(std::rbegin(logs)->first) == 10);

  }
  SECTION("Reverse GetDistance")
  {
    IndexedCircularMemory<int, 10> logs;
    logs.push_back(0);
    logs.push_back(1);
    logs.push_back(2);
    logs.push_back(3);
    logs.push_back(4);
    logs.push_back(5);
    logs.push_back(6);
    logs.push_back(7);
    logs.push_back(8);
    logs.push_back(9);
    logs.push_back(10);
    logs.push_back(11);
    logs.push_back(12);
    logs.push_back(13);
    logs.push_back(14);

    const auto f = GENERATE(range(0, 20));
    const auto GetDistance = [&logs](const uint8_t t_wantedIndex) ->IndexedCircularMemory<int, 10>::difference_type
    {
      const auto it = logs.get_index(t_wantedIndex);
      if (it != std::end(logs)) {
          return std::distance(std::rbegin(logs), std::make_reverse_iterator(it));
      }
      return -1;
    };

    if (f<5) {
      REQUIRE(GetDistance(f) == -1);
    } else if (f>14) {
      REQUIRE(GetDistance(f) == -1);
    } else {
      REQUIRE(GetDistance(f) == 15-f);
    }
    REQUIRE(GetDistance(std::rbegin(logs)->first) == 1);
    REQUIRE(GetDistance(std::begin(logs)->first) == 10);

  }
  SECTION("last index is wrapping_index")
  {
    IndexedCircularMemory<int, 10> logs;
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

    REQUIRE(std::rbegin(logs)->first == logs.m_wrappingIndex);
  }
}

TEST_CASE("IndexedCircularMemory - get_index", "[IndexedCircularMemory][get_index]")
{
  SECTION("get_index")
  {
    IndexedCircularMemory<int, 10> logs;
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
    IndexedCircularMemory<int, 10> logs;
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
    IndexedCircularMemory<int, 10> logs;
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
  }

  SECTION("get_index distance Wrapping Index")
  {
    constexpr uint8_t            somewhatBefore = std::numeric_limits<uint8_t>::max() - 5;
    IndexedCircularMemory<int, 10> logs(250);
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

    REQUIRE(logs.at(0).first < std::numeric_limits<uint8_t>::max());
    REQUIRE(logs.at(9).first > std::numeric_limits<uint8_t>::min());

    const auto f = GENERATE(range(static_cast<uint8_t>(0), static_cast<uint8_t>(9)));

    const auto indexIt = logs.get_index(somewhatBefore + f + 1);
    REQUIRE(indexIt != std::end(logs));
    const auto dist = std::distance(std::begin(logs), indexIt);
    REQUIRE_NOTHROW(logs.at(dist).first == static_cast<uint8_t>(somewhatBefore + f + 1));
    REQUIRE_NOTHROW(logs.at(dist).second == 10 + f * 10);
  }
}