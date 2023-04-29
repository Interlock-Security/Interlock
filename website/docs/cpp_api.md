# Webinix v2.2.0 C++ APIs

- [Download](/cpp_api?id=download)
- [Build From Source](/cpp_api?id=build-from-source)
- [Examples](/cpp_api?id=examples)
- Window
    - [New Window](/cpp_api?id=new-window)
    - [Show Window](/cpp_api?id=show-window)
    - [Window status](/cpp_api?id=window-status)
- Binding & Events
    - [Bind](/cpp_api?id=Bind)
    - [Events](/cpp_api?id=events)
- Application
    - [Wait](/cpp_api?id=wait)
    - [Exit](/cpp_api?id=exit)
    - [Close](/cpp_api?id=close)
    - [Startup Timeout](/cpp_api?id=startup-timeout)
    - [Multi Access](/cpp_api?id=multi-access)
- JavaScript
    - [Run JavaScript](/cpp_api?id=run-javascript)
    - [TypeScript Runtimes](/cpp_api?id=typescript-runtimes)

---
### Download

Download Webinix v2.2.0 prebuilt binaries here: https://webinix.me/#download
Download Webinix C++ header file here: https://github.com/alifcommunity/webinix/tree/main/examples/C%2B%2B/minimal

---
### Build from Source

You can build Webinix from source by cloning the Webinix repo and compile it using any C99 compiler, No need for any external dependencies.

Windows MSVC (_Using x64 Native Tools Command Prompt for VS 20xx_)
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\build\Windows\MSVC
nmake
nmake debug
```

Windows MinGW
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\build\Windows\GCC
mingw32-make
mingw32-make debug
```

Windows TCC
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\build\Windows\TCC
mingw32-make
mingw32-make debug
```

Linux GCC
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/build/Linux/GCC
make
make debug
```

Linux Clang
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/build/Linux/Clang
make
make debug
```

macOS Clang
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/build/macOS/Clang
make
make debug
```

---
You can also use the build script to automatically build Webinix and copy binaries into all examples folder.

Windows
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\build\
windows_build
windows_build debug
```

Linux
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/build
sh linux_build.sh
sh linux_build.sh debug
```

macOS
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/build
sh macos_build.sh
sh macos_build.sh debug
```

For more instructions, please visit [Build Webinix](https://github.com/alifcommunity/webinix/tree/main/build) in our GitHub repository.

---
### Examples

A minimal C++ example

```cpp
#include "webinix.h"

int main() {

    void* win = webinix::new_window();
    webinix::show(win, "<html>Hello!</html>");
	webinix::wait();
    return 0;
}
```

Using a local HTML file. Please not that you need to add `<script src="/webinix.js"></script>` to all your HTML files

```cpp
#include "webinix.h"

int main() {

    void* win = webinix::new_window();
    // Please add <script src="/webinix.js"></script> to your HTML files
    webinix::show(win, "my_file.html");
	webinix::wait();
    return 0;
}
```

Using a specific web browser

```cpp
#include "webinix.h"

int main() {

    void* win = webinix::new_window();
    webinix::show_browser(win, "<html>Hello!</html>", Chrome);
	webinix::wait();
    return 0;
}
```

Please visit [C++ Examples](https://github.com/alifcommunity/webinix/tree/main/examples/C%2B%2B) in our GitHub repository for instructions on compiling this example or finding more complete examples.

---
### New Window

To create a new window object, you can use `webinix::new_window()`, which returns a void pointer. Please note that this pointer does *NOT* need to be freed.

```cpp
void* my_window = webinix::new_window();
```

---
### Show Window

To show a window, you can use `webinix::show()`. If the window is already shown, the UI will get refreshed in the same window.

```cpp
// Show a window using the embedded HTML
const char* my_html = "<html>Hello!</html>";
webinix::show(my_window, my_html);

// Show a window using an .html local file
// Please add <script src="/webinix.js"></script> to your HTML files
webinix::show(my_window, "my_file.html");
```

Show a window using a specific web browser

```cpp
const char* my_html = "<html>Hello!</html>";

// Google Chrome
webinix::show_browser(my_window, my_html, Chrome);

// Mozilla Firefox
webinix::show_browser(my_window, my_html, Firefox);

// Microsoft Edge
webinix::show_browser(my_window, my_html, Edge);

// Microsoft Apple Safari (Not Ready)
webinix::show_browser(my_window, my_html, Safari);

// The Chromium Project
webinix::show_browser(my_window, my_html, Chromium);

// Microsoft Opera Browser (Not Ready)
webinix::show_browser(my_window, my_html, Opera);

// The Brave Browser
webinix::show_browser(my_window, my_html, Brave);

// The Vivaldi Browser
webinix::show_browser(my_window, my_html, Vivaldi);

// The Epic Browser
webinix::show_browser(my_window, my_html, Epic);

// The Yandex Browser
webinix::show_browser(my_window, my_html, Yandex);

// Default recommended web browser
webinix::show_browser(my_window, my_html, AnyBrowser);

// Or simply
webinix::show(my_window, my_html);
```

If you need to update the whole UI content, you can also use the same function `webinix::show()`, which allows you to refresh the window UI with any new HTML content.

```cpp
const char* html = "<html>Hello</html>";
const char* new_html = "<html>New World!</html>";

// Open a window
webinix::show(my_window, html);

// Later...

// Refresh the same window with the new content
webinix::show(my_window, new_html);
```

---
### Window Status

To know if a specific window is running, you can use `webinix::is_shown()`.

```cpp
if(webinix::is_shown(my_window))
    std::cout << "The window is still running" << std::endl;
else
    std::cout << "The window is closed." << std::endl;
```

---
### Bind

Use `webinix::bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```cpp
void my_function(webinix_event_t* e) {
    // <button id="MyID">Hello</button> gets clicked!
}

webinix::bind(my_window, "MyID", my_function);
```

Using `webinix::bind()` to call a class member method

```cpp
class MyClass {
    public: void my_function(webinix_event_t* e) {
        // <button id="MyID">Hello</button> gets clicked!
    }
};

// Wrapper:
// Because Webinix is written in C, so it can not
// access `MyClass` directly. That's why we should
// create a simple C++ wrapper.
MyClass obj;
void my_function_wrapper(webinix_event_t* e) { obj.my_function(e); }

webinix::bind(my_window, "MyID", my_function_wrapper);
```

### Events

The *e* corresponds to the word _Event_. `e` is a struct that has these elements:

```cpp
void* window; // Pointer to the window struct.
unsigned int event_type; // Event type (WEBUI_EVENT_MOUSE_CLICK, WEBUI_EVENT_NAVIGATION...).
char* element; // HTML element ID.
char* data; // The data are coming from JavaScript, if any.
char* response; // Internally used by webinix::return_xxx().
```

```cpp
void my_function(webinix_event_t* e){

    std::cout << "Hi!, You clicked on " << e.element << std::endl;

    if (e->event_type == WEBUI_EVENT_CONNECTED)
        std::cout << "Connected." << std::endl;
    else if (e->event_type == WEBUI_EVENT_DISCONNECTED)
        std::cout << "Disconnected." << std::endl;
    else if (e->event_type == WEBUI_EVENT_MOUSE_CLICK)
        std::cout << "Click." << std::endl;
    else if (e->event_type == WEBUI_EVENT_NAVIGATION)
        std::cout << "Starting navigation to: " << e->data << std::endl;

    // Send back a response to JavaScript
    webinix::return_int(e, 123); // As integer
    webinix::return_bool(e, true); // As boolean
    webinix::return_string(e, "My Response"); // As string
}

// Empty ID means all events on all elements
webinix::bind(my_window, "", my_function);
```

---
### Wait

It is essential to call `webinix::wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[webinix::exit()](/cpp_api?id=exit)*.

```cpp
int main() {

	// Create windows...
	// Bind HTML elements...
    // Show the windows...

    // Wait until all windows get closed
    // or when calling webinix::exit()
	webinix::wait();

    return 0;
}
```

---
### Exit

At any moment, you can call `webinix::exit()`, which tries to close all related opened windows and make *[webinix::wait](/cpp_api?id=wait)* break.

```cpp
webinix::exit();
```

---
### Close

You can call `webinix::close()` to close a specific window, if there is no running window left *[webinix::wait](/cpp_api?id=wait)* will break.

```cpp
webinix::close(my_window);
```

---
### Startup Timeout

Webinix waits a couple of seconds (_Default is 30 seconds_) to let the web browser start and connect. You can control this behavior by using `webinix::set_timeout()`.

```cpp
// Wait 10 seconds for the browser to start
webinix::set_timeout(10);

// Now, After 10 seconds, if the browser did
// not get started, wait() will break
webinix::wait();
```

```cpp
// Wait forever.
webinix::set_timeout(0);

// webinix::wait() will never end
webinix::wait();
```

---
### Multi Access

![webinix::access_denied](data/webinix::access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. Webinix will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `webinix::set_multi_access()`.

```cpp
webinix::set_multi_access(my_window, true);
```

---
### Run JavaScript

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```cpp
void my_function(webinix_event_t* e){

	// Create a buffer to hold the response
    char response[64];

    // Run JavaScript
    if(!webinix::script(
        e->window, // Window
        "return 2*2;", // JavaScript to be executed
        0, // Maximum waiting time in second
        response, // Local buffer to hold the JavaScript response
        64) // Size of the local buffer
    ) {
        std::cout << "JavaScript Error: " << response << std::endl;
        return;
    }

    // Print the result
    std::cout << "JavaScript Response: " << response << std::endl; // 4

    // Run JavaScript quickly with no waiting for the response
    webinix::run(e->window, "alert('Fast!');");
}
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `webinix::set_runtime()` and choose between `Deno` or `Nodejs` as your runtimes.

```cpp
// Deno
webinix::set_runtime(my_window, Deno);
webinix::show(my_window, "my_file.ts");

// Nodejs
webinix::set_runtime(my_window, Nodejs);
webinix::show(my_window, "my_file.js");
```
