cmake_minimum_required(VERSION 3.13.0)

# --[ Webinix version ] --------------------------------------------------------------

project(webinix VERSION 1.0.20 DESCRIPTION 
"Use your web browser as UI, and bring the web technologies to your app.")
message("-- [ Webinix version ${CMAKE_PROJECT_VERSION} ]")

# --[ Boost ] ----------------------------------------------------------------------

set(Boost_Version_Needed "1.76.0")
set(Boost_NO_WARN_NEW_VERSIONS ON)

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(EXISTS "C:/Alif3/")
            # Windows - Boost
            message("[*] Using Boost embeded with Alif v3 'C:/Alif3/boost'")
            set(Boost_Root "C:/Alif3/boost/include")
            set(Boost_Lib "C:/Alif3/boost/lib")
            set(BOOST_ROOT ${Boost_Root})
            set(Boost_LIBRARY_DIR ${Boost_Lib})
            message("[*] Using Boost root: ${BOOST_ROOT}")
            message("[*] Using Boost lib: ${Boost_LIBRARY_DIR}")
        endif()
    endif() 
endif()

# --[ C++ ] ------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --[ Targets ] --------------------------------------------------------------------

if (WIN32)
	set(webinix_lib "webinix_win_x86_64")
	set(cwebinix_lib "cwebinix_win_x86_64")
elseif(UNIX)
	set(webinix_lib "webinix_linux_x86_64")
	set(cwebinix_lib "cwebinix_linux_x86_64")
else()
	set(webinix_lib "webinix_macos_x86_64")
	set(cwebinix_lib "cwebinix_macos_x86_64")
endif()

# --[ Directories ] ----------------------------------------------------------------

# Webinix Library
add_subdirectory(${webinix_SOURCE_DIR}/src webinix)

# Examples
add_subdirectory(${webinix_SOURCE_DIR}/examples)

# cWebinix Library
# if(TARGET cwebinix)
    add_subdirectory(${webinix_SOURCE_DIR}/cwebinix EXCLUDE_FROM_ALL)
# endif()

# Test
# include(CTest)
# enable_testing()
# add_subdirectory(${webinix_SOURCE_DIR}/test)
