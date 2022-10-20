
# Webinix Examples - Python

To use Webinix in your Python script, you will need to [build](https://github.com/alifcommunity/webinix/tree/main/build) the Webinix library first using your favorite C compiler, then copy into this folder the dynamic Webinix library, `webinix-2-x64.dll` on Windows, or `webinix-2-x64.so` on Linux. Or, instead of coping the lib you can specify the path in your python script `webinix.set_library_path("path/to/webinix")`

```sh
python main.py
```
