cmake_minimum_required(VERSION 3.5)

project(test_sfinae)

add_executable(homework homework.cpp)
set_target_properties(homework PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)