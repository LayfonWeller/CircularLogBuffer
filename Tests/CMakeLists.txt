cmake_minimum_required(VERSION 3.10)
include(conan)
include(CatchTestUtils)

project(Tests LANGUAGES CXX C)

set(CONAN_EXTRA_REQUIRES "catch2/2.13.4")
set(CONAN_EXTRA_OPTIONS "")


run_conan()

# automatically enable catch2 to generate ctest targets
if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()


enable_testing()

Test_Utils_Init()
Add_Unit_Test(circularLogIterators.unit.cpp DEP_LIBS CircularIterator)
Add_Unit_Test(circularLog.unit.cpp DEP_LIBS CircularIterator)


project_add_dependencies(CircularIterator)
project_add_executable(test_Circular test_CircularLogs.cpp)
target_link_libraries(test_Circular PRIVATE CircularIterator)