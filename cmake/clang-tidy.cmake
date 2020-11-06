#TODO add ways to set the clang-tidy rules
#TODO add jobs for clang-tidy

find_program(CLANG_TIDY clang-tidy)
cmake_dependent_option(CLANG_TIDY_ACTIVATE "Create custom target for clang-tidy" ON "CLANG_TIDY" OFF)
if(CLANG_TIDY_ACTIVATE)
  set(CMAKE_CXX_CLANG_TIDY
      ${CLANG_TIDY} -checks=-*,readability-*
      CACHE INTERNAL "Path to Clang-Tidy for C++"
  )
  set(CMAKE_C_CLANG_TIDY
      ${CLANG_TIDY} -checks=-*,readability-*
      CACHE INTERNAL "Path to Clang-Tidy for C"
  )
endif()