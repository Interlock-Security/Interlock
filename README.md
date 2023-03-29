# Webinix

[![Website](https://img.shields.io/circleci/project/github/badges/shields/master?style=for-the-badge)](https://github.com/alifcommunity/webinix) [![Website](https://img.shields.io/github/issues/alifcommunity/webinix.svg?branch=master&style=for-the-badge&url=https://google.com)](https://github.com/alifcommunity/webinix/issues) [![Website](https://img.shields.io/website?label=webinix.me&style=for-the-badge&url=https://google.com)](https://webinix.me/)

> Use any web browser as GUI, with your preferred language in the backend and HTML5 in the frontend, all in a lightweight portable lib.

![ScreenShot](screenshot.png)

> :warning: **Notice**:
> 
> * Webinix it's not a web-server solution or a framework, but it's an lightweight portable lib to use any installed web browser as a user interface.
> 
> * We are currently writing documentation.
> 
> * Webinix is not ready yet for production release.

## Features

- Written in Pure C
- Fully Independent (*No need for any third-party runtimes*)
- Lightweight (*~160 Kb using TCC*) & Small memory footprint
- Fast binary communication protocol between Webinix and the browser (*Instead of JSON*)
- One header file
- Multi-platform & Multi-Browser
- Using private profile for safety

## CppCon 2019 Presentation

[Borislav Stanimirov](https://ibob.bg/) discusses using HTML5 in the web browser as GUI at the [C++ Conference 2019 (*YouTube*)](https://www.youtube.com/watch?v=bbbcZd4cuxg).

<!-- <div align="center">
  <a href="https://www.youtube.com/watch?v=bbbcZd4cuxg"><img src="https://img.youtube.com/vi/bbbcZd4cuxg/0.jpg" alt="Embrace Modern Technology: Using HTML 5 for GUI in C++ - Borislav Stanimirov - CppCon 2019"></a>
</div> -->

![ScreenShot](cppcon_2019.png)

## UI & The Web Technologies

Web application UI design is not just about how a product looks but how it works. Using web technologies in your UI makes your product modern and professional, And a well-designed web application will help you make a solid first impression on potential customers. Great web application design also assists you in nurturing leads and increasing conversions. In addition, it makes navigating and using your web app easier for your users.

## Why Use Web Browser?

Today's web browsers have everything a modern UI needs. Web browsers are very sophisticated and optimized. Therefore, using it as a GUI will be an excellent choice. While old legacy GUI lib is complex and outdated, a WebView-based app is still an option. However, a WebView needs a huge SDK to build and many dependencies to run, and it can only provide some features like a real web browser. That is why Webinix uses real web browsers to give you full features of comprehensive web technologies while keeping your software lightweight and portable.

## How does it work?

![ScreenShot](webinix_diagram.png)

Think of Webinix like a WebView controller, but instead of embedding the WebView controller in your program, which makes the final program big in size, and non-portable as it needs the WebView runtimes. Instead, by using Webinix, you use a tiny static/dynamic library to run any installed web browser and use it as GUI, which makes your program small, fast, and portable. **All it needs is a web browser**.

## Runtime Dependencies Comparison

|  | WebView | Qt | Webinix |
| ------ | ------ | ------ | ------ |
| Runtime Dependencies on Windows | *WebView2* | *QtCore, QtGui, QtWidgets* | ***A Web Browser*** |
| Runtime Dependencies on Linux | *GTK3, WebKitGTK* | *QtCore, QtGui, QtWidgets* | ***A Web Browser*** |
| Runtime Dependencies on macOS | *Cocoa, WebKit* | *QtCore, QtGui, QtWidgets* | ***A Web Browser*** |

## Documentation

 - [Online Documentation - C](https://webinix.me/docs/) (*Not Complete*)
 - Online Documentation - C++ (*Not Complete*)
 - [Online Documentation - Python](https://webinix.me/docs/) (*Not Complete*)
 - Online Documentation - TypeScript / JavaScript (*Not Complete*)
 - Online Documentation - Go (*Not Complete*)
 - Online Documentation - Rust (*Not Complete*)
 - Online Documentation - V (*Not Complete*)
 - Online Documentation - Nim (*Not Complete*)
 - Online Documentation - Zig (*Not Complete*)

## Build

 - [Windows](https://github.com/alifcommunity/webinix/tree/main/build/Windows)
 - [Linux](https://github.com/alifcommunity/webinix/tree/main/build/Linux)
 - macOS (*Not Complete*)

## Examples

 - [C](https://github.com/alifcommunity/webinix/tree/main/examples/C)
 - [C++](https://github.com/alifcommunity/webinix/tree/main/examples/C++) (*Not Complete*)
 - [Python](https://github.com/alifcommunity/webinix/tree/main/examples/Python) (*Not Complete*)
 - [TypeScript / JavaScript](https://github.com/alifcommunity/webinix/tree/main/examples/TypeScript) (*Not Complete*)
 - [Go](https://github.com/alifcommunity/webinix/tree/main/examples/Go/hello_world) (*Not Complete*)
 - [Rust](https://github.com/alifcommunity/webinix/tree/main/examples/Rust/hello_world) (*Not Complete*)
 - [V - malisipi/vwebinix](https://github.com/malisipi/vwebinix/tree/main/examples) (*Not Complete*)
 - [Nim - neroist/webinix](https://github.com/neroist/webinix/tree/main/examples) (*Not Complete*)
 - [Zig](https://github.com/alifcommunity/webinix/tree/main/examples/Zig) (*Not Complete*)

## Supported Web Browsers

| OS | Browser | Status |
| ------ | ------ | ------ |
| Windows | Mozilla Firefox | ✔️ |
| Windows | Google Chrome | ✔️ |
| Windows | Microsoft Edge | ✔️ |
| Windows | Chromium | ✔️ |
| Windows | Yandex | ✔️ |
| Windows | Brave | ✔️ |
| Windows | Vivaldi | ✔️ |
| Windows | Epic | ✔️ |
| Windows | Opera | *coming soon* |
| - | - | - |
| Linux | Mozilla Firefox | ✔️ |
| Linux | Google Chrome | ✔️ |
| Linux | Microsoft Edge | *coming soon* |
| Linux | Chromium | ✔️ |
| Linux | Yandex | *coming soon* |
| Linux | Brave | *coming soon* |
| Linux | Vivaldi | *coming soon* |
| Linux | Epic | *coming soon* |
| Linux | Opera | *coming soon* |
| - | - | - |
| macOS | Mozilla Firefox | *coming soon* |
| macOS | Google Chrome | ✔️ |
| macOS | Microsoft Edge | *coming soon* |
| macOS | Chromium | *coming soon* |
| macOS | Yandex | *coming soon* |
| macOS | Brave | *coming soon* |
| macOS | Vivaldi | *coming soon* |
| macOS | Epic | *coming soon* |
| macOS | Apple Safari | *coming soon* |
| macOS | Opera | *coming soon* |

## Supported Languages

| Language | Status | Link |
| ------ | ------ | ------ |
| C | ✔️ | [examples/C](https://github.com/alifcommunity/webinix/tree/main/examples/C) |
| C++ |  *Not Complete* | [examples/C++](https://github.com/alifcommunity/webinix/tree/main/examples/C%2B%2B) |
| Python | *Not Complete* | [examples/Python](https://github.com/alifcommunity/webinix/tree/main/examples/Python) |
| JavaScript | *Not Complete* | [examples/TypeScript/Nodejs](https://github.com/alifcommunity/webinix/tree/main/examples/TypeScript/Nodejs) |
| TypeScript | *Not Complete* | [examples/TypeScript/Deno](https://github.com/alifcommunity/webinix/tree/main/examples/TypeScript/Deno) |
| Go | *Not Complete* | [examples/Go](https://github.com/alifcommunity/webinix/tree/main/examples/Go) |
| Rust | *Not Complete* | [examples/Rust](https://github.com/alifcommunity/webinix/tree/main/examples/Rust) |
| V | *Not Complete* | [malisipi/vwebinix](https://github.com/malisipi/vwebinix) |
| Nim | *Not Complete* | [neroist/webinix](https://github.com/neroist/webinix) |
| Zig | *Not Complete* | [examples/Zig](https://github.com/alifcommunity/webinix/tree/main/examples/Zig) |

### License

> Licensed under GNU General Public License v2.0.

### Stargazers

[![Stargazers repo roster for @alifcommunity/webinix](https://reporoster.com/stars/alifcommunity/webinix)](https://github.com/alifcommunity/webinix/stargazers)
