include(CMakeDependentOption)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# FIXME UPDATE TO USE ARGN instead of FLAG or something
macro(add_if_warning_is_supported ADD_TO LANG)
  get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
  foreach(FLAG ${ARGN})
    #message(STATUS "Test for \"${FLAG}\"")
    string(MAKE_C_IDENTIFIER "${FLAG}_IS_SUPPORTED" FLAG_STRING_NICE)
    if("${LANG}" STREQUAL "C" )
      if("C" IN_LIST languages)
      check_c_compiler_flag(${FLAG} ${FLAG_STRING_NICE})
      endif()
    elseif("${LANG}" STREQUAL "CXX" )
      if ("CXX" IN_LIST languages)
      check_cxx_compiler_flag(${FLAG} ${FLAG_STRING_NICE})
      endif()
    else()
      message(FATAL_ERROR "LANGUAGE ${LANG} IS NOT SUPORTED")
    endif()
    # message(FATAL_ERROR "FLAG_STRING_NICE=${FLAG_STRING_NICE}=${FLAG_STRING_NICE}")
    if(${FLAG_STRING_NICE})
      list(APPEND ${ADD_TO} ${FLAG})
      #message(STATUS "${FLAG} SUPPORTED")
    else()
     # message(STATUS "${FLAG} UNsupported")
    endif()
  endforeach()
endmacro()

function(set_project_warnings _target_name)
  option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
  option(WARNINGS_MOST "Build with most warning turned on" ON)
  cmake_dependent_option(WARNINGS_ALL "Build with all warning turned on" ON "MOST_WARNING" OFF)

  list(APPEND CPP_WARNING_FLAGS)
  list(APPEND WARNING_FLAGS)
  list(APPEND C_ONLY_ERRORS)

  add_if_warning_is_supported(
    WARNING_FLAGS
    C
    -Wall
    # -Werror=missing-field-initializers
    -Wpedantic
    -Wunused
    -Wextra
    -Wshadow
    -Werror=return-type
    -Wfloat-equal
    -Wcast-align
    # -Werror=int-conversion
  )

  # add_if_warning_is_supported(WARNING_FLAGS C -Wmisleading-indentation -Werror=stringop-overflow -Wmissing-attributes)
  # add_if_warning_is_supported(WARNING_FLAGS C -Werror=int-conversion) #SEEM NOT SUPPORTED IN C++
  # add_if_warning_is_supported(WARNING_FLAGS C -Wsuggest-attribute=pure)
  # add_if_warning_is_supported(WARNING_FLAGS C -Wsuggest-attribute=const)
  # add_if_warning_is_supported(WARNING_FLAGS C -Wsuggest-attribute=noreturn)
  # add_if_warning_is_supported(WARNING_FLAGS C -Wsuggest-attribute=malloc)
  # add_if_warning_is_supported(WARNING_FLAGS C -Wsuggest-attribute=cold)

  add_if_warning_is_supported(
    C_ONLY_ERRORS C -Werror=incompatible-pointer-types -Werror=implicit-function-declaration
    -Werror=unused-but-set-parameter
  )

  add_if_warning_is_supported(CPP_WARNING_FLAGS CXX -Wctor-dtor-privacy -Wnon-virtual-dtor -Wstrict-null-sentinel)

  if(WARNINGS_MOST)
    add_if_warning_is_supported(
      WARNING_FLAGS
      C
      -Wstring-compare
      -Wnull-dereference
      -Wduplicated-cond
      -Wduplicated-branches
      -Wmissing-declarations
      -Wpointer-arith
      # -Wabi
      -Wcast-qual
      -Wdisabled-optimization
      -Wformat=2
      -Winit-self # -WALL has Winit-self for C++
      -Wmissing-include-dirs
      -Wredundant-decls
      -Wstrict-overflow=5
      -Wundef
      -Wno-parentheses
      -Wstring-compare
      -Wnull-dereference
      -Wduplicated-cond
      -Wduplicated-branches
      -Wlogical-op
    )

    add_if_warning_is_supported(
      CPP_WARNING_FLAGS
      CXX
      -Wnoexcept
      -Wsign-promo
      -Woverloaded-virtual
      -Wold-style-cast
      -Wuseless-cast
    )
  endif()

  if(WARNINGS_ALL)
    add_if_warning_is_supported(WARNING_FLAGS C -Wconversion -Wsign-conversion -Wpadded -Wdouble-promotion)
  endif()

  if(WARNINGS_AS_ERRORS)
    add_if_warning_is_supported(WARNING_FLAGS C -Werror)
  endif()

  #message(STATUS "WARNING FLAGS       = ${WARNING_FLAGS}")
  #message(STATUS "CPP_WARNING_FLAGS   = ${CPP_WARNING_FLAGS}")
  #message(STATUS "C_ONLY_ERRORS FLAGS = ${C_ONLY_ERRORS}")

  target_compile_options(
    ${_target_name} INTERFACE ${WARNING_FLAGS} $<$<COMPILE_LANGUAGE:CXX>:${CPP_WARNING_FLAGS}>
                           $<$<COMPILE_LANGUAGE:C>:${C_ONLY_ERRORS}>
  )
endfunction()
