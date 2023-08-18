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

RootPath="$PWD/.."
BuildPath="$RootPath/build/Linux"
DistPath="$RootPath/dist/Linux"
cd "$RootPath"

echo "";
echo "Building Webinix using GCC...";
echo "";

# Build Webinix Library using GCC
cd "$BuildPath/GCC"
$GCC_CMD

echo "";
echo "Building Webinix using Clang...";
echo "";

# Build Webinix Library using Clang
cd "$BuildPath/Clang"
$CLANG_CMD

echo "";
echo "Copying Webinix libs to the examples folder..."
echo "";

cd "$RootPath"

# C - Text Editor
cp -f "include/webinix.h" "examples/C/text-editor/webinix.h"
cp -f "$BuildPath/GCC/webinix-2-x64.so" "examples/C/text-editor/webinix-2-x64.so"

echo "";
if [ "$ARG1" = "" ]; then

    echo "Copying Webinix libs to $DistPath..."
    echo "";

    # Remove Linux distributable files directory if it exits
    [ -d "$DistPath" ] && rm -r "$DistPath"

    # Create Linux output directories
    mkdir -p "$DistPath/include"
    mkdir "$DistPath/GCC"
    mkdir "$DistPath/Clang"

    # Copy include files
    cp "include/webinix.h" "$DistPath/include/webinix.h"
    cp "include/webinix.hpp" "$DistPath/include/webinix.hpp"

    # Copy Linux GCC
    cp "$BuildPath/GCC/webinix-2-x64.so" "$DistPath/GCC/webinix-2-x64.so"
    cp "$BuildPath/GCC/libwebinix-2-static-x64.a" "$DistPath/GCC/libwebinix-2-static-x64.a"

    # Copy Linux Clang
    cp "$BuildPath/Clang/webinix-2-x64.so" "$DistPath/Clang/webinix-2-x64.so"
    cp "$BuildPath/Clang/libwebinix-2-static-x64.a" "$DistPath/Clang/libwebinix-2-static-x64.a"

    echo "";
    echo "Compressing distributable files..."
    echo "";

    TAR_OUT="webinix-linux-x64-v$WEBUI_VERSION.tar.gz"
    cd "dist"
    sleep 2
    tar -czf $TAR_OUT Linux/*
    cd "$RootPath"

    echo "";
    echo "Cleaning..."
    echo "";

    find ./ -type f -name "*.o" -exec rm -f {} \;
fi

cd "build"
