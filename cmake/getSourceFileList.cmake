set(CMAKE_CXX_HEADER_FILE_EXTENSIONS ".hpp" ".tpp" ".h")
set(CMAKE_C_HEADER_FILE_EXTENSIONS ".h")

function(get_full_source_list_for_target t_target_name t_output_list)
  set(options NO_CURRENT_ONLY NO_ADD_INCLUDE_DIRS)
  set(oneValueArgs BASE_PATH)
  set(multiValueArgs EXCLUDE INCLUDE REGEX LANG)
  cmake_parse_arguments(TARGET_FORMAT_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  get_target_property(_target_sources_tmp ${t_target_name} SOURCES)

  foreach(format_src ${_target_sources_tmp})
    get_filename_component(full_format_src ${format_src} ABSOLUTE)
    list(APPEND _output_source_list ${full_format_src})
  endforeach()
  list(REMOVE_DUPLICATES _output_source_list)

#   message(STATUS "1. _output_source_list (${t_target_name})= ${_output_source_list}")

  get_target_property(target_include_dirs_tmp ${t_target_name} INCLUDE_DIRECTORIES)

  foreach(format_incDir ${target_include_dirs_tmp})
    get_filename_component(full_format_incDir ${format_incDir} ABSOLUTE)
    list(APPEND target_include_dirs ${full_format_incDir})
  endforeach()

  if(NOT NO_ADD_INCLUDE_DIRS)
#   get_target_property(_target_enabled_languages ${t_target_name} ENABLED_LANGUAGES)
#   foreach(_target_enabled_language ${_target_enabled_languages})
#       foreach(_lang ${_target_enabled_languages})
#           if(CMAKE_${}_SOURCE_FILE_EXTENSIONS)
#           list(APPEND _target_file_extension_regex)
#       endforeach(_target_enabled_language ${_target_enabled_languages})
#   endforeach(_target_enabled_language ${_target_enabled_languages})
#   message(FATAL_ERROR "CMAKE_CXX_HEADER_FILE_EXTENSIONS=${CMAKE_CXX_SOURCE_FILE_EXTENSIONS}")

    foreach(_target_include_dir ${target_include_dirs})
      file(GLOB_RECURSE target_header_files "${_target_include_dir}/*.h" "${_target_include_dir}/*.hpp" "${_target_include_dir}/*.tpp")
      list(APPEND _output_possibility_list ${target_header_files})
    endforeach(_target_include_dir ${target_include_dirs})
  endif()

#   message(STATUS "2. _output_possibility_list (${t_target_name})= ${_output_possibility_list}")

  list(REMOVE_DUPLICATES _output_possibility_list)

#   message(STATUS "3. _output_possibility_list (${t_target_name})= ${_output_possibility_list}")

  if(NOT TARGET_FORMAT_ARGS_NO_CURRENT_ONLY)
    set(tmp_list ${_output_possibility_list})
    list(FILTER tmp_list INCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR})
    list(APPEND _output_list ${tmp_list})
  endif()

#   message(STATUS "4. _output_list (${t_target_name})= ${_output_list}")

  if(TARGET_FORMAT_ARGS_INCLUDE)
    foreach(_inclRegex ${TARGET_FORMAT_ARGS_INCLUDE})
      set(tmp_list ${_output_possibility_list})
      list(FILTER tmp_list INCLUDE REGEX ${_inclRegex})
      list(APPEND _output_list ${tmp_list})
    endforeach()
  endif()
#   message(STATUS "5. _output_list (${t_target_name})= ${_output_list}")
  if(TARGET_FORMAT_ARGS_EXCLUDE)
    foreach(_exclRegex ${TARGET_FORMAT_ARGS_EXCLUDE})
      list(FILTER _output_list EXCLUDE REGEX ${_exclRegex})
    endforeach()
  endif()
#   message(STATUS "6. _output_list (${t_target_name})= ${_output_list}")

  list(APPEND _output_list ${_output_source_list})
  list(REMOVE_DUPLICATES _output_list)

  set(${t_output_list}
      ${_output_list}
      PARENT_SCOPE
  )
#   message(STATUS "7. _output_list (${t_target_name})= ${_output_list}")
endfunction()
