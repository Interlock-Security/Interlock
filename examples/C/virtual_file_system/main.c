// Virtual File System Example

#include "webinix.h"
#include "vfs.h"

void exit_app(webinix_event_t* e) {
	webinix_exit();
}

int main() {

	// Create new windows
	size_t MyWindow = webinix_new_window();

	// Bind HTML element IDs with a C functions
	webinix_bind(MyWindow, "Exit", exit_app);

	// VSF (Virtual File System) Example
	//
	// 1. Run Python script to generate header file of a folder
	//    python vfs.py "/path/to/folder" "vfs.h"
	//
	// 2. Include header file in your C project
	//    #include "vfs.h"
	//
	// 3. use vfs in your custom files handler `webinix_set_file_handler()`
	//    webinix_set_file_handler(MyWindow, vfs);

	// Set a custom files handler
	webinix_set_file_handler(MyWindow, vfs);

	// Show a new window
	// webinix_show_browser(MyWindow, "index.html", Chrome);
	webinix_show(MyWindow, "index.html");

	// Wait until all windows get closed
	webinix_wait();

	// Free all memory resources (Optional)
	webinix_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
