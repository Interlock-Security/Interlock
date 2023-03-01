
/*
    Webinix Library 2.x
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga>.
*/

#include "webinix.h"

int main() {

	// Create a new window
	webinix_window_t* my_window;
	my_window = webinix_new_window();

	// Chose your preferable runtime for .js files
	// Deno: webinix.runtime.deno
	// Node.js: webinix.runtime.nodejs
	webinix_script_runtime(my_window, webinix.runtime.nodejs);

	// Create a new web server using Webinix
	const char* url = webinix_new_server(my_window, "");

    // Show the window
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
