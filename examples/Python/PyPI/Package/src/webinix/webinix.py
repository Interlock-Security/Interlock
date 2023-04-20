
# Webinix Library 2.2.0
#
# http://webinix.me
# https://github.com/alifcommunity/webinix
#
# Copyright (c) 2020-2023 Hassan Draga.
# Licensed under GNU General Public License v2.0.
# All rights reserved.
# Canada.


import os
import platform
import sys
import ctypes
from ctypes import *
import shutil


Webinix = None
Webinix_Path = os.path.dirname(__file__)
PTR_CHAR = ctypes.POINTER(ctypes.c_char)
PTR_PTR_CHAR = ctypes.POINTER(PTR_CHAR)


# Event
class event:
    Window = None
    EventType = 0
    Element = ""
    Data = ""


# JavaScript
class javascript:
    Error = False
    Response = ""


# Scripts Runtime
class runtime:
    none = 0
    Deno = 1
    Nodejs = 2


# The window class
class window:

    window = None
    window_id = None
    c_events = None
    cb_fun_list = {}

    def __init__(self):
        global Webinix
        try:
            # Load Webinix Shared Library
            load_library()
            # Check library if correctly loaded
            if Webinix is None:
                print(
                    'Please download the latest Webinix lib from https://webinix.me')
                sys.exit(1)
            # Create new Webinix window
            webinix_wrapper = None
            webinix_wrapper = Webinix.webinix_new_window
            webinix_wrapper.restype = c_void_p
            self.window = c_void_p(webinix_wrapper())
            # Get the window unique ID
            self.window_id = int(
                Webinix.webinix_interface_get_window_id(
                self.window))
            # Initializing events() to be used by
            # Webinix library as a callback
            py_fun = ctypes.CFUNCTYPE(
                ctypes.c_void_p, # Window
                ctypes.c_uint, # EventType
                ctypes.c_char_p, # Element
                ctypes.c_char_p, # Data
                ctypes.c_char_p) # Response
            self.c_events = py_fun(self.events)
        except OSError as e:
            print(
                "Webinix Exception: %s" % e)
            sys.exit(1)


    # def __del__(self):
    #     global Webinix
    #     if self.window is not None and Webinix is not None:
    #         Webinix.webinix_close(self.window)


    def events(self, window, event_type, 
                element, data, response):
        if self.cb_fun_list[self.window_id][element] is None:
            print('Webinix Error: Callback is None.')
            return
        # Create event
        e = event()
        e.Window = self
        e.EventType = int(event_type)
        e.Element = element.decode('utf-8')
        e.Data = data.decode('utf-8')
        # User callback
        cb_res = str(self.cb_fun_list[self.window_id][element](e))
        cb_res_encode = cb_res.encode('utf-8')
        # Set the response
        Webinix.webinix_interface_bind(response, cb_res_encode)


    def bind(self, element, func):
        global Webinix
        if self.window is None:
            err_window_is_none('bind')
            return
        if Webinix is None:
            err_library_not_found('bind')
            return
        # Bind
        Webinix.webinix_interface_bind(
            self.window,
            element.encode('utf-8'),
            self.c_events)
        # Add CB to the list
        self.cb_fun_list[self.window_id] = {element: func}


    def show(self, content="<html></html>"):
        global Webinix
        if self.window is None:
            err_window_is_none('show')
            return
        if Webinix is None:
            err_library_not_found('show')
            return
        # Show the window
        Webinix.webinix_show(self.window, content.encode('utf-8'))


    def set_runtime(self, rt=runtime.deno):
        global Webinix
        if self.window is None:
            err_window_is_none('set_runtime')
            return
        if Webinix is None:
            err_library_not_found('set_runtime')
            return
        Webinix.webinix_set_runtime(self.window, 
                        ctypes.c_uint(rt))


    def set_multi_access(self, status=False):
        global Webinix
        if self.window is None:
            err_window_is_none('set_multi_access')
            return
        if Webinix is None:
            err_library_not_found('set_multi_access')
            return
        Webinix.webinix_set_multi_access(self.window, 
                        ctypes.c_bool(status))


    def close(self):
        global Webinix
        if Webinix is None:
            err_library_not_found('close')
            return
        Webinix.webinix_close(self.window)


    def is_shown(self):
        global Webinix
        if Webinix is None:
            err_library_not_found('is_shown')
            return
        r = bool(Webinix.webinix_is_shown(self.window))
        return r

    def script(self, script, timeout=0, response_size=(1024 * 8)) -> javascript:
        global Webinix
        if self.window is None:
            err_window_is_none('show')
            return
        if Webinix is None:
            err_library_not_found('show')
            return
        # Create Buffer
        buffer = ctypes.create_string_buffer(response_size)
        buffer.value = b""
        # Create a pointer to the buffer
        buffer_ptr = ctypes.pointer(buffer)
        # Run JavaScript
        status = bool(Webinix.webinix_script(self.window, 
            ctypes.c_char_p(script.encode('utf-8')), 
            ctypes.c_uint(timeout), buffer_ptr,
            ctypes.c_uint(response_size)))
        # Initializing Result
        res = javascript()
        res.data = buffer.value.decode('utf-8')
        res.error = status
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
        file = '\\webinix-2-x64.dll'
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
    if Webinix is not None:
        return
    if platform.system() == 'Darwin':
        Webinix = ctypes.CDLL(get_library_path())
        if Webinix is None:
            print(
                "Webinix Error: Failed to load Webinix lib.")
    elif platform.system() == 'Windows':
        if sys.version_info.major==3 and sys.version_info.minor<=8:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            Webinix = ctypes.CDLL(get_library_path())
        else:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            Webinix = cdll.LoadLibrary(get_library_path())
        if Webinix is None:
            print("Webinix Error: Failed to load Webinix lib.")
    elif platform.system() == 'Linux':
        Webinix = ctypes.CDLL(get_library_path())
        if Webinix is None:
            print("Webinix Error: Failed to load Webinix lib.")
    else:
        print("Webinix Error: Unsupported OS")


# Exit app
def exit():
    global Webinix
    if Webinix is None:
        load_library()
        if Webinix is None:
            err_library_not_found('exit')
            return
    Webinix.webinix_exit()


# Set startup timeout
def set_timeout(second):
    global Webinix
    if Webinix is None:
        load_library()
        if Webinix is None:
            err_library_not_found('set_timeout')
            return
    Webinix.webinix_set_timeout(ctypes.c_uint(second))


def is_app_running():
    global Webinix
    if Webinix is None:
        load_library()
        if Webinix is None:
            err_library_not_found('is_app_running')
            return
    r = bool(Webinix.webinix_interface_is_app_running())
    return r


# Wait until all windows get closed
def wait():
    global Webinix
    if Webinix is None:
        load_library()
        if Webinix is None:
            err_library_not_found('wait')
            return
    Webinix.webinix_wait()
    try:
        shutil.rmtree(os.getcwd() + '/__intcache__/')
    except OSError:
        pass


# 
def err_library_not_found(f):
    print('Webinix ' + f + '(): Library Not Found.')


#
def err_window_is_none(f):
    print('Webinix ' + f + '(): Window is None.')


# Set the path to the Webinix prebuilt dynamic lib
def set_library_path(Path):
    global Webinix_Path
    Webinix_Path = Path
