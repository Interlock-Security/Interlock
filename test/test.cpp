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

const std::string html = R"V0G0N(
<!DOCTYPE html>
<html>
	<head>
		<title>Webinix TEST</title>
	</head>
	<body style="background-color:#515C6B; color:#fff; font-family:"Lucida Console", Courier, monospace">
		<h1>Please close this window.</h1>
		<p>This is Webinix test application.</p>
		<input type="hidden" id="testinput" value="webinix-test-communication">
	</body>
</html>
)V0G0N";

webinix::window win;

void bar(webinix::event e){

   printf("Webinix Test");
}

int main(){
   
   win.bind("foo", bar);

   if(!win.show(&html, webinix::browser::chrome))
      win.show(&html);

   std::thread t(webinix::loop);
   t.join();

   return 0;
}

// Win32 entry point (if needed)
#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
   main();
   return 0;
}
#endif
