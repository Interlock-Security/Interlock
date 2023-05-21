/*
  Webinix Library 2.3.0
  http://webinix.me
  https://github.com/webinix-dev/webinix
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
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

    const int DISCONNECTED = 0; // 0. Window disconnection event
    const int CONNECTED = 1; // 1. Window connection event
    const int MULTI_CONNECTION = 2; // 2. New window connection event
    const int UNWANTED_CONNECTION = 3; // 3. New unwanted window connection event
    const int MOUSE_CLICK = 4; // 4. Mouse click event
    const int NAVIGATION = 5; // 5. Window navigation event
    const int CALLBACKS = 6; // 6. Function call event

    class window;

    // Event struct
    struct event {
        webinix::window& window; // The window object
        unsigned int event_type; // Event type
        std::string element; // HTML element ID
        std::string data; // JavaScript data
        unsigned int event_number; // Internal Webinix

        // Window object constructor that
        // initializes the reference, This
        // is to avoid creating copies.
        event(webinix::window& window_obj) : window(window_obj) {}
    };

    // List of callbacks: user_function(webinix::event* e)
    void (*callback_list[512])(webinix::event*);

    // List of window objects: webinix::window
    webinix::window* window_list[512];

    // Event handler
    // Webinix is written in C. So there is no way
    // to make C call a C++ class member. That's
    // why this function should be outside class
    void event_handler(webinix_event_t* c_e) {

        // Get a unique ID. Same ID as `webinix_bind()`. Return > 0 if bind exist.
        unsigned int id = webinix_interface_get_bind_id(c_e->window, c_e->element);

        if(id < 1)
            return;

        // Create a new event struct
        webinix::event e(*window_list[id]);
        // `e.window` is already initialized by `e` constructor
        e.event_type = c_e->event_type;
        e.element = (c_e->element != NULL ? std::string(c_e->element) : std::string(""));
        e.data = (c_e->data != NULL ? std::string(c_e->data) : std::string(""));
        e.event_number = c_e->event_number;

        // Call the user callback
        if(callback_list[id] != NULL)
            callback_list[id](&e);
    }

    class window {
    private:
        size_t webinix_window = 0;

        webinix_event_t* convert_event_to_webinix_event(webinix::event* e) {
            // Convert C++ `webinix::event` to C `webinix_event_t`
            webinix_event_t* c_e = new webinix_event_t;
            c_e->window = this->webinix_window;
            c_e->event_type = e->event_type;
            c_e->element = (char*)e->element.c_str();
            c_e->data = (char*)e->data.c_str();
            c_e->event_number = e->event_number;
            return c_e;
        }
    public:
        // Constructor
        window() {
            this->webinix_window = webinix_new_window();
        }

        // Destructor
        ~window() {
            // Nothing to do.
            // No needs to call `webinix_close()`
        }

        // Bind a specific html element click event with a function. Empty element means all events.
        void bind(const std::string& element, void (*func)(webinix::event* e)) {

            // Get unique ID
            unsigned int id = webinix_bind(this->webinix_window, element.c_str(), webinix::event_handler);

            // Save window object
            window_list[id] = this;

            // Save callback
            callback_list[id] = func;
        }

        // Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
        bool show(const std::string& content) {
            return webinix_show(this->webinix_window, content.c_str());
        }

        // Same as show(). But with a specific web browser.
        bool show_browser(const std::string& content, unsigned int browser) {
            return webinix_show_browser(this->webinix_window, content.c_str(), browser);
        }

        // Close a specific window.
        void close() {
            webinix_close(this->webinix_window);
        }

        // Set the window in Kiosk mode (Full screen)
        void set_kiosk(bool status) {
            webinix_set_kiosk(this->webinix_window, status);
        }

        // -- Other ---------------------------
        // Check a specific window if it's still running
        bool is_shown() {
            return webinix_is_shown(this->webinix_window);
        }

        // Set the default embedded HTML favicon
        void set_icon(const std::string& icon, const std::string& icon_type) {
            webinix_set_icon(this->webinix_window, icon.c_str(), icon_type.c_str());
        }

        // Allow the window URL to be re-used in normal web browsers
        void set_multi_access(bool status) {
            webinix_set_multi_access(this->webinix_window, status);
        }

        // -- JavaScript ----------------------
        // Quickly run a JavaScript (no response waiting).
        void run(const std::string& script) {
            webinix_run(this->webinix_window, script.c_str());
        }

        // Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
        bool script(const std::string& script, unsigned int timeout, char* buffer, size_t buffer_length) {
            return webinix_script(this->webinix_window, script.c_str(), timeout, buffer, buffer_length);
        }

        // Chose between Deno and Nodejs runtime for .js and .ts files.
        void set_runtime(unsigned int runtime) {
            webinix_set_runtime(this->webinix_window, runtime);
        }

        // Parse argument as integer.
        long long int get_int(webinix::event* e) {
            webinix_event_t* c_e = convert_event_to_webinix_event(e);
            long long int ret = webinix_get_int(c_e);
            delete c_e;
            return ret;
        }

        // Parse argument as string.
        std::string get_string(webinix::event* e) {
            webinix_event_t* c_e = convert_event_to_webinix_event(e);
            std::string ret = std::string(webinix_get_string(c_e));
            delete c_e;
            return ret;
        }

        // Parse argument as boolean.
        bool get_bool(webinix::event* e) {
            webinix_event_t* c_e = convert_event_to_webinix_event(e);
            bool ret = webinix_get_bool(c_e);
            delete c_e;
            return ret;
        }

        // Return the response to JavaScript as integer.
        void return_int(webinix::event* e, long long int n) {
            webinix_event_t* c_e = convert_event_to_webinix_event(e);
            webinix_return_int(c_e, n);
            delete c_e;
        }

        // Return the response to JavaScript as string.
        void return_string(webinix::event* e, const std::string& s) {
            webinix_event_t* c_e = convert_event_to_webinix_event(e);
            webinix_return_string(c_e, s.c_str());
            delete c_e;
        }

        // Return the response to JavaScript as boolean.
        void return_bool(webinix::event* e, bool b) {
            webinix_event_t* c_e = convert_event_to_webinix_event(e);
            webinix_return_bool(c_e, b);
            delete c_e;
        }
    };

    // Wait until all opened windows get closed.
    void wait(void) {
        webinix_wait();
    }

    // Close all opened windows. wait() will break.
    void exit(void) {
        webinix_exit();
    }

    // Set the maximum time in seconds to wait for browser to start
    void set_timeout(unsigned int second) {
        webinix_set_timeout(second);
    }

    // Base64 encoding. Use this to safely send text based data to the UI. If it fails it will return NULL.
    std::string encode(const std::string& str) {
        std::string ret = std::string(webinix_encode(str.c_str()));
        return ret;
    }

    // Base64 decoding. Use this to safely decode received Base64 text from the UI. If it fails it will return NULL.
    std::string decode(const std::string& str) {
        std::string ret = std::string(webinix_decode(str.c_str()));
        return ret;
    }

    // Safely free a buffer allocated by Webinix, for example when using webinix_encode().
    void free(void* ptr) {
        webinix_free(ptr);
    }
}

#endif /* _WEBUI_HPP */
