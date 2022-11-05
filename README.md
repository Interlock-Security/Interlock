# Webinix

[![Website](https://img.shields.io/circleci/project/github/badges/shields/master?style=for-the-badge)](https://github.com/alifcommunity/webinix) [![Website](https://img.shields.io/github/issues/alifcommunity/webinix.svg?branch=master&style=for-the-badge&url=https://google.com)](https://github.com/alifcommunity/webinix/issues) [![Website](https://img.shields.io/website?label=webinix.me&style=for-the-badge&url=https://google.com)](https://webinix.me/)

> Use any web browser as GUI, with your preferred language in the backend, and HTML/JS/TS/CSS in the frontend.

![ScreenShot](screenshot.png)

> :warning: **Notice**:
> 
> * Webinix it's not a web-server solution or a framework, but it's an easy-embeddable tool to use any installed web browser as a user interface.
> 
> * We are currently writing documentation.
> 
> * Webinix is not ready yet for production release.

## Features

- Written in Pure C
- Fully Independent (*No need for any third-party library*)
- Lightweight (*~160 Kb using TCC*) & Small memory footprint
- Fast WS binary communication (*App--Webinix--Browser*)
- One header file
- Multi-platform & Multi-Browser
- Using private profile for safety

## UI & The Web Technologies

Web application UI design is not just about how a product looks but how it works. Using web technologies in your UI makes your product modern and professional, And a well-designed web application will help you make a solid first impression on potential customers. Great web application design also assists you in nurturing leads and increasing conversions. In addition, it makes navigating and using your web app easier for your users.

## Why Use Web Browser?

Today's web browsers have everything a modern UI needs. Web browsers are very sophisticated and optimized. Therefore, using it as a GUI will be an excellent choice. While old legacy GUI lib is complex and outdated, a WebView-based app is still an option. However, a WebView needs a huge SDK to build and many dependencies to run, and it can only provide some features like a real web browser. That is why Webinix uses real web browsers to give you full features of comprehensive web technologies while keeping your software lightweight and portable.

## Comparison

![ScreenShot](webinix_diagram.png)

|  | WebView | Qt | Webinix |
| ------ | ------ | ------ | ------ |
| Dependencies on Windows | *Windows APIs, WebView2* | *QtCore, QtGui, QtWidgets* | *No need* |
| Dependencies on Linux | *GTK3, WebKitGTK* | *QtCore, QtGui, QtWidgets* | *No need* |
| Dependencies on macOS | *Cocoa, WebKit* | *QtCore, QtGui, QtWidgets* | *No need* |

## Build

 - [Windows](https://github.com/alifcommunity/webinix/tree/main/build/Windows)
 - [Linux](https://github.com/alifcommunity/webinix/tree/main/build/Linux)

## Examples

 - [C](https://github.com/alifcommunity/webinix/tree/main/examples/C)
 - [C++](https://github.com/alifcommunity/webinix/tree/main/examples/C++)
 - [Python](https://github.com/alifcommunity/webinix/tree/main/examples/Python)
 - [TypeScript / JavaScript](https://github.com/alifcommunity/webinix/tree/main/examples/TypeScript)
 - [Go](https://github.com/alifcommunity/webinix/tree/main/examples/Go/hello_world)
 - [Rust](https://github.com/alifcommunity/webinix/tree/main/examples/Rust/hello_world)
 - [V on malisipi/vwebinix](https://github.com/malisipi/vwebinix/tree/main/examples)
 
## Supported Browser

| OS | Browser | Status |
| ------ | ------ | ------ |
| Windows | Firefox | ✔️ |
| Windows | Chrome | ✔️ |
| Windows | Edge | ✔️ |
| Linux | Firefox | ✔️ |
| Linux | Chrome | ✔️ |
| macOS | Firefox | *coming soon* |
| macOS | Chrome | *coming soon* |
| macOS | Safari | *coming soon* |

## Supported Language

| Language | Status | Link |
| ------ | ------ | ------ |
| C | ✔️ | [./examples/C](https://github.com/alifcommunity/webinix/tree/main/examples/C) |
| C++ | ✔️ | [./examples/C++](https://github.com/alifcommunity/webinix/tree/main/examples/C%2B%2B) |
| Python | ✔️ | [./examples/Python](https://github.com/alifcommunity/webinix/tree/main/examples/Python) |
| JavaScript | ✔️ | [./examples/TypeScript/Nodejs](https://github.com/alifcommunity/webinix/tree/main/examples/TypeScript/Nodejs) |
| TypeScript | ✔️ | [./examples/TypeScript/Deno](https://github.com/alifcommunity/webinix/tree/main/examples/TypeScript/Deno) |
| Go | ✔️ | [./examples/Go](https://github.com/alifcommunity/webinix/tree/main/examples/Go) |
| Rust | ✔️ | [./examples/Rust](https://github.com/alifcommunity/webinix/tree/main/examples/Rust) |
| V | ✔️ | [malisipi/vwebinix](https://github.com/malisipi/vwebinix) |

### License

> GNU General Public License v3.0

### Stargazers

[![Stargazers repo roster for @alifcommunity/webinix](https://reporoster.com/stars/alifcommunity/webinix)](https://github.com/alifcommunity/webinix/stargazers)
