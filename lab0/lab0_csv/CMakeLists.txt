# CMakeList.txt: проект CMake дл€ lab0; включите исходный код и определени€,
# укажите здесь логику дл€ конкретного проекта.
#
cmake_minimum_required (VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("lab0_csv")

# ƒобавьте источник в исполн€емый файл этого проекта.
add_executable (word_count "src/lab0_csv.cpp" "src/wordCount.h" "src/wordCount.cpp")
target_compile_options(word_count PRIVATE -Wall)
