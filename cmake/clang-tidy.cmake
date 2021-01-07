# TODO add ways to set the clang-tidy rules TODO add jobs for clang-tidy

find_program(CLANG_TIDY NAMES clang-tidy clang-tidy-11)
cmake_dependent_option(ENABLE_CLANG_TIDY "Have Cmake run Clang-tidy on build" !CMAKE_CROSSCOMPILING "CLANG_TIDY" OFF)
if(ENABLE_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY
      ${CLANG_TIDY} -checks=-*,readability-*
      CACHE INTERNAL "Path to Clang-Tidy for C++"
  )
  set(CMAKE_C_CLANG_TIDY
      ${CLANG_TIDY} -checks=-*,readability-*
      CACHE INTERNAL "Path to Clang-Tidy for C"
  )
endif()
