#!/bin/bash

WEBUI_VERSION=2.2.0

ARG1=$1
if [ "$ARG1" = "debug" ]; then
    CLANG_CMD="make debug"
    GCC_CMD="make debug"
else
    CLANG_CMD="make"
    GCC_CMD="make"
fi

echo "";
echo "Webinix v$WEBUI_VERSION Build Script"
echo "Platform: macOS x64"
echo "Compiler: Clang"

RootPath="$PWD/../"
cd "$RootPath"

echo "";
echo "Building Webinix using Clang...";
echo "";

# Build Webinix Library using Clang
cd "$RootPath"
cd "build/macOS/Clang"
$CLANG_CMD

echo "";
echo "Copying Webinix libs to the examples folder..."
echo "";

cd "$RootPath"

# Golang
cp -f "include/webinix.h" "examples/Go/hello_world/webinix/webinix.h"
cp -f "build/macOS/Clang/libwebinix-2-static-x64.a" "examples/Go/hello_world/webinix/libwebinix-2-static-x64.a"

# Deno
cp -f "build/macOS/Clang/webinix-2-x64.dyn" "examples/TypeScript/Deno/webinix-2-x64.dyn"

# Python
cp -f "build/macOS/Clang/webinix-2-x64.dyn" "examples/Python/PyPI/Package/src/webinix/webinix-2-x64.dyn"

# C++ (Minimal)
cp -f "include/webinix.hpp" "examples/C++/minimal/webinix.hpp"

echo "";
if [ "$ARG1" = "" ]; then

    echo "Copying Webinix libs to the release folder..."
    echo "";

    # Release macOS Include
    cp -f "include/webinix.h" "Release/macOS/include/webinix.h"
    cp -f "include/webinix.hpp" "Release/macOS/include/webinix.hpp"

    # Release macOS Clang
    cp -f "build/macOS/Clang/webinix-2-x64.dyn" "Release/macOS/Clang/webinix-2-x64.dyn"
    cp -f "build/macOS/Clang/libwebinix-2-static-x64.a" "Release/macOS/Clang/libwebinix-2-static-x64.a"

    echo "";
    echo "Compressing the release folder..."
    echo "";

    TAR_OUT="webinix-macos-x64-v$WEBUI_VERSION.zip"
    cd "Release"
    sleep 2
    tar -c -f $TAR_OUT macOS/*
    cd "$RootPath"

    echo "";
    echo "Cleaning..."
    echo "";

    find ./ -type f -name "*.o" -exec rm -f {} \;
fi

cd "build"
