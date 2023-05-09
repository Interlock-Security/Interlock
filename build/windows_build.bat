@echo off

set WEBUI_VERSION=2.3.0

set ARG1=%1
IF "%ARG1%"=="debug" (
    set MSVC_CMD=nmake debug
    set GCC_CMD=mingw32-make debug
) else (
    set MSVC_CMD=nmake
    set GCC_CMD=mingw32-make
)

echo.
echo Webinix v%WEBUI_VERSION% Build Script
echo Platform: Microsoft Windows x64
echo Compiler: MSVC, GCC and TCC

Set RootPath=%CD%\..\
cd "%RootPath%"

echo.
echo Building Webinix using MSVC...

REM Build Webinix Library using MSVC
cd "%RootPath%"
cd "build\Windows\MSVC"
%MSVC_CMD%

echo.
echo Building Webinix using GCC...
echo.

REM Build Webinix Library using GCC
cd "%RootPath%"
cd "build\Windows\GCC"
%GCC_CMD%

echo.
echo Building Webinix using TCC...
echo.

REM Build Webinix Library using TCC
cd "%RootPath%"
cd "build\Windows\TCC"
%GCC_CMD%

echo.
echo Copying Webinix libs to the examples folder...
echo.

cd "%RootPath%"

REM Golang
copy /Y "include\webinix.h" "examples\Go\hello_world\webinix\webinix.h"
copy /Y "build\Windows\GCC\libwebinix-2-static-x64.a" "examples\Go\hello_world\webinix\libwebinix-2-static-x64.a"

REM Deno
copy /Y "build\Windows\MSVC\webinix-2-x64.dll" "examples\TypeScript\Deno\webinix-2-x64.dll"

REM Python
copy /Y "build\Windows\MSVC\webinix-2-x64.dll" "examples\Python\PyPI\Package\src\webinix\webinix-2-x64.dll"

REM C++ (Visual Studio 2022)
copy /Y "include\webinix.h" "examples\C++\VS2022\serve_a_folder\my_webinix_app\webinix.h"
copy /Y "include\webinix.hpp" "examples\C++\VS2022\serve_a_folder\my_webinix_app\webinix.hpp"
copy /Y "build\Windows\MSVC\webinix-2-static-x64.lib" "examples\C++\VS2022\serve_a_folder\my_webinix_app\webinix-2-static-x64.lib"

REM C++ (Visual Studio 2019)
copy /Y "include\webinix.h" "examples\C++\VS2019\serve_a_folder\my_webinix_app\webinix.h"
copy /Y "include\webinix.hpp" "examples\C++\VS2019\serve_a_folder\my_webinix_app\webinix.hpp"
copy /Y "build\Windows\MSVC\webinix-2-static-x64.lib" "examples\C++\VS2019\serve_a_folder\my_webinix_app\webinix-2-static-x64.lib"

REM C - Text Editor
copy /Y "include\webinix.h" "examples\C\text-editor\webinix.h"
copy /Y "build\Windows\MSVC\webinix-2-x64.dll" "examples\C\text-editor\webinix-2-x64.dll"

echo.
IF "%ARG1%"=="" (

    echo Copying Webinix libs to the release folder...
    echo.

    REM Release Windows Include
    copy /Y "include\webinix.h" "Release\Windows\include\webinix.h"    

    REM Release Windows MSVC
    copy /Y "build\Windows\MSVC\webinix-2-x64.dll" "Release\Windows\MSVC\webinix-2-x64.dll"
    copy /Y "build\Windows\MSVC\webinix-2-x64.lib" "Release\Windows\MSVC\webinix-2-x64.lib"
    copy /Y "build\Windows\MSVC\webinix-2-static-x64.lib" "Release\Windows\MSVC\webinix-2-static-x64.lib"

    REM Release Windows GCC
    copy /Y "build\Windows\GCC\webinix-2-x64.dll" "Release\Windows\GCC\webinix-2-x64.dll"
    copy /Y "build\Windows\GCC\libwebinix-2-static-x64.a" "Release\Windows\GCC\libwebinix-2-static-x64.a"

    REM Release Windows TCC
    REM copy /Y "build\Windows\TCC\webinix-2-x64.dll" "Release\Windows\TCC\webinix-2-x64.dll"
    REM copy /Y "build\Windows\TCC\webinix-2-x64.def" "Release\Windows\TCC\webinix-2-x64.def"    
    copy /Y "build\Windows\TCC\libwebinix-2-static-x64.a" "Release\Windows\TCC\libwebinix-2-static-x64.a"

    echo.
    echo Compressing the release folder...

    set TAR_OUT=webinix-windows-x64-v%WEBUI_VERSION%.zip
    cd "Release"
    timeout 2 > NUL
    tar.exe -c -f %TAR_OUT% Windows\*
    cd "%RootPath%"

    echo.
    echo Cleaning...

    DEL /Q /F /S "*.exe" >nul 2>&1
    DEL /Q /F /S "*.o" >nul 2>&1
    DEL /Q /F /S "*.exp" >nul 2>&1
    DEL /Q /F /S "*.pdb" >nul 2>&1
    DEL /Q /F /S "*.ilk" >nul 2>&1
    DEL /Q /F /S "*.obj" >nul 2>&1
    DEL /Q /F /S "*.iobj" >nul 2>&1
    DEL /Q /F /S "*.res" >nul 2>&1
    DEL /Q /F /S "*.bak" >nul 2>&1
    DEL /Q /F /S "*.DS_Store" >nul 2>&1
)

cd "build"
