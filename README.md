# Webinix 2.x

![ScreenShot](screenshot.png)

[![Build Status](https://img.shields.io/github/issues/alifcommunity/webinix.svg?branch=master)](https://github.com/alifcommunity/webinix)

Bring the power of web browsers with the web technologies into your GUI, using your favourite programming language in the backend, and HTML/JS/CSS in the frontend.

## Why Webinix?

Web technology is everywhere, and the web browsers have everything a modern UI need. While all other "WebView" based GUI libraries can not provide all features like a real web browser provides, Webinix use any installed web browser to give you the full power of a web browser. Webinix is fully written in C, and the final result library is completely independent and does not need any third-party library.

## How its work?

Webinix use a WebSocket communication in binary mode between the web browser (UI) and your application. Your application will receive click events. And of course you can send/receive data or execute JavaScript from your favourite programming language. 

## Features

- Pure C & Independent (*No need for any third-party library*)
- Lightweight and small memory footprint
- Fast binary communication (*App--Webinix--Browser*)
- One header file
- Multiplatform & Multi Browser
- Private browser user-profiles
- Customized app mode look & feel

## Build

 - [Windows](https://github.com/alifcommunity/webinix/tree/main/build/Windows)

## Examples

 - [C](https://github.com/alifcommunity/webinix/tree/main/examples/C)
 - [Python](https://github.com/alifcommunity/webinix/tree/main/examples/Python)
 - [TypeScript / JavaScript](https://github.com/alifcommunity/webinix/tree/main/examples/TypeScript)
 - Go

## Supported Browser

| OS | Browser | Status |
| ------ | ------ | ------ |
| Windows | Firefox | ✔️ |
| Windows | Chrome | ✔️ |
| Windows | Edge | ✔️ |
| Linux | Firefox | *coming soon* |
| Linux | Chrome | *coming soon* |
| macOS | Firefox | *coming soon* |
| macOS | Chrome | *coming soon* |
| macOS | Safari | *coming soon* |

## Supported Language

| Language | Status |
| ------ | ------ |
| C | ✔️ |
| C++ | 94% |
| Python | ✔️ |
| JavaScript | ✔️ |
| TypeScript | ✔️ |
| Go | ✔️ |
| Rust | 94% |
| Java | *coming soon* |
| Nim | *coming soon* |
| Perl | *coming soon* |
| Ruby | *coming soon* |
| Scala | *coming soon* |

### License

GNU General Public License v3.0
