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
from ctypes import *

lib = None

# Event
class event:
	element_id = 0
	window_id = 0
	element_name = ""

# Webinix C-Struct
class webinix_javascript_py_t(ctypes.Structure):
	_fields_ = [
		("script", c_char_p),
		("timeout", c_uint),
		("error", c_bool),
		("length", c_uint),
		("data", c_char_p)
	]

# JavaScript
class javascript:
	script = ""
	timeout = 10
	error = False
	length = 0
	data = ""

class window:

	window = None
	c_events = None
	cb_fun_list = [64]

	def __init__(self):
		global lib
		try:
			# Load Webinix Shared Library
			load_library()
			# Check library if correctly loaded
			if lib is None:
				print('Please download the latest library from https://webinix.me')
				sys.exit(1)
			# Create new Webinix window
			webinix_wrapper = None
			webinix_wrapper = lib.webinix_new_window
			webinix_wrapper.restype = c_void_p
			self.window = c_void_p(webinix_wrapper())
			# Initializing events() to be called from Webinix Library
			py_fun = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint, ctypes.c_char_p)
			self.c_events = py_fun(self.events)
		except OSError as e:
			print("Webinix Exception: %s" % e)
			sys.exit(1)
	
	def __del__(self):
		global lib
		if self.window is not None and lib is not None:
			lib.webinix_close(self.window)
	
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
		global lib
		if self.window is None:
			err_window_is_none('bind')
			return
		if lib is None:
			err_library_not_found('bind')
			return
		cb_index = int(lib.webinix_bind_py(self.window, element.encode('utf-8'), self.c_events))
		self.cb_fun_list.insert(cb_index, func)
	
	def show(self, html):
		global lib
		if self.window is None:
			err_window_is_none('show')
			return
		if lib is None:
			err_library_not_found('show')
			return
		lib.webinix_show(self.window, html.encode('utf-8'))
	
	def close(self):
		global lib
		if lib is None:
			err_library_not_found('close')
			return
		lib.webinix_close(self.window)
	
	def run_js(self, script, timeout = 0) -> javascript:
		global lib
		if self.window is None:
			err_window_is_none('show')
			return
		if lib is None:
			err_library_not_found('show')
			return
		# Create Struct
		js = webinix_javascript_py_t()
		# Initializing
		js.script = ctypes.c_char_p(script.encode('utf-8'))
		js.timeout = ctypes.c_uint(timeout)
		js.error = ctypes.c_bool(False)
		js.length = ctypes.c_uint(0)
		js.data = ctypes.c_char_p("".encode('utf-8'))
		# Initializing Result
		res = javascript()
		res.script = script
		res.timeout = timeout
		res.error = True
		res.length = 7
		res.data = "UNKNOWN"		
		# Run JavaScript
		lib.webinix_run_js_py(self.window, ctypes.byref(js))
		res.length = int(js.length)
		res.data = js.data.decode('utf-8')
		res.error = js.error
		return res

# Exit app
def load_library():
	global lib
	if platform.system() == 'Darwin':
		lib = ctypes.CDLL('webinix-2-x64.dylib')
		if lib is None:
			print("Webinix Error: Failed to load 'webinix-2-x64.dylib' library.")
	elif platform.system() == 'Windows':
		if sys.version_info.major == 3 and sys.version_info.minor <= 8:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			lib = ctypes.CDLL('webinix-2-x64.dll')
		else:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			lib = cdll.LoadLibrary('webinix-2-x64.dll')
		if lib is None:
			print("Webinix Error: Failed to load 'webinix-2-x64.dll' library.")
	elif platform.system() == 'Linux':
		os.chdir(os.getcwd())
		lib = ctypes.CDLL(os.getcwd() + '/webinix-2-x64.so')
		if lib is None:
			print("Webinix Error: Failed to load 'webinix-2-x64.so' library.")
	else:
		print("Webinix Error: Unsupported OS")

# Exit app
def exit():
	global lib
	if lib is None:
		err_library_not_found('exit')
		return
	lib.webinix_exit()

# Wait until all windows get closed
def loop():
	global lib
	if lib is None:
		err_library_not_found('loop')
		return
	lib.webinix_loop()

def err_library_not_found(f):
	print('Webinix ' + f + '(): Library Not Found.')

def err_window_is_none(f):
	print('Webinix ' + f + '(): Window is None.')
