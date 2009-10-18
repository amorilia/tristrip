cmake_minimum_required(VERSION 2.6)
project(TRISTRIP)

# find boost
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# include tristrip headers
include_directories(${TRISTRIP_SOURCE_DIR}/include)

# build the actual library
add_library(tristrip
    src/trianglemesh.cpp
)
target_link_libraries(tristrip)

# build the tests
enable_testing()
add_subdirectory(test)

