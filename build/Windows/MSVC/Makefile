# Webinix Library
# Windows - Microsoft Visual C

all: release

debug:
#	Static with Debug info
	@echo Build Webinix Library (Debug Static)...
	@cl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" >nul 2>&1
	@cl /Zi /Fowebinix.obj /c /EHsc /DWEBUI_LOG "../../../src/webinix.c" /I "../../../include" >nul 2>&1
	@lib /OUT:webinix-2-static-x64.lib webinix.obj mongoose.obj >nul 2>&1
#	Dynamic with Debug info
	@echo Build Webinix Library (Debug Dynamic)...
	@cl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" >nul 2>&1
	@cl /Zi /Fowebinix.obj /c /EHsc /DWEBUI_LOG "../../../src/webinix.c" /I "../../../include" >nul 2>&1
	@link /DLL /OUT:webinix-2-x64.dll webinix.obj mongoose.obj user32.lib Advapi32.lib >nul 2>&1
#	Clean
	@- del *.obj >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build Webinix Library (Release Static)...
	@cl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" >nul 2>&1
	@cl /Fowebinix.obj /c /EHsc "../../../src/webinix.c" /I "../../../include" >nul 2>&1
	@lib /OUT:webinix-2-static-x64.lib webinix.obj mongoose.obj >nul 2>&1
#	Dynamic Release
	@echo Build Webinix Library (Release Dynamic)...
	@cl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" >nul 2>&1
	@cl /Fowebinix.obj /c /EHsc "../../../src/webinix.c" /I "../../../include" >nul 2>&1
	@link /DLL /OUT:webinix-2-x64.dll webinix.obj mongoose.obj user32.lib Advapi32.lib >nul 2>&1
#	Clean
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@echo Done.

clean:
	- del *.obj >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.ilk >nul 2>&1
	- del *.pdb >nul 2>&1
	- del *.lib >nul 2>&1
	- del *.exp >nul 2>&1
