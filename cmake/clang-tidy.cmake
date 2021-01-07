#TODO add ways to set the clang-tidy rules
#TODO add jobs for clang-tidy

find_program(CLANG_TIDY NAMES clang-tidy clang-tidy-11)
cmake_dependent_option(ENABLE_CLANG_TIDY "Have Cmake run Clang-tidy on build" !CMAKE_CROSSCOMPILING "CLANG_TIDY" OFF)
if(ENABLE_CLANG_TIDY)
  set(CLANG_TIDY_CHECKS
      "-*,readability-*"
      CACHE STRING "Clang-tidy checks"
  )
  set(CLANG_TIDY_CHECKS_ADDITIONAL_ARGS
      ""
      CACHE STRING "Clang-tidy additional args"
  )
  set(CMAKE_CXX_CLANG_TIDY
      ${CLANG_TIDY} -checks=${CLANG_TIDY_CHECKS} ${CLANG_TIDY_CHECKS_ADDITIONAL_ARGS}
      CACHE INTERNAL "Clang-tidy for C++" FORCE
  )
  set(CMAKE_C_CLANG_TIDY
      ${CLANG_TIDY} -checks=${CLANG_TIDY_CHECKS} ${CLANG_TIDY_CHECKS_ADDITIONAL_ARGS}
      CACHE INTERNAL "Clang-tidy for C" FORCE
  )
else()
  set(CMAKE_CXX_CLANG_TIDY
      ""
      CACHE INTERNAL "Clang-tidy for C++" FORCE
  )
  set(CMAKE_C_CLANG_TIDY
      ""
      CACHE INTERNAL "Clang-tidy for C" FORCE
  )
endif()
