# Webinix Library 2.0.3
#
# http://webinix.me
# https://github.com/alifcommunity/webinix
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

import os
import platform
import sys
import ctypes
from ctypes import *
import shutil

Webinix = None
Webinix_Path = os.path.dirname(__file__)

# Event
class event:
	element_id = 0
	window_id = 0
	element_name = ""
	window = None

# Webinix C-Struct
class webinix_script_interface_t(ctypes.Structure):
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

# The window class
class window:

	window = None
	c_events = None
	cb_fun_list = [64]

	def __init__(self):
		global Webinix
		try:
			# Load Webinix Shared Library
			load_library()
			# Check library if correctly loaded
			if Webinix is None:
				print('Please download the latest Webinix dynamic library from https://webinix.me')
				sys.exit(1)
			# Create new Webinix window
			webinix_wrapper = None
			webinix_wrapper = Webinix.webinix_new_window
			webinix_wrapper.restype = c_void_p
			self.window = c_void_p(webinix_wrapper())
			# Initializing events() to be called from Webinix Library
			py_fun = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint, ctypes.c_char_p, ctypes.c_void_p)
			self.c_events = py_fun(self.events)
		except OSError as e:
			print("Webinix Exception: %s" % e)
			sys.exit(1)
	
	def __del__(self):
		global Webinix
		if self.window is not None and Webinix is not None:
			Webinix.webinix_close(self.window)
	
	def events(self, element_id, window_id, element_name, window):
		if self.cb_fun_list[int(element_id)] is None:
			print('Webinix Error: Callback is None.')
			return
		e = event()
		e.element_id = element_id
		e.window_id = window_id
		e.element_name = element_name
		e.window = window
		self.cb_fun_list[element_id](e)

	def bind(self, element, func):
		global Webinix
		if self.window is None:
			err_window_is_none('bind')
			return
		if Webinix is None:
			err_library_not_found('bind')
			return
		cb_index = int(Webinix.webinix_bind_interface(self.window, element.encode('utf-8'), self.c_events))
		self.cb_fun_list.insert(cb_index, func)
	
	def show(self, html):
		global Webinix
		if self.window is None:
			err_window_is_none('show')
			return
		if Webinix is None:
			err_library_not_found('show')
			return
		Webinix.webinix_show(self.window, html.encode('utf-8'), 0)
	
	def close(self):
		global Webinix
		if Webinix is None:
			err_library_not_found('close')
			return
		Webinix.webinix_close(self.window)
	
	def run_js(self, script, timeout = 0) -> javascript:
		global Webinix
		if self.window is None:
			err_window_is_none('show')
			return
		if Webinix is None:
			err_library_not_found('show')
			return
		# Create Struct
		js = webinix_script_interface_t()
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
		Webinix.webinix_script_interface_struct(self.window, ctypes.byref(js))
		res.length = int(js.length)
		res.data = js.data.decode('utf-8')
		res.error = js.error
		return res

def get_library_path() -> str:
	global Webinix_Path
	if platform.system() == 'Darwin':
		file = '/webinix-2-x64.dylib'
		path = os.getcwd() + file
		if os.path.exists(path):
			return path
		path = Webinix_Path + file
		if os.path.exists(path):
			return path
		return path
	elif platform.system() == 'Windows':
		file = '\webinix-2-x64.dll'
		path = os.getcwd() + file
		if os.path.exists(path):
			return path
		path = Webinix_Path + file
		if os.path.exists(path):
			return path
		return path
	elif platform.system() == 'Linux':
		file = '/webinix-2-x64.so'
		path = os.getcwd() + file
		if os.path.exists(path):
			return path
		path = Webinix_Path + file
		if os.path.exists(path):
			return path
		return path
	else:
		return ""

# Load Webinix Dynamic Library
def load_library():
	global Webinix
	global Webinix_Path
	if platform.system() == 'Darwin':
		Webinix = ctypes.CDLL(get_library_path())
		if Webinix is None:
			print("Webinix Error: Failed to load Webinix dynamic library.")
	elif platform.system() == 'Windows':
		if sys.version_info.major == 3 and sys.version_info.minor <= 8:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			Webinix = ctypes.CDLL(get_library_path())
		else:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			Webinix = cdll.LoadLibrary(get_library_path())
		if Webinix is None:
			print("Webinix Error: Failed to load Webinix dynamic library.")
	elif platform.system() == 'Linux':
		Webinix = ctypes.CDLL(get_library_path())
		if Webinix is None:
			print("Webinix Error: Failed to load Webinix dynamic library.")
	else:
		print("Webinix Error: Unsupported OS")

# Exit app
def exit():
	global Webinix
	if Webinix is None:
		err_library_not_found('exit')
		return
	Webinix.webinix_exit()

# Wait until all windows get closed
def wait():
	global Webinix
	if Webinix is None:
		err_library_not_found('wait')
		return
	Webinix.webinix_wait()
	try:
		shutil.rmtree(os.getcwd() + '/__intcache__/')
	except OSError:
		pass

def err_library_not_found(f):
	print('Webinix ' + f + '(): Library Not Found.')

def err_window_is_none(f):
	print('Webinix ' + f + '(): Window is None.')

def set_library_path(Path):
	global Webinix_Path
	Webinix_Path = Path
