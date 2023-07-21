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
echo "Platform: macOS x64"
echo "Compiler: Clang"

RootPath="$PWD/.."
BuildPath="$RootPath/build/macOS"
DistPath="$RootPath/dist/macOS"
cd "$RootPath"

echo "";
echo "Converting JS source to C-String using xxd"
echo "";

# Transpiling TS to JS
echo "Transpile and bundle TS sources to webinix.js";
esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=./src/client ./src/client/webinix.ts

# Converting JS source to C-String using xxd
cd "src"
xxd -i client/webinix.js client/webinix.h

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
cp -f "build/macOS/Clang/webinix-2-x64.dyn" "examples/C/text-editor/webinix-2-x64.dyn"

echo "";
if [ "$ARG1" = "" ]; then

    echo "Copying Webinix libs to $DistPath..."
    echo "";

    # Remove macOS distributable files directory if it exits
    [ -d "$DistPath" ] && rm -r "$DistPath"

    # Create macOS output directories
    mkdir -p "$DistPath/include"
    mkdir "$DistPath/Clang"

    # Copy include files
    cp "include/webinix.h" "$DistPath/include/webinix.h"
    cp "include/webinix.hpp" "$DistPath/include/webinix.hpp"

    # Copy macOS Clang
    cp "$BuildPath/Clang/webinix-2-x64.dyn" "$DistPath/Clang/webinix-2-x64.dyn"
    cp "$BuildPath/Clang/libwebinix-2-static-x64.a" "$DistPath/Clang/libwebinix-2-static-x64.a"

    echo "";
    echo "Compressing the release folder..."
    echo "";

    TAR_OUT="webinix-macos-x64-v$WEBUI_VERSION.tar.gz"
    cd "dist"
    sleep 2
    tar -czf $TAR_OUT macOS/*
    cd "$RootPath"

    echo "";
    echo "Cleaning..."
    echo "";

    find ./ -type f -name "*.o" -exec rm -f {} \;
fi

cd "build"
