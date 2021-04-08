#ifndef UNICODE
#define UNICODE
#endif

// Webinix header
#include <webinix/webinix.hpp>

// C++ headers
#include <iostream>

// Win32 headers
#ifdef _WIN32
	#include <windows.h>
#endif

webinix::window my_window;

void welcome(webinix::event e){

	std::cout << "Welcome! You clicked on the watch!" << std::endl;
}

int main(){

	// This example show how to
	// make the Webinix server files
	// in a specific folder.

	// Path to server
	// Note: if it's empty the Webinix 
	// automaticaly server the current folder.
	my_window.serve_folder("");

	// Set Webinix to wait forever!
	// 0: infinit loop
	// n: wait for n seconds
	webinix::set_timeout_sec(0);

	// Set Webinix to allow multi
	// window serving, this is
	// because each file is a new
	// request
	my_window.allow_multi_serving(true);

	// Link the HTML button
	my_window.bind("MyDiv", welcome);

	// Create a new server (HTTP & Websocket)
	// and get URL
	std::string url = my_window.new_server();

	std::cout << "Starting Webinix server.." << std::endl;
	std::cout << "Please goto to this address: " << url << std::endl;
	std::cout << std::endl << "Wait forever!.." << std::endl;
	std::cout << "To stop just press Ctrl + C" << std::endl;

	// Loop
	std::thread ui(webinix::loop);
	ui.join();
	
	return 0;
}

// Win32 entry point (if needed)
#ifdef _WIN32
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
		main();
		return 0;
	}
#endif
