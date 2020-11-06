# TODO HAVE it generate XML of missed changes
# TODO Have it detect if git is installed and the repo is git based and if
# so have it add the ok

if(__add_clangformat)
  return()
endif()
set(__add_clangformat YES)

if(NOT clangFormat_FOUND)
  find_program(clangFormat_EXECUTABLE NAMES clang-format)
endif()

include(utils)

option(FORMAT_ENABLE_ALL "Add a project to force a format all files that should be formatted" ON)
option(FORMAT_ENABLE_CHANGES "Add a project to force a format files that were changed" ON)

function(_format_create_format_custom_cmd t_input_full t_input_name t_output)
  if(NOT TARGET ${t_output})
    add_custom_command(
      OUTPUT ${t_output}
      # MAIN_DEPENDENCY ${full_format_src}
      COMMAND ${clangFormat_EXECUTABLE} -style=file -i ${t_input_full}
      COMMAND ${CMAKE_COMMAND} -E touch ${t_output}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "FORMATING ${t_input_name}"
    )
  endif()
endfunction()

function(format_create_target_from_file_list t_format_targetname t_format_target_sources)
  set(options "ALL")
  set(oneValueArgs "")
  set(multiValueArgs "")
  # message(STATUS "t_format_target_sources (${t_format_targetname}) += ${t_format_target_sources}")
  cmake_parse_arguments(FROM_FILE_LIST_FORMAT_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(FROM_FILE_LIST_FORMAT_ARGS_ALL)
    set(_ALL ON)
    set(_format_basename format_all)
  else()
    set(_ALL OFF)
    set(_format_basename format)
  endif()
  foreach(format_src ${t_format_target_sources})
    get_filename_component(full_format_src ${format_src} ABSOLUTE)
    file(RELATIVE_PATH format_src ${CMAKE_CURRENT_SOURCE_DIR} ${full_format_src})
    set(format_src_speudo_path
        ${CMAKE_CURRENT_BINARY_DIR}/.format/${TARGET_FORMAT_ARGS_NAME}/${format_src}.${_format_basename}
    )

    get_source_file_property(_isGenerated ${format_src} GENERATED)
    if(_isGenerated)
      message(STATUS "Not formating generated file ${format_src}")
      continue()
    endif()

    if(format_src_speudo_path IN_LIST format_srcs)
      message(STATUS "Already in list ${format_src_speudo_path}")
      continue()
    endif()

    get_filename_component(dir "${format_src_speudo_path}" DIRECTORY)
    file(MAKE_DIRECTORY ${dir})

    _format_create_format_custom_cmd(${full_format_src} ${format_src} ${format_src_speudo_path})
    if(NOT _ALL)
      file(TOUCH ${format_src_speudo_path})
    endif()
    list(APPEND format_srcs ${format_src_speudo_path})
  endforeach()

  add_custom_target(
    ${_format_basename}_${TARGET_FORMAT_ARGS_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${format_srcs}
    COMMENT "Formating..."
  )

  if(TARGET ${_format_basename})
    add_dependencies(${_format_basename} ${_format_basename}_${TARGET_FORMAT_ARGS_NAME})
  else()
    add_custom_target(${_format_basename} DEPENDS ${_format_basename}_${TARGET_FORMAT_ARGS_NAME})
  endif()
endfunction(format_create_target_from_file_list)

function(target_format _target_name)

  if(NOT clangFormat_EXECUTABLE)
    message(WARNING "Please install Clang-Format")
  else()
    get_full_source_list_for_target(${_target_name} _format_target_sources ${ARGN})

    set(options "")
    set(oneValueArgs "NAME")
    set(multiValueArgs "")
    cmake_parse_arguments(TARGET_FORMAT_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT TARGET_FORMAT_ARGS_NAME)
      set(TARGET_FORMAT_ARGS_NAME ${_target_name})
    endif()

    if(FORMAT_ENABLE_ALL)
      format_create_target_from_file_list(${TARGET_FORMAT_ARGS_NAME} "${_format_target_sources}" ALL)
    else()
      message(STATUS "Not formatting all")
    endif()

    if(FORMAT_ENABLE_CHANGES)
      format_create_target_from_file_list(${TARGET_FORMAT_ARGS_NAME} "${_format_target_sources}")
    else()
      message(STATUS "Not formatting changes")
    endif()

  endif()
endfunction()
