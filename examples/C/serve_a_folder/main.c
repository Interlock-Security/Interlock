// Serve a Folder Example

#include "webinix.h"

// Those defines is to avoid using global variables
// you can also use `const size_t MyWindow;`.
// A window ID can be between 1 and 512 (WEBUI_MAX_IDS)
#define MyWindow       (1)
#define MySecondWindow (2)

void exit_app(webinix_event_t* e) {

    // Close all opened windows
    webinix_exit();
}

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
    webinix_show(MySecondWindow, "second.html");
}

const void* my_files_handler(const char* filename, int* length) {

    printf("File: %s \n", filename);

    if (!strcmp(filename, "/test.txt")) {

        // Const static file example
        return  "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 99\r\n\r\n"
                "<html>"
                "   This is a static embedded file content example."
                "   <script src=\"webinix.js\"></script>" // To keep connection with Webinix
                "</html>";
    }
    else if (!strcmp(filename, "/dynamic.html")) {

        // Dynamic file example

        // Allocate memory
        char* body = webinix_malloc(1024);
        char* header_and_body = webinix_malloc(1024);

        // Generate body
        static int count = 0;
        sprintf(
            body,
            "<html>"
            "   This is a dynamic file content example. <br>"
            "   Count: %d <a href=\"dynamic.html\">[Refresh]</a><br>"
            "   <script src=\"webinix.js\"></script>" // To keep connection with Webinix
            "</html>",
            ++count
        );

        // Generate header + body
        int body_size = strlen(body);
        sprintf(
            header_and_body,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %d\r\n\r\n"
            "%s",
            body_size, body
        );
        
        // Free body buffer
        webinix_free(body);

        // Set len (optional)
        *length = strlen(header_and_body);

        // By allocating resources using webinix_malloc()
        // Webinix will automaticaly free the resources.
        return header_and_body;
    }

    // Other files:
    // A NULL return will make Webinix
    // looks for the file locally.
    return NULL;
}

int main() {

    // Create new windows
    webinix_new_window_id(MyWindow);
    webinix_new_window_id(MySecondWindow);

    // Bind HTML element IDs with a C functions
    webinix_bind(MyWindow, "SwitchToSecondPage", switch_to_second_page);
    webinix_bind(MyWindow, "OpenNewWindow", show_second_window);
    webinix_bind(MyWindow, "Exit", exit_app);
    webinix_bind(MySecondWindow, "Exit", exit_app);

    // Bind events
    webinix_bind(MyWindow, "", events);

    // Set the `.ts` and `.js` runtime
    // webinix_set_runtime(MyWindow, NodeJS);
    // webinix_set_runtime(MyWindow, Bun);
    webinix_set_runtime(MyWindow, Deno);

    // Set a custom files handler
    webinix_set_file_handler(MyWindow, my_files_handler);

    // Set window size
    webinix_set_size(MyWindow, 800, 800);

    // Set window position
    webinix_set_position(MyWindow, 200, 200);

    // Show a new window
    // webinix_set_root_folder(MyWindow, "_MY_PATH_HERE_");
    // webinix_show_browser(MyWindow, "index.html", Chrome);
    webinix_show(MyWindow, "index.html");

    // Wait until all windows get closed
    webinix_wait();

    // Free all memory resources (Optional)
    webinix_clean();

    return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
