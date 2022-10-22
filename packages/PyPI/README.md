# Webinix

Use any web browser as GUI, with your preferred language in the backend, and HTML/JS/TS/CSS in the frontend.

```python
from webinix import webinix
MyWindow = webinix.window()
MyWindow.bind('MyID', my_function)
MyWindow.show("MyHTML")
webinix.loop()
```

```sh
python test.py
```

![ScreenShot](https://raw.githubusercontent.com/alifcommunity/webinix/main/screenshot.png)
