cmake_minimum_required(VERSION 3.16)
project(FastOrderBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)            # -DCMAKE_BUILD_TYPE=Debug
add_compile_options(-Wall -Wextra -Wpedantic -march=native -O3)

# Engine library
add_library(fastob
    src/order_book_map.cpp
    src/order_book_vector.cpp
    src/order_book_hash.cpp
    src/event_generator.cpp
)

target_include_directories(fastob PUBLIC include)

# Benchmark / runner
add_executable(fastob_bench src/main.cpp)
target_link_libraries(fastob_bench PRIVATE fastob)