/*
    Webinix Library 2.0.4
    
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

// --[Webinix Library References]--------
pub type size_t = ::std::os::raw::c_ulonglong;
pub const WEBUI_MAX_ARRAY: u32 = 32;
pub const __bool_true_false_are_defined: u32 = 1;
pub const true_: u32 = 1;
pub const false_: u32 = 0;
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_event_t {
    pub window_id: ::std::os::raw::c_uint,
    pub element_id: ::std::os::raw::c_uint,
    pub element_name: *mut ::std::os::raw::c_char,
    pub window: *mut webinix_window_t,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
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
    pub cb_all: [::std::option::Option<
        unsafe extern "C" fn(e: *mut webinix_event_t),
    >; 1usize],
    pub html: *const ::std::os::raw::c_char,
    pub html_cpy: *const ::std::os::raw::c_char,
    pub icon: *const ::std::os::raw::c_char,
    pub icon_type: *const ::std::os::raw::c_char,
    pub CurrentBrowser: ::std::os::raw::c_uint,
    pub browser_path: *mut ::std::os::raw::c_char,
    pub profile_path: *mut ::std::os::raw::c_char,
    pub connections: ::std::os::raw::c_uint,
    pub runtime: ::std::os::raw::c_uint,
    pub detect_process_close: bool,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_window_t {
    pub core: webinix_window_core_t,
    pub path: *mut ::std::os::raw::c_char,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_javascript_result_t {
    pub error: bool,
    pub length: ::std::os::raw::c_uint,
    pub data: *const ::std::os::raw::c_char,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_script_t {
    pub script: *const ::std::os::raw::c_char,
    pub timeout: ::std::os::raw::c_uint,
    pub result: webinix_javascript_result_t,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_cb_t {
    pub win: *mut webinix_window_t,
    pub element_id: *mut ::std::os::raw::c_char,
    pub element_name: *mut ::std::os::raw::c_char,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_cmd_async_t {
    pub win: *mut webinix_window_t,
    pub cmd: *mut ::std::os::raw::c_char,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_custom_browser_t {
    pub app: *mut ::std::os::raw::c_char,
    pub arg: *mut ::std::os::raw::c_char,
    pub auto_link: bool,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_browser_t {
    pub any: ::std::os::raw::c_uint,
    pub chrome: ::std::os::raw::c_uint,
    pub firefox: ::std::os::raw::c_uint,
    pub edge: ::std::os::raw::c_uint,
    pub safari: ::std::os::raw::c_uint,
    pub chromium: ::std::os::raw::c_uint,
    pub custom: ::std::os::raw::c_uint,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_runtime_t {
    pub none: ::std::os::raw::c_uint,
    pub deno: ::std::os::raw::c_uint,
    pub nodejs: ::std::os::raw::c_uint,
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
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
    pub run_responses: [*const ::std::os::raw::c_char; 32usize],
    pub run_done: [bool; 32usize],
    pub run_error: [bool; 32usize],
    pub run_last_id: ::std::os::raw::c_uint,
    pub browser: webinix_browser_t,
    pub runtime: webinix_runtime_t,
    pub initialized: bool,
    pub cb: [::std::option::Option<unsafe extern "C" fn(e: *mut webinix_event_t)>;
        32usize],
    pub cb_int: [::std::option::Option<
        unsafe extern "C" fn(
            element_id: ::std::os::raw::c_uint,
            window_id: ::std::os::raw::c_uint,
            element_name: *mut ::std::os::raw::c_char,
            window: *mut webinix_window_t,
        ),
    >; 32usize],
    pub executable_path: *mut ::std::os::raw::c_char,
    pub ptr_list: [*mut ::std::os::raw::c_void; 32usize],
    pub ptr_position: ::std::os::raw::c_uint,
    pub ptr_size: [usize; 32usize],
}
extern "C" {
    pub static mut webinix: webinix_t;
}
extern "C" {
    pub fn webinix_wait();
}
extern "C" {
    pub fn webinix_exit();
}
extern "C" {
    pub fn webinix_is_any_window_running() -> bool;
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
    pub fn webinix_show_cpy(
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
    pub fn webinix_multi_access(win: *mut webinix_window_t, status: bool);
}
extern "C" {
    pub fn _webinix_set_root_folder(
        win: *mut webinix_window_t,
        path: *const ::std::os::raw::c_char,
    ) -> bool;
}
extern "C" {
    pub fn webinix_new_server(
        win: *mut webinix_window_t,
        path: *const ::std::os::raw::c_char,
    ) -> *const ::std::os::raw::c_char;
}
extern "C" {
    pub fn webinix_close(win: *mut webinix_window_t);
}
extern "C" {
    pub fn webinix_is_shown(win: *mut webinix_window_t) -> bool;
}
extern "C" {
    pub fn webinix_script(
        win: *mut webinix_window_t,
        script: *mut webinix_script_t,
    );
}
extern "C" {
    pub fn webinix_bind(
        win: *mut webinix_window_t,
        element: *const ::std::os::raw::c_char,
        func: ::std::option::Option<
            // unsafe extern "C" fn(e: *mut webinix_event_t),
            unsafe fn(e: *mut webinix_event_t),
        >,
    ) -> ::std::os::raw::c_uint;
}
extern "C" {
    pub fn webinix_bind_all(
        win: *mut webinix_window_t,
        func: ::std::option::Option<
            unsafe extern "C" fn(e: *mut webinix_event_t),
        >,
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
    pub fn webinix_script_cleanup(script: *mut webinix_script_t);
}
extern "C" {
    pub fn webinix_script_runtime(
        win: *mut webinix_window_t,
        runtime: ::std::os::raw::c_uint,
    );
}
extern "C" {
    pub fn _webinix_wait_process(win: *mut webinix_window_t, status: bool);
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct webinix_script_interface_t {
    pub script: *mut ::std::os::raw::c_char,
    pub timeout: ::std::os::raw::c_uint,
    pub error: bool,
    pub length: ::std::os::raw::c_uint,
    pub data: *const ::std::os::raw::c_char,
}
extern "C" {
    pub fn webinix_bind_interface(
        win: *mut webinix_window_t,
        element: *const ::std::os::raw::c_char,
        func: ::std::option::Option<
            // unsafe extern "C" fn(
            //     element_id: ::std::os::raw::c_uint,
            //     window_id: ::std::os::raw::c_uint,
            //     element_name: *mut ::std::os::raw::c_char,
            //     window: *mut webinix_window_t,
            // ),
            unsafe fn(
                element_id: ::std::os::raw::c_uint,
                window_id: ::std::os::raw::c_uint,
                element_name: *mut ::std::os::raw::c_char,
                window: *mut webinix_window_t,
            ),
        >,
    ) -> ::std::os::raw::c_uint;
}
extern "C" {
    pub fn webinix_script_interface(
        win: *mut webinix_window_t,
        script: *const ::std::os::raw::c_char,
        timeout: ::std::os::raw::c_uint,
        error: *mut bool,
        length: *mut ::std::os::raw::c_uint,
        data: *mut ::std::os::raw::c_char,
    );
}
extern "C" {
    pub fn webinix_script_interface_struct(
        win: *mut webinix_window_t,
        //js_int: *mut webinix_script_interface_t,
        js_int: &webinix_script_interface_t,
    );
}
extern "C" {
    pub fn webinix_TEST(win: *mut webinix_window_t);
}
extern "C" {
    pub fn _webinix_init();
}
extern "C" {
    pub fn _webinix_get_cb_index(
        element: *mut ::std::os::raw::c_char,
    ) -> ::std::os::raw::c_uint;
}
extern "C" {
    pub fn _webinix_set_cb_index(
        element: *mut ::std::os::raw::c_char,
    ) -> ::std::os::raw::c_uint;
}
extern "C" {
    pub fn _webinix_get_free_port() -> ::std::os::raw::c_uint;
}
extern "C" {
    pub fn _webinix_get_new_window_number() -> ::std::os::raw::c_uint;
}
extern "C" {
    pub fn _webinix_wait_for_startup();
}
extern "C" {
    pub fn _webinix_free_port(port: ::std::os::raw::c_uint);
}
extern "C" {
    pub fn _webinix_set_custom_browser(p: *mut webinix_custom_browser_t);
}
extern "C" {
    pub fn _webinix_get_current_path() -> *mut ::std::os::raw::c_char;
}
extern "C" {
    pub fn _webinix_window_receive(
        win: *mut webinix_window_t,
        packet: *const ::std::os::raw::c_char,
        len: usize,
    );
}
extern "C" {
    pub fn _webinix_window_send(
        win: *mut webinix_window_t,
        packet: *mut ::std::os::raw::c_char,
        packets_size: usize,
    );
}
extern "C" {
    pub fn _webinix_window_event(
        win: *mut webinix_window_t,
        element_id: *mut ::std::os::raw::c_char,
        element: *mut ::std::os::raw::c_char,
    );
}
extern "C" {
    pub fn _webinix_window_get_number(
        win: *mut webinix_window_t,
    ) -> ::std::os::raw::c_uint;
}
extern "C" {
    pub fn _webinix_window_open(
        win: *mut webinix_window_t,
        link: *mut ::std::os::raw::c_char,
        browser: ::std::os::raw::c_uint,
    );
}
extern "C" {
    pub fn _webinix_cmd_sync(
        cmd: *mut ::std::os::raw::c_char,
        show: bool,
    ) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn _webinix_cmd_async(
        cmd: *mut ::std::os::raw::c_char,
        show: bool,
    ) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn _webinix_run_browser(
        win: *mut webinix_window_t,
        cmd: *mut ::std::os::raw::c_char,
    ) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn _webinix_clean();
}
extern "C" {
    pub fn _webinix_browser_exist(
        win: *mut webinix_window_t,
        browser: ::std::os::raw::c_uint,
    ) -> bool;
}
extern "C" {
    pub fn _webinix_browser_get_temp_path(
        browser: ::std::os::raw::c_uint,
    ) -> *const ::std::os::raw::c_char;
}
extern "C" {
    pub fn _webinix_folder_exist(folder: *mut ::std::os::raw::c_char) -> bool;
}
extern "C" {
    pub fn _webinix_browser_create_profile_folder(
        win: *mut webinix_window_t,
        browser: ::std::os::raw::c_uint,
    ) -> bool;
}
extern "C" {
    pub fn _webinix_browser_start_edge(
        win: *mut webinix_window_t,
        address: *const ::std::os::raw::c_char,
    ) -> bool;
}
extern "C" {
    pub fn _webinix_browser_start_firefox(
        win: *mut webinix_window_t,
        address: *const ::std::os::raw::c_char,
    ) -> bool;
}
extern "C" {
    pub fn _webinix_browser_start_custom(
        win: *mut webinix_window_t,
        address: *const ::std::os::raw::c_char,
    ) -> bool;
}
extern "C" {
    pub fn _webinix_browser_start_chrome(
        win: *mut webinix_window_t,
        address: *const ::std::os::raw::c_char,
    ) -> bool;
}
extern "C" {
    pub fn _webinix_browser_start(
        win: *mut webinix_window_t,
        address: *const ::std::os::raw::c_char,
        browser: ::std::os::raw::c_uint,
    ) -> bool;
}
extern "C" {
    pub fn _webinix_system_win32(
        cmd: *mut ::std::os::raw::c_char,
        show: bool,
    ) -> ::std::os::raw::c_int;
}

// --[Tools]---------------------------

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

// --[Wrapper]-------------------------

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

pub struct Event {
    pub ElementId:     u32,
	pub WindowId:      u32,
	pub ElementName:   String,
	pub Window:         *mut webinix_window_t,
}

// List of Rust user functions (2-dimensional array)
static mut func_list: [[Option::<fn(e: Event) -> ()>; 64]; 64] = [[None; 64]; 64];

pub fn RunJavaScript(win: *mut webinix_window_t, js: &mut JavaScript) {

    unsafe {

        // Script String to i8/u8
        let script_cpy = String::from(js.script.clone());
        let script_c_str = CString::new(script_cpy).unwrap();
        let script_c_char: *const c_char = script_c_str.as_ptr() as *const c_char;

        let script: webinix_script_interface_t = webinix_script_interface_t {
            timeout: js.timeout,
            script: script_c_char as *mut i8,
            data: script_c_char, // 'data' TODO: Should be null
            error: false,
            length: 0,
        };

        webinix_script_interface_struct(win, &script);

        js.error = script.error;
        js.data = char_to_string(script.data);
    }
}

pub fn NewWindow() -> *mut webinix_window_t {

    unsafe {

        return webinix_new_window();
    }
}

pub fn Wait() {

    unsafe {

        webinix_wait();
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

        return webinix_show_cpy(win, html_c_char, b);
    }
}

fn events_handler (element_id: ::std::os::raw::c_uint, window_id: ::std::os::raw::c_uint, element_name: *mut ::std::os::raw::c_char, window: *mut webinix_window_t) {

    let ElementId: u32 = element_id;
    let WindowId: u32 = window_id;
    let ElementName: String = char_to_string(element_name);
    let Window: *mut webinix_window_t = window;

    let E = Event {
        ElementId: ElementId,
        WindowId: WindowId,
        ElementName: ElementName,
        Window: Window,
    };

    // Call the Rust user function
    let WindowId_64 = WindowId as usize;
    let ElementId_64 = ElementId as usize;
    unsafe {
        (func_list[WindowId_64][ElementId_64]).expect("non-null function pointer")(E);
    }
}

pub fn Bind(win: *mut webinix_window_t, element: &str, func: fn(e: Event)) {

    // Element String to i8/u8
    let element_c_str = CString::new(element).unwrap();
    let element_c_char: *const c_char = element_c_str.as_ptr() as *const c_char;

    // Bind
    unsafe {
        
        let f: Option<unsafe fn(element_id: ::std::os::raw::c_uint, window_id: ::std::os::raw::c_uint, element_name: *mut ::std::os::raw::c_char, window: *mut webinix_window_t)> = Some(events_handler);

        let window_id: ::std::os::raw::c_uint = _webinix_window_get_number(win);
	    let cb_index: ::std::os::raw::c_uint = webinix_bind_interface(win, element_c_char, f);

        let window_id_64 = window_id as usize;
        let cb_index_64 = cb_index as usize;

        // Add the Rust user function to the list
        let user_cb: Option<fn(e: Event)> = Some(func);
        func_list[window_id_64][cb_index_64] = user_cb;
    }
}
