@echo off

set ARG1=%1
IF "%ARG1%"=="debug" (
    set MSVC_CMD=nmake debug
    set GCC_CMD=mingw32-make debug
) else (
    set MSVC_CMD=nmake
    set GCC_CMD=mingw32-make
)

echo.
echo - - - - - - - - - - - - - - - - - - - - - - -
echo Webinix v2.2.0 Build Script
echo - - - - - - - - - - - - - - - - - - - - - - -
echo.
echo OS: Micsoroft Windows x64
echo Compiler: Microsoft Visual C

Set RootPath=%CD%\..\
cd "%RootPath%"

echo.
echo - - [Build] - - - - - - - - - - - - - - - - -

REM Build Webinix Library using MSVC
cd "%RootPath%"
cd "build\Windows\MSVC"
%MSVC_CMD%

REM Build Webinix Library using GCC
cd "%RootPath%"
cd "build\Windows\GCC"
%GCC_CMD%

echo.
echo - - [Copy] - - - - - - - - - - - - - - - - -
echo.

cd "%RootPath%"

REM Golang
copy /Y "build\Windows\GCC\libwebinix-2-static-x64.a" "examples\Go\hello_world\webinix\libwebinix-2-static-x64.a"
copy /Y "include\webinix.h" "examples\Go\hello_world\webinix\webinix.h"

REM Deno
copy /Y "build\Windows\MSVC\webinix-2-x64.dll" "examples\TypeScript\Deno\webinix-2-x64.dll"

REM Python
copy /Y "build\Windows\MSVC\webinix-2-x64.dll" "examples\Python\PyPI\Package\src\webinix\webinix-2-x64.dll"

echo.
IF "%ARG1%"=="" (

    echo - - [Clean] - - - - - - - - - - - - - - - - -

    DEL /Q /F /S "*.exe" >nul 2>&1
    DEL /Q /F /S "*.o" >nul 2>&1
    DEL /Q /F /S "*.a" >nul 2>&1
    DEL /Q /F /S "*.def" >nul 2>&1
    DEL /Q /F /S "*.exp" >nul 2>&1
    DEL /Q /F /S "*.pdb" >nul 2>&1
    DEL /Q /F /S "*.ilk" >nul 2>&1
    DEL /Q /F /S "*.obj" >nul 2>&1
    DEL /Q /F /S "*.res" >nul 2>&1
    DEL /Q /F /S "*.bak" >nul 2>&1
    DEL /Q /F /S "*.DS_Store" >nul 2>&1
)

cd "scripts"
