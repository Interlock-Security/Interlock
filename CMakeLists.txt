cmake_minimum_required(VERSION 3.13.0)

# --[ Webinix version ] --------------------------------------------------------------

project(webinix VERSION 1.0.7 DESCRIPTION 
"Use your web browser as GUI, and bring web technologies to your desktop app.")
message("-- [ Webinix version ${CMAKE_PROJECT_VERSION} ]")

# --[ Boost ] ----------------------------------------------------------------------

set(Boost_Version_Needed "1.70.0")

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        # Boost GCC/MinGW
        # Example: C:/boost_1_60_0
        set(Boost_Root "C:/boost_1_75_0_mingw")
        set(Boost_Lib "C:/boost_1_75_0_mingw/stage/lib")

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        # Boost Microsoft C++
        # Example: C:/boost_1_60_0
        set(Boost_Root "C:/boost_1_75_0")
        set(Boost_Lib "C:/boost_1_75_0/lib64-msvc-14.2")

    endif()
endif()

# --[ C++ ] ------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --[ Directories ] ----------------------------------------------------------------

# Webinix Library
add_subdirectory(${webinix_SOURCE_DIR}/src webinix)

# Examples
add_subdirectory(${webinix_SOURCE_DIR}/examples)

# PyWebinix Library
# if(TARGET pywebinix)
    add_subdirectory(${webinix_SOURCE_DIR}/pywebinix EXCLUDE_FROM_ALL)
# endif()

# Test
# include(CTest)
enable_testing()
add_subdirectory(${webinix_SOURCE_DIR}/test)
