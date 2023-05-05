# Webinix Library
# Linux - GCC

SOURCE=../../../src
INCLUDE=../../../include
INCLUDE_CIVETWEB=../../../src/civetweb

all: release

debug:
#	Static with Debug info
	@echo "Build Webinix Library (GCC Debug Static)..."
	@gcc -g -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -g -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@ar rc libwebinix-2-static-x64.a webinix.o civetweb.o
	@ranlib libwebinix-2-static-x64.a
#	Dynamic with Debug info
	@echo "Build Webinix Library (GCC Debug Dynamic)..."
	@gcc -g -fPIC -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -g -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@gcc -g -shared -o webinix-2-x64.so webinix.o civetweb.o
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Static Release
	@echo "Build Webinix Library (GCC Release Static)..."
	@gcc -Os -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -Os -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@ar rc libwebinix-2-static-x64.a webinix.o civetweb.o
	@ranlib libwebinix-2-static-x64.a
#	Dynamic Release
	@echo "Build Webinix Library (GCC Release Dynamic)..."
	@gcc -O3 -fPIC -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -O3 -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@gcc -shared -o webinix-2-x64.so webinix.o civetweb.o
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
