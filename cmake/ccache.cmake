find_program(CCACHE ccache)
cmake_dependent_option(CCACHE_ACTIVATE "Use CCache to potential speed up re-compile time" ON "CCACHE" OFF)
if(CCACHE_ACTIVATE)
  message("CCACHE : In use")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})

  cmake_dependent_option(CCACHE_INFO_TARGET "Create custom target for ccache" OFF "CCACHE" OFF)
  if(CCACHE_INFO_TARGET)
    add_custom_target(
      CCACHE_STATS
      COMMAND ${CCACHE} -s
      COMMENT "[CCACHE] stats"
      USES_TERMINAL
    )

    add_custom_target(
      CCACHE_CLEAR
      COMMAND ${CCACHE} -c
      COMMENT "[CCACHE] clear cache"
      USES_TERMINAL
    )
  endif()

elseif(CCACHE)
  message("CCACHE Not Activate; Was found but requested to not be used")
else ()
  message("CCACHE not found; cannot use")
endif()