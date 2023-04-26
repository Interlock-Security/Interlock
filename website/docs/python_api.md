# Webinix v2.2.0 Python APIs

- [Download And Install](/python_api?id=download-and-install)
- [Examples](/python_api?id=examples)
- Window
    - [New Window](/python_api?id=new-window)
    - [Show Window](/python_api?id=show-window)
    - [Window status](/python_api?id=window-status)
- Binding & Events
    - [Bind](/python_api?id=Bind)
    - [Events](/python_api?id=events)
- Application
    - [Wait](/python_api?id=wait)
    - [Exit](/python_api?id=exit)
    - [Close](/python_api?id=close)
    - [Startup Timeout](/python_api?id=startup-timeout)
    - [Multi Access](/python_api?id=multi-access)
- JavaScript
    - [Run JavaScript](/python_api?id=run-javascript)
    - [TypeScript Runtimes](/python_api?id=typescript-runtimes)

---
### Download And Install

Install the Webinix package from PyPI (*~360 Kb*).

```sh
pip install webinix2
```

To see the Webinix Python wrapper source code, please visit [Webinix](https://github.com/alifcommunity/webinix/) in our GitHub repository.

---
### Examples

A minimal Python example

```python
from webinix import webinix

MyWindow = webinix.window()
MyWindow.show('<html>Hello World</html>')
webinix.wait()
```

Using a local HTML file. Please not that you need to add `<script src="/webinix.js"></script>` to all your HTML files

```python
from webinix import webinix

MyWindow = webinix.window()
# Please add <script src="/webinix.js"></script> to your HTML files
MyWindow.show('my_file.html')
webinix.wait()
```

Using a specific web browser

```python
from webinix import webinix

MyWindow = webinix.window()
MyWindow.show('<html>Hello World</html>', webinix.browser.chrome)
webinix.wait()
```

Please visit [Python Examples](https://github.com/alifcommunity/webinix/tree/main/examples/Python) in our GitHub repository for more complete examples.

---
### New Window

To create a new window object, you can use `webinix.window()`, which returns a `window` class object.

```python
MyWindow = webinix.window()
```

---
### Show Window

To show a window, you can use `show()`. If the window is already shown, the UI will get refreshed in the same window.

```python
# Show a window using the embedded HTML
MyWindow.show("<html>Hello!</html>")
```

```python
# Show a window using an .html local file
# Please add <script src="/webinix.js"></script> to your HTML files
MyWindow.show("my_file.html")
```

Show a window using a specific web browser

```python
my_html = "<html>Hello!</html>"

# Google Chrome
MyWindow.show(my_html, webinix.browser.chrome)

# Mozilla Firefox
MyWindow.show(my_html, webinix.browser.firefox)

# Microsoft Edge
MyWindow.show(my_html, webinix.browser.edge)

# Microsoft Apple Safari (Not Ready)
MyWindow.show(my_html, webinix.browser.safari)

# The Chromium Project
MyWindow.show(my_html, webinix.browser.chromium)

# Microsoft Opera Browser (Not Ready)
MyWindow.show(my_html, webinix.browser.opera)

# The Brave Browser
MyWindow.show(my_html, webinix.browser.brave)

# The Vivaldi Browser
MyWindow.show(my_html, webinix.browser.vivaldi)

# The Epic Browser
MyWindow.show(my_html, webinix.browser.epic)

# The Yandex Browser
MyWindow.show(my_html, webinix.browser.yandex)

# Default recommended web browser
MyWindow.show(my_html, webinix.browser.any)

# Or simply
MyWindow.show(my_html)
```

If you need to update the whole UI content, you can also use the same function `show()`, which allows you to refresh the window UI with any new HTML content.

```python
html = "<html>Hello</html>"
new_html = "<html>New World!</html>"

# Open a window
MyWindow.show(html)

# Later...
time.sleep(5)

# Refresh the same window with the new content
MyWindow.show(new_html)
```

---
### Window Status

To know if a specific window is running, you can use `is_shown()`.

```python
if MyWindow.is_shown():
	print("The window is still running")
else
	print("The window is closed.")
```

---
### Bind

Use `bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```python
def my_function(e : webinix.event)
	# <button id="MyID">Hello</button> gets clicked!

MyWindow.bind("MyID", my_function)
```

### Events

```python
def events(e : webinix.event)
	print('Hi!, You clicked on ' + e.element + ' element')

# Empty ID means all events on all elements
MyWindow.bind("", events)
```

The *e* corresponds to the word _Event_. `e` is a struct that has these elements:

```python
window; # The window object.
type; # Integer: Event type (EVENT_MOUSE_CLICK, EVENT_NAVIGATION...).
element; # String: HTML element ID.
data; # String: The data are coming from JavaScript, if any.
```

---
### Wait

It is essential to call `wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[exit()](/python_api?id=exit)*.

```python
# Create windows...
# Bind HTML elements...
# Show the windows...

# Wait until all windows get closed
# or when calling MyWindow.exit()
webinix.wait()
```

---
### Exit

At any moment, you can call `exit()`, which tries to close all related opened windows and make *[wait](/python_api?id=wait)* break.

```python
webinix.exit()
```

---
### Close

You can call `close()` to close a specific window, if there is no running window left *[wait](/python_api?id=wait)* will break.

```python
MyWindow.close()
```

---
### Startup Timeout

Webinix waits a couple of seconds (_Default is 30 seconds_) to let the web browser start and connect. You can control this behavior by using `set_timeout()`.

```python
# Wait 10 seconds for the browser to start
webinix.set_timeout(10)

# Now, After 10 seconds, if the browser did
# not get started, wait() will break
webinix.wait()
```

```python
# Wait forever.
webinix.set_timeout(0)

# wait() will never end
webinix.wait()
```

---
### Multi Access

![access_denied](data/webinix_access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. Webinix will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `set_multi_access()`.

```python
MyWindow.set_multi_access(True)
```

---
### Run JavaScript

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```python
# Run JavaScript to get the password
res = e.window.script("return 2*2;")

# Check for any error
if res.error is True:
	print("JavaScript Error: " + res.data)
else:
	print("JavaScript Response: " + res.data) # 4

# Run JavaScript quickly with no waiting for the response
e.window.run("alert('Fast!')")
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `set_runtime()` and choose between `Deno` or `Nodejs` as your runtimes.

```python
# Deno
MyWindow.set_runtime(webinix.runtime.deno)
MyWindow.show("my_file.ts")

# Nodejs
MyWindow.set_runtime(webinix.runtime.nodejs)
MyWindow.show("my_file.js")
```
