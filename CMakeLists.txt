cmake_minimum_required(VERSION 3.25)
project(sort_by_map_bench LANGUAGES CXX)

## optimization
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
endif()
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox")
endif()

## other flag
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## google perf tools
# HEAPPROFILE=heap CPUPROFILE=test.prof xxx
# pprof --text xxx yyy.prof
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lprofiler -ltcmalloc")

find_package(absl REQUIRED CONFIG)
find_package(faker-cxx REQUIRED CONFIG)

add_executable(sort_by_map_bench sort_by_map_bench.cpp)
target_link_libraries(sort_by_map_bench PRIVATE
    absl::container_common
    faker-cxx::faker-cxx
)
# requires C++23 for flat_map
target_compile_features(sort_by_map_bench PRIVATE cxx_std_23)
