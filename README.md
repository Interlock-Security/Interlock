<div align="center">

![Logo](https://github.com/webinix-dev/webinix/assets/34311583/c92e712f-0698-486a-a460-d4acea28a4f8)

# Webinix v2.4.0

[![BuildStatus](https://img.shields.io/circleci/project/github/badges/shields/master?style=for-the-badge)](https://github.com/webinix-dev/webinix/actions?query=branch%3Amain) [![Issues](https://img.shields.io/github/issues/webinix-dev/webinix.svg?branch=master&style=for-the-badge&url=https://google.com)](https://github.com/webinix-dev/webinix/issues) [![Website](https://img.shields.io/website?label=webinix.me&style=for-the-badge&url=https://google.com)](https://webinix.me/)

> Webinix is not a web-server solution or a framework, but it allows you to use any web browser as a GUI, with your preferred language in the backend and HTML5 in the frontend. All in a lightweight portable lib.

![Screenshot](https://github.com/webinix-dev/webinix/assets/34311583/57992ef1-4f7f-4d60-8045-7b07df4088c6)

</div>

## Download

- [Latest Stable Release](https://github.com/webinix-dev/webinix/releases)
- [Nightly Build](https://github.com/webinix-dev/webinix/releases/tag/nightly)

## Contents

- [Features](#features)
- [Showcase](#showcase)
- [UI & The Web Technologies](#ui--the-web-technologies)
- [Documentation](#documentation)
- [Build](#build)
- [Examples](#examples)
- [Wrappers](#wrappers)
- [Supported Web Browsers](#supported-web-browsers)
- [License](#license)

## Features

- Written in Pure C
- Fully Independent (_No need for any third-party runtimes_)
- Lightweight ~200 Kb & Small memory footprint
- Fast binary communication protocol between Webinix and the browser (_Instead of JSON_)
- One header file
- Multi-platform & Multi-Browser
- Using private profile for safety

## Showcase

This [text editor](https://github.com/webinix-dev/webinix/tree/main/examples/C/text-editor) is a lightweight and portable example written in C using Webinix as the GUI library.

<div align="center">

![Example](https://github.com/webinix-dev/webinix/assets/34311583/c1ccf29c-806a-4742-bfd7-a3dc30cab70b)

</div>

## UI & The Web Technologies

[Borislav Stanimirov](https://ibob.bg/) discusses using HTML5 in the web browser as GUI at the [C++ Conference 2019 (_YouTube_)](https://www.youtube.com/watch?v=bbbcZd4cuxg).

<!-- <div align="center">
  <a href="https://www.youtube.com/watch?v=bbbcZd4cuxg"><img src="https://img.youtube.com/vi/bbbcZd4cuxg/0.jpg" alt="Embrace Modern Technology: Using HTML 5 for GUI in C++ - Borislav Stanimirov - CppCon 2019"></a>
</div> -->

<div align="center">

![CPPCon](https://github.com/webinix-dev/webinix/assets/34311583/4e830caa-4ca0-44ff-825f-7cd6d94083c8)

</div>

Web application UI design is not just about how a product looks but how it works. Using web technologies in your UI makes your product modern and professional, And a well-designed web application will help you make a solid first impression on potential customers. Great web application design also assists you in nurturing leads and increasing conversions. In addition, it makes navigating and using your web app easier for your users.

### Why Use Web Browsers?

Today's web browsers have everything a modern UI needs. Web browsers are very sophisticated and optimized. Therefore, using it as a GUI will be an excellent choice. While old legacy GUI lib is complex and outdated, a WebView-based app is still an option. However, a WebView needs a huge SDK to build and many dependencies to run, and it can only provide some features like a real web browser. That is why Webinix uses real web browsers to give you full features of comprehensive web technologies while keeping your software lightweight and portable.

### How Does it Work?

<div align="center">

![Diagram](https://github.com/ttytm/webinix/assets/34311583/dbde3573-3161-421e-925c-392a39f45ab3)

</div>

Think of Webinix like a WebView controller, but instead of embedding the WebView controller in your program, which makes the final program big in size, and non-portable as it needs the WebView runtimes. Instead, by using Webinix, you use a tiny static/dynamic library to run any installed web browser and use it as GUI, which makes your program small, fast, and portable. **All it needs is a web browser**.

### Runtime Dependencies Comparison

|                                 | WebView           | Qt                         | Webinix               |
| ------------------------------- | ----------------- | -------------------------- | ------------------- |
| Runtime Dependencies on Windows | _WebView2_        | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on Linux   | _GTK3, WebKitGTK_ | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on macOS   | _Cocoa, WebKit_   | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |

## Documentation

> **Note**
> We are currently writing documentation.

- [Online Documentation - C](https://webinix.me/docs/#/c_api)
- [Online Documentation - C++](https://webinix.me/docs/#/cpp_api)

## Build

- **Windows**

  ```powershell
  # GCC
  mingw32-make

  # MSVC
  nmake
  ```

  **Windows SSL/TLS (_Optional_)**

  Download and install the OpenSSL pre-compiled binaries for Windows:

  - MSVC: [x64 OpenSSL v3.1.3](https://slproweb.com/download/Win64OpenSSL-3_1_3.msi) or [_32Bit_](https://slproweb.com/download/Win32OpenSSL-3_1_3.msi). Please check this [Wiki list](https://wiki.openssl.org/index.php/Binaries) for more info.
  - MinGW: [Curl for Windows win OpenSSL](https://curl.se/windows/)

  ```powershell
  # GCC
  mingw32-make WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\include" WEBUI_TLS_LIB="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\lib"

  # MSVC
  nmake WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\Program Files\OpenSSL-xxx\include" WEBUI_TLS_LIB="C:\Program Files\OpenSSL-xxx\lib"
  ```

- **Linux**

  ```sh
  # GCC
  make

  # Clang
  make CC=clang
  ```

  **Linux SSL/TLS (_Optional_)**

  ```sh
  sudo apt update
  sudo apt install libssl-dev

  # GCC
  make WEBUI_USE_TLS=1

  # Clang
  make WEBUI_USE_TLS=1 CC=clang
  ```

- **macOS**

  ```sh
  make
  ```

  **macOS SSL/TLS (_Optional_)**

  ```sh
  brew install openssl
  make WEBUI_USE_TLS=1
  ```

## Examples

- [C](https://github.com/webinix-dev/webinix/tree/main/examples/C)
- [C++](https://github.com/webinix-dev/webinix/tree/main/examples/C++)

## Wrappers

| Language                | Status         | Link                                                      |
| ----------------------- | -------------- | --------------------------------------------------------- |
| Go                      | ✔️             | [Go-Webinix](https://github.com/webinix-dev/go-webinix)         |
| Nim                     | ✔️             | [Nim-Webinix](https://github.com/webinix-dev/nim-webinix)       |
| Pascal                  | ✔️             | [Pascal-Webinix](https://github.com/webinix-dev/pascal-webinix) |
| Python                  | ✔️             | [Python-Webinix](https://github.com/webinix-dev/python-webinix) |
| Rust                    | _not complete_ | [Rust-Webinix](https://github.com/webinix-dev/rust-webinix)     |
| TypeScript / JavaScript | ✔️             | [Deno-Webinix](https://github.com/webinix-dev/deno-webinix)     |
| V                       | ✔️             | [V-Webinix](https://github.com/webinix-dev/v-webinix)           |
| Zig                     | _not complete_ | [Zig-Webinix](https://github.com/webinix-dev/zig-webinix)       |
| Odin                    | _not complete_ | [Odin-Webinix](https://github.com/webinix-dev/odin-webinix)       |

## Supported Web Browsers

| Browser         | Windows         | macOS         | Linux           |
| --------------- | --------------- | ------------- | --------------- |
| Mozilla Firefox | ✔️              | ✔️            | ✔️              |
| Google Chrome   | ✔️              | ✔️            | ✔️              |
| Microsoft Edge  | ✔️              | ✔️            | ✔️              |
| Chromium        | ✔️              | ✔️            | ✔️              |
| Yandex          | ✔️              | ✔️            | ✔️              |
| Brave           | ✔️              | ✔️            | ✔️              |
| Vivaldi         | ✔️              | ✔️            | ✔️              |
| Epic            | ✔️              | ✔️            | _not available_ |
| Apple Safari    | _not available_ | _coming soon_ | _not available_ |
| Opera           | _coming soon_   | _coming soon_ | _coming soon_   |

### License

> Licensed under MIT License.

### Stargazers

[![Stargazers repo roster for @webinix-dev/webinix](https://reporoster.com/stars/webinix-dev/webinix)](https://github.com/webinix-dev/webinix/stargazers)
