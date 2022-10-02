# Webinix Python API's

- [Import Webinix](/python_api?id=Import-Webinix)
- [New Window](/python_api?id=New-Window)
- [Bind](/python_api?id=Bind)
- [Bind All](/python_api?id=Bind-All)
- [Show Window](/python_api?id=Show-Window)
- [Loop](/python_api?id=Loop)
- [Event](/python_api?id=Event)

### Import Webinix

Your Python script should import the Webinix module which is a Webinix wrapper in Python, basically this module use the Webinix dynamic library to provide you an easy to use API's to create Webinix windows and all available features. You can download the latest module from the [GitHub Repo](https://github.com/alifcommunity/webinix/blob/main/examples/Python/webinix.py).

```python
# Importing the 'webinix.py' file
import webinix
```

### New Window

To create a new window object we use `webinix.window()` which return the window object.

```python
MyWindow = webinix.window()
```

### Bind

To receive a click event (*function call*) when the user click on an HTML element (*all kind of elements*) thats have an HTML id `MyID` for example `<button id="MyID">Hello</button>`, we use `MyWindow.bind()`.

```python
def my_function(e : webinix.event):
	...

MyWindow.bind('MyID', my_function)
```

### Bind All

You can also automatically bind all HTML element with one function call.

```python
...
```

### Show Window

To show a window we use `webinix_show()`. If the window is already shown the UI will get refreshed in the same window.

```python
my_html = """
<!DOCTYPE html>
<html>
	...
</html>
"""

# Chrome
...

# Firefox
...

# Microsoft Edge
...

# Any available browser
MyWindow.show(my_html)
```

### Loop

It's very important to call `webinix.loop()` at the end of your main function after you created/shows all your windows.

```python
webinix.loop()
```

### Event

When you use [MyWindow.bind()](/python_api?id=Bind) your application receive an event every time the user click on the HTML element you selected. The event come with the `element_name` which is The HTML ID of the clicked element, example `MyButton`, `MyInput`.., The event come also with the Webinix unique element ID and the unique window ID, those two IDs are not needed in general except if you are written a wrapper for Webinix in other language than C.

```python
def my_function(e : webinix.event):
    print('Hi!, You clicked on ' + e.element_name.decode('utf-8') + ' element')
```
