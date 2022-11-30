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

    // JavaScript: webinix_fn('One', 'Hello');

    const char* str = webinix_get_string(e);
    printf("function_one: %s\n", str); // Hello
}

void function_two(webinix_event_t* e) {

    // JavaScript: webinix_fn('Two', 2022);

    int number = webinix_get_int(e);
    printf("function_two: %d\n", number); // 2022
}

void function_three(webinix_event_t* e) {

    // JavaScript: webinix_fn('Three', true);

    bool status = webinix_get_bool(e);
    if(status)
        printf("function_three: True\n"); // True
    else
        printf("function_three: False\n"); // False
}

void function_four(webinix_event_t* e) {

    // JavaScript: const result = webinix_fn('Four', 2);

    int number = webinix_get_int(e);
    number = number * 2;
    printf("function_four: %d\n", number); // 4

    // Send back the response to JavaScript
    webinix_return_int(e, number);
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
    "<button OnClick=\"webinix_fn('One', 'Hello');\">Call C function one</button><br><br>"
    "<button OnClick=\"webinix_fn('Two', 2022);\">Call C function two</button><br><br>"
    "<button OnClick=\"webinix_fn('Three', true);\">Call C function three</button><br><br>"
    "<p>Call C function four, and wait for the result</p><br>"
    "<button OnClick=\"MyJS();\">Call C function four</button><br><br>"
    "<input type=\"text\" id=\"MyInput\" value=\"2\">"
    "<script>"
    "   function MyJS() {"
    "       const number = document.getElementById('MyInput').value;"
    "       const result = webinix_fn('Four', number);"
    "       document.getElementById('MyInput').value = result;"
    "   }"
    "</script>"
    "</body></html>";

    // Create a window
    webinix_window_t* my_window = webinix_new_window();

    // Bind HTML elements with functions
    webinix_bind(my_window, "One", function_one);
    webinix_bind(my_window, "Two", function_two);
    webinix_bind(my_window, "Three", function_three);
    webinix_bind(my_window, "Four", function_four);

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
