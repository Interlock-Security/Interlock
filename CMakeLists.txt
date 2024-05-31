cmake_minimum_required(VERSION 3.10)

# Project name
project(WebinixLibrary)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Variables for library names, source files, etc.
set(WEBUI_OUT_LIB_NAME "webinix-2")

# Platform and compiler specific settings
if(WIN32)

  # Windows specific compiler flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DNO_SSL")

  # Add Windows specific source files or flags here
  # Example: set(SOURCE_FILES ${SOURCE_FILES} additional_windows_sources.c)

elseif(UNIX)
  # Linux specific settings

  # Linux specific compiler flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_SSL -w")

  # Add Linux specific source files or flags here
  # Example: set(SOURCE_FILES ${SOURCE_FILES} additional_linux_sources.c)
endif()

# Conditional compilation for TLS
option(WEBUI_USE_TLS "Enable TLS support" OFF)
if(WEBUI_USE_TLS)
  # Add definitions and libraries for TLS
  add_definitions(-DWEBUI_TLS)

  if(WIN32)
    # Windows TLS settings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DWEBUI_TLS /DNO_SSL_DL /DOPENSSL_API_1_1")
    # Specify the TLS library for Windows, if needed
    # Example: target_link_libraries(webinix_static PRIVATE ssl crypto)
  else()
    # Linux TLS settings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWEBUI_TLS -DNO_SSL_DL -DOPENSSL_API_1_1")
    # Specify the TLS library for Linux, if needed
    # Example: target_link_libraries(webinix_static PRIVATE ssl crypto)
  endif()
endif()

# Include directories
include_directories(include bridge)
add_subdirectory(src/civetweb)

# Source files (already filled)
set(SOURCE_FILES
    src/webinix.c
    bridge/webinix_bridge.h
)

# Library targets (static and dynamic)
add_library(webinix_static STATIC ${SOURCE_FILES})
set_target_properties(webinix_static PROPERTIES
    OUTPUT_NAME ${WEBUI_OUT_LIB_NAME}-static
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist")
target_link_libraries(webinix_static PRIVATE civetweb)

add_library(webinix_dynamic SHARED ${SOURCE_FILES})
set_target_properties(webinix_dynamic PROPERTIES
    OUTPUT_NAME ${WEBUI_OUT_LIB_NAME}
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist")
target_link_libraries(webinix_dynamic PRIVATE civetweb)

# Install targets
install(TARGETS webinix_static webinix_dynamic
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
