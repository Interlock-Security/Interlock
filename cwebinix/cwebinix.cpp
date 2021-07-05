/*
	cWebinix Library
	- - - - - - -
	http://webinix.me
	https://github.com/alifcommunity/webinix
	Licensed under GNU General Public License v3.0.
	Copyright (C)2020 Hassan DRAGA <https://github.com/hassandraga>.
*/

#ifndef UNICODE
#define UNICODE
#endif

// C++ headers
#include <iostream>
#include <string>
#include <new>
#include <thread>
#include <vector>

// Webinix headers
#include <webinix/webinix.hpp>

// Python headers
#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Defines
#ifdef _WIN32
	#define DLLEXPORT extern "C" __declspec(dllexport)
#else
	#define DLLEXPORT extern "C"
#endif

bool py_initialized = false;
static std::vector<PyObject *> callback_obj_v;
static std::vector<void (*)()> callback_void_v;

DLLEXPORT void c_handler(webinix::event e) {

    if (!PyCallable_Check(callback_obj_v[e.id])){

        std::cerr << "Webinix Err: callback obj not callable. " << std::endl;
        return;
    }

    try {

        // Calling python function using PyObject
        // PyObject *result;
        // result = PyObject_CallObject(callback_obj_v[e.id], NULL); <-- This crash python interepter!
        // if(result == NULL)
        //     std::cerr << "Webinix Err: failed to call obj (null return). " << std::endl;
        
        // Calling python function using void pointer
        if(callback_void_v[e.id])
            (*callback_void_v[e.id])();
    }
    catch(...) {

        std::cerr << "Webinix Err: failed to call obj (exception). " << std::endl;
    }
}

DLLEXPORT void * c_create_window(void) {

    return new(std::nothrow) webinix::window;
}

DLLEXPORT void c_destroy_window(void *ptr) {

    if(ptr)
        delete ptr;
}

DLLEXPORT int c_show_window(void *ptr, char * html) {

    try {

        std::string _html = html;
        webinix::window * ref = reinterpret_cast<webinix::window *>(ptr);
        return ref->show(_html);
    }
    catch(...) {

        return -1;
    }
}

DLLEXPORT void c_bind_element(void *ptr, char const *id, PyObject *callback_obj, void (*callback_void)()) {

    // callback_obj:    python function as PyObject
    // callback_void:   python function as void pointer

    static PyObject *callback_obj_temp = NULL;
    std::string _id = id;
    webinix::window * ref = reinterpret_cast<webinix::window *>(ptr);

    if (!PyCallable_Check(callback_obj)){

        std::cerr << "Webinix Err: callback obj not callable. " << std::endl;
        return;
    }

    int elem_id = ref->bind(_id, c_handler);

    Py_XINCREF(callback_obj);           // Add a reference to new callback
    Py_XDECREF(callback_obj_temp);      // Dispose of previous callback
    callback_obj_temp = callback_obj;   // Remember new callback

    callback_obj_v.push_back(callback_obj_temp);
    callback_void_v.push_back(callback_void);
}

DLLEXPORT void c_loop (void) {

    std::thread ui(webinix::loop);
    ui.join();
}

DLLEXPORT bool c_any_is_show (void) {

    return webinix::any_is_show();
}

DLLEXPORT void c_ini (void) {

    if(!py_initialized){

        py_initialized = true;
        callback_obj_v.clear();
        callback_obj_v.push_back(NULL);
        callback_void_v.clear();
        callback_void_v.push_back(NULL);
        webinix::ini();
    }
}
