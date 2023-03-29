// Serve a Folder Example

#include "webinix.h"

webinix_window_t* my_window;
webinix_window_t* my_second_window;

void exit_app(webinix_event_t* e) {

    // Close all opened windows
    webinix_exit();
}

void switch_to_second_page(webinix_event_t* e) {

    // This function gets called every
    // time the user clicks on "SwitchToSecondPage"

    // Switch to `/second.html` in the same opened window.
    webinix_show(e->window, "second.html");
}

void show_second_window(webinix_event_t* e) {

    // This function gets called every
    // time the user clicks on "OpenNewWindow"

    // Show a new window, and navigate to `/second.html`
    // if it's already open, then switch in the same window
    webinix_show(my_second_window, "second.html");
}

int main() {

    // Create new windows
    my_window = webinix_new_window();
    my_second_window = webinix_new_window();

    // Bind HTML element IDs with a C functions
    webinix_bind(my_window, "SwitchToSecondPage", switch_to_second_page);
    webinix_bind(my_window, "OpenNewWindow", show_second_window);
    webinix_bind(my_window, "Exit", exit_app);
    webinix_bind(my_second_window, "Exit", exit_app);

    // Show a new window
    webinix_show(my_window, "index.html");

    // Wait until all windows get closed
    webinix_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
