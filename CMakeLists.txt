cmake_minimum_required(VERSION 3.15)
list(APPEND CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/catch2_codecov)

include(StandardProjectSettings)
include(clang-tidy)
include(clang-format)
include(cppcheck)

project(CircularIterator CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(projectSettings)
project_add_library(CircularIterator INTERFACE)
project_set_warnings()

project_compile_features(cxx_std_17 cxx_relaxed_constexpr)

target_include_directories(CircularIterator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_compile_features(CircularIterator INTERFACE cxx_relaxed_constexpr)

include(CatchTestUtils)
Test_Add_Test_Folder()