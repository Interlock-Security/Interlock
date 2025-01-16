cmake_minimum_required(VERSION 3.10)

# Project name
project(WebinixLibrary)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Variables for library names, source files, etc.
set(WEBUI_OUT_LIB_NAME "webinix-2")

# Conditional compilation for TLS
option(WEBUI_USE_TLS "Enable TLS support" OFF)
if(WEBUI_USE_TLS)
  find_package(OpenSSL REQUIRED)
  set(WEBUI_OUT_LIB_NAME "webinix-2-secure")
endif()

# Source files (already filled)
set(SOURCE_FILES
    src/civetweb/civetweb.c
    src/webinix.c
)

# Library targets
add_library(webinix ${SOURCE_FILES})
target_include_directories(webinix PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_compile_definitions(webinix PUBLIC NDEBUG NO_CACHING NO_CGI USE_WEBSOCKET)

if(BUILD_SHARED_LIBS AND WIN32)
    target_compile_definitions(webinix PRIVATE CIVETWEB_DLL_EXPORTS PUBLIC CIVETWEB_DLL_IMPORTS)
endif()

if(WEBUI_USE_TLS)
    target_compile_definitions(webinix PUBLIC WEBUI_TLS NO_SSL_DL OPENSSL_API_1_1)
    target_link_libraries(webinix PRIVATE OpenSSL::SSL OpenSSL::Crypto)
else()
    target_compile_definitions(webinix PUBLIC NO_SSL)
endif()

set_target_properties(webinix PROPERTIES
    OUTPUT_NAME ${WEBUI_OUT_LIB_NAME}
    PREFIX "")

# Install headers
install(FILES include/webinix.h include/webinix.hpp DESTINATION include)

# Install targets
install(TARGETS webinix
    EXPORT webinix
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(EXPORT webinix
    FILE webinix-config.cmake
    NAMESPACE webinix::
    DESTINATION share/webinix
)
