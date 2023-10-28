// Serve a Folder Example

#include "webinix.h"

void events(webinix_event_t* e) {

	// This function gets called every time
	// there is an event

	if (e->event_type == WEBUI_EVENT_CONNECTED)
		printf("Connected. \n");
	else if (e->event_type == WEBUI_EVENT_DISCONNECTED)
		printf("Disconnected. \n");
	else if (e->event_type == WEBUI_EVENT_MOUSE_CLICK)
		printf("Click. \n");
	else if (e->event_type == WEBUI_EVENT_NAVIGATION) {
		const char* url = webinix_get_string(e);
		printf("Starting navigation to: %s \n", url);

		// Because we used `webinix_bind(MyWindow, "", events);`
		// Webinix will block all `href` link clicks and sent here instead.
		// We can then control the behaviour of links as needed.
		webinix_navigate(e->window, url);
	}
}

void my_backend_func(webinix_event_t* e) {

	// JavaScript:
	// my_backend_func(123, 456, 789);
	// or webinix.my_backend_func(...);

	long long number_1 = webinix_get_int_at(e, 0);
	long long number_2 = webinix_get_int_at(e, 1);
	long long number_3 = webinix_get_int_at(e, 2);

	printf("my_backend_func 1: %lld\n", number_1); // 123
	printf("my_backend_func 2: %lld\n", number_2); // 456
	printf("my_backend_func 3: %lld\n", number_3); // 789
}

int main() {

	// Create new windows
	size_t window = webinix_new_window();

	// Bind all events
	webinix_bind(window, "", events);

	// Bind HTML elements with C functions
	webinix_bind(window, "my_backend_func", my_backend_func);

	// Set web server network port Webinix should use
	// this mean `webinix.js` will be available at:
	// http://localhost:8081/webinix.js
	webinix_set_port(window, 8081);

	// Show a new window and show our custom web server
	// Assuming the custom web server is running on port
	// 8080...
	webinix_show(window, "http://localhost:8080/");

	// Wait until all windows get closed
	webinix_wait();

	// Free all memory resources (Optional)
	webinix_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
