


# Install Webinix
# pip install --upgrade webinix2

from webinix import webinix

MyWindow = webinix.window()
MyWindow.show('<html>Hello World</html>')
webinix.wait()
print('Thank you.')
