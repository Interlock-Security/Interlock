# Webinix Library
# Windows - Tiny C Compiler

SHELL=CMD
SOURCE=../../../src
INCLUDE=../../../include
INCLUDE_CIVETWEB=../../../src/civetweb

all: release

debug:
#	Static with Debug info
	@echo Build Webinix Library (TCC Debug Static)...
	@tcc -g -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -w -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@tcc -g -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG -w
	@tcc -m64 -ar rcs libwebinix-2-static-x64.a webinix.o civetweb.o
#	Dynamic with Debug info
	@echo Build Webinix Library (TCC Debug Dynamic)...
	@tcc -g -fPIC -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -w -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@tcc -g -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG -w
	@tcc -g -shared -o webinix-2-x64.dll webinix.o civetweb.o -lws2_32 -lAdvapi32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build Webinix Library (TCC Release Static)...
	@tcc -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -w -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@tcc -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -w
	@tcc -m64 -ar rcs libwebinix-2-static-x64.a webinix.o civetweb.o
#	Dynamic Release
	@echo Build Webinix Library (TCC Release Dynamic)...
	@tcc -fPIC -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -w -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@tcc -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -w
	@tcc -shared -o webinix-2-x64.dll webinix.o civetweb.o -lws2_32 -lAdvapi32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.def >nul 2>&1
