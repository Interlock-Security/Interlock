// Call C from JavaScript Example

#include "webinix.h"

void my_function_string(webinix_event_t* e) {

    // JavaScript:
    // webinix_fn('MyID_One', 'Hello');

    const char* str = webinix_get_string(e);
    printf("my_function_string: %s\n", str); // Hello

    // Need Multiple Arguments?
    //
    // Webinix support only one argument. To get multiple arguments
    // you can send a JSON string from JavaScript then decode it.
    // Example:
    //
    // my_json = my_json_decoder(str);
    // foo = my_json[0];
    // bar = my_json[1];
}

void my_function_integer(webinix_event_t* e) {

    // JavaScript:
    // webinix_fn('MyID_Two', 123456789);

    long long number = webinix_get_int(e);
    printf("my_function_integer: %lld\n", number); // 123456789
}

void my_function_boolean(webinix_event_t* e) {

    // JavaScript:
    // webinix_fn('MyID_Three', true);

    bool status = webinix_get_bool(e); // True
    if(status)
        printf("my_function_boolean: True\n");
    else
        printf("my_function_boolean: False\n");
}

void my_function_with_response(webinix_event_t* e) {

    // JavaScript:
    // const result = webinix_fn('MyID_Four', number);

    long long number = webinix_get_int(e);
    number = number * 2;
    printf("my_function_with_response: %lld\n", number);

    // Send back the response to JavaScript
    webinix_return_int(e, number);
}

int main() {

    // HTML
    const char* my_html = 
    "<html>"
    "  <head>"
    "    <title>Call C from JavaScript Example</title>"
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
    "    <h2>Webinix - Call C from JavaScript Example</h2>"
    "    <p>Call C function with argument (<em>See the logs in your terminal</em>)</p>"
    "    <br>"
    "    <button onclick=\"webinix_fn('MyID_One', 'Hello');\">Call my_function_string()</button>"
    "    <br>"
    "    <br>"
    "    <button onclick=\"webinix_fn('MyID_Two', 123456789);\">Call my_function_integer()</button>"
    "    <br>"
    "    <br>"
    "    <button onclick=\"webinix_fn('MyID_Three', true);\">Call my_function_boolean()</button>"
    "    <br>"
    "    <br>"
    "    <p>Call C function and wait for the response</p>"
    "    <br>"
    "    <button onclick=\"MyJS();\">Call my_function_with_response()</button>"
    "    <br>"
    "    <br>"
    "    <input type=\"text\" id=\"MyInputID\" value=\"2\">"
    "    <script>"
    "      function MyJS() {"
    "        const MyInput = document.getElementById('MyInputID');"
    "        const number = MyInput.value;"
    "        const result = webinix_fn('MyID_Four', number);"
    "        MyInput.value = result;"
    "      }"
    "    </script>"
    "  </body>"
    "</html>";

    // Create a window
    webinix_window_t* my_window = webinix_new_window();

    // Bind HTML elements with C functions
    webinix_bind(my_window, "MyID_One", my_function_string);
    webinix_bind(my_window, "MyID_Two", my_function_integer);
    webinix_bind(my_window, "MyID_Three", my_function_boolean);
    webinix_bind(my_window, "MyID_Four", my_function_with_response);

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
