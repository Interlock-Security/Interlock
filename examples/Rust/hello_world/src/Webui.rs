/*
    Webinix Library 2.0.2
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

// Flags
#![allow(non_camel_case_types)]
#![allow(unsafe_code)]
#![allow(dead_code)]
#![allow(non_snake_case)]
#![allow(improper_ctypes)]
#![allow(non_upper_case_globals)]

// Modules
use std::os::raw::c_char;
use std::ffi::CString;
use std::ffi::CStr;

// --[Webinix Library References]-------------------------------

pub type size_t = ::std::os::raw::c_ulonglong;

#[repr(C)]
pub struct webinix_event_t {
    pub window: *mut webinix_window_t,
    pub window_id: ::std::os::raw::c_uint,
    pub element_id: ::std::os::raw::c_uint,
    pub element_name: *mut ::std::os::raw::c_char,
}

#[repr(C)]
pub struct webinix_custom_browser_t {
    pub app: *mut ::std::os::raw::c_char,
    pub arg: *mut ::std::os::raw::c_char,
    pub auto_link: bool,
}

#[repr(C)]
pub struct webinix_window_core_t {
    pub window_number: ::std::os::raw::c_uint,
    pub server_running: bool,
    pub connected: bool,
    pub server_handled: bool,
    pub multi_access: bool,
    pub server_root: bool,
    pub server_port: ::std::os::raw::c_uint,
    pub is_bind_all: bool,
    pub url: *mut ::std::os::raw::c_char,
    pub cb_all: [::std::option::Option<unsafe extern "C" fn(e: webinix_event_t)>; 1usize],
    pub html: *const ::std::os::raw::c_char,
    pub icon: *const ::std::os::raw::c_char,
    pub icon_type: *const ::std::os::raw::c_char,
    pub CurrentBrowser: ::std::os::raw::c_uint,
    pub browser_path: *mut ::std::os::raw::c_char,
    pub profile_path: *mut ::std::os::raw::c_char,
    pub connections: ::std::os::raw::c_uint,
    pub runtime: ::std::os::raw::c_uint,
    pub detect_process_close: bool,
    pub server_thread: ::std::os::raw::c_int,
}

#[repr(C)]
pub struct webinix_window_t {
    pub core: webinix_window_core_t,
    pub path: *mut ::std::os::raw::c_char,
}

#[repr(C)]
pub struct webinix_javascript_result_t {
    pub error: bool,
    pub length: ::std::os::raw::c_uint,
    pub data: *const ::std::os::raw::c_char,
}

#[repr(C)]
pub struct webinix_javascript_t {
    pub script: *mut ::std::os::raw::c_char,
    pub timeout: ::std::os::raw::c_uint,
    pub result: webinix_javascript_result_t,
}

#[repr(C)]
pub struct webinix_runtime_t {
    pub none: ::std::os::raw::c_uint,
    pub deno: ::std::os::raw::c_uint,
    pub nodejs: ::std::os::raw::c_uint,
}

#[repr(C)]
pub struct webinix_t {
    pub servers: ::std::os::raw::c_uint,
    pub connections: ::std::os::raw::c_uint,
    pub custom_browser: *mut webinix_custom_browser_t,
    pub wait_for_socket_window: bool,
    pub html_elements: [*mut ::std::os::raw::c_char; 32usize],
    pub used_ports: [::std::os::raw::c_uint; 32usize],
    pub last_window: ::std::os::raw::c_uint,
    pub startup_timeout: ::std::os::raw::c_uint,
    pub use_timeout: bool,
    pub timeout_extra: bool,
    pub exit_now: bool,
    pub run_responses: [*mut ::std::os::raw::c_char; 32usize],
    pub run_done: [bool; 32usize],
    pub run_error: [bool; 32usize],
    pub run_last_id: ::std::os::raw::c_uint,
    pub runtime: webinix_runtime_t,
    pub initialized: bool,
    pub cb: [::std::option::Option<unsafe extern "C" fn(e: webinix_event_t)>; 32usize],
    pub cb_int: [::std::option::Option<
        unsafe extern "C" fn(
            arg1: ::std::os::raw::c_uint,
            arg2: ::std::os::raw::c_uint,
            arg3: *mut ::std::os::raw::c_char,
        ),
    >; 32usize],
    pub executable_path: *mut ::std::os::raw::c_char,
    pub ptr_list: [*mut ::std::os::raw::c_void; 32usize],
    pub ptr_position: size_t,
    pub ptr_size: [size_t; 32usize],
}

extern "C" {
    pub static mut webinix: webinix_t;
}

extern "C" {
    pub fn webinix_loop();
}

extern "C" {
    pub fn webinix_exit();
}

extern "C" {
    pub fn webinix_any_window_is_open() -> bool;
}

extern "C" {
    pub fn webinix_set_timeout(second: ::std::os::raw::c_uint);
}

extern "C" {
    pub fn webinix_new_window() -> *mut webinix_window_t;
}

extern "C" {
    pub fn webinix_show(
        win: *mut webinix_window_t,
        html: *const ::std::os::raw::c_char,
        browser: ::std::os::raw::c_uint,
    ) -> bool;
}

extern "C" {
    pub fn webinix_copy_show(
        win: *mut webinix_window_t,
        html: *const ::std::os::raw::c_char,
        browser: ::std::os::raw::c_uint,
    ) -> bool;
}

extern "C" {
    pub fn webinix_set_icon(
        win: *mut webinix_window_t,
        icon_s: *const ::std::os::raw::c_char,
        type_s: *const ::std::os::raw::c_char,
    );
}

extern "C" {
    pub fn webinix_allow_multi_access(win: *mut webinix_window_t, status: bool);
}

extern "C" {
    pub fn webinix_set_root_folder(
        win: *mut webinix_window_t,
        path: *const ::std::os::raw::c_char,
    ) -> bool;
}

extern "C" {
    pub fn webinix_new_server(
        win: *mut webinix_window_t,
        path: *const ::std::os::raw::c_char,
        index_html: *const ::std::os::raw::c_char,
    ) -> *const ::std::os::raw::c_char;
}

extern "C" {
    pub fn webinix_close(win: *mut webinix_window_t);
}

extern "C" {
    pub fn webinix_is_show(win: *mut webinix_window_t) -> bool;
}

extern "C" {
    pub fn webinix_run_js(win: *mut webinix_window_t, javascript: *mut webinix_javascript_t);
}

extern "C" {
    pub fn webinix_run_js_int(
        win: *mut webinix_window_t,
        script: *const ::std::os::raw::c_char,
        timeout: ::std::os::raw::c_uint,
        error: *mut bool,
        length: *mut ::std::os::raw::c_uint,
        data: *mut ::std::os::raw::c_char,
    );
}

extern "C" {
    pub fn webinix_bind(
        win: *mut webinix_window_t,
        element: *const ::std::os::raw::c_char,
        func: ::std::option::Option<unsafe fn(e: webinix_event_t) -> ()>,
    ) -> ::std::os::raw::c_uint;
}

extern "C" {
    pub fn webinix_bind_all(
        win: *mut webinix_window_t,
        func: ::std::option::Option<unsafe extern "C" fn(e: webinix_event_t)>,
    );
}

extern "C" {
    pub fn webinix_open(
        win: *mut webinix_window_t,
        url: *const ::std::os::raw::c_char,
        browser: ::std::os::raw::c_uint,
    ) -> bool;
}

extern "C" {
    pub fn webinix_free_js(javascript: *mut webinix_javascript_t);
}

extern "C" {
    pub fn webinix_runtime(win: *mut webinix_window_t, runtime: ::std::os::raw::c_uint);
}

extern "C" {
    pub fn webinix_detect_process_close(win: *mut webinix_window_t, status: bool);
}

// --[Tools]--------------------------------------------------

// fn char_to_string(c : *mut ::std::os::raw::c_char) -> String {
//     let cstr = unsafe {CStr::from_ptr(c)};
//     let s : String = String::from_utf8_lossy(cstr.to_bytes()).to_string();
//     return s;
// }

fn char_to_string(c : *const i8) -> String {

    let cstr = unsafe {CStr::from_ptr(c)};
    let s : String = String::from_utf8_lossy(cstr.to_bytes()).to_string();
    return s;
}

fn cstr_to_string(c : CString) -> String {

    let s : String = String::from_utf8_lossy(c.to_bytes()).to_string();
    return s;
}

// --[Wrapper]------------------------------------------------

pub const AnyBrowser: u32 = 0;
pub const Chrome: u32 = 1;
pub const Firefox: u32 = 2;
pub const Edge: u32 = 3;
pub const Safari: u32 = 4;
pub const Chromium: u32 = 5;
pub const Custom: u32 = 99;

pub struct JavaScript {
    pub timeout: u32,
	pub script:  String,
	pub error:   bool,
	pub data:    String,
}

pub fn RunJavaScript(win: *mut webinix_window_t, js: &mut JavaScript) {

    unsafe {

        // Script String to i8/u8
        let script_cpy = String::from(js.script.clone());
        let script_c_str = CString::new(script_cpy).unwrap();
        let script_c_char: *const c_char = script_c_str.as_ptr() as *const c_char;

        // Interface
        let script: *const ::std::os::raw::c_char = script_c_char;
        let timeout: ::std::os::raw::c_uint = js.timeout;
        let mut error: bool = false;
        let mut length: ::std::os::raw::c_uint = 0;
        let data = CString::new("").expect("");

        // Pointers
        let error_ptr: *mut bool = &mut error;
        let length_ptr: *mut ::std::os::raw::c_uint = &mut length;
        let data_ptr = data.into_raw();

        webinix_run_js_int(win, script, timeout, error_ptr, length_ptr, data_ptr);

        js.error = error;
        js.data = char_to_string(data_ptr);
    }
}

pub fn NewWindow() -> *mut webinix_window_t {

    unsafe {

        return webinix_new_window();
    }
}

pub fn Loop() {

    unsafe {

        webinix_loop();
    }
}

pub fn Exit() {

    unsafe {

        webinix_exit();
    }
}

pub fn Show(win: *mut webinix_window_t, html: &str, b: u32) -> bool {

    unsafe {

        // HTML String to i8/u8
        let html_c_str = CString::new(html).unwrap();
        let html_c_char: *const c_char = html_c_str.as_ptr() as *const c_char;

        return webinix_copy_show(win, html_c_char, b);
    }
}

pub fn Bind(win: *mut webinix_window_t, element: &str, func: fn(e: webinix_event_t)) -> u32 {

    // Element String to i8/u8
    let element_c_str = CString::new(element).unwrap();
    let element_c_char: *const c_char = element_c_str.as_ptr() as *const c_char;

    // Func to Option
    let f: Option<unsafe fn(e: webinix_event_t)> = Some(func);

    // Bind
    unsafe {

        webinix_bind(win, element_c_char, f);
    }
    
    return 0;
}
