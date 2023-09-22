// Text Editor in C using Webinix

#include "webinix.h"

void Close(webinix_event_t* e) {
    printf("Exit.\n");

    // Close all opened windows
    webinix_exit();
}

int main() {

    // Create new windows
    int MainWindow = webinix_new_window();

    // Bind HTML element IDs with a C functions
    webinix_bind(MainWindow, "close-button", Close);

    // Show a new window
    webinix_set_root_folder(MainWindow, "ui");
    if (!webinix_show_browser(MainWindow, "MainWindow.html", ChromiumBased))
        webinix_show(MainWindow, "MainWindow.html");

    // Wait until all windows get closed
    webinix_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
