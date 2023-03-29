// Webinix C - Minimal Example

#include "webinix.h"

int main() {

    webinix_window_t* my_window = webinix_new_window();
    webinix_show(my_window, "<html>Hello</html>");
    webinix_wait();
    return 0;
}
