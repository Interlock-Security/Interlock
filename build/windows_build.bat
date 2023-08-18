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

Set RootPath=%CD%\..
Set BuildPath=%RootPath%\build\Windows
Set DistPath=%RootPath%\dist\Windows
cd "%RootPath%"

echo.
echo Building Webinix using MSVC...

REM Build Webinix Library using MSVC
cd "%BuildPath%\MSVC"
%MSVC_CMD%

echo.
echo Building Webinix using GCC...
echo.

REM Build Webinix Library using GCC
cd "%BuildPath%\GCC"
%GCC_CMD%

echo.
echo Building Webinix using TCC...
echo.

REM Build Webinix Library using TCC
cd "%BuildPath%\TCC"
%GCC_CMD%

echo.
echo Copying Webinix libs to the examples folder...
echo.

cd "%RootPath%"

REM C++ (Visual Studio 2022)
copy /Y "include\webinix.h" "examples\C++\VS2022\serve_a_folder\my_webinix_app\webinix.h"
copy /Y "include\webinix.hpp" "examples\C++\VS2022\serve_a_folder\my_webinix_app\webinix.hpp"
copy /Y "%BuildPath%\MSVC\webinix-2-static-x64.lib" "examples\C++\VS2022\serve_a_folder\my_webinix_app\webinix-2-static-x64.lib"

REM C++ (Visual Studio 2019)
copy /Y "include\webinix.h" "examples\C++\VS2019\serve_a_folder\my_webinix_app\webinix.h"
copy /Y "include\webinix.hpp" "examples\C++\VS2019\serve_a_folder\my_webinix_app\webinix.hpp"
copy /Y "%BuildPath%\MSVC\webinix-2-static-x64.lib" "examples\C++\VS2019\serve_a_folder\my_webinix_app\webinix-2-static-x64.lib"

REM C - Text Editor
copy /Y "include\webinix.h" "examples\C\text-editor\webinix.h"
copy /Y "%BuildPath%\MSVC\webinix-2-x64.dll" "examples\C\text-editor\webinix-2-x64.dll"

echo.
IF "%ARG1%"=="" (

    echo Copying Webinix libs to %DistPath%...
    echo.

    REM Remove Windows distributable files directory if it exits
    if exist "%DistPath%" rmdir /s /q "%DistPath%"

    REM Create Windows output directories
    mkdir "%DistPath%\include" 2>nul
    mkdir "%DistPath%\MSVC" 2>nul
    mkdir "%DistPath%\GCC" 2>nul

    REM Copy include files
    copy /Y "include\webinix.h" "%DistPath%\include\webinix.h"

    REM Copy Windows MSVC
    copy /Y "%BuildPath%\MSVC\webinix-2-x64.dll" "%DistPath%\MSVC\webinix-2-x64.dll"
    copy /Y "%BuildPath%\MSVC\webinix-2-x64.lib" "%DistPath%\MSVC\webinix-2-x64.lib"
    copy /Y "%BuildPath%\MSVC\webinix-2-static-x64.lib" "%DistPath%\MSVC\webinix-2-static-x64.lib"

    REM Copy Windows GCC
    copy /Y "%BuildPath%\GCC\webinix-2-x64.dll" "%DistPath%\GCC\webinix-2-x64.dll"
    copy /Y "%BuildPath%\GCC\libwebinix-2-static-x64.a" "%DistPath%\GCC\libwebinix-2-static-x64.a"

    REM Copy Windows TCC
    REM copy /Y "%BuildPath%\TCC\webinix-2-x64.dll" "%DistPath%\TCC\webinix-2-x64.dll"
    REM copy /Y "%BuildPath%\TCC\webinix-2-x64.def" "%DistPath%\TCC\webinix-2-x64.def"
    copy /Y "%BuildPath%\TCC\libwebinix-2-static-x64.a" "%DistPath%\TCC\libwebinix-2-static-x64.a"

    echo.
    echo Compressing the release folder...

    set TAR_OUT=webinix-windows-x64-v%WEBUI_VERSION%.tar.gz
    cd "dist"
    timeout 2 > NUL
    tar.exe -czf %TAR_OUT% Windows\*
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
