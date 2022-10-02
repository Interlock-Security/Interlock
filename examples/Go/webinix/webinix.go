package webinix

import (
	"C"
	"syscall"
	"unsafe"
)

var (
	dll_webinix, _ = syscall.LoadLibrary("webinix-2-x64.dll")

	//webinix_window_t* webinix_new_window();
	dll_webinix_new_window, _ = syscall.GetProcAddress(dll_webinix, "webinix_new_window")

	// bool webinix_show(webinix_window_t* win, const char* html, unsigned int browser);
	dll_webinix_show, _ = syscall.GetProcAddress(dll_webinix, "webinix_show")

	// void webinix_loop();
	dll_webinix_loop, _ = syscall.GetProcAddress(dll_webinix, "webinix_loop")
)

func webinix_new_window() (result uintptr) {
	var arg uintptr = 0
	ret, _, _ := syscall.Syscall(uintptr(dll_webinix_new_window), arg, 0, 0, 0)
	result = ret
	return
}

func webinix_show(win uintptr, html string, browser uint) (result bool) {
	chtml := C.CString(html)
	var arg uintptr = 3
	ret, _, _ := syscall.Syscall(uintptr(dll_webinix_show), arg, win, uintptr(unsafe.Pointer((chtml))), uintptr(browser))
	if ret == 0x00 {
		return false
	}
	return true
}

func webinix_loop() {
	var arg uintptr = 0
	syscall.Syscall(uintptr(dll_webinix_loop), arg, 0, 0, 0)
}

func ini() {

	defer syscall.FreeLibrary(dll_webinix)
}

func webinix_test() (res int) {

	return 0
}
