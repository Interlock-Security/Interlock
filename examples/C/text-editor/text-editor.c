// Text Editor in C using Webinix

#include "webinix.h"

#include <stdio.h>
#include <stdlib.h>

void Close(webinix_event_t* e) {
    // Close all opened windows
    printf("Exit.\n");
    webinix_exit();
}

void Save(webinix_event_t* e) {
    // Save data received from the UI
    printf("Save.\n");
}

void Open(webinix_event_t* e) {
    // Open a new file
    printf("Open file.\n");
}

int main() {

    // Create new windows
    int MainWindow = webinix_new_window();

    // Bind HTML element IDs with a C functions
    webinix_bind(MainWindow, "Open", Open);
    webinix_bind(MainWindow, "Save", Save);
    webinix_bind(MainWindow, "Close", Close);

    // Show a new window
    webinix_show(MainWindow, "ui/MainWindow.html");

    // Wait until all windows get closed
    webinix_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
