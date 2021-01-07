cmake_minimum_required(VERSION 3.10)
include(conan)
include(CatchTestUtils)

project(Tests LANGUAGES CXX C)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ${FORCE_COLORED_OUTPUT_PREFERENCE})
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

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