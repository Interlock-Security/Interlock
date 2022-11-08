# Webinix Python APIs

- [Get Started](/python_api?id=get-started)
- [Example](/python_api?id=example)
- Window
	- [New Window](/python_api?id=new-window)
	- [Show Window](/python_api?id=show-window)
	- [Window status](/python_api?id=window-status)
- Binding
	- [Bind](/python_api?id=Bind)
	- [Bind All](/python_api?id=bind-all)
- Application
	- [Wait](/python_api?id=wait)
	- [Exit](/python_api?id=exit)
	- [Close](/python_api?id=close)
	- [App status](/python_api?id=app-status)
	- [Startup Timeout](/python_api?id=startup-timeout)
	- [Multi Access](/python_api?id=multi-access)
- [Event](/python_api?id=event)
- [Run JavaScript](/python_api?id=script)
- [Server](/python_api?id=server)

---
### Get Started

This document includes all Webinix APIs available for Python. To begin, you need to install Webinix (*~360 Kb*).

PIP
```console
pip install webinix2
```

To see the core Webinix Python wrapper, please visit [Webinix Python](https://github.com/alifcommunity/webinix/tree/main/packages/PyPI/src/webinix) in our GitHub repository.

---
### Example

A minimal Python example

```python
from webinix import webinix

MyWindow = webinix.window()
MyWindow.show('<html>Hello World</html>')
webinix.wait()
```

Please visit [Python Examples](https://github.com/alifcommunity/webinix/tree/main/examples/Python) in our GitHub repository for finding more complete examples.

---
### New Window

To create a new window object, you can use `webinix.window()`, which returns a `window` object.

```python
MyWindow = webinix.window()
```

---
### Show Window

To show a window, you can use `show()`. If the window is already shown, the UI will get refreshed in the same window.

```python
my_html = "<html>Hello!</html>"

MyWindow.show(my_html)
```

Show a window in a specific web browser

```python
my_html = "<html>Hello!</html>"

# Chrome
MyWindow.show(my_html, webinix.browser.chrome)

# Firefox
MyWindow.show(my_html, webinix.browser.firefox)

# Microsoft Edge
MyWindow.show(my_html, webinix.browser.edge)

# Any available web browser
MyWindow.show(my_html, webinix.browser.any)
```

If you need to update the whole UI content, you can use `refresh()`, which allows you to refresh the window UI with any new HTML content.

```python
html = "<html>Hello</html>"
new_html = "<html>New World!</html>"

# Open a window in Chrome
MyWindow.show(html, webinix.browser.chrome)

# Later...
time.sleep(5)

# Refresh the same window with the new content
MyWindow.refresh(new_html)
```

---
### Window Status

In some exceptional cases, you want to know if any opened window exists, for that, please use `is_any_window_running()`, which returns *True* or *False*.

```python
if MyWindow.is_any_window_running():
	print("A window is running...")
else:
	print("No window is running.")
```

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
	...

MyWindow.bind("MyID", my_function)
```

---
### Bind All

You can also automatically bind all HTML elements with one function call.

```python
def all_clicks(e : webinix.event)
	...

MyWindow.bind_all(all_clicks)
```

---
### Wait

It is essential to call `wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[MyWindow.exit](/python_api?id=exit)*.

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

At any moment, you can call `exit()`, which tries to close all related opened windows and make *[MyWindow.wait](/python_api?id=wait)* break.

```python
webinix.exit()
```

---
### Close

You can call `close()` to close a specific window, if there is no running window left *[MyWindow.wait](/python_api?id=wait)* will break.

```python
MyWindow.close()
```

---
### App Status

In some exceptional cases, like in the Webinix-TypeScript wrapper, you want to know if the whole application still running or not, for that, please use `is_app_running()`, which returns *True* or *False*.

```python
if webinix.is_app_running():
	print("The application is still running")
else:
	print("The application is closed.")
```

---
### Startup Timeout

Webinix waits a couple of seconds to let the web browser start and connect, you can control this behavior by using `set_timeout()`.

```python
# Wait 10 seconds for the web browser to start
webinix.set_timeout(10)
webinix.wait()	# After 10 seconds, if the web browser
				# did not start yet, this function will return
```
```python
# Wait forever.
webinix.set_timeout(0)
webinix.wait() # this function will never end
```

### Multi Access

![webinix_access_denied](data/webinix_access_denied.png)

After the window is loaded, for safety, the used URL is not valid anymore, if someone else tries to access the URL Webinix will show an error. To allow multi-user access to the same URL, you can use `multi_access()`.

```python
MyWindow.multi_access(True)
```

---
### Event

When you use *[MyWindow.bind](/python_api?id=bind)*, your application will receive an event every time the user clicks on the specified HTML element. The event comes with the `element_name`, which is The HTML ID of the clicked element, for example, `MyButton`, `MyInput`.., The event also comes with the Webinix unique element ID & the unique window ID. Those two IDs are not generally needed, except if you write a wrapper for Webinix in a language other than C.

```python
def my_function(e : webinix.event):
	print('Hi!, You clicked on ' + e.element_name + ' element')
```

The *e* corresponds to Event, and it has those elements:

```python
e.window_id     # Webinix unique window ID
e.element_id    # Webinix unique element ID
e.element_name  # The HTML ID of the clicked element
e.window        # The current window object
```

---
### Script

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check for execution errors, as well as receive data.

```python
def my_function(e : webinix.event):
	e.window.run_js("alert('Hello');")
```

An example of how to run a JavaScript and get back the output as string, and check for errors, if any.

```python
def my_function(e : webinix.event):
	res = e.window.run_js("var foo = 4; var bar = 2; return foo*bar;") # Return '8'

	# Check for any error
	if res.error is True:
		print("JavaScript Error: " + res.data)
	else:
		print("Output: " + res.data) # '8'
```

---
### Server

You can use Webinix to serve a folder, which makes Webinix act like a web server. To do that, please use `new_server()`, which returns the complete URL of the server.

```python
# Serve a folder
url = MyWindow.new_server("/path/to/folder")
```

```python
# Automatically select the current path
url = MyWindow.new_server("")
```

When you serve a folder, you probably want to run JavaScript & TypeScript files and show the output in the UI. To do that, you can use `script_runtime`, which makes Webinix act like Nodejs.

```python
# Chose your preferable runtime for .js & .ts files
# Deno: webinix.runtime.deno
# Node.js: webinix.runtime.nodejs

# Deno
MyWindow.script_runtime(webinix.runtime.deno)

# Nodejs
MyWindow.script_runtime(webinix.runtime.nodejs)

# Disable
MyWindow.script_runtime(webinix.runtime.none)
```

If you already have a URL, you can use Webinix to open a window using this URL. For that, please use `open()`.

```python
webinix.set_timeout(0) # (Optional) Let the server run forever
MyWindow.open(my_url, webinix.browser.chrome)
```

In addition, it can make Webinix track clicks and send you events by embedding the Webinix JavaScript bridge file `webinix.js`. Of course, this will work only if the server is Webinix.

```html
<script src="/webinix.js"></script>
```
