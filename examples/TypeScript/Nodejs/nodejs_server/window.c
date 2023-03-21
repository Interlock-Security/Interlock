
/*
    Webinix Library 2.0.7
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga> - Canada.
*/

#include "webinix.h"

int main(int argc, char* argv[]) {

	// argc should be exactly 2
	if(argc != 2)
		return 1;
	
	// Get the Nodejs server url
	const char *url = argv[1];

	// Create a new window
	webinix_window_t* my_window;
	my_window = webinix_new_window();

    // Show the window
	if(!webinix_open(my_window, url, webinix.browser.chrome))	// Run the window on Chrome
		webinix_open(my_window, url, webinix.browser.any);		// If not, run on any other installed web browser

    // Wait until all windows get closed
	webinix_wait();

    return 0;
}
