#include <celero/Utilities.h>
#include <iostream>
#include <flat_map>
#include <map>

#include "absl/container/btree_map.h"
#include "faker-cxx/faker.h"

#include "celero/Celero.h"

// int -> int map benchmarks

void loop_number_number_baseline(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = rand();
    celero::DoNotOptimizeAway(key);
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T>
void loop_number_number(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = rand();
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}

void loop_number_string_baseline(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = faker::color::name(faker::Locale::ja_JP);
    celero::DoNotOptimizeAway(key);
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T>
void loop_number_string(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = faker::color::name(faker::Locale::ja_JP);
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}

void loop_string_number_baseline(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = rand();
    celero::DoNotOptimizeAway(key);
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T>
void loop_string_number(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = rand();
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}

void loop_string_string_baseline(int count) {
  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = faker::company::catchPhrase(faker::Locale::ja_JP);
    celero::DoNotOptimizeAway(key);
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T>
void loop_string_string(int count) {
  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = faker::company::catchPhrase(faker::Locale::ja_JP);
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}

CELERO_MAIN

BASELINE(INT_INT_10, Baseline, 100, 10000) {
  loop_number_number_baseline(10);
}

BENCHMARK(INT_INT_10, STD_MAP, 100, 10000) {
  loop_number_number<std::map<int, int>>(10);
}

BENCHMARK(INT_INT_10, STD_FLAT_MAP, 100, 10000) {
  loop_number_number<std::flat_map<int, int>>(10);
}

BENCHMARK(INT_INT_10, ASBEIL_BTREE, 100, 10000) {
  loop_number_number<absl::btree_map<int, int>>(10);
}

BASELINE(INT_INT_50, Baseline, 10, 10000) {
  loop_number_number_baseline(50);
}

BENCHMARK(INT_INT_50 , STD_MAP, 10, 10000) {
  loop_number_number<std::map<int, int>>(50);
}

BENCHMARK(INT_INT_50 , STD_FLAT_MAP, 10, 10000) {
  loop_number_number<std::flat_map<int, int>>(50);
}

BENCHMARK(INT_INT_50 , ASBEIL_BTREE, 10, 10000) {
  loop_number_number<absl::btree_map<int, int>>(50);
}

BASELINE(INT_INT_100, Baseline, 10, 10000) {
  loop_number_number_baseline(100);
}

BENCHMARK(INT_INT_100, STD_MAP, 10, 10000) {
  loop_number_number<std::map<int, int>>(100);
}

BENCHMARK(INT_INT_100 , STD_FLAT_MAP, 10, 10000) {
  loop_number_number<std::flat_map<int, int>>(100);
}

BENCHMARK(INT_INT_100 , ASBEIL_BTREE, 10, 10000) {
  loop_number_number<absl::btree_map<int, int>>(100);
}

BASELINE(INT_INT_1000, Baseline, 10, 10000) {
  loop_number_number_baseline(1000);
}

BENCHMARK(INT_INT_1000, STD_MAP, 10, 10000) {
  loop_number_number<std::map<int, int>>(1000);
}

BENCHMARK(INT_INT_1000, STD_FLAT_MAP, 10, 10000) {
  loop_number_number<std::flat_map<int, int>>(1000);
}

BENCHMARK(INT_INT_1000, ASBEIL_BTREE, 10, 10000) {
  loop_number_number<absl::btree_map<int, int>>(1000);
}

BASELINE(INT_INT_10000, Baseline, 10, 100) {
  loop_number_number_baseline(10000);
}

BENCHMARK(INT_INT_10000, STD_MAP, 10, 100) {
  loop_number_number<std::map<int, int>>(10000);
}

BENCHMARK(INT_INT_10000, STD_FLAT_MAP, 10, 100) {
  loop_number_number<std::flat_map<int, int>>(10000);
}

BENCHMARK(INT_INT_10000, ASBEIL_BTREE, 10, 100) {
  loop_number_number<absl::btree_map<int, int>>(10000);
}

// int -> string map benchmarks

BASELINE(INT_STRING_10, Baseline, 100, 10000) {
  loop_number_string_baseline(10);
}

BENCHMARK(INT_STRING_10, STD_MAP, 100, 10000) {
  loop_number_string<std::map<int, std::string>>(10);
}

BENCHMARK(INT_STRING_10, STD_FLAT_MAP, 100, 10000) {
  loop_number_string<std::flat_map<int, std::string>>(10);
}

BENCHMARK(INT_STRING_10, ASBEIL_BTREE, 100, 10000) {
  loop_number_string<absl::btree_map<int, std::string>>(10);
}

BASELINE(INT_STRING_50, Baseline, 10, 10000) {
  loop_number_string_baseline(50);
}

BENCHMARK(INT_STRING_50 , STD_MAP, 10, 10000) {
  loop_number_string<std::map<int, std::string>>(50);
}

BENCHMARK(INT_STRING_50 , STD_FLAT_MAP, 10, 10000) {
  loop_number_string<std::flat_map<int, std::string>>(50);
}

BENCHMARK(INT_STRING_50 , ASBEIL_BTREE, 10, 10000) {
  loop_number_string<absl::btree_map<int, std::string>>(50);
}

BASELINE(INT_STRING_100, Baseline, 10, 10000) {
  loop_number_string_baseline(100);
}

BENCHMARK(INT_STRING_100, STD_MAP, 10, 10000) {
  loop_number_string<std::map<int, std::string>>(100);
}

BENCHMARK(INT_STRING_100 , STD_FLAT_MAP, 10, 10000) {
  loop_number_string<std::flat_map<int, std::string>>(100);
}

BENCHMARK(INT_STRING_100 , ASBEIL_BTREE, 10, 10000) {
  loop_number_string<absl::btree_map<int, std::string>>(100);
}

BASELINE(INT_STRING_1000, Baseline, 10, 10000) {
  loop_number_string_baseline(1000);
}

BENCHMARK(INT_STRING_1000, STD_MAP, 10, 10000) {
  loop_number_string<std::map<int, std::string>>(1000);
}

BENCHMARK(INT_STRING_1000, STD_FLAT_MAP, 10, 10000) {
  loop_number_string<std::flat_map<int, std::string>>(1000);
}

BENCHMARK(INT_STRING_1000, ASBEIL_BTREE, 10, 10000) {
  loop_number_string<absl::btree_map<int, std::string>>(1000);
}

BASELINE(INT_STRING_10000, Baseline, 10, 10000) {
  loop_number_string_baseline(10000);
}

BENCHMARK(INT_STRING_10000, STD_MAP, 10, 100) {
  loop_number_string<std::map<int, std::string>>(10000);
}

BENCHMARK(INT_STRING_10000, STD_FLAT_MAP, 10, 100) {
  loop_number_string<std::flat_map<int, std::string>>(10000);
}

BENCHMARK(INT_STRING_10000, ASBEIL_BTREE, 10, 100) {
  loop_number_string<absl::btree_map<int, std::string>>(10000);
}

// string -> int map benchmarks

BASELINE(STRING_INT_10, Baseline, 100, 10000) {
  loop_string_number_baseline(10);
}

BENCHMARK(STRING_INT_10, STD_MAP, 100, 10000) {
  loop_string_number<std::map<std::string, int>>(10);
}

BENCHMARK(STRING_INT_10, STD_FLAT_MAP, 100, 10000) {
  loop_string_number<std::flat_map<std::string, int>>(10);
}

BENCHMARK(STRING_INT_10, ASBEIL_BTREE, 100, 10000) {
  loop_string_number<absl::btree_map<std::string, int>>(10);
}

BASELINE(STRING_INT_50, Baseline, 10, 10000) {
  loop_string_number_baseline(50);
}

BENCHMARK(STRING_INT_50 , STD_MAP, 10, 10000) {
  loop_string_number<std::map<std::string, int>>(50);
}

BENCHMARK(STRING_INT_50 , STD_FLAT_MAP, 10, 10000) {
  loop_string_number<std::flat_map<std::string, int>>(50);
}

BENCHMARK(STRING_INT_50 , ASBEIL_BTREE, 10, 10000) {
  loop_string_number<absl::btree_map<std::string, int>>(50);
}

BASELINE(STRING_INT_100, Baseline, 10, 10000) {
  loop_string_number_baseline(100);
}

BENCHMARK(STRING_INT_100, STD_MAP, 10, 10000) {
  loop_string_number<std::map<std::string, int>>(100);
}

BENCHMARK(STRING_INT_100 , STD_FLAT_MAP, 10, 10000) {
  loop_string_number<std::flat_map<std::string, int>>(100);
}

BENCHMARK(STRING_INT_100 , ASBEIL_BTREE, 10, 10000) {
  loop_string_number<absl::btree_map<std::string, int>>(100);
}

BASELINE(STRING_INT_1000, Baseline, 10, 10000) {
  loop_string_number_baseline(1000);
}

BENCHMARK(STRING_INT_1000, STD_MAP, 10, 10000) {
  loop_string_number<std::map<std::string, int>>(1000);
}

BENCHMARK(STRING_INT_1000, STD_FLAT_MAP, 10, 10000) {
  loop_string_number<std::flat_map<std::string, int>>(1000);
}

BENCHMARK(STRING_INT_1000, ASBEIL_BTREE, 10, 10000) {
  loop_string_number<absl::btree_map<std::string, int>>(1000);
}

BASELINE(STRING_INT_10000, Baseline, 10, 100) {
  loop_string_number_baseline(10000);
}

BENCHMARK(STRING_INT_10000, STD_MAP, 10, 100) {
  loop_string_number<std::map<std::string, int>>(10000);
}

BENCHMARK(STRING_INT_10000, STD_FLAT_MAP, 10, 100) {
  loop_string_number<std::flat_map<std::string, int>>(10000);
}

BENCHMARK(STRING_INT_10000, ASBEIL_BTREE, 10, 100) {
  loop_string_number<absl::btree_map<std::string, int>>(10000);
}

// string -> string map benchmarks

BASELINE(STRING_STRING_10, Baseline, 100, 10000) {
  loop_string_string_baseline(10);
}

BENCHMARK(STRING_STRING_10, STD_MAP, 100, 10000) {
  loop_string_string<std::map<std::string, std::string>>(10);
}

BENCHMARK(STRING_STRING_10, STD_FLAT_MAP, 100, 10000) {
  loop_string_string<std::flat_map<std::string, std::string>>(10);
}

BENCHMARK(STRING_STRING_10, ASBEIL_BTREE, 100, 10000) {
  loop_string_string<absl::btree_map<std::string, std::string>>(10);
}

BASELINE(STRING_STRING_50, Baseline, 10, 10000) {
  loop_string_string_baseline(50);
}

BENCHMARK(STRING_STRING_50 , STD_MAP, 10, 10000) {
  loop_string_string<std::map<std::string, std::string>>(50);
}

BENCHMARK(STRING_STRING_50 , STD_FLAT_MAP, 10, 10000) {
  loop_string_string<std::flat_map<std::string, std::string>>(50);
}

BENCHMARK(STRING_STRING_50 , ASBEIL_BTREE, 10, 10000) {
  loop_string_string<absl::btree_map<std::string, std::string>>(50);
}

BASELINE(STRING_STRING_100, Baseline, 10, 10000) {
  loop_string_string_baseline(100);
}

BENCHMARK(STRING_STRING_100, STD_MAP, 10, 10000) {
  loop_string_string<std::map<std::string, std::string>>(100);
}

BENCHMARK(STRING_STRING_100 , STD_FLAT_MAP, 10, 10000) {
  loop_string_string<std::flat_map<std::string, std::string>>(100);
}

BENCHMARK(STRING_STRING_100 , ASBEIL_BTREE, 10, 10000) {
  loop_string_string<absl::btree_map<std::string, std::string>>(100);
}

BASELINE(STRING_STRING_1000, Baseline, 10, 10000) {
  loop_string_string_baseline(1000);
}

BENCHMARK(STRING_STRING_1000, STD_MAP, 10, 10000) {
  loop_string_string<std::map<std::string, std::string>>(1000);
}

BENCHMARK(STRING_STRING_1000, STD_FLAT_MAP, 10, 10000) {
  loop_string_string<std::flat_map<std::string, std::string>>(1000);
}

BENCHMARK(STRING_STRING_1000, ASBEIL_BTREE, 10, 10000) {
  loop_string_string<absl::btree_map<std::string, std::string>>(1000);
}

BASELINE(STRING_STRING_10000, Baseline, 10, 100) {
  loop_string_string_baseline(10000);
}

BENCHMARK(STRING_STRING_10000, STD_MAP, 10, 100) {
  loop_string_string<std::map<std::string, std::string>>(10000);
}

BENCHMARK(STRING_STRING_10000, STD_FLAT_MAP, 10, 100) {
  loop_string_string<std::flat_map<std::string, std::string>>(10000);
}

BENCHMARK(STRING_STRING_10000, ASBEIL_BTREE, 10, 100) {
  loop_string_string<absl::btree_map<std::string, std::string>>(10000);
}
