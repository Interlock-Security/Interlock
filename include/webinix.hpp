/*
  Webinix Library 2.2.0
  http://webinix.me
  https://github.com/alifcommunity/webinix
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under GNU General Public License v2.0.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_HPP
#define _WEBUI_HPP

// C++ STD
#include <string>

// Webinix C Header
extern "C" {
	#include "webinix.h"
}

namespace webinix {

	// Create a new webinix window object.
	void* new_window(void) {
		return webinix_new_window();
	}

	// Bind a specific html element click event with a function. Empty element means all events.
	unsigned int bind(void* window, std::string element, void (*func)(webinix_event_t* e)) {
		return webinix_bind(window, element.c_str(), func);
	}

	// Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
	bool show(void* window, std::string content) {
		return webinix_show(window, content.c_str());
	}

	// Same as show(). But with a specific web browser.
	bool show_browser(void* window, std::string content, unsigned int browser) {
		return webinix_show_browser(window, content.c_str(), browser);
	}

	// Wait until all opened windows get closed.
	void wait(void) {
		webinix_wait();
	}

	// Close a specific window.
	void close(void* window) {
		webinix_close(window);
	}

	// Set the window in Kiosk mode (Full screen)
	void set_kiosk(void* window, bool status) {
		webinix_set_kiosk(window, status);
	}

	// Close all opened windows. wait() will break.
	void exit(void) {
		webinix_exit();
	}

	// -- Other ---------------------------
	// Check a specific window if it's still running
	bool is_shown(void* window) {
		return webinix_is_shown(window);
	}

	// Set the maximum time in seconds to wait for browser to start
	void set_timeout(unsigned int second) {
		webinix_set_timeout(second);
	}

	// Set the default embedded HTML favicon
	void set_icon(void* window, std::string icon, std::string icon_type) {
		webinix_set_icon(window, icon.c_str(), icon_type.c_str());
	}

	// Allow the window URL to be re-used in normal web browsers
	void set_multi_access(void* window, bool status) {
		webinix_set_multi_access(window, status);
	}

	// -- JavaScript ----------------------
	// Quickly run a JavaScript (no response waiting).
	bool run(void* window, std::string script) {
		return webinix_run(window, script.c_str());
	}

	// Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
	bool script(void* window, std::string script, unsigned int timeout, char* buffer, size_t buffer_length) {
		return webinix_script(window, script.c_str(), timeout, buffer, buffer_length);
	}

	// Chose between Deno and Nodejs runtime for .js and .ts files.
	void set_runtime(void* window, unsigned int runtime) {
		webinix_set_runtime(window, runtime);
	}

	// Parse argument as integer.
	long long int get_int(webinix_event_t* e) {
		return webinix_get_int(e);
	}

	// Parse argument as string.
	std::string get_string(webinix_event_t* e) {
		return std::string(webinix_get_string(e));
	}

	// Parse argument as boolean.
	bool get_bool(webinix_event_t* e) {
		return webinix_get_bool(e);
	}

	// Return the response to JavaScript as integer.
	void return_int(webinix_event_t* e, long long int n) {
		webinix_return_int(e, n);
	}

	// Return the response to JavaScript as string.
	void return_string(webinix_event_t* e, std::string s) {
		webinix_return_string(e, &s[0]);
	}

	// Return the response to JavaScript as boolean.
	void return_bool(webinix_event_t* e, bool b) {
		webinix_return_bool(e, b);
	}

	// -- Interface -----------------------
	// Bind a specific html element click event with a function. Empty element means all events. This replace bind(). The func is (Window, EventType, Element, Data, Response)
	unsigned int interface_bind(void* window, std::string element, void (*func)(void*, unsigned int, char*, char*, unsigned int)) {
		return webinix_interface_bind(window, element.c_str(), func);
	}

	// When using `interface_bind()` you need this function to easily set your callback response.
	void interface_set_response(void *window, webinix_event_t* e, std::string response) {
		webinix_interface_set_response(window, e->event_number, response.c_str());
	}

	// Check if the app still running or not. This replace wait().
	bool interface_is_app_running(void) {
		return webinix_interface_is_app_running();
	}

	// Get window unique ID
	unsigned int interface_get_window_id(void* window) {
		return webinix_interface_get_window_id(window);
	}
}

#endif /* _WEBUI_HPP */
