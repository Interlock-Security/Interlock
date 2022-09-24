# Webinix Library 2.0.0
#
# http://webinix.me
# https://github.com/alifcommunity/webinix
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

# [!] IMPORTANT
# Please build a dynamic version of Webinix library using
# your favorite C compiler, then copy file 'webinix-2-x64'
# into this folder.

import os
import platform
import sys
import ctypes
from ctypes import cdll, c_void_p, CFUNCTYPE, POINTER

webinix_lib_loader = None

class window:

	window = None
	c_events = None
	cb_fun_list = [64]

	def __init__(self):
		global webinix_lib_loader
		try:
			# Load Webinix Shared Library
			load_library()
			# Check library if correctly loaded
			if webinix_lib_loader is None:
				print('Please download the latest library from https://webinix.me')
				sys.exit(1)
			# Create new Webinix window
			webinix_wrapper = None
			webinix_wrapper = webinix_lib_loader.webinix_new_window
			webinix_wrapper.restype = c_void_p
			self.window = c_void_p(webinix_wrapper())
			# Initializing events() to be called from Webinix Library
			py_fun = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint, ctypes.c_char_p)
			self.c_events = py_fun(self.events)
		except OSError as e:
			print("Webinix Exception: %s" % e)
			sys.exit(1)
	
	def __del__(self):
		global webinix_lib_loader
		if self.window is not None and webinix_lib_loader is not None:
			webinix_lib_loader.webinix_close(self.window)
	
	def events(self, element_id, window_id, element_name):
		if self.cb_fun_list[int(element_id)] is None:
			print('Webinix Error: Callback is None.')
			return
		e = event()
		e.element_id = element_id
		e.window_id = window_id
		e.element_name = element_name
		self.cb_fun_list[element_id](e)

	def bind(self, element, func):
		global webinix_lib_loader
		if self.window is None:
			err_window_is_none('bind')
			return
		if webinix_lib_loader is None:
			err_library_not_found('bind')
			return
		cb_index = int(webinix_lib_loader.webinix_bind_py(self.window, element.encode('utf-8'), self.c_events))
		self.cb_fun_list.insert(cb_index, func)
	
	def show(self, html):
		global webinix_lib_loader
		if self.window is None:
			err_window_is_none('show')
			return
		if webinix_lib_loader is None:
			err_library_not_found('show')
			return
		webinix_lib_loader.webinix_show(self.window, html.encode('utf-8'))
	
	def close(self):
		global webinix_lib_loader
		if webinix_lib_loader is None:
			err_library_not_found('close')
			return
		webinix_lib_loader.webinix_close(self.window)

# Exit app
def load_library():
	global webinix_lib_loader
	if platform.system() == 'Darwin':
		webinix_lib_loader = ctypes.CDLL('webinix-2-x64.dylib')
		if webinix_lib_loader is None:
			print("Webinix Error: Failed to load 'webinix-2-x64.dylib' library.")
	elif platform.system() == 'Windows':
		if sys.version_info.major == 3 and sys.version_info.minor <= 8:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			webinix_lib_loader = ctypes.CDLL('webinix-2-x64.dll')
		else:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			webinix_lib_loader = cdll.LoadLibrary('webinix-2-x64.dll')
		if webinix_lib_loader is None:
			print("Webinix Error: Failed to load 'webinix-2-x64.dll' library.")
	elif platform.system() == 'Linux':
		os.chdir(os.getcwd())
		webinix_lib_loader = ctypes.CDLL(os.getcwd() + '/webinix-2-x64.so')
		if webinix_lib_loader is None:
			print("Webinix Error: Failed to load 'webinix-2-x64.so' library.")
	else:
		print("Webinix Error: Unsupported OS")

# Exit app
def exit():
	global webinix_lib_loader
	if webinix_lib_loader is None:
		err_library_not_found('exit')
		return
	webinix_lib_loader.webinix_exit()

# Wait until all windows get closed
def loop():
	global webinix_lib_loader
	if webinix_lib_loader is None:
		err_library_not_found('loop')
		return
	webinix_lib_loader.webinix_loop()

def err_library_not_found(f):
	print('Webinix ' + f + '(): Library Not Found.')

def err_window_is_none(f):
	print('Webinix ' + f + '(): Window is None.')

# Event
class event:
	element_id = 0
	window_id = 0
	element_name = ""
