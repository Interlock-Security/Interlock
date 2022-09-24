# Build Webinix Dynamic & Static library
# Windows - GCC - Tiny C Compiler

all: static

dynamic:
	tcc -fPIC -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	tcc -fPIC -m64 -o webinix.o -I "../../../include" -c "../../../src/webinix.c"
	tcc -shared -o webinix-2-x64.dll webinix.o mongoose.o -lws2_32
	tcc -impdef webinix-2-x64.dll -o webinix-2-x64.def
	tcc -m64 -o example_dynamic.exe "../../example.c" -I "../../../include" -L. webinix-2-x64.def -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	- del webinix.o
	- del mongoose.o

dynamic-debug:
	tcc -g -fPIC -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	tcc -g -fPIC -m64 -o webinix.o -I "../../../include" -c "../../../src/webinix.c"
	tcc -g -shared -o webinix-2-x64.dll webinix.o mongoose.o -lws2_32
	tcc -impdef webinix-2-x64.dll -o webinix-2-x64.def
	tcc -g -m64 -o example_dynamic.exe "../../example.c" -I "../../../include" -L. webinix-2-x64.def -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	- del webinix.o
	- del mongoose.o

static:
	tcc -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	tcc -m64 -o webinix.o -I "../../../include" -c "../../../src/webinix.c"
	tcc -m64 -ar rcs libwebinix-2-x64.a webinix.o mongoose.o
	tcc -m64 -o example_static.exe "../../example.c" -I "../../../include" -L. -lwebinix-2-x64 -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	- del webinix.o
	- del mongoose.o

static-debug:
	tcc -g -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	tcc -g -m64 -o webinix.o -I "../../../include" -c "../../../src/webinix.c"
	tcc -m64 -ar rcs libwebinix-2-x64.a webinix.o mongoose.o
	tcc -g -m64 -o example_static_debug.exe "../../example.c" -I "../../../include" -L. -lwebinix-2-x64 -lws2_32
	- del webinix.o
	- del mongoose.o

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.exe >nul 2>&1
	- del *.def >nul 2>&1
