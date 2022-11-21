/*
    Webinix Library 2.x
    C99 Example

    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

// Note:
// To see the console logs (printf) you need to build in debug mode
// example 'make debug', or 'nmake debug', or 'mingw32-make debug'

#include "webinix.h"

void function_one(webinix_event_t* e) {

    // JS: webinix_fn('One', 'Hello');

    const char* str = webinix_as_string(e);
    printf("function_one: %s\n", str); // Hello
}

void function_two(webinix_event_t* e) {

    // JS: webinix_fn('Two', 2022);

    int number = webinix_as_int(e);
    printf("function_two: %d\n", number); // 2022
}

void function_three(webinix_event_t* e) {

    // JS: webinix_fn('Three', true);

    bool status = webinix_as_bool(e);
    if(status)
        printf("function_three: True\n"); // True
}

int main() {

    // HTML
    const char* my_html = "<!DOCTYPE html>"
    "<html><head><title>Webinix 2 - C99 Example</title>"
    "<style>body{color: white; background: #0F2027;"
    "background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);"
    "background: linear-gradient(to right, #2C5364, #203A43, #0F2027);"
    "text-align:center; font-size: 18px; font-family: sans-serif;}</style></head><body>"
    "<h2>Webinix 2 - C99 Example</h2>"
    "<p>Call C function with arguments</p><br>"
    "<button OnClick=\"webinix_fn('One', 'Hello');\">Call C Function One</button><br><br>"
    "<button OnClick=\"webinix_fn('Two', 2022);\">Call C Function Two</button><br><br>"
    "<button OnClick=\"webinix_fn('Three', true);\">Call C Function Three</button><br><br>"
    "</body></html>";

    // Create a window
    webinix_window_t* my_window = webinix_new_window();

    // Bind HTML elements with functions
    webinix_bind(my_window, "One", function_one);
    webinix_bind(my_window, "Two", function_two);
    webinix_bind(my_window, "Three", function_three);

    // Show the window
    if(!webinix_show(my_window, my_html, webinix.browser.chrome))   // Run the window on Chrome
        webinix_show(my_window, my_html, webinix.browser.any);      // If not, run on any other installed web browser

    // Wait until all windows get closed
    webinix_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
