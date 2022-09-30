package main

import (
	"fmt"
)

const my_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>Webinix 2.0 Example</title>
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
    <h1>Webinix 2.0 Example</h1>
    <br>
    <input type="password" id="MyInput">
    <br>
    <br>
    <button id="MyButton1">Check Password</button> - <button id="MyButton2">Exit</button>
  </body>
</html>`

func main() {

	var my_window = webinix_new_window()

	webinix_show(my_window, my_html, 0)

	webinix_loop()

	fmt.Print("Bye.")
}
