# CMakeList.txt: проект CMake для lab0; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

project ("lab0_csv")

# Добавьте источник в исполняемый файл этого проекта.
# add_executable (lab0 "lab0_csv/lab0.cpp" "lab0_csv/lab0.h")
#add_subdirectory(lab0_namespaces)
add_subdirectory(lab0_csv)

# if (CMAKE_VERSION VERSION_GREATER 3.12)
# set_property(TARGET lab0 PROPERTY CXX_STANDARD 20)
# endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
