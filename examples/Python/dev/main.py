
# Webinix Library 2.x
# Python Example
#
# http://webinix.me
# https://github.com/alifcommunity/webinix
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

# This script is for debugging & development of the Webinix Python wrapper
# The source code is located at 'webinix/packages/PyPI/src/webinix/webinix.py'

# [!] Make sure to remove the Webinix package
# pip uninstall webinix2

# Import the Webinix local module
import sys
sys.path.append('../../../packages/PyPI/src/webinix')
import webinix

# HTML
html = """
<!DOCTYPE html>
<html>
	<head>
		<title>Webinix 2 - Python Debug & Development</title>
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
		<h2>Python Debug & Development</h2>
		<br>
		<input type="password" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off">
		<br>
	<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
		<br>
	<button id="TestID">Test</button> - <button id="ExitID">Exit</button>
	</body>
</html>
"""

def test(e : webinix.event):

	# Print some info (optional)
	print('Element_id: ' + str(e.element_id))
	print('Window_id: ' + str(e.window_id))
	print('Element_name: ' + e.element_name.decode('utf-8'))

	# Run JavaScript to get the password
	res = e.window.run_js("return document.getElementById(\"MyInput\").value;")

	# Check for any error
	if res.error is True:
		print("JavaScript Error -> Output: [" + res.data + "]")
	else:
		print("JavaScript OK -> Output: [" + res.data + "]")

def close(e : webinix.event):
	webinix.exit()

def main():

	# Create a window object
	MyWindow = webinix.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind('TestID', test)
	MyWindow.bind('ExitID', close)

	# Show the window
	MyWindow.show(html)

	# Wait until all windows are closed
	webinix.wait()

	print('Done.')

if __name__ == "__main__":
	main()
