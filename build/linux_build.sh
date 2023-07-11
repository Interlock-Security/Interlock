#!/bin/bash

WEBUI_VERSION=2.3.0

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
echo "Platform: Linux x64"
echo "Compiler: GCC and Clang"

RootPath="$PWD/../"
cd "$RootPath"

echo "";
echo "Converting JS source to C-String using xxd"
echo "";

#Converting JS source to C-String using xxd
cd "$RootPath"
cd "src/client"
xxd -i ./webinix.js ./webinix.h

echo "";
echo "Building Webinix using GCC...";
echo "";

# Build Webinix Library using GCC
cd "$RootPath"
cd "build/Linux/GCC"
$GCC_CMD

echo "";
echo "Building Webinix using Clang...";
echo "";

# Build Webinix Library using Clang
cd "$RootPath"
cd "build/Linux/Clang"
$CLANG_CMD

echo "";
echo "Copying Webinix libs to the examples folder..."
echo "";

cd "$RootPath"

# C - Text Editor
cp -f "include/webinix.h" "examples/C/text-editor/webinix.h"
cp -f "build/Linux/GCC/webinix-2-x64.so" "examples/C/text-editor/webinix-2-x64.so"

echo "";
if [ "$ARG1" = "" ]; then

    echo "Copying Webinix libs to the release folder..."
    echo "";

    # Release Linux Include
    cp -f "include/webinix.h" "Release/Linux/include/webinix.h"
    cp -f "include/webinix.hpp" "Release/Linux/include/webinix.hpp"

    # Release Linux GCC
    cp -f "build/Linux/GCC/webinix-2-x64.so" "Release/Linux/GCC/webinix-2-x64.so"
    cp -f "build/Linux/GCC/libwebinix-2-static-x64.a" "Release/Linux/GCC/libwebinix-2-static-x64.a"

    # Release Linux Clang
    cp -f "build/Linux/Clang/webinix-2-x64.so" "Release/Linux/Clang/webinix-2-x64.so"
    cp -f "build/Linux/Clang/libwebinix-2-static-x64.a" "Release/Linux/Clang/libwebinix-2-static-x64.a"

    echo "";
    echo "Compressing the release folder..."
    echo "";

    TAR_OUT="webinix-linux-x64-v$WEBUI_VERSION.tar.gz"
    cd "Release"
    sleep 2
    tar -czf $TAR_OUT Linux/*
    cd "$RootPath"

    echo "";
    echo "Cleaning..."
    echo "";

    find ./ -type f -name "*.o" -exec rm -f {} \;
fi

cd "build"
