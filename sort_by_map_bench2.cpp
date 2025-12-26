#include <flat_map>
#include <map>
#include <random>
#include <ranges>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "absl/container/btree_map.h"
#include "faker-cxx/faker.h"
#include "cpp-sort/sorters/pdq_sorter.h"

#include "celero/Celero.h"

namespace {

// 型Tがstd::flat_mapかどうかを判定するためのメタ関数
template<class>
struct is_std_flat_map : std::false_type {};

template<class Key, class Mapped, class Compare, class KeyContainer, class MappedContainer>
struct is_std_flat_map<std::flat_map<Key, Mapped, Compare, KeyContainer, MappedContainer>>
  : std::true_type {};

template<class T>
inline constexpr bool is_std_flat_map_v =
  is_std_flat_map<std::remove_cvref_t<T>>::value;


/*===============================================================================*\
 * 以下Celeroを使ったベンチマーク定義
\*===============================================================================*/

/**
 * @brief 共通テストデータを生成・管理する構造体
 *
 */
struct SharedTestData {
  std::vector<int> int_keys;
  std::vector<int> int_values;
  std::vector<std::string> string_keys;
  std::vector<std::string> string_values;

  bool initialized = false;

  void initialize(int count) {
    if (initialized) {
      return;
    }

    auto const&& rand = []() noexcept {
      auto engine = std::mt19937{std::random_device{}()};
      return std::uniform_int_distribution<>{0, 65536}(engine);
    };

    int_keys.resize(count);
    int_values.resize(count);
    string_keys.resize(count);
    string_values.resize(count);

    for (auto const idx : std::ranges::views::iota(0, count)) {
      int_keys[idx] = rand();
      int_values[idx] = rand();
      string_keys[idx] = std::string{faker::color::name(faker::Locale::ja_JP)} + "#" + std::to_string(idx);
      string_values[idx] = faker::company::catchPhrase(faker::Locale::ja_JP);
    }

    initialized = true;
  }
};

/**
 * @brief 共通テストデータを共有するためのCeleroフィクスチャ
 *
 */
class SharedDataFixture : public celero::TestFixture {
public:
  std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> getExperimentValues() const override {
    auto values = std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>>{};
    values.emplace_back(makeExperimentValue(10, 1000));
    values.emplace_back(makeExperimentValue(100, 1000));
    values.emplace_back(makeExperimentValue(1000, 1000));
    values.emplace_back(makeExperimentValue(10000, 100));
    return values;
  }

  void setUp(const celero::TestFixture::ExperimentValue* experimentValue) override {
    this->count = static_cast<int>(experimentValue->Value);
    static std::map<int, SharedTestData> cache;
    if (not cache[count].initialized) {
      cache[count].initialize(this->count);
    }
    shared_data = &cache[count];
  }

  int count = 0;
  SharedTestData* shared_data = nullptr;

private:
  static std::shared_ptr<celero::TestFixture::ExperimentValue> makeExperimentValue(std::int64_t value, std::int64_t iterations) {
    return std::make_shared<celero::TestFixture::ExperimentValue>(value, iterations);
  }
};

// int -> int map benchmarks

void loop_number_number_baseline(SharedTestData const* bench_data) {
  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(bench_data->int_keys.size()))) {
    auto const key = bench_data->int_keys[idx];
    auto const value = bench_data->int_values[idx];
    celero::DoNotOptimizeAway(key);
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T>
void loop_number_number(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto map = T{};

  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(COUNT); }) {
      keys.reserve(COUNT);
    }
    if constexpr (requires { values.reserve(COUNT); }) {
      values.reserve(COUNT);
    }
    map.replace(std::move(keys), std::move(values));
  }

  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(COUNT))) {
    auto const key = bench_data->int_keys[idx];
    auto const value = bench_data->int_values[idx];
    map.try_emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T, typename U>
void loop_number_number_array(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->int_keys;
  auto const& values = bench_data->int_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});

  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index]);
  }
}

template<typename T, typename U>
void loop_number_number_array_cppsort(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->int_keys;
  auto const& values = bench_data->int_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});

  auto sorter = cppsort::pdq_sorter{};
  sorter(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index]);
  }
}

void loop_number_string_baseline(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(COUNT))) {
    auto const key = bench_data->int_keys[idx];
    auto const value = &bench_data->string_values[idx];
    celero::DoNotOptimizeAway(key);
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T>
void loop_number_string(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  T map;
  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(COUNT); }) {
      keys.reserve(COUNT);
    }
    if constexpr (requires { values.reserve(COUNT); }) {
      values.reserve(COUNT);
    }
    map.replace(std::move(keys), std::move(values));
  }
  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(COUNT))) {
    auto const key = bench_data->int_keys[idx];
    auto const value = &bench_data->string_values[idx];
    map.try_emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value->size());
  }
}

template<typename T, typename U>
void loop_number_string_array(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->int_keys;
  auto const& values = bench_data->string_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});
  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index].size());
  }
}

template<typename T, typename U>
void loop_number_string_array_cppsort(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->int_keys;
  auto const& values = bench_data->string_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});

  auto sorter = cppsort::pdq_sorter{};
  sorter(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index].size());
  }
}

void loop_string_number_baseline(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(COUNT))) {
    auto const key = &bench_data->string_keys[idx];
    auto const value = bench_data->int_values[idx];
    celero::DoNotOptimizeAway(key->size());
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T>
void loop_string_number(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  T map;
  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(COUNT); }) {
      keys.reserve(COUNT);
    }
    if constexpr (requires { values.reserve(COUNT); }) {
      values.reserve(COUNT);
    }
    map.replace(std::move(keys), std::move(values));
  }
  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(COUNT))) {
    auto const key = bench_data->string_keys[idx];
    auto const value = bench_data->int_values[idx];
    map.try_emplace(key, value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value);
  }
}

template<typename T, typename U>
void loop_string_number_array(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->string_keys;
  auto const& values = bench_data->int_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});
  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index]);
  }
}

template<typename T, typename U>
void loop_string_number_array_cppsort(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->string_keys;
  auto const& values = bench_data->int_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});

  auto sorter = cppsort::pdq_sorter{};
  sorter(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index]);
  }
}

void loop_string_string_baseline(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(COUNT))) {
    auto const key = bench_data->string_keys[idx];
    auto const value = bench_data->string_values[idx];
    celero::DoNotOptimizeAway(key.size());
    celero::DoNotOptimizeAway(value.size());
  }
}

template<typename T>
void loop_string_string(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  T map;
  // std::flat_mapについては事前に容量を確保しておく
  if constexpr (is_std_flat_map_v<T>) {
    auto [keys, values] = std::move(map).extract();
    if constexpr (requires { keys.reserve(COUNT); }) {
      keys.reserve(COUNT);
    }
    if constexpr (requires { values.reserve(COUNT); }) {
      values.reserve(COUNT);
    }
    map.replace(std::move(keys), std::move(values));
  }
  for (auto const idx : std::ranges::views::iota(0, static_cast<int>(COUNT))) {
    auto const& key = bench_data->string_keys[idx];
    auto const& value = bench_data->string_values[idx];
    map.try_emplace(key, &value);
  }

  for (const auto& [_, value] : map) {
    celero::DoNotOptimizeAway(value->size());
  }
}

template<typename T, typename U>
void loop_string_string_array(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->string_keys;
  auto const& values = bench_data->string_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});
  std::ranges::sort(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index].size());
  }
}

template<typename T, typename U>
void loop_string_string_array_cppsort(SharedTestData const* bench_data) {
  auto const COUNT = bench_data->int_keys.size();

  auto const& keys = bench_data->string_keys;
  auto const& values = bench_data->string_values;

  std::vector<std::size_t> indices(COUNT);
  std::ranges::iota(indices, std::size_t{});

  auto sorter = cppsort::pdq_sorter{};
  sorter(indices, [&](auto const lhs, auto const rhs) {
    return keys[lhs] < keys[rhs];
  });

  for (auto const index : indices) {
    celero::DoNotOptimizeAway(values[index].size());
  }
}

} // namespace

CELERO_MAIN

// samples/iterationsを0にしてCeleroに自動調整させる。
// countはfixtureのExperimentValueで変化させる。

// int -> int
BASELINE_F(INT_INT, Baseline, SharedDataFixture, 30, 1) {
  loop_number_number_baseline(this->shared_data);
}
BENCHMARK_F(INT_INT, 01_STD_MAP, SharedDataFixture, 30, 1) {
  loop_number_number<std::map<int, int>>(this->shared_data);
}
BENCHMARK_F(INT_INT, 02_STD_FLAT_MAP, SharedDataFixture, 30, 1) {
  loop_number_number<std::flat_map<int, int>>(this->shared_data);
}
BENCHMARK_F(INT_INT, 03_ABSEIL_BTREE, SharedDataFixture, 30, 1) {
  loop_number_number<absl::btree_map<int, int>>(this->shared_data);
}
BENCHMARK_F(INT_INT, 04_ARRAY, SharedDataFixture, 30, 1) {
  loop_number_number_array<int, int>(this->shared_data);
}
BENCHMARK_F(INT_INT, 05_ARRAY_CPPSORT, SharedDataFixture, 30, 1) {
  loop_number_number_array_cppsort<int, int>(this->shared_data);
}

// int -> string
BASELINE_F(INT_STRING, Baseline, SharedDataFixture, 30, 1) {
  loop_number_string_baseline(this->shared_data);
}
BENCHMARK_F(INT_STRING, 01_STD_MAP, SharedDataFixture, 30, 1) {
  loop_number_string<std::map<int, std::string const*>>(this->shared_data);
}
BENCHMARK_F(INT_STRING, 02_STD_FLAT_MAP, SharedDataFixture, 30, 1) {
  loop_number_string<std::flat_map<int, std::string const*>>(this->shared_data);
}
BENCHMARK_F(INT_STRING, 03_ABSEIL_BTREE, SharedDataFixture, 30, 1) {
  loop_number_string<absl::btree_map<int, std::string const*>>(this->shared_data);
}
BENCHMARK_F(INT_STRING, 04_ARRAY, SharedDataFixture, 30, 1) {
  loop_number_string_array<int, std::string const*>(this->shared_data);
}
BENCHMARK_F(INT_STRING, 05_ARRAY_CPPSORT, SharedDataFixture, 30, 1) {
  loop_number_string_array_cppsort<int, std::string const*>(this->shared_data);
}

// string -> int
BASELINE_F(STRING_INT, Baseline, SharedDataFixture, 30, 1) {
  loop_string_number_baseline(this->shared_data);
}
BENCHMARK_F(STRING_INT, 01_STD_MAP, SharedDataFixture, 30, 1) {
  loop_string_number<std::map<std::string, int>>(this->shared_data);
}
BENCHMARK_F(STRING_INT, 02_STD_FLAT_MAP, SharedDataFixture, 30, 1) {
  loop_string_number<std::flat_map<std::string, int>>(this->shared_data);
}
BENCHMARK_F(STRING_INT, 03_ABSEIL_BTREE, SharedDataFixture, 30, 1) {
  loop_string_number<absl::btree_map<std::string, int>>(this->shared_data);
}
BENCHMARK_F(STRING_INT, 04_ARRAY, SharedDataFixture, 30, 1) {
  loop_string_number_array<std::string, int>(this->shared_data);
}
BENCHMARK_F(STRING_INT, 05_ARRAY_CPPSORT, SharedDataFixture, 30, 1) {
  loop_string_number_array_cppsort<std::string, int>(this->shared_data);
}

// string -> string
BASELINE_F(STRING_STRING, Baseline, SharedDataFixture, 30, 1) {
  loop_string_string_baseline(this->shared_data);
}
BENCHMARK_F(STRING_STRING, 01_STD_MAP, SharedDataFixture, 30, 1) {
  loop_string_string<std::map<std::string, std::string const*>>(this->shared_data);
}
BENCHMARK_F(STRING_STRING, 02_STD_FLAT_MAP, SharedDataFixture, 30, 1) {
  loop_string_string<std::flat_map<std::string, std::string const*>>(this->shared_data);
}
BENCHMARK_F(STRING_STRING, 03_ABSEIL_BTREE, SharedDataFixture, 30, 1) {
  loop_string_string<absl::btree_map<std::string, std::string const*>>(this->shared_data);
}
BENCHMARK_F(STRING_STRING, 04_ARRAY, SharedDataFixture, 30, 1) {
  loop_string_string_array<std::string, std::string const*>(this->shared_data);
}
BENCHMARK_F(STRING_STRING, 05_ARRAY_CPPSORT, SharedDataFixture, 30, 1) {
  loop_string_string_array_cppsort<std::string, std::string const*>(this->shared_data);
}
