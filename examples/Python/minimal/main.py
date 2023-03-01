
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

MyWindow = webinix.window()
MyWindow.show('<html>Hello World</html>')
webinix.wait()
print('Bye.')
