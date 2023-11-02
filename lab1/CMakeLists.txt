# CMakeList.txt: проект CMake для lab0; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("lab1")

include_directories(src)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/af5ddbde4d57b0331e1d0ed3659ca2e39b37580d.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (lab1.exe "src/main.cpp" "src/buffer.h" "src/buffer.cpp")
target_compile_options(lab1.exe PRIVATE -Wall)
# set_property(TARGET word_count PROPERTY CXX_STANDARD 20)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

target_link_libraries(lab1 GTest::gtest_main)
enable_testing()
include(GoogleTest)
gtest_discover_tests(lab1)
