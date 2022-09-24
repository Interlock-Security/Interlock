# Webinix 2

![ScreenShot](screenshot.png)

[![Build Status](https://img.shields.io/github/issues/alifcommunity/webinix.svg?branch=master)](https://github.com/alifcommunity/webinix)

Bring the power of web browsers with the web technologies into you GUI, using your favorit programming language in the backend, and HTML/JS/CSS in the frontend.

## Why Webinix?

Web technology is everywhere, and the web browsers have everything a modern UI need. While all other "WebView" based GUI libraries can not provide all features like a real web browser provides, Webinix use any installed web browser to give you the full power of a web browser. Webinix is fully written in C, and the final result library is completely independent an does not need any third-party library.

## How its work?

Webinix use a WebSocket communication in binary mode between the web browser (UI) and your application. Your application will receive click events. And of course you can send/receive data or execute JavaScript from your favorit programming language. 

## Example in C

- **Header** - Include one header file:
```sh
#include "webinix.h"
```

- **HTML/CSS** - Set your HTML code
```sh
const char* my_html = "<!DOCTYPE html>"
"<html>"
"	<head>"
"		<title>Webinix Application</title>"
"	</head>"
"	<body>"
"		<h1>Welcome to Webinix 2.0</h1>"
"		<button id=\"MyButtonID\">Click!</button>"
"	</body>"
"</html>";
```

- **Window** - Create a window object
```sh
webinix_window_t* my_window;
```

- **Handler** - Create your click handler function
```sh
void my_function(const webinix_event_t e) {

    printf("You clicked on a button!");
}
```

- **Bind** - Bind your HTML element with your function
```sh
webinix_bind(my_window, "MyButtonID", check_the_password);
```

- **Show** - Show the window
```sh
if(!webinix_show(my_window, my_html, webinix.browser.chrome))  // Run the window on Chrome if is available
	webinix_show(my_window, my_html, webinix.browser.any); // If not, run on any other installed web browser
```

- **Loop** - Wait for all windows to be close
```sh
webinix_loop();
```

## Features

- Pure C & Independent (Shared/Static)
- Lightweight and small memory footprint (~117 Kb)
- Fast binary communication mode
- One header file
- Multiplatform & Multi Browser
- Private browser user-profiles
- Customized app mode look & feel

## Status

| OS | Browser  | Status |
| ------ | ------ | ------ |
| Windows | Firefox | ✔️ |
| Windows | Chrome | ✔️ |
| Windows | Edge | ✔️ |
| Linux | Firefox | *coming soon* |
| Linux | Chrome | *coming soon* |
| macOS | Firefox | *coming soon* |
| macOS | Chrome | *coming soon* |
| macOS | Safari | *coming soon* |

| Language | Status |
| ------ | ------ |
| C | ✔️ |
| C++ | *coming soon* |
| Python | 85% |
| Go | *coming soon* |
| Rust | *coming soon* |
| Java | *coming soon* |
| Nim | *coming soon* |
| Perl | *coming soon* |
| Ruby | *coming soon* |
| Scala | *coming soon* |

## Build - Windows

- **Microsoft Visual Studio**
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\examples\Windows\MSVC
nmake static
nmake static-debug
nmake dynamic
nmake dynamic-debug
```

- **MinGW**
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\examples\Windows\GCC
mingw32-make static
mingw32-make static-debug
mingw32-make dynamic
mingw32-make dynamic-debug
```

- **TCC**
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\examples\Windows\GCC
mingw32-make static
mingw32-make static-debug
mingw32-make dynamic
mingw32-make dynamic-debug
```

- **Clang**
```sh
coming soon
```

## Build - Linux

- **GCC**
```sh
coming soon
```

- **TCC**
```sh
coming soon
```

- **Clang**
```sh
coming soon
```

## Build - macOS

- **Clang**
```sh
coming soon
```

### License

GNU General Public License v3.0
