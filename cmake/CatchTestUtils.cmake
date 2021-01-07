if(__add_catchtestutils)
  return()
endif()
set(__add_catchtestutils YES)

include(CompilerWarnings)
include(Findcodecov)

include(CMakeDependentOption)

option(ENABLE_TESTING "Enable Test Builds" OFF)

if(ENABLE_TESTING)
  option(TEST_JUNIT_FORMAT "CREATE" OFF)
  if(TEST_JUNIT_FORMAT)
    set(TEST_XML_FORMAT "junit")
  else()
    set(TEST_XML_FORMAT "xml")
  endif()
endif()

macro(Test_Add_Test_Folder)
  if(NOT TARGET catch_main)
    cmake_dependent_option(ENABLE_TESTING_${PROJECT_NAME} "Enable ${PROJECT_NAME} Test Builds" OFF "ENABLE_TESTING" ON)
    if(ENABLE_TESTING_${PROJECT_NAME})
      if(CMAKE_CROSSCOMPILING)
        include(ExternalProject)
        ExternalProject_Add(
          tests
          SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests
          UPDATE_COMMAND ${CMAKE_COMMAND} -E true
          CMAKE_ARGS "-G${CMAKE_GENERATOR}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                     "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
                     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} -DTEST_XML_FORMAT:STRING=${TEST_XML_FORMAT}"
          INSTALL_COMMAND ""
          TEST_BEFORE_INSTALL true
        )
      else()
        enable_testing()
        add_subdirectory(tests)
      endif()
    endif()
  endif()
endmacro(Test_Add_Test_Folder)

macro(Test_Utils_Init)
  # FIXME This need to be passed to the cmd line, it seems, needs to be validated...
  if(NOT TEST_XML_FORMAT)
    set(TEST_XML_FORMAT "junit") # TODO Prefer XML, but it doesn't work well with devops
  endif()

  project_add_library(catch_main STATIC catch_main.cpp)

  target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)

endmacro()

function(Add_Unit_Test SOURCE_FILE)
  string(LENGTH ${PROJECT_SOURCE_DIR} PROJECT_DIR_LENGHT)
  string(SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_DIR_LENGHT} -1 REL_PATH_TO_PROJECT)
  get_filename_component(SOURCE_FILE_NOEXT ${SOURCE_FILE} NAME_WE)
  get_filename_component(SOURCE_FILE_DIR ${SOURCE_FILE} DIRECTORY)
  string(MAKE_C_IDENTIFIER "${REL_PATH_TO_PROJECT}/${SOURCE_FILE_DIR}/${SOURCE_FILE_NOEXT}" TEST_IDENTIFIER)
  set(options "")
  set(oneValueArgs "")
  set(multiValueArgs DEP_LIBS)
  cmake_parse_arguments(_ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_executable(${TEST_IDENTIFIER} ${SOURCE_FILE})
  target_link_libraries(${TEST_IDENTIFIER} PRIVATE catch_main ${__DEP_LIBS})
  set_project_warnings(${TEST_IDENTIFIER})

  # automatically discover tests that are defined in catch based test files you can modify the unittests. TEST_PREFIX to
  # whatever you want, or use different for different binaries
  catch_discover_tests(
    ${TEST_IDENTIFIER}
    TEST_PREFIX
    "unittests."
    EXTRA_ARGS
    -s
    --reporter=${TEST_XML_FORMAT}
    --out=${TEST_IDENTIFIER}.unit.xml
  )
  add_coverage(${TEST_IDENTIFIER})


  # # Add a file containing a set of constexpr tests add_executable(constexpr_tests constexpr_tests.cpp)
# target_link_libraries(constexpr_tests PRIVATE project_options project_warnings catch_main)

# catch_discover_tests( constexpr_tests TEST_PREFIX "constexpr." EXTRA_ARGS -s --reporter=xml --out=constexpr.xml)

# # Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug #
# when things go wrong with the constexpr testing add_executable(relaxed_constexpr_tests constexpr_tests.cpp)
# target_link_libraries(relaxed_constexpr_tests PRIVATE project_options project_warnings catch_main)
# target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
#
# catch_discover_tests( relaxed_constexpr_tests TEST_PREFIX "relaxed_constexpr." EXTRA_ARGS -s --reporter=xml
# --out=relaxed_constexpr.xml )

# target_link_libraries(catch_main PRIVATE project_options)
endfunction(Add_Unit_Test)


