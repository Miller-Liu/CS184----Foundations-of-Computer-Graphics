# Original file Copyright CMU462 Fall 2015: 
# Kayvon Fatahalian, Keenan Crane,
# Sky Gao, Bryce Summers, Michael Choquette.
cmake_minimum_required(VERSION 3.18)

include_directories(${PROJECT_SOURCE_DIR}/include/CGL)

# CGL library source files
set(CGL_SOURCE
    vector2D.cpp
    vector3D.cpp
    vector4D.cpp
    matrix3x3.cpp
    matrix4x4.cpp
    quaternion.cpp
    complex.cpp
    color.cpp
    spectrum.cpp
    osdtext.cpp
    osdfont.cpp
    viewer.cpp
    base64.cpp
    lodepng.cpp
    tinyxml2.cpp
    path.cpp
)

#-------------------------------------------------------------------------------
# Building static library (always)
#-------------------------------------------------------------------------------
add_library(CGL STATIC ${CGL_SOURCE})

target_link_libraries(
  CGL
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${FREETYPE_LIBRARIES}
)

#-------------------------------------------------------------------------------
# Building shared library
#-------------------------------------------------------------------------------
if(CGL_BUILD_SHARED)
  add_library(CGL_SHARED SHARED ${CGL_SOURCE})
  target_link_libraries(
    CGL_SHARED
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
  )
endif()

#-------------------------------------------------------------------------------
# Platform-specific target configurations
#-------------------------------------------------------------------------------

###################
# Building on OSX #
###################
if (APPLE)

  # Clang options
  target_compile_options(CGL PRIVATE -Wno-constant-conversion)

  # Framework dependencies
  target_link_libraries(CGL
    ${COCOA_LIBRARIES}
    ${IOKIT_LIBRARIES}
    ${COREVIDEO_LIBRARIES})

  # Output name
  if(CGL_BUILD_DEBUG)
    set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL_osx_d)
    if(CGL_BUILD_SHARED)
      set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL_osx_d)
    endif()
  else(CGL_BUILD_DEBUG)
    set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL_osx)
    if(CGL_BUILD_SHARED)
      set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL_osx)
    endif()
  endif(CGL_BUILD_DEBUG)

endif(APPLE)

##################
# Build on Linux #
##################
if(UNIX AND NOT APPLE)
  if(CGL_BUILD_DEBUG)
    set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL_d)
    if(CGL_BUILD_SHARED)
      set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL_d)
    endif()
  else(CGL_BUILD_DEBUG)
    set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL)
    if(CGL_BUILD_SHARED)
      set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL)
    endif()
  endif(CGL_BUILD_DEBUG)
endif()

####################
# Build on Windows #
####################
if(WIN32)

  if(MSVC)
    if(CGL_BUILD_DEBUG)
      set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL_d)
      if(CGL_BUILD_SHARED)
        set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL_d)
      endif()
    else(CGL_BUILD_DEBUG)
      set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL)
      if(CGL_BUILD_SHARED)
        set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL)
      endif()
    endif(CGL_BUILD_DEBUG)
  endif(MSVC)

  if(MINGW)
    if(CGL_BUILD_DEBUG)
      set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL_d)
      if(CGL_BUILD_SHARED)
        set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL_d)
      endif()
    else(CGL_BUILD_DEBUG)
      set_target_properties(CGL PROPERTIES OUTPUT_NAME CGL)
      if(CGL_BUILD_SHARED)
        set_target_properties(CGL_SHARED PROPERTIES OUTPUT_NAME CGL)
      endif()
    endif(CGL_BUILD_DEBUG)
  endif(MINGW)

endif(WIN32)

#-------------------------------------------------------------------------------
# Installation configurations
#-------------------------------------------------------------------------------
install(TARGETS CGL DESTINATION lib)
if(CGL_BUILD_SHARED)
  install(TARGETS CGL_SHARED DESTINATION lib)
endif()

# install headers
install(FILES ${CGL_HEADER} DESTINATION include/CGL)
