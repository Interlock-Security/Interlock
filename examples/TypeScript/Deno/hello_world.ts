
// Run this script by:
// deno run --allow-all --unstable hello_world.ts

// Import Webinix module (Local file)
import * as webinix from "../module/webinix.ts";

// Optional - Set a custom library path:
//  const lib_full_path = '../../../../build/Windows/MSVC/webinix-2-x64.dll';
//  console.log("Looking for the Webinix dynamic library at: " + lib_full_path);
//  webinix.set_lib_path(lib_full_path);

const my_html = `
<!DOCTYPE html>
<html>
	<head>
		<title>Webinix 2 - Deno Hello World Example</title>
        <style>
            body {
                color: white;
                background: #101314;
                background: linear-gradient(to right, #101314, #0f1c20, #061f2a);
                text-align: center;
                font-size: 16px;
                font-family: sans-serif;
            }
        </style>
    </head>
    <body>
        <h1>Webinix 2 - Deno Hello World Example</h1><br>
        A: <input id="MyInputA" value="4"><br><br>
        B: <input id="MyInputB" value="6"><br><br>
        <div id="Result" style="color: #dbdd52">A + B = ?</div><br><br>
	    <button id="Calculate">Calculate</button> - <button id="Exit">Exit</button>
        <script>
            function get_A() {
              return parseInt(document.getElementById('MyInputA').value);
            }

            function get_B() {
              return parseInt(document.getElementById('MyInputB').value);
            }

            function set_result(res) {
              document.getElementById("Result").innerHTML = 'A + B = ' + res;
            }
        </script>
    </body>
</html>
`;

function calculate(e : webinix.event) {

  // Create a JavaScript object
  const my_js = webinix.js;

  // Settings if needed
  // my_js.timeout = 30; // Set javascript execution timeout
  // my_js.response_size = 64; // Set the response size in bytes

  // Call a js function
  if(!webinix.script(e.win, my_js, "return get_A()")) {
    // Error
    console.log("Error in the JavaScript: " + my_js.response);
    return;
  }

  // Get A
  const A = my_js.response;

  // Call a js function
  if(!webinix.script(e.win, my_js, "return get_B();")) {
    // Error
    console.log("Error in the JavaScript: " + my_js.response);
    return;
  }

  // Get B
  const B = my_js.response;

  // Calculate
  const C : number = parseInt(A) + parseInt(B);

  // Run js (Quick Way)
  webinix.run(e.win, "set_result(" + C + ");");
}

// Create new window
const my_window = webinix.new_window();

// Bind
webinix.bind(my_window, "Calculate", calculate);
webinix.bind(my_window, "Exit", function(e : webinix.event) {
  // Close all windows and exit
  webinix.exit();
});

// Show the window
webinix.show(my_window, my_html); // Or webinix.show(my_window, 'hello_world.html');

// Wait until all windows get closed
await webinix.wait();

console.log("Thank you.");
Deno.exit(0);
