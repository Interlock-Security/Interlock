
# Webinix C - Text Editor (Not Complete)

A text editor application in C using Webinix.

### Windows

- **MinGW**
```sh
git clone https://github.com/alifcommunity/webinix.git
cd webinix\examples\C\text-editor
gcc -o text-editor.exe text-editor.c webinix-2-x64.dll -Wl,-subsystem=windows -lcomdlg32
```
