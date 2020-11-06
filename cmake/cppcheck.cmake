#TODO Use --file-list instead of --project
#TODO Merge XML files?
if(__add_cppcheck)
  return()
endif()
set(__add_cppcheck YES)

if(NOT CPPCHECK_FOUND)
  find_package(cppcheck QUIET)
endif()

if(CPPCHECK_FOUND)
  option(CPPCHECK_ENABLE_BUILDTIME_CHECKS "Add cppmake check running when we compile a file" ON)

  if(CPPCHECK_ENABLE_BUILDTIME_CHECKS)
    # message(STATUS "CPPCHECK found")
    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK_EXECUTABLE}
        CACHE INTERNAL "Path to CPPcheck for C++"
    )
    set(CMAKE_C_CPPCHECK
        ${CPPCHECK_EXECUTABLE}
        CACHE INTERNAL "Path to CPPcheck for C"
    )
    list(
      APPEND
      CMAKE_CXX_CPPCHECK
      "--enable=warning,style,performance,portability"
      "--inconclusive"
      "--inline-suppr"
      "--std=c++11"
      "-D__GNUC__"
      "--template=gcc"
      # "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
    )
    list(
      APPEND
      CMAKE_C_CPPCHECK
      "--enable=warning,style,performance,portability"
      "--inconclusive"
      "--inline-suppr"
      "--std=c11"
      "--template=gcc"
      "-D__GNUC__"
      # "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
    )
    # message(STATUS "CMAKE_CXX_CPPCHECK = ${CMAKE_CXX_CPPCHECK}")
    set(CMAKE_CXX_CPPCHECK
        ${CMAKE_CXX_CPPCHECK}
        CACHE INTERNAL "Path to CPPcheck for C++"
    )
    set(CMAKE_C_CPPCHECK
        ${CMAKE_C_CPPCHECK}
        CACHE INTERNAL "Path to CPPcheck for C"
    )
  endif()
endif()

if(CPPCHECK_FOUND)
  if(NOT TARGET all_cppcheck)
    add_custom_target(all_cppcheck)
  endif()
  if(NOT TARGET cppcheck)
    add_custom_target(cppcheck)
  endif()
endif()

function(_cppcheckBuildargs_for_target _target_name args)
  get_target_property(compile_defs ${_target_name} COMPILE_DEFINITIONS)
  get_target_property(includeDirectory ${_target_name} INCLUDE_DIRECTORIES)

  set(include_args)
  foreach(folder ${includeDirectory})
    list(APPEND include_args "-I${folder}")
  endforeach()

  set(compile_defs_args)
  foreach(def ${compile_defs})
    # message(STATUS "CPPCHECK Defines : ${def}")
    list(APPEND compile_defs_args "-D${def}")
  endforeach()

  set(${args}
      ${compile_defs_args} ${include_args}
      PARENT_SCOPE
  )
endfunction(_cppcheckBuildargs_for_target)

function(_cppcheckListSource_for_target _target_name _cppcheck_sources)
  get_target_property(target_sources "${_target_name}" SOURCES)
  set(cppcheck_sources)
  if(target_sources)
    foreach(source ${target_sources})
      get_source_file_property(path "${source}" LOCATION)
      get_source_file_property(lang "${source}" LANGUAGE)
      if("${lang}" MATCHES "CXX" OR "${lang}" MATCHES "C")
        list(APPEND cppcheck_sources "${path}")
      endif()
    endforeach()
  else()
    message(WARNING "Could not find a source file")
  endif()
  set(${_cppcheck_sources}
      ${cppcheck_sources}
      PARENT_SCOPE
  )
endfunction(_cppcheckListSource_for_target source)

function(target_cppcheck _target_name)

  if(NOT CPPCHECK_FOUND)
    message(WARNING "cppcheck not found! Static code analysis disabled.")
    return()
  endif()

  set(cppcheck_args)
  set(cppcheck_sources)
  # _cppcheckbuildargs_for_target(${_target_name} cppcheck_args)
  _cppchecklistsource_for_target(${_target_name} cppcheck_sources)

  set(_cppcheck_target_name cppcheck_${_target_name})
  set(_cppcheck_target_name_filename ${CMAKE_BINARY_DIR}/reports/cppcheck_${_target_name}.cppcheck.xml)

  if(cppcheck_sources)
    # foreach(_src ${cppcheck_sources})
    add_custom_command(
      OUTPUT ${_cppcheck_target_name_filename}
      COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/cppcheck-build-dir"
      COMMAND
        "${CPPCHECK_EXECUTABLE}" ${cppcheck_args} --cppcheck-build-dir="${CMAKE_BINARY_DIR}/cppcheck-build-dir"
        -D__GNUC__ --enable=all --inconclusive --inline-suppr --xml --xml-version=2
        --project=${CMAKE_BINARY_DIR}/compile_commands.json --output-file="${_cppcheck_target_name_filename}"
      COMMENT [${_target_name}][CPPCHECK] Generating cppcheck xml
      DEPENDS ${cppcheck_sources}
      COMMENT Building CPPCHeck database ${_cppcheck_target_name_filename}
    )
    # endforeach()

    add_custom_target(${_cppcheck_target_name} DEPENDS ${_cppcheck_target_name_filename})

    add_dependencies(cppcheck ${_cppcheck_target_name})
  else()
    message(WARNING "Could not find a source file, no CPPCHECK is applicable")
  endif()
endfunction(target_cppcheck)

function(add_test_cppcheck _target_name)
  if(NOT TARGET ${_target_name})
    message(FATAL_ERROR "add_test_cppcheck given a target name that does not exist: '${_target_name}' !")
  endif()

  if(NOT CPPCHECK_FOUND)
    message(WARNING "cppcheck not found! Static code analysis disabled.")
    return()
  endif()

  set(cppcheck_args)

  list(FIND ARGN UNUSED_FUNCTIONS index)
  if("${index}" GREATER "-1")
    list(APPEND cppcheck_args ${CPPCHECK_UNUSEDFUNC_ARG})
  endif()

  list(FIND ARGN STYLE index)
  if("${index}" GREATER "-1")
    list(APPEND cppcheck_args ${CPPCHECK_STYLE_ARG})
  endif()

  list(FIND ARGN POSSIBLE_ERROR index)
  if("${index}" GREATER "-1")
    list(APPEND cppcheck_args ${CPPCHECK_POSSIBLEERROR_ARG})
  endif()

  list(FIND ARGN MISSING_INCLUDE index)
  if("${index}" GREATER "-1")
    list(APPEND cppcheck_args ${CPPCHECK_MISSINGINCLUDE_ARG})
  endif()

  list(FIND _input FAIL_ON_WARNINGS index)
  if("${index}" GREATER "-1")
    list(APPEND CPPCHECK_FAIL_REGULAR_EXPRESSION ${CPPCHECK_WARN_REGULAR_EXPRESSION})
    list(REMOVE_AT _input ${_unused_func})
  endif()

  get_target_property(target_sources "${_target_name}" SOURCES)
  set(cppcheck_sources)
  foreach(source ${target_sources})
    get_source_file_property(path "${source}" LOCATION)
    get_source_file_property(lang "${source}" LANGUAGE)
    if("${lang}" MATCHES "CXX")
      list(APPEND cppcheck_sources "${path}")
    endif()
  endforeach()

  get_target_property(target_definitions "${_target_name}" SOURCES)

  set(test_target_name "${_target_name}_cppcheck_test")

  get_target_property(include_folders "${_target_name}" INCLUDE_DIRECTORIES)

  set(include_args)
  foreach(folder ${include_folders})
    list(APPEND include_args "-I${folder}")
  endforeach()

  add_test(NAME "${test_target_name}" COMMAND "${CPPCHECK_EXECUTABLE}" ${CPPCHECK_TEMPLATE_ARG} ${cppcheck_args}
                                              ${include_args} --project=${CMAKE_BINARY_DIR}/compile_commands.json
  )

  set_tests_properties("${test_target_name}" PROPERTIES FAIL_REGULAR_EXPRESSION "${CPPCHECK_FAIL_REGULAR_EXPRESSION}")

  add_custom_command(
    TARGET all_cppcheck
    PRE_BUILD
    COMMAND "${CPPCHECK_EXECUTABLE}" ${CPPCHECK_QUIET_ARG} ${CPPCHECK_TEMPLATE_ARG} ${cppcheck_args} ${include_args}
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "${test_target_name}: Running cppcheck on target ${_target_name}..."
  )

endfunction()
