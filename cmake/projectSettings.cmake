# set(options ) set(oneValueArgs ) set(multiValueArgs ) cmake_parse_arguments(PROGADDEXEC "${options}" "${oneValueArgs}"
# "${multiValueArgs}" ${ARGN})

macro(_project_ParseCommonArg _project)
  set(options "")
  set(oneValueArgs PROJECT)
  set(multiValueArgs INTERFACE PUBLIC PRIVATE)
  cmake_parse_arguments(_PROJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(NOT _PROJ_PROJECT)
    set(_project ${PROJECT_NAME})
  else()
    # message(WARNING "_project_ParseCommonArg : _project 2=${_PROJ_PROJECT}")
    set(_project "${_PROG_PROJECT}")
  endif()

  if(_PROJ_PUBLIC)
    list(APPEND ${_PROJ_INTERFACE} ${_PROJ_PUBLIC})
    list(APPEND ${_PROJ_PRIVATE} ${_PROJ_PUBLIC})
  endif()
  # message(WARNING "_project_ParseCommonArg : _project = 1=${${_project}}")

  # message(WARNING "_ARGN : \"${ARGN}\" = \"${_PROJ_UNPARSED_ARGUMENTS}\"")
  set(ARGN ${_PROJ_UNPARSED_ARGUMENTS})
  # message(WARNING "_ARGN : \"${ARGN}\" = \"${_PROJ_UNPARSED_ARGUMENTS}\"")
endmacro()

macro(_project_ParsePRIVACYLISTArg)
  set(options "")
  set(oneValueArgs)
  set(multiValueArgs INTERFACE PUBLIC PRIVATE)
  cmake_parse_arguments(_PROJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(_PROJ_PUBLIC)
    list(APPEND ${_PROJ_INTERFACE} ${_PROJ_PUBLIC})
    list(APPEND ${_PROJ_PRIVATE} ${_PROJ_PUBLIC})
  endif()

  # message(WARNING "_ARGN : \"${ARGN}\" = \"${_PROJ_UNPARSED_ARGUMENTS}\"")
  set(ARGN ${_PROJ_UNPARSED_ARGUMENTS})
  # message(WARNING "_ARGN : \"${ARGN}\" = \"${_PROJ_UNPARSED_ARGUMENTS}\"")
endmacro()

macro(_project_ParsePrivacyPrecenceArg)
  set(options INTERFACE PUBLIC PRIVATE)
  set(oneValueArgs "")
  set(multiValueArgs "")
  cmake_parse_arguments(_PROJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(_PROJ_PUBLIC)
    set(_PROJ_INTERFACE ON)
    list(_PROJ_PRIVATE ON)
  endif()

  # message(WARNING "_ARGN : \"${ARGN}\" = \"${_PROJ_UNPARSED_ARGUMENTS}\"")
  set(ARGN ${_PROJ_UNPARSED_ARGUMENTS})
  # message(WARNING "_ARGN : \"${ARGN}\" = \"${_PROJ_UNPARSED_ARGUMENTS}\"")
endmacro()

function(project_GetProjectSetting _projectSetting)
  # message(WARNING "ARGN = ${ARGN}")
  _project_parsecommonarg(_project ${ARGN})

  # message(WARNING "_project = ${_project}")
  set(projectSettingName PROJECT_${_project}_SETTING)
  # message(WARNING "projectSettingName = ${projectSettingName}")
  set(${_projectSetting} ${projectSettingName})
  # message(WARNING "${_projectSetting} = ${projectSettingName}")

endfunction()

macro(_project_FetchProjectSetting _projectSetting)
  _project_parsecommonarg(_project ${ARGN})

  set(projectSettingName PROJECT_${_project}_SETTING)
  set(${_projectSetting} ${projectSettingName})

  if(NOT TARGET ${projectSettingName}_INTERFACE)
    message(STATUS "Creating Project Setting Interface target \"${projectSettingName}\"")
    add_library(${projectSettingName}_INTERFACE INTERFACE)
    add_library(${projectSettingName}_PRIVATE INTERFACE)
  endif()
endmacro()

function(project_set_property)
  _project_fetchprojectsetting(projectSetting ${ARGN})
  set_target_properties(${projectSetting}_INTERFACE ${ARGN})
endfunction()

function(project_compile_option)
  _project_fetchprojectsetting(projectSetting ${ARGN})
  message(STATUS "Setting compile options \"${ARGN}\" to \"${projectSetting}\"")
  target_compile_options(${projectSetting} ${ARGN})
endfunction()

function(project_compile_features)
  _project_fetchprojectsetting(projectSetting ${ARGN})
  _project_parseprivacylistarg(${ARGN})
  message(STATUS "Setting  compile features \"${ARGN}\" to \"${projectSetting}\"")
  # if(_PROJ_INTERFACE)
    target_compile_features(${projectSetting}_INTERFACE INTERFACE ${_PROJ_INTERFACE})
  # endif()
  # if(_PROJ_PRIVATE)
    target_compile_features(${projectSetting}_PRIVATE INTERFACE ${_PROJ_PRIVATE})
  # endif()
endfunction()

include(CompilerWarnings)
function(project_set_warnings)
  _project_fetchprojectsetting(projectSetting ${ARGN})
  set_project_warnings(${projectSetting}_PRIVATE)
endfunction()

function(project_compile_definitions)
  _project_fetchprojectsetting(projectSetting ${ARGN})
  message(STATUS "Setting  compile definitions \"${ARGN}\" to \"${projectSetting}\"")
  _project_parseprivacylistarg(${ARGN})
  message(STATUS "Setting  compile features \"${ARGN}\" to \"${projectSetting}\"")
  if(_PROJ_INTERFACE)
    target_compile_definitions(${projectSetting}_INTERFACE INTERFACE ${_PROJ_INTERFACE})
  endif()
  if(_PROJ_PRIVATE)
    target_compile_definitions(${projectSetting}_PRIVATE INTERFACE ${_PROJ_PRIVATE})
  endif()

endfunction()

function(project_add_target TARGET_NAME)
  _project_fetchprojectsetting(projectSetting ${ARGN})
  get_property(
    typ
    TARGET ${TARGET_NAME}
    PROPERTY TYPE
  )
  if(NOT typ STREQUAL "INTERFACE_LIBRARY")
    target_link_libraries(
      ${TARGET_NAME} INTERFACE ${projectSetting}_INTERFACE
                              PRIVATE ${projectSetting}_PRIVATE
    )
    message(STATUS "Adding dependencies ${projectSetting} to ${TARGET_NAME}")
  else()
  # message(WARNING "UNTESTED PATH  ${TARGET_NAME}") #This path is tested, but still less certain then some others, so need to double check
    target_link_libraries(
      ${TARGET_NAME} INTERFACE ${projectSetting}_INTERFACE)
  endif()
endfunction()

function(project_add_executable TARGET_NAME)
  add_executable(${TARGET_NAME} ${ARGN})
  project_add_target(${TARGET_NAME} ${ARGN})
endfunction()

function(project_add_library TARGET_NAME)
  add_library(${TARGET_NAME} ${ARGN})
  project_add_target(${TARGET_NAME} ${ARGN})
endfunction()

function(project_add_dependencies)
  _project_fetchprojectsetting(projectSetting ${ARGN})
  _project_parseprivacylistarg(${ARGN})
  list(APPEND _PROJ_PRIVATE ${ARGN})
  foreach(_dep ${_PROJ_PRIVATE})
    # message(STATUS "_dep = ${_dep}")
    set(_depprojectSetting PROJECT_${_dep}_SETTING)
    # project_getprojectsetting(_depprojectSetting PROJECT ${_dep}) message(WARNING "_depprojectSetting=
    # ${_depprojectSetting}")
    if(TARGET ${_depprojectSetting}_INTERFACE)
      target_link_libraries(${projectSetting}_PRIVATE INTERFACE ${_depprojectSetting}_INTERFACE)
      message(STATUS "Adding ${_depprojectSetting} as a deps to ${projectSetting}")
    else()
      message(FATAL_ERROR "${_dep} doesn't seem to be a valid project name ${_depprojectSetting}")
    endif()
  endforeach()

  foreach(_dep ${_PROJ_INTERFACE})
    # message(STATUS "_dep = ${_dep}")
    set(_depprojectSetting PROJECT_${_dep}_SETTING)
    # project_getprojectsetting(_depprojectSetting PROJECT ${_dep}) message(WARNING "_depprojectSetting=
    # ${_depprojectSetting}")
    if(TARGET ${_depprojectSetting}_INTERFACE)
      target_link_libraries(${projectSetting}_INTERFACE INTERFACE ${_depprojectSetting}_INTERFACE)
      message(STATUS "Adding ${_depprojectSetting} as a deps to ${projectSetting}")
    else()
      message(FATAL_ERROR "${_dep} doesn't seem to be a valid project name ${_depprojectSetting}")
    endif()
  endforeach()
endfunction()
