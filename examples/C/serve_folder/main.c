
/*
    Webinix Library 2.x

	Serve a Folder Example
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

#include "webinix.h"

void all_clicks(webinix_event_t* e) {
    printf("You clicked on '%s' element.\n", e->element_name);
}

int main() {

	// Crate new window
	webinix_window_t* my_window;
	my_window = webinix_new_window();

	// Bind all clicks
	webinix_bind_all(my_window, all_clicks);

	// Create a new web server using Webinix
	const char* url = webinix_new_server(my_window, "");

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
