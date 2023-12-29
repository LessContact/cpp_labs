cmake_minimum_required(VERSION 3.27)
project(lab3)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(processorlib
        src/ConfigParser.h
        src/ConfigParser.cpp
        src/CommandLineParser.h
        src/CommandLineParser.cpp
        src/WavParser.h
        src/WavParser.cpp
        src/ConverterImpl.cpp
)
target_include_directories(processorlib PUBLIC src)

add_executable(PCMprocess src/main.cpp)
target_link_libraries(PCMprocess processorlib)

add_executable(PCMprocessTests gtest/gtest.cpp)
target_link_libraries(PCMprocessTests GTest::gtest GTest::gtest_main processorlib)

enable_testing()
include(GoogleTest)
gtest_discover_tests(PCMprocessTests)
