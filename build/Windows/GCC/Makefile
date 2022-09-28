# Webinix Library
# Windows - GNU Compiler Collection

SOURCE=../../../src
INCLUDE=../../../include
EXAMPLES=../../../examples

all: release

debug:
	@echo Build Webinix (Debug Static)...
	@gcc -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@gcc -g -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@ar rc libwebinix-2-static-x64.a webinix.o mongoose.o
	@ranlib libwebinix-2-static-x64.a
	@echo Build Webinix (Debug Dynamic)...
	@gcc -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@gcc -g -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@gcc -g -shared -o webinix-2-x64.dll webinix.o mongoose.o -lws2_32
	@echo Done.

release:
	@echo Build Webinix (Release Static)...
	@gcc -Os -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@gcc -Os -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@ar rc libwebinix-2-static-x64.a webinix.o mongoose.o
	@ranlib libwebinix-2-static-x64.a
	@echo Build Webinix (Release Dynamic)...
	@gcc -O3 -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@gcc -O3 -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@gcc -shared -o webinix-2-x64.dll webinix.o mongoose.o -lws2_32
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
