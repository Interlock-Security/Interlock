# Webinix PyPI Package

```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix/packages/PyPI
python -m pip install --upgrade pip
python -m pip install --upgrade build
python -m pip install --upgrade twine
python -m build
python -m twine upload --repository pypi dist/*
```

```sh
pip install --upgrade webinix2
```
