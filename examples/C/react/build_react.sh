#!/bin/bash

echo
echo "* Build React project..."

cd webinix-react-example
npm install || exit
npm run build || exit
cd ..

echo
echo "* Embedding React's build files into 'vfs.h'"

python3 vfs.py "./webinix-react-example/build" "vfs.h"

echo
echo "* Compiling 'main.c' into 'main' using GCC..."

make
