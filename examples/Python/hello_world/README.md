
# Webinix Examples - Python

To use Webinix in your Python script, you will need to install it using pip, `pip install --upgrade webinix2`. The source code of the Python Webinix module is [here](https://github.com/alifcommunity/webinix/tree/main/packages/PyPI/src/webinix)

```sh
pip install --upgrade webinix2
```

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
