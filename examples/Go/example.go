package main

//--[Webinix]-------------------------------------
import (
	"C"
	"fmt"
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

//----------------------------------------------

// Application Example

const my_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>Webinix 2.0 Example</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>Webinix 2.0 Example</h1>
    <br>
    <input type="password" id="MyInput">
    <br>
    <br>
    <button id="MyButton1">Check Password</button> - <button id="MyButton2">Exit</button>
  </body>
</html>`

func main() {

	var my_window = webinix_new_window()

	webinix_show(my_window, my_html, 0)

	webinix_loop()

	fmt.Print("Bye.")
}
