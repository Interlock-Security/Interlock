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
    Call Secret() function and get the response
    <br>
    <br>
    <button OnClick="webinix_fn('Sec').then((response) => { alert('Response is ' + response) });">Secret</button>
    <br>
    <br>
    <button id="Exit">Exit</button>
  </body>
</html>`

func Exit(e webinix.Event) string {

	webinix.Exit()
	return ""
}

func Secret(e webinix.Event) string {

	return "I Love Go!"
}

func Check(e webinix.Event) string {

	// Create new JavaScript object
	js := webinix.NewJavaScript()

	// Run the script
	if !webinix.Script(e.Window, &js, "return document.getElementById('MyInput').value;") {

		// There is an error in our script
		fmt.Printf("JavaScript Error: %s\n", js.Response)
		return ""
	}

	fmt.Printf("Password: [%s]\n", js.Response)

	// Check the password
	if js.Response == "123456" {
		webinix.Show(e.Window, dashboard_html)
	} else {
		webinix.Script(e.Window, &js, "document.getElementById('err').innerHTML = 'Sorry. Wrong password';")
	}

	return ""
}

func main() {

	// New window
	var my_window = webinix.NewWindow()

	// Bind
	webinix.Bind(my_window, "CheckPassword", Check)
	webinix.Bind(my_window, "Sec", Secret)
	webinix.Bind(my_window, "Exit", Exit)

	// Show window
	webinix.Show(my_window, login_html)

	// Loop
	webinix.Wait()

	fmt.Println("Thank you.")
}
