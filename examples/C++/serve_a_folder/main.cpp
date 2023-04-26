// Include the Webinix header
#include "webinix.hpp"

// Include C++ STD
#include <iostream>

// Making this object global so show_second_window() can access it.
void* my_second_window;

// Example of a simple Class
class MyClass {
    public:

    // This method gets called every time the
    // user clicks on "OpenNewWindow"
    void show_second_window(webinix_event_t* e) {

        // Show a new window, and navigate to `/second.html`
        // if the window is already opened, then switch in the same window
        webinix::show(my_second_window, "second.html");
    }

    // This method gets called every time the
    // user clicks on "SwitchToSecondPage"
    void switch_to_second_page(webinix_event_t* e) {

        // Switch to `/second.html` in the same opened window.
        webinix::show(e->window, "second.html");
    }

    // Example of a simple function (Not a method)
    // This function receives all events because
    // it's get bind with an empty HTML ID.
    void events(webinix_event_t* e) {

        if (e->type == WEBUI_EVENT_CONNECTED)
            std::cout << "Window Connected." << std::endl;
        else if (e->type == WEBUI_EVENT_DISCONNECTED)
            std::cout << "Window Disconnected." << std::endl;
        else if (e->type == WEBUI_EVENT_MOUSE_CLICK)
            std::cout << "Click on element: " << e->element << std::endl;
        else if (e->type == WEBUI_EVENT_NAVIGATION)
            std::cout << "Starting navigation to: " << e->data << std::endl;
    }

    // Example of a simple function (Not a method)
    void exit_app(webinix_event_t* e) {

        // Close all opened windows
        webinix::exit();
    }    
};

// Wrapper:
// Because Webinix is written in C, so it can not
// access `MyClass` directly. That's why we should
// create a simple C++ wrapper.
MyClass obj;
void show_second_window_wrp(webinix_event_t* e) { obj.show_second_window(e); }
void switch_to_second_page_wrp(webinix_event_t* e) { obj.switch_to_second_page(e); }
void events_wrp(webinix_event_t* e) { obj.events(e); }
void exit_app_wrp(webinix_event_t* e) { obj.exit_app(e); }

int main() {

    // Print logs (debug build only)
    std::cout << "Starting..." << std::endl;

    // Create a new window
    void* my_window = webinix::new_window();
    my_second_window = webinix::new_window();

    // Bind HTML element IDs with a C functions
    webinix::bind(my_window, "SwitchToSecondPage", switch_to_second_page_wrp);
    webinix::bind(my_window, "OpenNewWindow", show_second_window_wrp);
    webinix::bind(my_window, "Exit", exit_app_wrp);
    webinix::bind(my_second_window, "Exit", exit_app_wrp);

    // Bind events
    webinix::bind(my_window, "", events_wrp);

    // Show a new window
    webinix::show(my_window, "index.html"); // webinix::show_browser(my_window, "index.html", Chrome);

    // Wait until all windows get closed
    webinix::wait();

    // Print logs (debug build only)
    std::cout << "Thank you." << std::endl;

    return 0;
}

// Release build
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
) {
	return main();
}
