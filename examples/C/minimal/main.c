// Webinix C - Minimal Example

#include "webinix.h"

int main() {

    size_t my_window = webinix_new_window();
    webinix_show(my_window, "<html><head><script src=\"webinix.js\"></script></head> Hello World ! </html>");
    webinix_wait();
    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
