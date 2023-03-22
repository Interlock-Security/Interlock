/*
    Webinix Library 2.0.7
    C99 Example

    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga> - Canada.
*/

#include "webinix.h"

int main() {

    webinix_window_t* my_window = webinix_new_window();
    webinix_show(my_window, "<html>Hello</html>", webinix.browser.any);
    webinix_wait();
    return 0;
}
