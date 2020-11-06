find_package(Doxygen OPTIONAL_COMPONENTS dot)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

# function(doxygen_from_file target_name doxyfile_in) if(BUILD_DOCUMENTATION) if(NOT DOXYGEN_FOUND) message(FATAL_ERROR
# "Doxygen is needed to build the documentation.") endif() set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
# configure_file(${doxyfile_in} ${doxyfile} @ONLY)

# set(options "ALL") set(oneValueArgs "WORKING_DIRECTORY") set(multiValueArgs "COMMENT")
# cmake_parse_arguments(__DOXYGEN_FROM_FILE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

# if (__DOXYGEN_FROM_FILE_WORKING_DIRECTORY) set(__DOXYGEN_FROM_FILE_WORKING_DIRECTORY WORKING_DIRECTORY
# ${__DOXYGEN_FROM_FILE_WORKING_DIRECTORY}) endif()

# if (__DOXYGEN_FROM_FILE_ALL) set(__DOXYGEN_FROM_FILE_ALL "ALL") else() set(__DOXYGEN_FROM_FILE_ALL "") endif()

# if (NOT __DOXYGEN_FROM_FILE_COMMENT) set(__DOXYGEN_FROM_FILE_COMMENT COMMENT "Generating API documentation with
# Doxygen") endif()

# message(STATUS "__DOXYGEN_FROM_FILE_WORKING_DIRECTORY=${__DOXYGEN_FROM_FILE_WORKING_DIRECTORY}") message(STATUS
# "__DOXYGEN_FROM_FILE_COMMENT=${__DOXYGEN_FROM_FILE_COMMENT}") message(STATUS
# "__DOXYGEN_FROM_FILE_ALL=${__DOXYGEN_FROM_FILE_ALL}")

# add_custom_target(${target_name} COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_in} ${__DOXYGEN_FROM_FILE_ALL}
# ${__DOXYGEN_FROM_FILE_WORKING_DIRECTORY} ${__DOXYGEN_FROM_FILE_COMMENT} VERBATIM USES_TERMINAL)

# #    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc) endif() endfunction()

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(ERROR "Doxygen is needed to build the documentation.")
  else()
    # doxygen_from_file(doc ${CMAKE_CURRENT_SOURCE_DIR}/Documentation/doxygen/pex.doxyfile WORKING_DIRECTORY
    # ${CMAKE_CURRENT_SOURCE_DIR}/Documentation/doxygen )

    if(NOT TARGET docs)
      add_custom_target(docs)
    endif()

    set(DOXYGEN_CALLER_GRAPH TRUE)
    set(DOXYGEN_CALL_GRAPH TRUE)

    set(DOXYGEN_DOT_IMAGE_FORMAT "svg")
    set(DOXYGEN_INTERACTIVE_SVG TRUE)

    set(DOXYGEN_EXTRACT_ALL FALSE)
    set(DOXYGEN_EXTRACT_PRIVATE FALSE)
    set(DOXYGEN_EXTRACT_STATIC FALSE)
    set(DOXYGEN_EXTRACT_ANON_NSPACES FALSE)
    set(DOXYGEN_INTERNAL_DOCS TRUE)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C TRUE)
    set(DOXYGEN_EXCLUDE "Sources/CMSIS")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/${PROJECT_NAME}")
    # GENERATE_TESTLIST GENERATE_BUGLIST GENERATE_DEPRECATEDLIST

    # set(DOXYGEN_HTML_STYLESHEET ${CMAKE_CURRENT_SOURCE_DIR}/Documentation/doxygen/pex.css)
    doxygen_add_docs(docs-${PROJECT_NAME} ${PROJECT_SOURCE_DIR})
    install(DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}/html DESTINATION share/doc COMPONENT Documentation)

    add_dependencies(docs docs-${PROJECT_NAME})
  endif()
endif()
