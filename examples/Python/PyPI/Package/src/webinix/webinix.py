
# webinix_lib Library 2.3.0
#
# http://webinix.me
# https://github.com/alifcommunity/webinix
#
# Copyright (c) 2020-2023 Hassan Draga.
# Licensed under MIT License.
# All rights reserved.
# Canada.


import os
import platform
import sys
import ctypes
from ctypes import *
import shutil


webinix_lib = None
webinix_path = os.path.dirname(__file__)
PTR_CHAR = ctypes.POINTER(ctypes.c_char)
PTR_PTR_CHAR = ctypes.POINTER(PTR_CHAR)

# Scripts Runtime
class browser:
    any:int = 0 # Default recommended web browser
    chrome:int = 1 # Google Chrome
    firefox:int = 2 # Mozilla Firefox
    edge:int = 3 # Microsoft Edge
    safari:int = 4 # Apple Safari
    chromium:int = 5 # The Chromium Project
    opera:int = 6 # Opera Browser
    brave:int = 7 # The Brave Browser
    vivaldi:int = 8 # The Vivaldi Browser
    epic:int = 9 # The Epic Browser
    yandex:int = 10 # The Yandex Browser

# event
class event:
    window = 0
    event_type = 0
    element = ""
    data = ""


# JavaScript
class javascript:
    error = False
    response = ""


# Scripts Runtime
class runtime:
    none = 0
    deno = 1
    nodejs = 2


# The window class
class window:

    window = 0
    window_id = ""
    c_events = None
    cb_fun_list = {}

    def __init__(self):
        global webinix_lib
        try:
            # Load webinix_lib Shared Library
            _load_library()
            # Check library if correctly loaded
            if webinix_lib is None:
                print(
                    'Please download the latest webinix_lib lib from https://webinix.me')
                sys.exit(1)
            # Create new webinix_lib window
            webinix_wrapper = None
            webinix_wrapper = webinix_lib.webinix_new_window
            webinix_wrapper.restype = c_size_t
            self.window = c_size_t(webinix_wrapper())
            # Get the window unique ID
            self.window_id = str(self.window)
            # Initializing events() to be used by
            # webinix_lib library as a callback
            py_fun = ctypes.CFUNCTYPE(
                ctypes.c_void_p, # RESERVED
                ctypes.c_size_t, # window
                ctypes.c_uint, # event type
                ctypes.c_char_p, # element
                ctypes.c_char_p, # data
                ctypes.c_uint) # event number
            self.c_events = py_fun(self._events)
        except OSError as e:
            print(
                "webinix_lib Exception: %s" % e)
            sys.exit(1)


    # def __del__(self):
    #     global webinix_lib
    #     if self.window is not None and webinix_lib is not None:
    #         webinix_lib.webinix_close(self.window)


    def _events(self, window: ctypes.c_size_t,
               event_type: ctypes.c_uint,
               _element: ctypes.c_char_p,
               data: ctypes.c_char_p,
               event_number: ctypes.c_uint):
        element = _element.decode('utf-8')
        func_id = self.window_id + element
        if self.cb_fun_list[func_id] is None:
            print('webinix_lib error: Callback is None.')
            return
        # Create event
        e = event()
        e.window = self # e.window should refer to this class
        e.event_type = int(event_type)
        e.element = element
        if data is not None:
            e.data = data.decode('utf-8')
        else:
            e.data = ''
        # User callback
        cb_result = self.cb_fun_list[func_id](e)
        if cb_result is not None:
            cb_result_str = str(cb_result)
            cb_result_encode = cb_result_str.encode('utf-8')
            # Set the response
            webinix_lib.webinix_interface_set_response(window, event_number, cb_result_encode)


    # Bind a specific html element click event with a function. Empty element means all events.
    def bind(self, element, func):
        global webinix_lib
        if self.window == 0:
            _err_window_is_none('bind')
            return
        if webinix_lib is None:
            _err_library_not_found('bind')
            return
        # Bind
        webinix_lib.webinix_interface_bind(
            self.window,
            element.encode('utf-8'),
            self.c_events)
        # Add CB to the list
        func_id = self.window_id + element
        self.cb_fun_list[func_id] = func


    # Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
    def show(self, content="<html></html>", browser:int=0):
        global webinix_lib
        if self.window == 0:
            _err_window_is_none('show')
            return
        if webinix_lib is None:
            _err_library_not_found('show')
            return
        # Show the window
        webinix_lib.webinix_show_browser(self.window, content.encode('utf-8'), ctypes.c_uint(browser))


    # Chose between Deno and Nodejs runtime for .js and .ts files.
    def set_runtime(self, rt=runtime.deno):
        global webinix_lib
        if self.window == 0:
            _err_window_is_none('set_runtime')
            return
        if webinix_lib is None:
            _err_library_not_found('set_runtime')
            return
        webinix_lib.webinix_set_runtime(self.window, 
                        ctypes.c_uint(rt))


    def set_multi_access(self, status=False):
        global webinix_lib
        if self.window == 0:
            _err_window_is_none('set_multi_access')
            return
        if webinix_lib is None:
            _err_library_not_found('set_multi_access')
            return
        webinix_lib.webinix_set_multi_access(self.window, 
                        ctypes.c_bool(status))


    # Close the window.
    def close(self):
        global webinix_lib
        if webinix_lib is None:
            _err_library_not_found('close')
            return
        webinix_lib.webinix_close(self.window)


    def is_shown(self):
        global webinix_lib
        if webinix_lib is None:
            _err_library_not_found('is_shown')
            return
        r = bool(webinix_lib.webinix_is_shown(self.window))
        return r

    # Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
    def script(self, script, timeout=0, response_size=(1024 * 8)) -> javascript:
        global webinix_lib
        if self.window == 0:
            _err_window_is_none('script')
            return
        if webinix_lib is None:
            _err_library_not_found('script')
            return
        # Create Buffer
        buffer = ctypes.create_string_buffer(response_size)
        buffer.value = b""
        # Create a pointer to the buffer
        buffer_ptr = ctypes.pointer(buffer)
        # Run JavaScript
        status = bool(webinix_lib.webinix_script(self.window, 
            ctypes.c_char_p(script.encode('utf-8')), 
            ctypes.c_uint(timeout), buffer_ptr,
            ctypes.c_uint(response_size)))
        # Initializing Result
        res = javascript()
        res.data = buffer.value.decode('utf-8')
        res.error = not status
        return res

    # Run JavaScript quickly with no waiting for the response
    def run(self, script):
        global webinix_lib
        if self.window == 0:
            _err_window_is_none('run')
            return
        if webinix_lib is None:
            _err_library_not_found('run')
            return
        # Run JavaScript
        webinix_lib.webinix_run(self.window, 
            ctypes.c_char_p(script.encode('utf-8')))


def _get_library_path() -> str:
    global webinix_path
    if platform.system() == 'Darwin':
        file = '/webinix-2-x64.dyn'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = webinix_path + file
        if os.path.exists(path):
            return path
        return path
    elif platform.system() == 'Windows':
        file = '\\webinix-2-x64.dll'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = webinix_path + file
        if os.path.exists(path):
            return path
        return path
    elif platform.system() == 'Linux':
        file = '/webinix-2-x64.so'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = webinix_path + file
        if os.path.exists(path):
            return path
        return path
    else:
        return ""


# Load webinix_lib Dynamic Library
def _load_library():
    global webinix_lib
    global webinix_path
    if webinix_lib is not None:
        return
    if platform.system() == 'Darwin':
        webinix_lib = ctypes.CDLL(_get_library_path())
        if webinix_lib is None:
            print(
                "webinix_lib error: Failed to load webinix_lib lib.")
    elif platform.system() == 'Windows':
        if sys.version_info.major==3 and sys.version_info.minor<=8:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            webinix_lib = ctypes.CDLL(_get_library_path())
        else:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            webinix_lib = cdll.LoadLibrary(_get_library_path())
        if webinix_lib is None:
            print("webinix_lib error: Failed to load webinix_lib lib.")
    elif platform.system() == 'Linux':
        webinix_lib = ctypes.CDLL(_get_library_path())
        if webinix_lib is None:
            print("webinix_lib error: Failed to load webinix_lib lib.")
    else:
        print("webinix_lib error: Unsupported OS")


# Close all opened windows. webinix_wait() will break.
def exit():
    global webinix_lib
    if webinix_lib is not None:
        webinix_lib.webinix_exit()


# Set startup timeout
def set_timeout(second):
    global webinix_lib
    if webinix_lib is None:
        _load_library()
        if webinix_lib is None:
            _err_library_not_found('set_timeout')
            return
    webinix_lib.webinix_set_timeout(ctypes.c_uint(second))


def is_app_running():
    global webinix_lib
    if webinix_lib is None:
        _load_library()
        if webinix_lib is None:
            _err_library_not_found('is_app_running')
            return
    r = bool(webinix_lib.webinix_interface_is_app_running())
    return r


# Wait until all opened windows get closed.
def wait():
    global webinix_lib
    if webinix_lib is None:
        _load_library()
        if webinix_lib is None:
            _err_library_not_found('wait')
            return
    webinix_lib.webinix_wait()
    try:
        shutil.rmtree(os.getcwd() + '/__intcache__/')
    except OSError:
        pass


# 
def _err_library_not_found(f):
    print('webinix_lib ' + f + '(): Library Not Found.')


#
def _err_window_is_none(f):
    print('webinix_lib ' + f + '(): window is None.')


# Set the path to the webinix_lib prebuilt dynamic lib
def set_library_path(Path):
    global webinix_path
    webinix_path = Path
