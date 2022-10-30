// Webinix Go Hello World Example
// https://github.com/alifcommunity/webinix

package main

import (
	"fmt"

	"github.com/alifcommunity/webinix/webinix"
)

const login_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>Webinix 2 - Go Example</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        background: linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>Webinix 2 - Go Example</h1>
    <br>
    <input type="password" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off">
    <br>
    <h3 id="err" style="color: #dbdd52">&nbsp;</h3>
    <br>
    <button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
  </body>
</html>`

const dashboard_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>Dashboard</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        background: linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>Welcome !</h1>
    <br>
    <br>
    <button id="Exit">Exit</button>
  </body>
</html>`

func Exit(e webinix.Event) {

	webinix.Exit()
}

func Check(e webinix.Event) {

	// Script to get the text value
	MyScript := webinix.JavaScript{
		Timeout: 10,
		Script:  " return document.getElementById('MyInput').value; ",
	}

	// Run the script
	webinix.RunJavaScript(e.Window, &MyScript)

	// Check if any error
	if !MyScript.Error {

		fmt.Printf("Password: %s\n", MyScript.Data)

		// Check the password
		if MyScript.Data == "123456" {

			webinix.Show(e.Window, dashboard_html, webinix.AnyBrowser)
		} else {

			MyScript.Script = " document.getElementById('err').innerHTML = 'Sorry. Wrong password'; "
			webinix.RunJavaScript(e.Window, &MyScript)
		}
	} else {

		// There is an error in our script
		fmt.Printf("JavaScript Error: %s\n", MyScript.Data)
	}
}

func main() {

	// New window
	var my_window = webinix.NewWindow()

	// Bind
	webinix.Bind(my_window, "CheckPassword", Check)
	webinix.Bind(my_window, "Exit", Exit)

	// Show window
	webinix.Show(my_window, login_html, webinix.AnyBrowser)

	// Loop
	webinix.Wait()

	fmt.Println("Bye.")
}
