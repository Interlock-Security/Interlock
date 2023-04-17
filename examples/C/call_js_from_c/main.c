// Call JavaScript from C Example

#include "webinix.h"

void my_function_exit(webinix_event_t* e) {

    // Close all opened windows
    webinix_exit();
}

void my_function_count(webinix_event_t* e) {

    // This function gets called every time the user clicks on "MyButton1"

    // Create a buffer to hold the response
    char response[64];

    // Run JavaScript
    if(!webinix_script(e->window, "return GetCount();", 0, response, 64)) {

        printf("JavaScript Error: %s\n", response);
        return;
    }

    // Get the count
    int count = atoi(response);

    // Increment
    count++;

    // Generate a JavaScript
    char js[64];
    sprintf(js, "SetCount(%d);", count);

    // Run JavaScript (Quick Way)
    webinix_run(e->window, js);
}

int main() {

    // HTML
    const char* my_html = 
    "<html>"
    "  <head>"
    "    <title>Call JavaScript from C Example</title>"
    "    <style>"
    "      body {"
    "        color: white;"
    "        background: #0F2027;"
    "        text-align: center;"
    "        font-size: 16px;"
    "        font-family: sans-serif;"
    "      }"
    "    </style>"
    "  </head>"
    "  <body>"
    "    <h2>Webinix - Call JavaScript from C Example</h2>"
    "    <br>"
    "    <h1 id=\"MyElementID\">Count is ?</h1>"
    "    <br>"
    "    <br>"
    "    <button id=\"MyButton1\">Count</button>"
    "    <br>"
    "    <br>"
    "    <button id=\"MyButton2\">Exit</button>"
    "    <script>"
    "      var count = 1;"
    "      function GetCount() {"
    "        return count;"
    "      }"
    "      function SetCount(number) {"
    "        const MyElement = document.getElementById('MyElementID');"
    "        MyElement.innerHTML = 'Count is ' + number;"
    "        count = number;"
    "      }"
    "    </script>"
    "  </body>"
    "</html>";

    // Create a window
    void* my_window = webinix_new_window();

    // Bind HTML elements with C functions
    webinix_bind(my_window, "MyButton1", my_function_count);
    webinix_bind(my_window, "MyButton2", my_function_exit);

    // Show the window
    webinix_show(my_window, my_html);

    // Wait until all windows get closed
    webinix_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
