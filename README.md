<div align="center">

# Webinix v2.5.0-beta.4


> Use any web browser or WebView as the graphical interface, integrating your favorite backend language with modern web technologies on the frontend — all packaged in a lightweight and portable library. 
Portable (Needs only a web browser or a WebView at runtime).

![Screenshot](https://github.com/user-attachments/assets/5b706693-0cc8-4710-b706-cac2de58478f)

</div>

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

- Portable – Requires only a web browser or WebView at runtime
- Single Header File – Easy integration with just one include
- Lightweight – Compact library (a few KB) with minimal memory usage
- High Performance – Uses a fast binary communication protocol
- Cross-Platform & Multi-Browser Support – Works seamlessly across devices and browsers
- Secure by Design – Utilizes a private profile for enhanced safety

## Universal WebView Support – Compatible across platforms via WebView

</div>

Web application UI design goes beyond aesthetics—it’s about functionality. Leveraging modern web technologies ensures your product looks professional and up-to-date. A thoughtfully designed interface creates a strong first impression, helps guide potential customers through your funnel, boosts conversions, and enhances the overall user experience by making navigation intuitive and seamless.


### Why Use Web Browsers?

Web application UI design is more than just visual appeal—it’s about delivering functionality. By leveraging modern web technologies, you ensure your product looks current and professional. A well-crafted interface makes a strong first impression, guides users effectively through your funnel, increases conversions, and enhances the user experience with intuitive, seamless navigation.

Modern web browsers offer all the capabilities required for a cutting-edge UI. They are highly advanced, optimized, and well-suited to serve as a graphical interface. In contrast, traditional legacy GUI libraries are often outdated and complex. While WebView-based applications are a possible alternative, they come with drawbacks—including large SDKs, numerous dependencies, and limited feature support compared to full browsers. That’s why Webinix leverages real web browsers, delivering the complete power of modern web technologies while keeping your application lightweight and portable.


### How Does it Work?

<div align="center">

![Diagram](https://github.com/user-attachments/assets/f63694ec-44c1-474b-931f-4a0ce3d5c74f)

</div>

Modern web browsers provide everything needed for a high-performance, cutting-edge UI. They’re powerful, optimized, and ideal for graphical interfaces—unlike legacy GUI libraries, which are often outdated and cumbersome. While WebView-based apps are an option, they bring trade-offs like bulky SDKs, heavy dependencies, and limited features. Webinix avoids these limitations by using full web browsers, harnessing the full potential of modern web technologies while keeping your application lightweight and portable.
Webinix is a smarter, more efficient alternative to traditional WebView controllers. Instead of embedding a heavy WebView—which bloats your application and adds runtime dependencies—Webinix uses a lightweight static or dynamic library to launch the system’s installed web browser as the GUI. This approach results in a smaller, faster, and fully portable application that only requires a web browser to run.


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
  mingw32-make WEBINIX_USE_TLS=1 WEBINIX_TLS_INCLUDE="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\include" WEBINIX_TLS_LIB="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\lib"

  # MSVC
  nmake WEBINIX_USE_TLS=1 WEBINIX_TLS_INCLUDE="C:\Program Files\OpenSSL-xxx\include" WEBINIX_TLS_LIB="C:\Program Files\OpenSSL-xxx\lib"
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
  make WEBINIX_USE_TLS=1

  # Clang
  make WEBINIX_USE_TLS=1 CC=clang
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
  make WEBINIX_USE_TLS=1
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
| GCC      | Static  | `gcc -Os -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-static -lws2_32 -Wall -luser32 -static -lole32 -o my_application.exe` |
| GCC      | Dynamic | `gcc -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" "webinix-2.dll" -lws2_32 -Wall -luser32 -lole32 -o my_application.exe` |
| MSVC     | Static  | `cl my_application.c /I"_PATH_TO_WEBINIX_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBINIX_LIB_" /SUBSYSTEM:WINDOWS webinix-2-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
| MSVC     | Dynamic | `cl my_application.c /I"_PATH_TO_WEBINIX_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBINIX_LIB_" /SUBSYSTEM:WINDOWS webinix-2.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |

<details>
  <summary><strong>Windows With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | GCC      | Static  | `gcc -Os -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-secure-static -lws2_32 -Wall -luser32 -static -lole32 -o my_application.exe` |
  | GCC      | Dynamic | `gcc -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" "webinix-2-secure.dll" -lws2_32 -Wall -luser32 -lole32 -o my_application.exe` |
  | MSVC     | Static  | `cl my_application.c /I"_PATH_TO_WEBINIX_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBINIX_LIB_" /SUBSYSTEM:WINDOWS webinix-2-secure-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
  | MSVC     | Dynamic | `cl my_application.c /I"_PATH_TO_WEBINIX_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBINIX_LIB_" /SUBSYSTEM:WINDOWS webinix-2-secure.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
</details>

### Linux

| Compiler | Type    | Command |
|----------|--------|---------|
| GCC      | Static  | `gcc -Os my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-static -lpthread -lm -ldl -o my_application` |
| GCC      | Dynamic | `gcc my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2 -lpthread -lm -ldl -o my_application` |
| Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-static -lpthread -lm -ldl -o my_application` |
| Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2 -lpthread -lm -ldl -o my_application` |

<details>
  <summary><strong>Linux With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | GCC      | Static  | `gcc -Os my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-secure-static -lpthread -lm -ldl -o my_application` |
  | GCC      | Dynamic | `gcc my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-secure -lpthread -lm -ldl -o my_application` |
  | Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-secure-static -lpthread -lm -ldl -o my_application` |
  | Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-secure -lpthread -lm -ldl -o my_application` |
</details>

### macOS

| Compiler | Type    | Command |
|----------|--------|---------|
| Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-static -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
| Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2 -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |

<details>
  <summary><strong>macOS With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-secure-static -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
  | Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBINIX_INCLUDE_" -L"_PATH_TO_WEBINIX_LIB_" -lwebinix-2-secure -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
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
| PHP             | _not complete_ |  _not complete_ | [PHPWEBINIXComposer](https://github.com/KingBes/php-webinix-composer) |

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
