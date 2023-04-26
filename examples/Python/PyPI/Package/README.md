# Webinix

Use any web browser as GUI, with Python in the backend and HTML5 in the frontend, all in a lightweight portable lib.

* [Online Documentation](https://webinix.me/docs/#/python_api)

```sh
pip install webinix2
```

```python
from webinix import webinix

def my_function(e : webinix.event)
    print("Hi!, You clicked on " + e.element + " element")

MyWindow = webinix.window()
MyWindow.bind("MyID", my_function)
MyWindow.show("<html>Hello World</html>")
webinix.wait()
```

```sh
python test.py
```

![ScreenShot](https://raw.githubusercontent.com/alifcommunity/webinix/main/screenshot.png)
