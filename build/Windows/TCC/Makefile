# Webinix Library
# Windows - Tiny C Compiler

SHELL=CMD
SOURCE=../../../src
INCLUDE=../../../include

all: release

debug:
#	Static with Debug info
	@echo Build Webinix Library (Debug Static)...
	@tcc -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -g -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@tcc -m64 -ar rcs libwebinix-2-static-x64.a webinix.o mongoose.o
#	Dynamic with Debug info
	@echo Build Webinix Library (Debug Dynamic)...
	@tcc -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -g -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@tcc -g -shared -o webinix-2-x64.dll webinix.o mongoose.o -lws2_32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build Webinix Library (Release Static)...
	@tcc -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@tcc -m64 -ar rcs libwebinix-2-static-x64.a webinix.o mongoose.o
#	Dynamic Release
	@echo Build Webinix Library (Release Dynamic)...
	@tcc -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@tcc -shared -o webinix-2-x64.dll webinix.o mongoose.o -lws2_32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.def >nul 2>&1
