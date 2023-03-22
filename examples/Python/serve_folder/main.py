
# Webinix Library 2.0.7
# Python Example
#
# http://webinix.me
# https://github.com/alifcommunity/webinix
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga> - Canada.

# Install Webinix
# pip install --upgrade webinix2

from webinix import webinix

def switch_to_second_page(e : webinix.event):
	# This function get called every time 
	# the user click on "SwitchToSecondPage" button
	e.window.open("second.html", webinix.browser.any)

def close_the_application(e : webinix.event):
	webinix.exit()

def main():

	# Create a window object
	MyWindow = webinix.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind('SwitchToSecondPage', switch_to_second_page)
	MyWindow.bind('Exit', close_the_application)

	# The root path. Leave it empty to let the Webinix 
	# automatically select the current working folder
	root_path = ""

	# Create a new web server using Webinix
	url = MyWindow.new_server(root_path)

	# Show a window using the generated URL
	MyWindow.open(url, webinix.browser.chrome)

	# Wait until all windows are closed
	webinix.wait()

	# --[ Note ]-----------------
	# Add this script to all your .html files:
	# <script src="webinix.js"></script>
	# ---------------------------

	print('Thank you.')

if __name__ == "__main__":
	main()
