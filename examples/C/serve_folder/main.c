
/*
    Webinix Library 2.x

	Serve a Folder Example
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga>.
*/

// Note:
// To see the console logs (printf) you need to build in debug mode
// example 'make debug', or 'nmake debug', or 'mingw32-make debug'

#include "webinix.h"

void switch_to_second_page(webinix_event_t* e) {

	// This function get called every time 
	// the user click on "SwitchToSecondPage" button
    webinix_open(e->window, "second.html", webinix.browser.any);
}

void exit_app(webinix_event_t* e) {

    webinix_exit();
}

int main() {

	// Create a new window
	webinix_window_t* my_window;
	my_window = webinix_new_window();

	// Bind am HTML element ID with a C function
	webinix_bind(my_window, "SwitchToSecondPage", switch_to_second_page);
	webinix_bind(my_window, "Exit", exit_app);

	// The root path. Leave it empty to let the Webinix 
	// automatically select the current working folder
	const char* root_path = "";

	// Create a new web server using Webinix
	const char* url = webinix_new_server(my_window, root_path);

    // Show the window using the generated URL
	if(!webinix_open(my_window, url, webinix.browser.chrome))	// Run the window on Chrome
		webinix_open(my_window, url, webinix.browser.any);		// If not, run on any other installed web browser

    // Wait until all windows get closed
	webinix_wait();

    return 0;
}

#if defined(_MSC_VER)
	int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
		main();
	}
#endif
