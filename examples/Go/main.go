// Webinix Go Example
// https://github.com/alifcommunity/webinix

package main

import (
	"fmt"

	"github.com/alifcommunity/webinix/webinix"
)

const my_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>Webinix 2 - Go Example</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>Webinix 2 - Go Example</h1>
    <br>
    <input type="password" id="MyInput">
    <br>
    <br>
	<button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
  </body>
</html>`

func Exit(e webinix.Event) {

	webinix.Exit()
}

func Check(e webinix.Event) {

	// Script to get the text value
	js := webinix.JavaScript{
		Timeout: 10,
		Script:  "return document.getElementById(\"MyInput\").value;",
	}

	// Run the script
	webinix.RunJavaScript(e.Window, &js)

	// Check if any error
	if !js.Error {

		fmt.Printf("Password: %s\n", js.Data)

		// Check the password
		if js.Data == "123456" {

			js.Script = "alert('Good. Password is correct.')"
			webinix.RunJavaScript(e.Window, &js)
		} else {

			js.Script = "alert('Sorry. Wrong password.')"
			webinix.RunJavaScript(e.Window, &js)
		}
	} else {

		// There is an error in our script
		fmt.Printf("JavaScript Error: %s\n", js.Data)
	}
}

func main() {

	// New window
	var my_window = webinix.NewWindow()

	// Bind
	webinix.Bind(my_window, "CheckPassword", Check)
	webinix.Bind(my_window, "Exit", Exit)

	// Show window
	webinix.Show(my_window, my_html, 0)

	// Loop
	webinix.Loop()

	fmt.Println("Bye.")
}
