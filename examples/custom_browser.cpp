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

// HTML Code
const std::string my_html = R"V0G0N(
<!DOCTYPE html>
<html>
	<head>
		<title>Custom Browser - Webinix</title>
		<style>
			body{

				background: #ad5389;  /* fallback for old browsers */
				background: -webkit-linear-gradient(to right, #3c1053, #ad5389);  /* Chrome 10-25, Safari 5.1-6 */
				background: linear-gradient(to right, #3c1053, #ad5389); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */
				
				color:#fff; 
				font-family: Avant Garde,Avantgarde,Century Gothic,CenturyGothic,AppleGothic,sans-serif; 
				font-size: 18px;
				text-align: center;
			}
		</style>
	</head>
	<body>

		<h1>Welcome to Webinix!</h1>
		<h2>Custom Browser Example</h2>
		<br>
		<p>This example show how to use custom settings to run your favorite web browser.</p>
		<button id="MyButtonID1">Exit</button>

	</body>
</html>
)V0G0N";

webinix::window my_window;

void close(webinix::event e){

	std::cout << "Bye!" << std::endl;
	my_window.close();
}

int main(){

	// This example show how to set
	// Webinix to use a custom browser

	webinix::custom_browser_t MyBrowser {

		// Firefox on Linux
		// Command: 'firefox -private -url http://127.0.0.1:1234'

		.app = "firefox",
		.arg = "-private -url ",
		.link = true
	};

	my_window.bind("MyButtonID1", close);

	// Show window
	if(!my_window.show(&my_html, &MyBrowser)){

		// Failed to start our custom web browser
		std::cout << "Webinix has failed to start the web browser" << std::endl;
	}

	// Loop
	std::thread ui(webinix::loop);
	ui.join();

	// All windows are closed.
	printf("Good! All Webinix windows are closed now.\n");
	
	return 0;
}

// Win32 entry point (if needed)
#ifdef _WIN32
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
		main();
		return 0;
	}
#endif
