# Webinix Library
# Windows - Microsoft Visual C

# == 1. VARIABLES =============================================================

WEBUI_OUT_LIB_NAME = webinix-2

# TLS
WEBUI_USE_TLS =
WEBUI_TLS_INCLUDE = .
WEBUI_TLS_LIB = .
TLS_CFLAG = /D NO_SSL
TLS_LDFLAG_DYNAMIC =
!IF "$(WEBUI_USE_TLS)" == "1"
WEBUI_OUT_LIB_NAME = webinix-2-secure
TLS_CFLAG = /D WEBUI_TLS /D NO_SSL_DL /D OPENSSL_API_1_1
TLS_LDFLAG_DYNAMIC = libssl.lib libcrypto.lib
!ENDIF

# Build Flags
CIVETWEB_BUILD_FLAGS = /Fo"civetweb.obj" /c /EHsc "$(MAKEDIR)/src/civetweb/civetweb.c" /I"$(MAKEDIR)/src/civetweb/" /I"$(WEBUI_TLS_INCLUDE)"
CIVETWEB_DEFINE_FLAGS = /D NDEBUG /D NO_CACHING /D NO_CGI /D USE_WEBSOCKET $(TLS_CFLAG)
WEBUI_BUILD_FLAGS = /Fo"webinix.obj" /c /EHsc "$(MAKEDIR)/src/webinix.c" /I"$(MAKEDIR)/include" /I"$(WEBUI_TLS_INCLUDE)" $(TLS_CFLAG)

# Output Commands
LIB_STATIC_OUT = /OUT:"$(WEBUI_OUT_LIB_NAME)-static.lib" "webinix.obj" "civetweb.obj"
LIB_DYN_OUT = /DLL /OUT:"$(WEBUI_OUT_LIB_NAME).dll" "webinix.obj" "civetweb.obj" user32.lib Advapi32.lib Shell32.lib Ole32.lib /LIBPATH:"$(WEBUI_TLS_LIB)" $(TLS_LDFLAG_DYNAMIC)

# == 2.TARGETS ================================================================

all: release

debug:
	@- mkdir dist\debug >nul 2>&1
#	Static with Debug info
	@- cd $(MAKEDIR)/dist/debug
	@echo Build Webinix Library (MSVC Debug Static)...
	@cl /Zl /Zi $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zl /Zi $(WEBUI_BUILD_FLAGS) /D WEBUI_LOG
	@lib $(LIB_STATIC_OUT)
#	Dynamic with Debug info
	@echo Build Webinix Library (MSVC Debug Dynamic)...
	@cl /Zi $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zi $(WEBUI_BUILD_FLAGS) /D WEBUI_LOG
	@link $(LIB_DYN_OUT)
#	Clean
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@echo Done.

release:
	@- mkdir dist >nul 2>&1
	@- cd $(MAKEDIR)/dist
#	Static Release
	@echo Build Webinix Library (MSVC Release Static)...
	@cl /Zl $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zl $(WEBUI_BUILD_FLAGS)
	@lib $(LIB_STATIC_OUT)
#	Dynamic Release
	@echo Build Webinix Library (MSVC Release Dynamic)...
	@cl $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl $(WEBUI_BUILD_FLAGS)
	@link $(LIB_DYN_OUT)
#	Clean
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@- echo Done.

clean:
	@- cd $(BUILD_DIR)
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@- del *.dll >nul 2>&1
	@- del *.lib >nul 2>&1
	@- echo Done.
