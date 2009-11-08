cmake_minimum_required(VERSION 2.6)
project(TRISTRIP)

# find boost
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# use cython
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(UseCython)

find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)

# include tristrip headers
include_directories(
    ${TRISTRIP_SOURCE_DIR}/include
    ${PYTHON_INCLUDE_PATH}
)

# build the actual library
add_library(tristrip SHARED
    src/trianglemesh.cpp
#    src/trianglestripifier.cpp
#    src/tristrip.cpp
)
target_link_libraries(tristrip)

# build the python bindings
#add_subdirectory(python)

# build the tests
enable_testing()
add_subdirectory(test)

