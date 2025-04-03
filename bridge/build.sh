#!/bin/bash

# https://webinix.me
# https://github.com/webinix-dev/webinix
# Copyright (c) 2020-2025 Hassan Draga.
# Licensed under MIT License.
# All rights reserved.
# Canada.
#
# Special Thanks to Turiiya (https://github.com/ttytm)

project_root=$(git rev-parse --show-toplevel)
cd $project_root/bridge

echo "Transpile and bundle TS sources to webinix.js"
esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --minify-syntax --minify-whitespace --outdir=. ./webinix.ts

echo "Convert JS source to C header"
node js2c.js

echo "Done."
