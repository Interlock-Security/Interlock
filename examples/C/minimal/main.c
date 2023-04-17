// Webinix C - Minimal Example

#include "webinix.h"

int main() {

    void* my_window = webinix_new_window();
    webinix_show(my_window, "<html>Hello</html>");
    webinix_wait();
    return 0;
}
