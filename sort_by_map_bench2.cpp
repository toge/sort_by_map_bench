#include <iostream>
#include <flat_map>
#include <map>
#include <random>
#include <functional>
#include <ranges>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "absl/container/btree_map.h"
#include "faker-cxx/faker.h"

#include "celero/Celero.h"

namespace {

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
    auto const key = std::string{faker::color::name(faker::Locale::ja_JP)} + "#" + std::to_string(i);
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
    auto const key = std::string{faker::color::name(faker::Locale::ja_JP)} + "#" + std::to_string(i);
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
    keys.emplace_back(std::string{faker::color::name(faker::Locale::ja_JP)} + "#" + std::to_string(i));
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
    auto const key = std::string{faker::color::name(faker::Locale::ja_JP)} + "#" + std::to_string(i);
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
    auto const key = std::string{faker::color::name(faker::Locale::ja_JP)} + "#" + std::to_string(i);
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
    keys.emplace_back(std::string{faker::color::name(faker::Locale::ja_JP)} + "#" + std::to_string(i));
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

/*===============================================================================*\
 * 以下Celeroを使ったベンチマーク定義
\*===============================================================================*/

// CeleroのTestFixtureでcount(問題サイズ)を列挙し、ベンチ定義のコピペを減らす。
// Context7(Celero README)にある getExperimentValues()/setUp()/BASELINE_F/BENCHMARK_F の流儀。
class CountFixture : public celero::TestFixture {
public:
  std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> getExperimentValues() const override {
    // ここに値を足すだけでcountのバリエーションを増やせる
    std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> values;
    // Celero v2.9系では、ExperimentValue::Iterations が 0 の場合
    // 「テストケース側のデフォルトiterations」を使う。
    // *_F マクロ側の iterations を 0 にすると、結果が 0 になり得て -nan になるため、ここで明示する。
    values.emplace_back(makeExperimentValue(10, 1'000));
    values.emplace_back(makeExperimentValue(100, 1'000));
    values.emplace_back(makeExperimentValue(1'000, 1'000));
    values.emplace_back(makeExperimentValue(10'000, 100));
    return values;
  }

  void setUp(const celero::TestFixture::ExperimentValue* experimentValue) override {
    this->count = static_cast<int>(experimentValue->Value);
  }

  int count = 0;

private:
  static std::shared_ptr<celero::TestFixture::ExperimentValue> makeExperimentValue(int64_t value, int64_t iterations) {
    return std::make_shared<celero::TestFixture::ExperimentValue>(value, iterations);
  }
};

} // namespace

CELERO_MAIN

// samples/iterationsを0にしてCeleroに自動調整させる。
// countはfixtureのExperimentValueで変化させる。

// int -> int
BASELINE_F(INT_INT, Baseline, CountFixture, 30, 1) {
  loop_number_number_baseline(this->count);
}
BENCHMARK_F(INT_INT, 01_STD_MAP, CountFixture, 30, 1) {
  loop_number_number<std::map<int, int>>(this->count);
}
BENCHMARK_F(INT_INT, 02_STD_FLAT_MAP, CountFixture, 30, 1) {
  loop_number_number<std::flat_map<int, int>>(this->count);
}
BENCHMARK_F(INT_INT, 03_ABSEIL_BTREE, CountFixture, 30, 1) {
  loop_number_number<absl::btree_map<int, int>>(this->count);
}
BENCHMARK_F(INT_INT, 04_ARRAY, CountFixture, 30, 1) {
  loop_number_number_array<int, int>(this->count);
}

// int -> string
BASELINE_F(INT_STRING, Baseline, CountFixture, 30, 1) {
  loop_number_string_baseline(this->count);
}
BENCHMARK_F(INT_STRING, 01_STD_MAP, CountFixture, 30, 1) {
  loop_number_string<std::map<int, std::string>>(this->count);
}
BENCHMARK_F(INT_STRING, 02_STD_FLAT_MAP, CountFixture, 30, 1) {
  loop_number_string<std::flat_map<int, std::string>>(this->count);
}
BENCHMARK_F(INT_STRING, 03_ABSEIL_BTREE, CountFixture, 30, 1) {
  loop_number_string<absl::btree_map<int, std::string>>(this->count);
}
BENCHMARK_F(INT_STRING, 04_ARRAY, CountFixture, 30, 1) {
  loop_number_string_array<int, std::string>(this->count);
}

// string -> int
BASELINE_F(STRING_INT, Baseline, CountFixture, 30, 1) {
  loop_string_number_baseline(this->count);
}
BENCHMARK_F(STRING_INT, 01_STD_MAP, CountFixture, 30, 1) {
  loop_string_number<std::map<std::string, int>>(this->count);
}
BENCHMARK_F(STRING_INT, 02_STD_FLAT_MAP, CountFixture, 30, 1) {
  loop_string_number<std::flat_map<std::string, int>>(this->count);
}
BENCHMARK_F(STRING_INT, 03_ABSEIL_BTREE, CountFixture, 30, 1) {
  loop_string_number<absl::btree_map<std::string, int>>(this->count);
}
BENCHMARK_F(STRING_INT, 04_ARRAY, CountFixture, 30, 1) {
  loop_string_number_array<std::string, int>(this->count);
}

// string -> string
BASELINE_F(STRING_STRING, Baseline, CountFixture, 30, 1) {
  loop_string_string_baseline(this->count);
}
BENCHMARK_F(STRING_STRING, 01_STD_MAP, CountFixture, 30, 1) {
  loop_string_string<std::map<std::string, std::string>>(this->count);
}
BENCHMARK_F(STRING_STRING, 02_STD_FLAT_MAP, CountFixture, 30, 1) {
  loop_string_string<std::flat_map<std::string, std::string>>(this->count);
}
BENCHMARK_F(STRING_STRING, 03_ABSEIL_BTREE, CountFixture, 30, 1) {
  loop_string_string<absl::btree_map<std::string, std::string>>(this->count);
}
BENCHMARK_F(STRING_STRING, 04_ARRAY, CountFixture, 30, 1) {
  loop_string_string_array<std::string, std::string>(this->count);
}

