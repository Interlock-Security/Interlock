// Text Editor in C using Webinix

#include "webinix.h"

void close_app(webinix_event_t* e) {
	printf("Exit.\n");

	// Close all opened windows
	webinix_exit();
}

int main() {

	// Create a new window
	int MainWindow = webinix_new_window();

	// Set the root folder for the UI
	webinix_set_root_folder(MainWindow, "ui");

	// Bind HTML elements with the specified ID to C functions
	webinix_bind(MainWindow, "close_app", close_app);

	// Show the window, preferably in a chromium based browser
	if (!webinix_show_browser(MainWindow, "index.html", ChromiumBased))
		webinix_show(MainWindow, "index.html");

	// Wait until all windows get closed
	webinix_wait();

	// Free all memory resources (Optional)
	webinix_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
