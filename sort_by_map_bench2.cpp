#include <iostream>
#include <flat_map>
#include <map>
#include <random>
#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>

#include "absl/container/btree_map.h"
#include "faker-cxx/faker.h"

#include "celero/Celero.h"

template<class>
struct is_std_flat_map : std::false_type {};

template<class Key, class Mapped, class Compare, class KeyContainer, class MappedContainer>
struct is_std_flat_map<std::flat_map<Key, Mapped, Compare, KeyContainer, MappedContainer>>
  : std::true_type {};

template<class T>
inline constexpr bool is_std_flat_map_v =
  is_std_flat_map<std::remove_cvref_t<T>>::value;

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
  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(count); }) {
      keys.reserve(count);
    }
    if constexpr (requires { values.reserve(count); }) {
      values.reserve(count);
    }
    map.replace(std::move(keys), std::move(values));
  }
  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = rand();
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T, typename U>
void loop_number_number_array(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  std::vector<T> keys(count);
  std::vector<U> values(count);

  for (int i = 0; i < count; ++i) {
    keys[i] = rand();
    values[i] = rand();
  }

  std::vector<std::size_t> indices(count);
  std::ranges::iota(indices, std::size_t{});
  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index]);
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
  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(count); }) {
      keys.reserve(count);
    }
    if constexpr (requires { values.reserve(count); }) {
      values.reserve(count);
    }
    map.replace(std::move(keys), std::move(values));
  }
  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = faker::color::name(faker::Locale::ja_JP);
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value.size());
  }
}

template<typename T, typename U>
void loop_number_string_array(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  std::vector<T> keys(count);
  std::vector<U> values;
  values.reserve(count);

  for (int i = 0; i < count; ++i) {
    keys[i] = rand();
    values.emplace_back(faker::color::name(faker::Locale::ja_JP));
  }

  std::vector<std::size_t> indices(count);
  std::ranges::iota(indices, std::size_t{});
  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index].size());
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
    celero::DoNotOptimizeAway(key.size());
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
  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(count); }) {
      keys.reserve(count);
    }
    if constexpr (requires { values.reserve(count); }) {
      values.reserve(count);
    }
    map.replace(std::move(keys), std::move(values));
  }
  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = rand();
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}
template<typename T, typename U>
void loop_string_number_array(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  std::vector<T> keys;
  keys.reserve(count);
  std::vector<U> values(count);

  for (int i = 0; i < count; ++i) {
    keys.emplace_back(faker::color::name(faker::Locale::ja_JP));
    values[i] = rand();
  }

  std::vector<std::size_t> indices(count);
  std::ranges::iota(indices, std::size_t{});
  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index]);
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
  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(count); }) {
      keys.reserve(count);
    }
    if constexpr (requires { values.reserve(count); }) {
      values.reserve(count);
    }
    map.replace(std::move(keys), std::move(values));
  }
  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = faker::company::catchPhrase(faker::Locale::ja_JP);
    map.emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value.size());
  }
}

template<typename T, typename U>
void loop_string_string_array(int count) {
  std::vector<T> keys;
  keys.reserve(count);
  std::vector<U> values;
  values.reserve(count);

  for (int i = 0; i < count; ++i) {
    keys.emplace_back(faker::color::name(faker::Locale::ja_JP));
    values.emplace_back(faker::company::catchPhrase(faker::Locale::ja_JP));
  }

  std::vector<std::size_t> indices(count);
  std::ranges::iota(indices, std::size_t{});
  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index].size());
  }
}

CELERO_MAIN

// int -> int map benchmarks

BASELINE(INT_INT_10, Baseline, 100, 1000) {
  loop_number_number_baseline(10);
}

BENCHMARK(INT_INT_10, 01_STD_MAP, 100, 1000) {
  loop_number_number<std::map<int, int>>(10);
}

BENCHMARK(INT_INT_10, 02_STD_FLAT_MAP, 100, 1000) {
  loop_number_number<std::flat_map<int, int>>(10);
}

BENCHMARK(INT_INT_10, 03_ABSEIL_BTREE, 100, 1000) {
  loop_number_number<absl::btree_map<int, int>>(10);
}

BENCHMARK(INT_INT_10, 04_ARRAY, 100, 1000) {
  loop_number_number_array<int, int>(10);
}

BASELINE(INT_INT_100, Baseline, 10, 1000) {
  loop_number_number_baseline(100);
}

BENCHMARK(INT_INT_100, 01_STD_MAP, 10, 1000) {
  loop_number_number<std::map<int, int>>(100);
}

BENCHMARK(INT_INT_100, 02_STD_FLAT_MAP, 10, 1000) {
  loop_number_number<std::flat_map<int, int>>(100);
}

BENCHMARK(INT_INT_100, 03_ABSEIL_BTREE, 10, 1000) {
  loop_number_number<absl::btree_map<int, int>>(100);
}

BENCHMARK(INT_INT_100, 04_ARRAY, 100, 1000) {
  loop_number_number_array<int, int>(100);
}

BASELINE(INT_INT_1000, Baseline, 10, 1000) {
  loop_number_number_baseline(1000);
}

BENCHMARK(INT_INT_1000, 01_STD_MAP, 10, 1000) {
  loop_number_number<std::map<int, int>>(1000);
}

BENCHMARK(INT_INT_1000, 02_STD_FLAT_MAP, 10, 1000) {
  loop_number_number<std::flat_map<int, int>>(1000);
}

BENCHMARK(INT_INT_1000, 03_ABSEIL_BTREE, 10, 1000) {
  loop_number_number<absl::btree_map<int, int>>(1000);
}

BENCHMARK(INT_INT_1000, 04_ARRAY, 100, 1000) {
  loop_number_number_array<int, int>(1000);
}

BASELINE(INT_INT_10000, Baseline, 10, 100) {
  loop_number_number_baseline(10000);
}

BENCHMARK(INT_INT_10000, 01_STD_MAP, 10, 100) {
  loop_number_number<std::map<int, int>>(10000);
}

BENCHMARK(INT_INT_10000, 02_STD_FLAT_MAP, 10, 100) {
  loop_number_number<std::flat_map<int, int>>(10000);
}

BENCHMARK(INT_INT_10000, 03_ABSEIL_BTREE, 10, 100) {
  loop_number_number<absl::btree_map<int, int>>(10000);
}

BENCHMARK(INT_INT_10000, 04_ARRAY, 100, 100) {
  loop_number_number_array<int, int>(10000);
}

// int -> string map benchmarks

BASELINE(INT_STRING_10, Baseline, 100, 1000) {
  loop_number_string_baseline(10);
}

BENCHMARK(INT_STRING_10, 01_STD_MAP, 100, 1000) {
  loop_number_string<std::map<int, std::string>>(10);
}

BENCHMARK(INT_STRING_10, 02_STD_FLAT_MAP, 100, 1000) {
  loop_number_string<std::flat_map<int, std::string>>(10);
}

BENCHMARK(INT_STRING_10, 03_ABSEIL_BTREE, 100, 1000) {
  loop_number_string<absl::btree_map<int, std::string>>(10);
}

BENCHMARK(INT_STRING_10, 04_ARRAY, 100, 1000) {
  loop_number_string_array<int, std::string>(10);
}

BASELINE(INT_STRING_100, Baseline, 10, 1000) {
  loop_number_string_baseline(100);
}

BENCHMARK(INT_STRING_100, 01_STD_MAP, 10, 1000) {
  loop_number_string<std::map<int, std::string>>(100);
}

BENCHMARK(INT_STRING_100, 02_STD_FLAT_MAP, 10, 1000) {
  loop_number_string<std::flat_map<int, std::string>>(100);
}

BENCHMARK(INT_STRING_100, 03_ABSEIL_BTREE, 10, 1000) {
  loop_number_string<absl::btree_map<int, std::string>>(100);
}

BENCHMARK(INT_STRING_100, 04_ARRAY, 100, 1000) {
  loop_number_string_array<int, std::string>(100);
}

BASELINE(INT_STRING_1000, Baseline, 10, 1000) {
  loop_number_string_baseline(1000);
}

BENCHMARK(INT_STRING_1000, 01_STD_MAP, 10, 1000) {
  loop_number_string<std::map<int, std::string>>(1000);
}

BENCHMARK(INT_STRING_1000, 02_STD_FLAT_MAP, 10, 1000) {
  loop_number_string<std::flat_map<int, std::string>>(1000);
}

BENCHMARK(INT_STRING_1000, 03_ABSEIL_BTREE, 10, 1000) {
  loop_number_string<absl::btree_map<int, std::string>>(1000);
}

BENCHMARK(INT_STRING_1000, 04_ARRAY, 100, 1000) {
  loop_number_string_array<int, std::string>(1000);
}

BASELINE(INT_STRING_10000, Baseline, 10, 1000) {
  loop_number_string_baseline(10000);
}

BENCHMARK(INT_STRING_10000, 01_STD_MAP, 10, 100) {
  loop_number_string<std::map<int, std::string>>(10000);
}

BENCHMARK(INT_STRING_10000, 02_STD_FLAT_MAP, 10, 100) {
  loop_number_string<std::flat_map<int, std::string>>(10000);
}

BENCHMARK(INT_STRING_10000, 03_ABSEIL_BTREE, 10, 100) {
  loop_number_string<absl::btree_map<int, std::string>>(10000);
}

BENCHMARK(INT_STRING_10000, 04_ARRAY, 100, 1000) {
  loop_number_string_array<int, std::string>(10000);
}

// string -> int map benchmarks

BASELINE(STRING_INT_10, Baseline, 100, 1000) {
  loop_string_number_baseline(10);
}

BENCHMARK(STRING_INT_10, 01_STD_MAP, 100, 1000) {
  loop_string_number<std::map<std::string, int>>(10);
}

BENCHMARK(STRING_INT_10, 02_STD_FLAT_MAP, 100, 1000) {
  loop_string_number<std::flat_map<std::string, int>>(10);
}

BENCHMARK(STRING_INT_10, 03_ABSEIL_BTREE, 100, 1000) {
  loop_string_number<absl::btree_map<std::string, int>>(10);
}

BENCHMARK(STRING_INT_10, 04_ARRAY, 100, 1000) {
  loop_string_number_array<std::string, int>(10);
}

BASELINE(STRING_INT_100, Baseline, 10, 1000) {
  loop_string_number_baseline(100);
}

BENCHMARK(STRING_INT_100, 01_STD_MAP, 10, 1000) {
  loop_string_number<std::map<std::string, int>>(100);
}

BENCHMARK(STRING_INT_100, 02_STD_FLAT_MAP, 10, 1000) {
  loop_string_number<std::flat_map<std::string, int>>(100);
}

BENCHMARK(STRING_INT_100, 03_ABSEIL_BTREE, 10, 1000) {
  loop_string_number<absl::btree_map<std::string, int>>(100);
}

BENCHMARK(STRING_INT_100, 04_ARRAY, 100, 1000) {
  loop_string_number_array<std::string, int>(100);
}

BASELINE(STRING_INT_1000, Baseline, 10, 1000) {
  loop_string_number_baseline(1000);
}

BENCHMARK(STRING_INT_1000, 01_STD_MAP, 10, 1000) {
  loop_string_number<std::map<std::string, int>>(1000);
}

BENCHMARK(STRING_INT_1000, 02_STD_FLAT_MAP, 10, 1000) {
  loop_string_number<std::flat_map<std::string, int>>(1000);
}

BENCHMARK(STRING_INT_1000, 03_ABSEIL_BTREE, 10, 1000) {
  loop_string_number<absl::btree_map<std::string, int>>(1000);
}

BENCHMARK(STRING_INT_1000, 04_ARRAY, 10, 1000) {
  loop_string_number_array<std::string, int>(1000);
}

BASELINE(STRING_INT_10000, Baseline, 10, 100) {
  loop_string_number_baseline(10000);
}

BENCHMARK(STRING_INT_10000, 01_STD_MAP, 10, 100) {
  loop_string_number<std::map<std::string, int>>(10000);
}

BENCHMARK(STRING_INT_10000, 02_STD_FLAT_MAP, 10, 100) {
  loop_string_number<std::flat_map<std::string, int>>(10000);
}

BENCHMARK(STRING_INT_10000, 03_ABSEIL_BTREE, 10, 100) {
  loop_string_number<absl::btree_map<std::string, int>>(10000);
}

BENCHMARK(STRING_INT_10000, 04_ARRAY, 10, 100) {
  loop_string_number_array<std::string, int>(10000);
}

// string -> string map benchmarks

BASELINE(STRING_STRING_10, Baseline, 100, 10000) {
  loop_string_string_baseline(10);
}

BENCHMARK(STRING_STRING_10, 01_STD_MAP, 100, 10000) {
  loop_string_string<std::map<std::string, std::string>>(10);
}

BENCHMARK(STRING_STRING_10, 02_STD_FLAT_MAP, 100, 10000) {
  loop_string_string<std::flat_map<std::string, std::string>>(10);
}

BENCHMARK(STRING_STRING_10, 03_ABSEIL_BTREE, 100, 10000) {
  loop_string_string<absl::btree_map<std::string, std::string>>(10);
}

BENCHMARK(STRING_STRING_10, 04_ARRAY, 100, 10000) {
  loop_string_string_array<std::string, std::string>(10);
}

BASELINE(STRING_STRING_100, Baseline, 10, 10000) {
  loop_string_string_baseline(100);
}

BENCHMARK(STRING_STRING_100, 01_STD_MAP, 10, 10000) {
  loop_string_string<std::map<std::string, std::string>>(100);
}

BENCHMARK(STRING_STRING_100, 02_STD_FLAT_MAP, 10, 10000) {
  loop_string_string<std::flat_map<std::string, std::string>>(100);
}

BENCHMARK(STRING_STRING_100, 03_ABSEIL_BTREE, 10, 10000) {
  loop_string_string<absl::btree_map<std::string, std::string>>(100);
}

BENCHMARK(STRING_STRING_100, 04_ARRAY, 100, 10000) {
  loop_string_string_array<std::string, std::string>(100);
}

BASELINE(STRING_STRING_1000, Baseline, 10, 10000) {
  loop_string_string_baseline(1000);
}

BENCHMARK(STRING_STRING_1000, 01_STD_MAP, 10, 10000) {
  loop_string_string<std::map<std::string, std::string>>(1000);
}

BENCHMARK(STRING_STRING_1000, 02_STD_FLAT_MAP, 10, 10000) {
  loop_string_string<std::flat_map<std::string, std::string>>(1000);
}

BENCHMARK(STRING_STRING_1000, 03_ABSEIL_BTREE, 10, 10000) {
  loop_string_string<absl::btree_map<std::string, std::string>>(1000);
}

BENCHMARK(STRING_STRING_1000, 04_ARRAY, 100, 10000) {
  loop_string_string_array<std::string, std::string>(1000);
}

BASELINE(STRING_STRING_10000, Baseline, 10, 100) {
  loop_string_string_baseline(10000);
}

BENCHMARK(STRING_STRING_10000, 01_STD_MAP, 10, 100) {
  loop_string_string<std::map<std::string, std::string>>(10000);
}

BENCHMARK(STRING_STRING_10000, 02_STD_FLAT_MAP, 10, 100) {
  loop_string_string<std::flat_map<std::string, std::string>>(10000);
}

BENCHMARK(STRING_STRING_10000, 03_ABSEIL_BTREE, 10, 100) {
  loop_string_string<absl::btree_map<std::string, std::string>>(10000);
}

BENCHMARK(STRING_STRING_10000, 04_ARRAY, 10, 100) {
  loop_string_string_array<std::string, std::string>(10000);
}

