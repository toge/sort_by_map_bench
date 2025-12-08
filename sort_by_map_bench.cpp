#include <iostream>
#include <flat_map>
#include <map>

#include "absl/container/btree_map.h"
#include "faker-cxx/faker.h"

void loop_int_int_baseline(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  auto const start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = rand();
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: baseline, Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

template<typename T>
void loop_int_int(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  auto const start = std::chrono::high_resolution_clock::now();

  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = rand();
    map.emplace(key, value);
  }

  for (const auto& [key, value] : map) {
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: " << typeid(T).name() << ", Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

void loop_int_string_baseline(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  auto const start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = faker::color::name(faker::Locale::ja_JP);
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: baseline, Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

template<typename T>
void loop_int_string(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  auto const start = std::chrono::high_resolution_clock::now();

  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = rand();
    auto const value = faker::color::name(faker::Locale::ja_JP);
    map.emplace(key, value);
  }

  for (const auto& [key, value] : map) {
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: " << typeid(T).name() << ", Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

void loop_string_int_baseline(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  auto const start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = rand();
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: baseline, Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

template<typename T>
void loop_string_int(int count) {
  auto&& rand = []() noexcept {
    auto device = std::random_device{};
    auto engine = std::mt19937{device()};

    return std::bind(std::uniform_int_distribution<>{0, 65536}, engine);
  }();

  auto const start = std::chrono::high_resolution_clock::now();
  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = rand();
    map.emplace(key, value);
  }

  for (const auto& [key, value] : map) {
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: " << typeid(T).name() << ", Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

void loop_string_string_baseline(int count) {
  auto const start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = faker::company::catchPhrase(faker::Locale::ja_JP);
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: baseline, Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

template<typename T>
void loop_string_string(int count) {
  auto const start = std::chrono::high_resolution_clock::now();

  T map;
  for (int i = 0; i < count; ++i) {
    auto const key = faker::color::name(faker::Locale::ja_JP);
    auto const value = faker::company::catchPhrase(faker::Locale::ja_JP);
    map.emplace(key, value);
  }

  for (const auto& [key, value] : map) {
    (void)key;
    (void)value;
  }

  auto const end =  std::chrono::high_resolution_clock::now();
  auto const duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Type: " << typeid(T).name() << ", Count: " << count << ", Duration: " << duration << " ms" << std::endl;
}

int main() {
  auto const count_list = std::array{10, 50, 100, 1000, 10000, 100000, 1000000};

  for (auto const count : count_list) {
    loop_int_int_baseline(count);
    loop_int_int<std::map<int, int>>(count);
    loop_int_int<std::flat_map<int, int>>(count);
    loop_int_int<absl::btree_map<int, int>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  for (auto const count : count_list) {
    loop_int_int_baseline(count);
    loop_int_int<std::map<int, double>>(count);
    loop_int_int<std::flat_map<int, double>>(count);
    loop_int_int<absl::btree_map<int, double>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  for (auto const count : count_list) {
    loop_int_int_baseline(count);
    loop_int_int<std::map<double, int>>(count);
    loop_int_int<std::flat_map<double, int>>(count);
    loop_int_int<absl::btree_map<double, int>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  for (auto const count : count_list) {
    loop_int_int_baseline(count);
    loop_int_int<std::map<double, double>>(count);
    loop_int_int<std::flat_map<double, double>>(count);
    loop_int_int<absl::btree_map<double, double>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  for (auto const count : count_list) {
    loop_int_string_baseline(count);
    loop_int_string<std::map<int, std::string>>(count);
    loop_int_string<std::flat_map<int, std::string>>(count);
    loop_int_string<absl::btree_map<int, std::string>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  for (auto const count : count_list) {
    loop_int_string_baseline(count);
    loop_int_string<std::map<double, std::string>>(count);
    loop_int_string<std::flat_map<double, std::string>>(count);
    loop_int_string<absl::btree_map<double, std::string>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  for (auto const count : count_list) {
    loop_string_int_baseline(count);
    loop_string_int<std::map<std::string, int>>(count);
    loop_string_int<std::flat_map<std::string, int>>(count);
    loop_string_int<absl::btree_map<std::string, int>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  for (auto const count : count_list) {
    loop_string_string_baseline(count);
    loop_string_string<std::map<std::string, std::string>>(count);
    loop_string_string<std::flat_map<std::string, std::string>>(count);
    loop_string_string<absl::btree_map<std::string, std::string>>(count);
  }
  std::cout << '\n';
  std::cout << '\n';

  return 0;
}
