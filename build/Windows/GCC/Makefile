# Webinix Library
# Windows - GNU Compiler Collection

SHELL=CMD
SOURCE=../../../src
INCLUDE=../../../include
INCLUDE_CIVETWEB=../../../src/civetweb

all: release

debug:
#	Static with Debug info
	@echo Build Webinix Library (GCC Debug Static)...
	@gcc -g -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -g -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@ar rc libwebinix-2-static-x64.a webinix.o civetweb.o
	@ranlib libwebinix-2-static-x64.a
#	Dynamic with Debug info
	@echo Build Webinix Library (GCC Debug Dynamic)...
	@gcc -g -fPIC -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -g -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@gcc -g -shared -o webinix-2-x64.dll webinix.o civetweb.o -lws2_32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build Webinix Library (GCC Release Static)...
	@gcc -Os -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -Os -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@ar rc libwebinix-2-static-x64.a webinix.o civetweb.o
	@ranlib libwebinix-2-static-x64.a
#	Dynamic Release
	@echo Build Webinix Library (GCC Release Dynamic)...
	@gcc -Os -fPIC -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -O3 -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@gcc -shared -o webinix-2-x64.dll webinix.o civetweb.o -lws2_32
	@strip --strip-unneeded webinix-2-x64.dll
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
