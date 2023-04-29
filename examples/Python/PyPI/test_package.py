# This script is for debugging & the development of the Webinix Python wrapper.
# The wrapper source code is located at 'webinix/examples/Python/PyPI/Package/src/webinix/webinix.py'

# [!] Make sure to remove the Webinix package
# pip uninstall webinix2

# Import the Webinix local module
import sys
sys.path.append('Package/src/webinix')
import webinix

# Use the local Webinix Dynamic lib
# webinix.set_library_path('../../../build/Windows/MSVC')

# HTML
html = """
<!DOCTYPE html>
<html>
	<head>
		<title>Webinix 2 - Python Wrapper Test</title>
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
		<h2>Python Wrapper Test</h2>
		<br>
		<input type="text" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off" value=\"2\">
		<br>
		<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
		<br>
		<button id="P2JS">Test Python-To-JS</button>
		<button OnClick="MyJS();">Test JS-To-Python</button>
		<button id="Exit">Exit</button>
		<script>
			function MyJS() {
				const number = document.getElementById('MyInput').value;
				var result = webinix_fn('JS2P', number);
				document.getElementById('MyInput').value = result;
			}
		</script>
    </body></html>
"""

def all_events(e : webinix.event):
	print('Function: all_events()')
	print('Element: ' + e.element)
	print('Type: ' + str(e.event_type))
	print('Data: ' + e.data)

def python_to_js(e : webinix.event):
	print('Function: python_to_js()')
	print('Element: ' + e.element)
	print('Type: ' + str(e.event_type))
	print('Data: ' + e.data)
	# Run JavaScript to get the password
	res = e.window.script("return document.getElementById('MyInput').value;")
	# Check for any error
	if res.error is True:
		print("JavaScript Error: [" + res.data + "]")
	else:
		print("JavaScript OK: [" + res.data + "]")
	# Quick JavaScript (no response waiting)
	# e.window.run("alert('Fast!')")

def js_to_python(e : webinix.event):
	print('Function: js_to_python()')
	print('Element: ' + e.element)
	print('Type: ' + str(e.event_type))
	print('Data: ' + e.data)
	v = int(e.data)
	v = v * 2
	return v

def exit(e : webinix.event):
	print('Function: exit()')
	print('Element: ' + e.element)
	print('Type: ' + str(e.event_type))
	print('Data: ' + e.data)
	webinix.exit()

def main():

	# Create a window object
	MyWindow = webinix.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind('', all_events)
	MyWindow.bind('P2JS', python_to_js)
	MyWindow.bind('JS2P', js_to_python)
	MyWindow.bind('Exit', exit)

	# Show the window
	MyWindow.show(html, webinix.browser.any)

	# Wait until all windows are closed
	webinix.wait()

	print('Test Done.')

if __name__ == "__main__":
	main()
