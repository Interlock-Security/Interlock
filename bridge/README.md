# Webinix Bridge

The Webinix Bridge connects the UI (_Web Browser_) with the backend application through WebSocket. This bridge is written in TypeScript, and it needs to be transpiled to JavaScript using [ESBuild](https://esbuild.github.io/) to produce `webinix_bridge.js`, then converted to C99 header using the Python script `js2c.py` to generate `webinix_bridge.h`.

### Windows

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webinix\bridge`
- `npm install esbuild`
- `.\node_modules\.bin\esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=.\ .\webinix_bridge.ts`
- `python js2c.py`

### Windows PowerShell

- cd `webinix\bridge`
- `./build.ps1`
- If you get _running scripts is disabled on this
  system_ error. Then run `Set-ExecutionPolicy RemoteSigned` to enable script execution. After done, you can roll back by running `Set-ExecutionPolicy Restricted`

### Linux

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webinix/bridge`
- `npm install esbuild`
- `./node_modules/.bin/esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=./ ./webinix_bridge.ts`
- `python js2c.py`

### Linux Bash

- cd `webinix\bridge`
- sh `./build.sh`

### macOS

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webinix/bridge`
- `npm install esbuild`
- `./node_modules/.bin/esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=./ ./webinix_bridge.ts`
- `python js2c.py`

### macOS Bash

- cd `webinix\bridge`
- sh `./build.sh`
