# Get all subdirectories under ${current_dir} and store them in ${result} variable
macro(subdirlist result current_dir)
  file(GLOB children ${current_dir}/*)
  set(dirlist "")

  foreach(child ${children})
    if(IS_DIRECTORY ${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()

  set(${result} ${dirlist})
endmacro()

include(embeddedUtilities)

macro(GenerateTargetSettings)
  if(NOT TARGET target_settings)
    add_library(target_settings INTERFACE)
  endif()

  target_compile_options(target_settings INTERFACE ${CPU_FLAGS} $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>)

  target_link_options(target_settings INTERFACE ${CPU_FLAGS} ${LINKER_FLAGS})
endmacro()

include(flashUtilities)

include(getSourceFileList)
