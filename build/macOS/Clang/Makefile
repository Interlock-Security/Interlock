# Webinix Library
# macOS - Clang

SOURCE=../../../src
INCLUDE=../../../include
INCLUDE_CIVETWEB=../../../src/civetweb

all: release

debug:
    # Static with Debug info
	@echo "Build Webinix Library (Clang Debug Static)..."
	@clang -g -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@clang -DWEBUI_LOG -g -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@ar rc libwebinix-2-static-x64.a webinix.o civetweb.o
	@ranlib libwebinix-2-static-x64.a
    # Dynamic with Debug info
	@echo "Build Webinix Library (Clang Debug Dynamic)..."
	@clang -g -fPIC -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@clang -DWEBUI_LOG -g -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@clang -g -shared -o webinix-2-x64.dyn webinix.o civetweb.o
	#Clean
	@- rm -f *.o
	@echo "Done."

release:
    # Static Release
	@echo "Build Webinix Library (Clang Release Static)..."
	@clang -Os -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@clang -Os -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@ar rc libwebinix-2-static-x64.a webinix.o civetweb.o
	@ranlib libwebinix-2-static-x64.a
    # Dynamic Release
	@echo "Build Webinix Library (Clang Release Dynamic)..."
	@clang -O3 -fPIC -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@clang -O3 -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@clang -shared -o webinix-2-x64.dyn webinix.o civetweb.o
    # Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.dylib
	- rm -f *.a
