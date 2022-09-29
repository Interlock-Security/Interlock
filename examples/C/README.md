
# Webinix Examples - C

To build a C Webinix application, you will need to [build](https://github.com/alifcommunity/webinix/tree/main/build) the Webinix library first, then build your application using the `static` version of Webinix, or the `dynamic` one. Static version make your final executable totally portable. While the dynamic version make your application need the Webinix shared library (*.dll/.so*) to run properly.

## Windows

- **Build Tools for Microsoft Visual Studio - Static**
```sh
copy ..\..\build\Windows\MSVC\webinix-2-static-x64.lib webinix-2-static-x64.lib
rc win.rc
cl "example.c" /I "../../include" /link /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webinix-2-static-x64.lib /OUT:example.exe
```

- **Build Tools for Microsoft Visual Studio - Dynamic**
```sh
copy ..\..\build\Windows\MSVC\webinix-2-x64.dll webinix-2-x64.dll
copy ..\..\build\Windows\MSVC\webinix-2-x64.lib webinix-2-x64.lib
rc win.rc
cl "example.c" /I "../../include" /link /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webinix-2-x64.lib /OUT:example.exe
```

- **MinGW - Static**
```sh
copy ..\..\build\Windows\GCC\libwebinix-2-static-x64.a libwebinix-2-static-x64.a
windres win.rc -O coff -o win.res
gcc -static -Os -m64 -o example.exe "example.c" -I "../../include" -L. win.res -lwebinix-2-static-x64 -lws2_32 -Wall -Wl,-subsystem=windows -luser32
strip --strip-all example.exe
```

- **MinGW - Dynamic**
```sh
copy ..\..\build\Windows\GCC\webinix-2-x64.dll webinix-2-x64.dll
windres win.rc -O coff -o win.res
gcc -m64 -o example.exe "example.c" -I "../../include" -L. win.res webinix-2-x64.dll -lws2_32 -Wall -Wl,-subsystem=windows -luser32
strip --strip-all example_dynamic.exe
```

- **TCC - Static**
```sh
copy ..\..\build\Windows\TCC\libwebinix-2-static-x64.a libwebinix-2-static-x64.a
tcc -m64 -o example.exe "example.c" -I "../../include" -L. -lwebinix-2-static-x64 -lws2_32 -Wall -Wl,-subsystem=windows -luser32
```

- **TCC - Dynamic**
```sh
copy ..\..\build\Windows\TCC\webinix-2-x64.dll webinix-2-x64.dll
tcc -impdef webinix-2-x64.dll -o webinix-2-x64.def
tcc -m64 -o example_dynamic.exe "example.c" -I "../../include" -L. webinix-2-x64.def -lws2_32 -Wall -Wl,-subsystem=windows -luser32
```
