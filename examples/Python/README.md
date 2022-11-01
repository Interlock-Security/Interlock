
# Webinix Examples - Python

[![PyPI version](https://img.shields.io/pypi/v/webinix2?style=for-the-badge)](https://pypi.org/project/webinix2/)

* **Minimal**: The minimal code to use Webinix
* **Hello World**: An example of how  to use Webinix & JavaScript
* **Dev**: A test script to use the local Webinix module instead of the installed one. It's for debugging & development of the Webinix purpose only.

```sh
pip install webinix2
```

```python
from webinix import webinix
MyWindow = webinix.window()
MyWindow.bind('MyID', my_function)
MyWindow.show("MyHTML")
webinix.wait()
```

```sh
python test.py
```
