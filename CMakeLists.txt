cmake_minimum_required(VERSION 3.18)

# Project name
project(WebinixLibrary 
    VERSION 2.5.0
    DESCRIPTION "Use any web browser or WebView as GUI, with your preferred language in the backend and modern web technologies in the frontend, all in a lightweight portable library."
    HOMEPAGE_URL "https://webinix.me/")

# Set C & C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Variables for library names, source files, etc.
set(WEBUI_DEFAULT_OUT_LIB_NAME "webinix-2")

# Conditional compilation for TLS
option(WEBUI_USE_TLS "Enable TLS support" OFF)
if(WEBUI_USE_TLS)
    find_package(OpenSSL REQUIRED)
    set(WEBUI_DEFAULT_OUT_LIB_NAME "webinix-2-secure")
endif()

if(NOT BUILD_SHARED_LIBS)
    set(WEBUI_DEFAULT_OUT_LIB_NAME "${WEBUI_DEFAULT_OUT_LIB_NAME}-static")
endif()

# Output library name
set(WEBUI_OUT_LIB_NAME "${WEBUI_DEFAULT_OUT_LIB_NAME}" CACHE STRING "Name of the output library")

# Source files (already filled)
set(SOURCE_FILES
    src/civetweb/civetweb.c
    src/webinix.c
)
if (APPLE)
    # enable macos webview
    enable_language(OBJC)
    set(CMAKE_OBJC_STANDARD 11)
    list(APPEND SOURCE_FILES src/webview/wkwebview.m)
endif()

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

if (WIN32)
    target_link_libraries(webinix PRIVATE ws2_32 user32 shell32 ole32)
elseif(APPLE)
    # link required frameworks
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(WEBKIT_FRAMEWORK WebKit REQUIRED)

    target_link_libraries(webinix PRIVATE ${COCOA_FRAMEWORK} ${WEBKIT_FRAMEWORK})
endif()

set_target_properties(webinix PROPERTIES
    OUTPUT_NAME ${WEBUI_OUT_LIB_NAME})

# Install headers
install(FILES include/webinix.h include/webinix.hpp DESTINATION include)

# Install targets
install(TARGETS webinix
    EXPORT webinix
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(EXPORT webinix
    FILE webinix-config.cmake
    NAMESPACE webinix::
    DESTINATION share/webinix
)
