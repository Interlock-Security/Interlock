
# Webinix Library 2.x
# Python Example
#
# http://webinix.me
# https://github.com/alifcommunity/webinix
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga>.

# Install Webinix
# pip install --upgrade webinix2

from webinix import webinix

# HTML
login_html = """
<!DOCTYPE html>
<html>
	<head>
		<title>Webinix 2 - Python Example</title>
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
		<h1>Webinix 2 - Python Example</h1>
		<br>
		<input type="password" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off">
		<br>
	<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
		<br>
	<button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
	</body>
</html>
"""

dashboard_html = """
<!DOCTYPE html>
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
</html>
"""

# This function get called every time the user click on "MyButton1"
def check_the_password(e : webinix.event):

	# Run JavaScript to get the password
	res = e.window.run_js("return document.getElementById(\"MyInput\").value;")

	# Check for any error
	if res.error is True:
		print("JavaScript Error: " + res.data)
		return

	# Check the password
	if res.data == "123456":
		print("Password is correct.")
		e.window.show(dashboard_html)
	else:
		print("Wrong password: " + res.data)
		e.window.run_js(" document.getElementById('err').innerHTML = 'Sorry. Wrong password'; ")

def close_the_application(e : webinix.event):
	webinix.exit()

def main():

	# Create a window object
	MyWindow = webinix.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind('CheckPassword', check_the_password)
	MyWindow.bind('Exit', close_the_application)

	# Show the window
	MyWindow.show(login_html)

	# Wait until all windows are closed
	webinix.wait()

	print('Bye.')

if __name__ == "__main__":
	main()
