// Call JavaScript from C++ Example

// Include the Webinix header
#include "webinix.hpp"

// Include C++ STD
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

void my_function_exit(webinix_event_t* e) {

    // Close all opened windows
    webinix::exit();
}

void my_function_count(webinix_event_t* e) {

    // This function gets called every time the user clicks on "MyButton1"

    // Create a buffer to hold the response
    char response[64];

    // Another way to create a buffer:
    //  std::string buffer;
    //  buffer.reserve(32);
    //  webinix::script(..., &buffer[0], 32);

    // Run JavaScript
    if(!webinix::script(e->window, "return GetCount();", 0, response, 64)) {

        std::cout << "JavaScript Error: " << response << std::endl;
        return;
    }

    // Get the count
    int count = std::stoi(response);

    // Increment
    count++;

    // Generate a JavaScript
    std::stringstream js;
    js << "SetCount(" << count << ");";

    // Run JavaScript (Quick Way)
    webinix::run(e->window, js.str());
}

int main() {

    // HTML
    const std::string my_html = R"V0G0N(
    <html>
      <head>
        <title>Call JavaScript from C++ Example</title>
        <style>
          body {
            color: white;
            background: #0F2027;
            text-align: center;
            font-size: 16px;
            font-family: sans-serif;
          }
        </style>
      </head>
      <body>
        <h2>Webinix - Call JavaScript from C++ Example</h2>
        <br>
        <h1 id="MyElementID">Count is ?</h1>
        <br>
        <br>
        <button id="MyButton1">Count</button>
        <br>
        <br>
        <button id="MyButton2">Exit</button>
        <script>
          var count = 0;
          function GetCount() {
            return count;
          }
          function SetCount(number) {
            const MyElement = document.getElementById('MyElementID');
            MyElement.innerHTML = 'Count is ' + number;
            count = number;
          }
        </script>
      </body>
    </html>
    )V0G0N";

    // Create a window
    size_t my_window = webinix::new_window();

    // Bind HTML elements with C++ functions
    webinix::bind(my_window, "MyButton1", my_function_count);
    webinix::bind(my_window, "MyButton2", my_function_exit);

    // Show the window
    webinix::show(my_window, my_html); // webinix::show_browser(my_window, my_html, Chrome);

    // Wait until all windows get closed
    webinix::wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
