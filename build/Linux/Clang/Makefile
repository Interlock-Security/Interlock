# Webinix Library
# Linux - Clang

SOURCE=../../../src
INCLUDE=../../../include

all: release

debug:
#	Static with Debug info
	@echo "Build Webinix Library (Clang Debug Static)..."
	@clang -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -g -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@llvm-ar rc libwebinix-2-static-x64.a webinix.o mongoose.o
	@llvm-ranlib libwebinix-2-static-x64.a
#	Dynamic with Debug info
	@echo "Build Webinix Library (Clang Debug Dynamic)..."
	@clang -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -g -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c" -DWEBUI_LOG
	@clang -g -shared -o webinix-2-x64.so webinix.o mongoose.o
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Static Release
	@echo "Build Webinix Library (Clang Release Static)..."
	@clang -Os -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -Os -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@llvm-ar rc libwebinix-2-static-x64.a webinix.o mongoose.o
	@llvm-ranlib libwebinix-2-static-x64.a
#	Dynamic Release
	@echo "Build Webinix Library (Clang Release Dynamic)..."
	@clang -O3 -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -O3 -fPIC -m64 -o webinix.o -I "$(INCLUDE)" -c "$(SOURCE)/webinix.c"
	@clang -shared -o webinix-2-x64.so webinix.o mongoose.o
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
