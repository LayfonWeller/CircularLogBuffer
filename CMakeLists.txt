cmake_minimum_required(VERSION 3.15)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
project(CircularIterator CXX)

include(projectSettings)
project_add_library(CircularIterator INTERFACE)
project_set_warnings()

project_compile_features(cxx_std_17)

target_include_directories(CircularIterator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_compile_features(CircularIterator INTERFACE cxx_relaxed_constexpr)

include(CatchTestUtils)
Test_Add_Test_Folder()