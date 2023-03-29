// Call JavaScript from C Example

#include "webinix.h"

void my_function_exit(webinix_event_t* e) {

    // Close all opened windows
    webinix_exit();
}

void my_function_count(webinix_event_t* e) {

    // This function gets called every time the user clicks on "MyButton1"

    // Create a Webinix JavaScript struct
    webinix_script_t MyJavaScript = {
        .script = "return GetCount();",
        .timeout = 3
    };

    // Run JavaScript
    webinix_script(e->window, &MyJavaScript);

    // Check if there is any JavaScript error
    if(MyJavaScript.result.error) {

        printf("JavaScript Error: %s\n", MyJavaScript.result.data);
        return;
    }

    // Get the count
    int count = atoi(MyJavaScript.result.data);

    // Increment
    count++;

    // Generate a JavaScript
    char buf[64];
    sprintf(buf, "SetCount(%d);", count);

    // Run JavaScript
    MyJavaScript.script = buf;
    webinix_script(e->window, &MyJavaScript);

    // Free data resources
    webinix_script_cleanup(&MyJavaScript);
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
    webinix_window_t* my_window = webinix_new_window();

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
