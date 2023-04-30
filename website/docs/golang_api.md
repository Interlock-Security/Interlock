# Webinix v2.2.0 Go APIs

- [Build From Source](/c_api?id=build-from-source)
- [Examples](/golang_api?id=examples)
- Window
    - [New Window](/golang_api?id=new-window)
    - [Show Window](/golang_api?id=show-window)
    - [Window status](/golang_api?id=window-status)
- Binding & Events
    - [Bind](/golang_api?id=bind)
    - [Events](/golang_api?id=events)
- Application
    - [Wait](/golang_api?id=wait)
    - [Exit](/golang_api?id=exit)
    - [Close](/golang_api?id=close)
    - [Startup Timeout](/golang_api?id=startup-timeout)
    - [Multi Access](/golang_api?id=multi-access)
- JavaScript
    - [Run JavaScript From Go](/golang_api?id=run-javascript-from-go)
    - [Run Go From JavaScript](/golang_api?id=run-go-from-javascript)
    - [TypeScript Runtimes](/golang_api?id=typescript-runtimes)

---
### Build from Source

You can build Webinix Golang wrapper from source by cloning the Webinix repo and compile it, tested with Go v1.20.3.

### Windows
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\examples\Go\hello_world
go clean
go build -o hello_world.exe
hello_world
```

### Linux
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/examples/Go/hello_world
go clean
go build -o hello_world
./hello_world
```

### macOS
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/examples/Go/hello_world
go clean
go build -o hello_world
./hello_world
```

To see the Webinix Golang wrapper source code, please visit [Webinix](https://github.com/alifcommunity/webinix/) in our GitHub repository.

---
### Examples

A minimal Golang example

```go
import "github.com/alifcommunity/webinix/webinix"

var my_window = webinix.NewWindow()
webinix.Show(my_window, "<html>Hello World</html>")
webinix.Wait()
```

Using a local HTML file. Please not that you need to add `<script src="/webinix.js"></script>` to all your HTML files

```go
import "github.com/alifcommunity/webinix/webinix"

var my_window = webinix.NewWindow()
webinix.Show(my_window, "my_file.html")
webinix.Wait()
```

Using a specific web browser

```go
import "github.com/alifcommunity/webinix/webinix"

var my_window = webinix.NewWindow()
// Please add <script src="/webinix.js"></script> to your HTML files
webinix.ShowBrowser(my_window, "my_file.html", webinix.Chrome)
webinix.Wait()
```

Please visit [Golang Examples](https://github.com/alifcommunity/webinix/tree/main/examples/Go) in our GitHub repository for more complete examples.

---
### New Window

To create a new window object, you can use `webinix.NewWindow()`, which returns a `Pointer` to the window object.

```go
var my_window = webinix.NewWindow()
```

---
### Show Window

To show a window, you can use `show()`. If the window is already shown, the UI will get refreshed in the same window.

```go
// Show a window using the embedded HTML
webinix.Show(my_window, "<html>Hello!</html>")
```

```go
// Show a window using an .html local file
// Please add <script src="/webinix.js"></script> to your HTML files
webinix.Show(my_window, "my_file.html")
```

Show a window using a specific web browser

```go
const my_html string = "<html>Hello!</html>"

// Google Chrome
webinix.ShowBrowser(my_window, my_html, webinix.Chrome)

// Mozilla Firefox
webinix.ShowBrowser(my_window, my_html, webinix.Firefox)

// Microsoft Edge
webinix.ShowBrowser(my_window, my_html, webinix.Edge)

// Microsoft Apple Safari (Not Ready)
webinix.ShowBrowser(my_window, my_html, webinix.Safari)

// The Chromium Project
webinix.ShowBrowser(my_window, my_html, webinix.Chromium)

// Microsoft Opera Browser (Not Ready)
webinix.ShowBrowser(my_window, my_html, webinix.Opera)

// The Brave Browser
webinix.ShowBrowser(my_window, my_html, webinix.Brave)

// The Vivaldi Browser
webinix.ShowBrowser(my_window, my_html, webinix.Vivaldi)

// The Epic Browser
webinix.ShowBrowser(my_window, my_html, webinix.Epic)

// The Yandex Browser
webinix.ShowBrowser(my_window, my_html, webinix.Yandex)

// Default recommended web browser
webinix.ShowBrowser(my_window, my_html, webinix.AnyBrowser)

// Or simply
webinix.Show(my_window, my_html)
```

If you need to update the whole UI content, you can also use the same function `show()`, which allows you to refresh the window UI with any new HTML content.

```go
const html string = "<html>Hello</html>"
const new_html string = "<html>New World!</html>"

// Open a window
webinix.Show(my_window, html)

// Later...

// Refresh the same window with the new content
webinix.Show(my_window, new_html)
```

---
### Window Status

To know if a specific window is running, you can use `IsShown()`.

```go
if webinix.IsShown(my_window) {
    fmt.Printf("The window is still running")
} else {
    fmt.Printf("The window is closed.")
}
```

---
### Bind

Use `Bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```go
func my_function(e webinix.Event) string {
	// <button id="MyID">Hello</button> gets clicked!
	return ""
}

webinix.Bind(my_window, "MyID", my_function)
```

### Events

```go
func my_function(e webinix.Event) string {
	fmt.Printf("Hi!, You clicked on element: %s\n", e.Element)
	return ""
}

// Empty ID means all events on all elements
webinix.Bind(my_window, "", events)
```

The *e* corresponds to the word _Event_. `e` is a struct that has these elements:

```go
Window // Pointer: The window object
EventType // uint: Event type (EVENT_MOUSE_CLICK, EVENT_NAVIGATION...).
Element // string: HTML element ID.
Data // string: The data are coming from JavaScript, if any.
```

---
### Wait

It is essential to call `Wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[Exit()](/golang_api?id=exit)*.

```go
// Create windows...
// Bind HTML elements...
// Show the windows...

// Wait until all windows get closed
// or when calling webinix.Exit()
webinix.Wait()
```

---
### Exit

At any moment, you can call `Exit()`, which tries to close all related opened windows and make *[Wait](/golang_api?id=wait)* break.

```go
webinix.Exit()
```

---
### Close

You can call `Close()` to close a specific window, if there is no running window left *[Wait](/golang_api?id=wait)* will break.

```go
webinix.Close(my_window)
```

---
### Startup Timeout

Webinix waits a couple of seconds (_Default is 30 seconds_) to let the web browser start and connect. You can control this behavior by using `SetTimeout()`.

```go
// Wait 10 seconds for the browser to start
webinix.SetTimeout(10)

// Now, After 10 seconds, if the browser did
// not get started, Wait() will break
webinix.Wait()
```

```go
// Wait forever.
webinix.SetTimeout(0)

// Wait() will never end
webinix.Wait()
```

---
### Multi Access

![access_denied](data/webinix_access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. Webinix will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `SetMultiAccess()`.

```go
webinix.SetMultiAccess(my_window, true)
```

---
### Run JavaScript From Go

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```go
// Create new JavaScript object
js := webinix.NewJavaScript()

// Run the script
if !webinix.Script(e.Window, &js, "return 2*2;") {
    // Error
    fmt.Printf("JavaScript Error: %s\n", js.Response)
}

// Print the Response
fmt.Printf("JavaScript Response: %s\n", js.Response)

// Run JavaScript quickly with no waiting for the response
webinix.Run(e.Window, "alert('Fast!')")
```

---
### Run Go From JavaScript

To call a Go function from JavaScript and get the result back please use `webinix_fn('MyID', 'My Data').then((response) => { ... });`. If the function does not have a response then it's safe to remove the `then` method like this `webinix_fn('MyID_NoResponse', 'My Data');`.

```go
func my_function(e webinix.Event) string {
	
    // Get data from JavaScript
    fmt.Printf("Data from JavaScript: %s\n", e.Data) // Message from JS

    // Return back a response to JavaScript
    return "Message from Go"
}

webinix.Bind(my_window, "MyID", my_function)
```

JavsScript:

```js
webinix_fn('MyID', 'Message from JS').then((response) => {
    console.log(response); // "Message from Go
});
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `SetRuntime()` and choose between `Deno` or `Nodejs` as your runtimes.

```go
// Deno
webinix.SetRuntime(my_window, webinix.Deno)
webinix.Show(my_window, "my_file.ts")

// Nodejs
webinix.SetRuntime(my_window, webinix.Nodejs)
webinix.Show(my_window, "my_file.js")
```
