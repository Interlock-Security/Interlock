// React Example

#include "webinix.h"
#include "vfs.h"

void exit_app(webinix_event_t* e) {
    webinix_exit();
}

int main() {

    // Create new windows
    size_t react_window = webinix_new_window();

    // Set window size
    webinix_set_size(react_window, 550, 450);

    // Set window position
    webinix_set_position(react_window, 250, 250);

    // Allow multi-user connection to Webinix window
    webinix_set_config(multi_client, true);

    // Disable Webinix's cookies
    webinix_set_config(use_cookies, false);

    // Bind React HTML element IDs with a C functions
    webinix_bind(react_window, "Exit", exit_app);

    // VSF (Virtual File System) Example
    //
    // 1. Run Python script to generate header file of a folder
    //    python vfs.py "/path/to/folder" "vfs.h"
    //
    // 2. Include header file in your C project
    //    #include "vfs.h"
    //
    // 3. use vfs in your custom files handler `webinix_set_file_handler()`
    //    webinix_set_file_handler(react_window, vfs);

    // Set a custom files handler
    webinix_set_file_handler(react_window, vfs);

    // Show the React window
    // webinix_show_browser(react_window, "index.html", Chrome);
    webinix_show(react_window, "index.html");

    // Wait until all windows get closed
    webinix_wait();

    // Free all memory resources (Optional)
    webinix_clean();

    return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
