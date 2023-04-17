// Serve a Folder Example

#include "webinix.h"

void* my_window;
void* my_second_window;

void exit_app(webinix_event_t* e) {

    // Close all opened windows
    webinix_exit();
}

void events(webinix_event_t* e) {

    // This function gets called every time
    // there is an event

    if(e->type == WEBUI_EVENT_CONNECTED)
        printf("Connected. \n");
    else if(e->type == WEBUI_EVENT_DISCONNECTED)
        printf("Disconnected. \n");
    else if(e->type == WEBUI_EVENT_MOUSE_CLICK)
        printf("Click. \n");
    else if(e->type == WEBUI_EVENT_NAVIGATION)
        printf("Starting navigation to: %s \n", (char *)e->data);
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

    // Bind events
    webinix_bind(my_window, "", events);

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
