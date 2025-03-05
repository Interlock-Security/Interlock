<div align="center">

![Logo](https://raw.githubusercontent.com/webinix-dev/webinix-logo/14fd595844f57ce751dfc751297b1468b10de77a/webinix_120.svg)

# Webinix v2.5.0-beta.3

[build-status]: https://img.shields.io/github/actions/workflow/status/webinix-dev/webinix/ci.yml?branch=main&style=for-the-badge&logo=githubactions&labelColor=414868&logoColor=C0CAF5
[last-commit]: https://img.shields.io/github/last-commit/webinix-dev/webinix?style=for-the-badge&logo=github&logoColor=C0CAF5&labelColor=414868
[release-version]: https://img.shields.io/github/v/tag/webinix-dev/webinix?style=for-the-badge&logo=webtrees&logoColor=C0CAF5&labelColor=414868&color=7664C6
[license]: https://img.shields.io/github/license/webinix-dev/webinix?style=for-the-badge&logo=opensourcehardware&label=License&logoColor=C0CAF5&labelColor=414868&color=8c73cc

[![][build-status]](https://github.com/webinix-dev/webinix/actions?query=branch%3Amain)
[![][last-commit]](https://github.com/webinix-dev/webinix/pulse)
[![][release-version]](https://github.com/webinix-dev/webinix/releases/latest)
[![][license]](https://github.com/webinix-dev/webinix/blob/main/LICENSE)

> Use any web browser or WebView as GUI, with your preferred language in the backend and modern web technologies in the frontend, all in a lightweight portable library.

![Screenshot](https://raw.githubusercontent.com/webinix-dev/webinix-logo/main/screenshot.png)

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

- Portable (*Needs only a web browser or a WebView at runtime*)
- One header file
- Lightweight (*Few Kb library*) & Small memory footprint
- Fast binary communication protocol
- Multi-platform & Multi-Browser
- Using private profile for safety
- Cross-platform WebView

## UI & The Web Technologies

[Borislav Stanimirov](https://ibob.bg/) discusses using HTML5 in the web browser as GUI at the [C++ Conference 2019 (_YouTube_)](https://www.youtube.com/watch?v=bbbcZd4cuxg).

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

|                                 | Tauri / WebView   | Qt                         | Webinix               |
| ------------------------------- | ----------------- | -------------------------- | ------------------- |
| Runtime Dependencies on Windows | _WebView2_        | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on Linux   | _GTK3, WebKitGTK_ | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on macOS   | _Cocoa, WebKit_   | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |

## Documentation

> **Note**
> We are currently writing documentation.

- [Online Documentation](https://webinix.me/docs/)

## Build Webinix Library

### Windows

| Compiler | Command |
|----------|---------|
| GCC      | `mingw32-make` |
| MSVC     | `nmake` |

<details>
  <summary><strong>Windows SSL/TLS (Optional)</strong></summary>
  
  Download and install the OpenSSL pre-compiled binaries for Windows:

  - **MSVC**: [x64 OpenSSL v3.3.1](https://slproweb.com/download/Win64OpenSSL-3_3_1.msi) or [_32Bit_](https://slproweb.com/download/Win32OpenSSL-3_3_1.msi). See the [Wiki list](https://wiki.openssl.org/index.php/Binaries) for more info.
  - **MinGW**: [Curl for Windows with OpenSSL](https://curl.se/windows/)

  ```powershell
  # GCC
  mingw32-make WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\include" WEBUI_TLS_LIB="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\lib"

  # MSVC
  nmake WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\Program Files\OpenSSL-xxx\include" WEBUI_TLS_LIB="C:\Program Files\OpenSSL-xxx\lib"
  ```
</details>

### Linux

| Compiler | Command |
|----------|---------|
| GCC      | `make` |
| Clang    | `make CC=clang` |

<details>
  <summary><strong>Linux SSL/TLS (Optional)</strong></summary>
  
  ```sh
  sudo apt update
  sudo apt install libssl-dev

  # GCC
  make WEBUI_USE_TLS=1

  # Clang
  make WEBUI_USE_TLS=1 CC=clang
  ```
</details>

### macOS

| Compiler | Command |
|----------|---------|
| Default  | `make` |

<details>
  <summary><strong>macOS SSL/TLS (Optional)</strong></summary>
  
  ```sh
  brew install openssl
  make WEBUI_USE_TLS=1
  ```
</details>

## Minimal Webinix Application

- **C**

  ```c
  #include "webinix.h"

  int main() {
    size_t my_window = webinix_new_window();
    webinix_show(my_window, "<html><head><script src=\"webinix.js\"></script></head> Hello World ! </html>");
    webinix_wait();
    return 0;
  }
  ```

- **C++**

  ```cpp
  #include "webinix.hpp"
  #include <iostream>

  int main() {
    webinix::window my_window;
    my_window.show("<html><head><script src=\"webinix.js\"></script></head> C++ Hello World ! </html>");
    webinix::wait();
    return 0;
  }
  ```

- **More C/C++ Examples**

  - [C](https://github.com/webinix-dev/webinix/tree/main/examples/C)
  - [C++](https://github.com/webinix-dev/webinix/tree/main/examples/C++)

- **Other Languages**

  - [Wrappers List](#Wrappers)

## Build Webinix Application

### Windows

| Compiler | Type    | Command |
|----------|--------|---------|
| GCC      | Static  | `gcc -Os -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-static -lws2_32 -Wall -luser32 -static -lole32 -o my_application.exe` |
| GCC      | Dynamic | `gcc -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "webinix-2.dll" -lws2_32 -Wall -luser32 -lole32 -o my_application.exe` |
| MSVC     | Static  | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webinix-2-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
| MSVC     | Dynamic | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webinix-2.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |

<details>
  <summary><strong>Windows With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | GCC      | Static  | `gcc -Os -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-secure-static -lws2_32 -Wall -luser32 -static -lole32 -o my_application.exe` |
  | GCC      | Dynamic | `gcc -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "webinix-2-secure.dll" -lws2_32 -Wall -luser32 -lole32 -o my_application.exe` |
  | MSVC     | Static  | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webinix-2-secure-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
  | MSVC     | Dynamic | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webinix-2-secure.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
</details>

### Linux

| Compiler | Type    | Command |
|----------|--------|---------|
| GCC      | Static  | `gcc -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-static -lpthread -lm -ldl -o my_application` |
| GCC      | Dynamic | `gcc my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "./webinix-2.so" -lpthread -lm -ldl -o my_application` |
| Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-static -lpthread -lm -ldl -o my_application` |
| Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "./webinix-2.so" -lpthread -lm -ldl -o my_application` |

<details>
  <summary><strong>Linux With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | GCC      | Static  | `gcc -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-secure-static -lpthread -lm -ldl -o my_application` |
  | GCC      | Dynamic | `gcc my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "./webinix-2-secure.so" -lpthread -lm -ldl -o my_application` |
  | Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-secure-static -lpthread -lm -ldl -o my_application` |
  | Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "./webinix-2-secure.so" -lpthread -lm -ldl -o my_application` |
</details>

### macOS

| Compiler | Type    | Command |
|----------|--------|---------|
| Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-static -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
| Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "./webinix-2.dylib" -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |

<details>
  <summary><strong>macOS With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebinix-2-secure-static -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
  | Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "./webinix-2-secure.dylib" -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
</details>

## Wrappers

| Language        | v2.4.0 API | v2.5.0 API | Link                                                    |
| --------------- | --- | -------------- | ---------------------------------------------------------  |
| Python          | ✔️ | _not complete_ | [Python-Webinix](https://github.com/webinix-dev/python-webinix)  |
| Go              | ✔️ | _not complete_ | [Go-Webinix](https://github.com/webinix-dev/go-webinix)          |
| Zig             | ✔️ |  _not complete_ | [Zig-Webinix](https://github.com/webinix-dev/zig-webinix)        |
| Nim             | ✔️ |  _not complete_ | [Nim-Webinix](https://github.com/webinix-dev/nim-webinix)        |
| V               | ✔️ |  _not complete_ | [V-Webinix](https://github.com/webinix-dev/v-webinix)            |
| Rust            | _not complete_ |  _not complete_ | [Rust-Webinix](https://github.com/webinix-dev/rust-webinix)      |
| TS / JS (Deno)  | ✔️ |  _not complete_ | [Deno-Webinix](https://github.com/webinix-dev/deno-webinix)      |
| TS / JS (Bun)   | _not complete_ |  _not complete_ | [Bun-Webinix](https://github.com/webinix-dev/bun-webinix)        |
| Swift           | _not complete_ |  _not complete_ | [Swift-Webinix](https://github.com/webinix-dev/swift-webinix)    |
| Odin            | _not complete_ |  _not complete_ | [Odin-Webinix](https://github.com/webinix-dev/odin-webinix)      |
| Pascal          | _not complete_ |  _not complete_ | [Pascal-Webinix](https://github.com/webinix-dev/pascal-webinix)  |
| Purebasic       | _not complete_ |  _not complete_ | [Purebasic-Webinix](https://github.com/webinix-dev/purebasic-webinix)|
| - |  |  |
| Common Lisp     | _not complete_ |  _not complete_ | [cl-webinix](https://github.com/garlic0x1/cl-webinix)          |
| Delphi          | _not complete_ |  _not complete_ | [Webinix4Delphi](https://github.com/salvadordf/Webinix4Delphi) |
| C#              | _not complete_ |  _not complete_ | [Webinix4CSharp](https://github.com/salvadordf/Webinix4CSharp) |
| Webinix.NET       | _not complete_ |  _not complete_ | [Webinix.NET](https://github.com/Juff-Ma/Webinix.NET)          |
| QuickJS         | _not complete_ |  _not complete_ | [QuickUI](https://github.com/xland/QuickUI)                |
| PHP             | _not complete_ |  _not complete_ | [PHPWebUiComposer](https://github.com/KingBes/php-webinix-composer) |

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

## Supported WebView

| WebView         | Status         |
| --------------- | --------------- |
| Windows WebView2 | ✔️ |
| Linux GTK WebView   | ✔️ |
| macOS WKWebView  | ✔️ |

### License

> Licensed under MIT License.

### Stargazers

[![Stargazers repo roster for @webinix-dev/webinix](https://reporoster.com/stars/webinix-dev/webinix)](https://github.com/webinix-dev/webinix/stargazers)
