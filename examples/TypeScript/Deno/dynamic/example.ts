/*
    Webinix Library 2.x
    TypeScript Dynamic Lib Example

    http://webinix.me
    https://github.com/alifcommunity/webinix
    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/  

import * as webinix from "./webinix.ts";

// Optional: Set the library path
webinix.set_lib_path('../../../../build/Windows/MSVC/webinix-2-x64.dll');

function check_password() {
    
	console.log("You clicked on the check password button...");
    // ...
}

const my_html = `
<!DOCTYPE html>
<html>
	<head>
		<title>Webinix 2 - Deno Dynamic Lib Example</title>
        <style>
            body {
                color: white;
                background: #0F2027;
                background: -webkit-linear-gradient(to right, #101314, #0f1c20, #061f2a);
                background: linear-gradient(to right, #101314, #0f1c20, #061f2a);
                text-align: center;
                font-size: 18px;
                font-family: sans-serif;
            }
        </style>
    </head>
    <body>
        <h1>Webinix 2 - Deno Dynamic Lib Example</h1>
        <br>
        <input type="password" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off">
        <br>
	<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
        <br>
	<button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
    </body>
</html>
`;

// Create new window
const my_window = webinix.new_window();

// Bind
webinix.bind(my_window, "Exit", function(){
    console.log("You clicked on the exit button...");
    webinix.exit();
});
webinix.bind(my_window, "CheckPassword", check_password);

// Show the window
if(!webinix.show(my_window, my_html, webinix.browser.chrome))
	webinix.show(my_window, my_html, webinix.browser.any);

// Wait until all windows get closed
await webinix.wait();

console.log("Bye");
