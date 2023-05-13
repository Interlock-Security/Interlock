# V-Webinix v2.3.0 APIs

- [Download and Install](/v_api?id=download-and-install)
- [Examples](/v_api?id=examples)
- Window
    - [New Window](/v_api?id=new-window)
    - [Show Window](/v_api?id=show-window)
    - [Window status](/v_api?id=window-status)
- Binding & Events
    - [Bind](/v_api?id=Bind)
    - [Events](/v_api?id=events)
- Application
    - [Wait](/v_api?id=wait)
    - [Exit](/v_api?id=exit)
    - [Close](/v_api?id=close)
    - [Startup Timeout](/v_api?id=startup-timeout)
    - [Multi Access](/v_api?id=multi-access)
- JavaScript
    - [Run JavaScript From V](/v_api?id=run-javascript-from-c)
    - [Run V From JavaScript](/v_api?id=run-c-from-javascript)
    - [TypeScript Runtimes](/v_api?id=typescript-runtimes)

---
### Download and Install

Install the Webinix package from vpm (*~1.5 Mb*).

`v install webinix-dev.v-webinix`

Or from github

`v install https://github.com/webinix-dev/v-webinix`

---
### Examples

A minimal V example

```v
import webinix-dev.v-webinix as webinix

mut my_window := webinix.new_window()
my_window.show("<html>Hello</html>")
webinix.wait()
```

Using a local HTML file. Please not that you need to add `<script src="/webinix.js"></script>` to all your HTML files

```v
import webinix-dev.v-webinix as webinix

// Please add <script src="/webinix.js"></script> to your HTML files
mut my_window := webinix.new_window()
my_window.show("my_file.html")
webinix.wait()
```

Using a specific web browser

```v
import webinix-dev.v-webinix as webinix

mut my_window := webinix.new_window()
my_window.show_browser("<html>Hello</html>", webinix.browser_chrome)
webinix.wait()
```

Please visit [V Examples](https://github.com/webinix-dev/v-webinix/tree/main/examples/V) in our GitHub repository for instructions on compiling this example or finding more complete examples.

---
### New Window

To create a new window object, you can use `new_window()`, which returns a void pointer. Please note that this pointer does *NOT* need to be freed.

```v
import webinix-dev.v-webinix as webinix

mut my_window := webinix.new_window()
```

---
### Show Window

To show a window, you can use `webinix_show()`. If the window is already shown, the UI will get refreshed in the same window.

```v
import webinix-dev.v-webinix as webinix
// Create a new window
mut my_window := webinix.new_window()
// Show a window using the embedded HTML
my_window.show("<html>Hello!</html>")

// Show a window using an .html local file
// Please add <script src="/webinix.js"></script> to your HTML files
my_window.show("my_file.html")
```

Show a window using a specific web browser

```v
import webinix-dev.v-webinix as webinix

my_html := "<html>Hello!</html>";

// Google Chrome
my_window.show_browser(my_html, webinix.browser_chrome)

// Mozilla Firefox
my_window.show_browser(my_html, webinix.browser_firefox)

// Microsoft Edge
my_window.show_browser(my_html, webinix.browser_edge)

// Apple Safari (Not Ready)
my_window.show_browser(my_html, webinix.browser_safari)

// The Chromium Project
my_window.show_browser(my_html, webinix.browser_chromium)

// The Opera Browser (Not Ready)
my_window.show_browser(my_html, webinix.browser_opera)

// The Brave Browser
my_window.show_browser(my_html, webinix.browser_brave)

// The Vivaldi Browsex
my_window.show_browser(my_html, webinix.browser_vivaldi)

// The Epic Browser
my_window.show_browser(my_html, webinix.browser_epic)

// The Yandex Browser
my_window.show_browser(my_html, webinix.browser_yandex)

// Default recommended web browser
my_window.show_browser(my_html, webinix.browser_any)

// Or simply
webinix_show(my_window, my_html)
```

If you need to update the whole UI content, you can also use the same function `show()`, which allows you to refresh the window UI with any new HTML content.

```v
import webinix-dev.v-webinix as webinix

html := "<html>Hello</html>"
new_html := "<html>New World!</html>"

// Open a window
my_window.show(html)

// Later...

// Refresh the same window with the new content
my_window.show(new_html)
```

---
### Window Status

To know if a specific window is running, you can use `is_shown()`.

```v
import webinix-dev.v-webinix as webinix

if webinix.is_shown(my_window) {
    println("The window is still running")
} else {
    println("The window is closed.")
}
```

---
### Bind

Use `bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```v
import webinix-dev.v-webinix as webinix
fn my_function_string(e &webinix.Event) {
    // <button id="MyID">Hello</button> gets clicked!
}

my_window.bind("MyID", my_function)
```

### Events

`Event` is a struct that has these elements:

```v
import webinix-dev.v-webinix as webinix
pub struct Event {
	pub mut:
		window			Window // Pointer to the window object
		event_type		u64 // Event type
		element			&char // HTML element ID
		data			&char // JavaScript data
		event_number	u64 // To set the callback response
}
```

Also you can see the `element` and `data` is not native V string. So you can use these functions to get values as native string without requiring a conversion steps.

```v
import webinix-dev.v-webinix as webinix

fn my_function(e &webinix.Event) {
    // Get data
    str := e.get().str // As a string
    number := e.get().int // As an int
    status := e.get().bool // As a boolean
    // Also you can use `json` module from V standart library to parse json

    // Get target element
    target_element := e.element()
}
```

```v
import webinix-dev.v-webinix as webinix

fn my_function(e &webinix.Event) { 
    // This function gets called every time
    // there is an event
    if e.event_type == webinix.event_connected {
        println("Connected.")
    } else if e.event_type == webinix.event_disconnected {
        println("Disconnected.")
    } else if e.event_type == webinix.event_mouse_click {
        println("Click.")
    } else if e.event_type == webinix.event_navigation {
        //println("Starting navigation to: ${e.data}")
    }

    // Send back a response to JavaScript
    e.@return("Hi!") // Webinix will handle type automatically
    // string, int, bool is supported
}

// Empty ID means all events on all elements
webinix_bind(my_window, "", my_function)
```

---
### Wait

It is essential to call `wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[exit()](/v_api?id=exit)*.

```v
import webinix-dev.v-webinix as webinix
int main() {

	mut my_window := webinix.new_window() // Create windows...
	// Bind HTML elements...
    // Show the windows...
    // Show a window using the embedded HTML

    // Wait until all windows get closed
    // or when calling webinix.exit()
	my_window.wait()

    return 0;
}
```

---
### Exit

At any moment, you can call `exit()`, which tries to close all related opened windows and make *[webinix.wait](/v_api?id=wait)* break.

```v
import webinix-dev.v-webinix as webinix
webinix.exit()
```

---
### Close

You can call `webinix.close()` to close a specific window, if there is no running window left *[webinix_wait](/v_api?id=wait)* will break.

```v
import webinix-dev.v-webinix as webinix
mut my_window := webinix.new_window() 
my_window.close()
```

---
### Startup Timeout

Webinix waits a couple of seconds (_Default is 30 seconds_) to let the web browser start and connect. You can control this behavior by using `webinix.set_timeout()`.

```v
import webinix-dev.v-webinix as webinix
// Wait 10 seconds for the browser to start
webinix.set_timeout(10)

mut my_window := webinix.new_window() 

// Now, After 10 seconds, if the browser did
// not get started, wait() will break
my_window.wait()
```

```v
import webinix-dev.v-webinix as webinix
// Wait forever.
webinix.set_timeout(0)

mut my_window := webinix.new_window() 

// webinix_wait() will never end
my_window.wait()
```

---
### Multi Access

![webinix_access_denied](data/webinix_access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. V-Webinix will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `webinix_set_multi_access()`.

```v
import webinix-dev.v-webinix as webinix
mut my_window := webinix.new_window() 

my_window.set_multi_access(true)
```

---
### Run JavaScript From V

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```v
import webinix-dev.v-webinix as webinix

fn my_function(e &webinix.Event) {

    // Run JavaScript
    response := e.window.script("return 2*2;", 0, 64)

    // Print the result
    println("JavaScript Response: ${response.int()}") // 4

    // Run JavaScript quickly with no waiting for the response
    e.window.run("alert('Fast!');")
}
```

---
### Run V From JavaScript

To call a V function from JavaScript and get the result back please use `webinix_fn('MyID', 'My Data').then((response) => { ... });`. If the function does not have a response then it's safe to remove the `then` method like this `webinix_fn('MyID_NoResponse', 'My Data');`.

```v
import webinix-dev.v-webinix as webinix

mut my_window := webinix.new_window() 

void my_function(webinix_event_t* e) {
    // Get data from JavaScript
    str = e.get().str
    // number = e.get().int
    // status = e.get().bool

    // Print the received data
    println("Data from JavaScript: ${str}") // Message from JS

    // Return back a response to JavaScript
    e.@return(e, "Message from V")
    // e.@return(e, number)
    // e.@return(e, true)
    // Webinix will handle type automatically
}

my_window.bind("MyID", my_function)
```

JavsScript:

```js
webinix_fn('MyID', 'Message from JS').then((response) => {
    console.log(response); // "Message from V
});
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `set_runtime()` and choose between `runtime_deno` or `runtime_nodejs` as your runtimes.

```v
import webinix-dev.v-webinix as webinix

mut my_window := webinix.new_window()

// Deno
my_window.set_runtime(webinix.runtime_deno)
my_window.show("my_file.ts")

// Nodejs
my_window.set_runtime(webinix.runtime_nodejs)
my_window.show("my_file.js")
```
