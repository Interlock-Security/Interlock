/*
    Webinix Library 2.0.0
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

#ifndef _WEBUI_H
#define _WEBUI_H

#if defined(_MSC_VER) || defined(__TINYC__)
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT extern
#endif

#define WEBUI_HEADER_SIGNATURE  0xFF        // All packets should start with this 8bit
#define WEBUI_HEADER_JS         0xFE        // Javascript result in frontend
#define WEBUI_HEADER_CLICK      0xFD        // Click event
#define WEBUI_HEADER_SWITCH     0xFC        // Frontend refresh
#define WEBUI_HEADER_CLOSE      0xFB        // Close window
#define WEBUI_HEADER_CALL_FUNC  0xFA        // Call a backend function
#define WEBUI_MAX_ARRAY         (32)        // Max thread, servers, windows..
#define WEBUI_MIN_PORT          (8080)      // Minimum socket port
#define WEBUI_MAX_PORT          (8335)      // Should be less than 65535
#define WEBUI_MAX_BUF           (512000)    // 512 Kb max dynamic memory
#define WEBUI_DEFAULT_PATH      "."         // Default root path

// -- C STD -----------------------------------
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <time.h>
#include <errno.h>
#if defined(__GNUC__) || defined(__TINYC__)
    #include <dirent.h>
#endif

// -- Windows ---------------------------------
#ifdef _WIN32
    // #include <SDKDDKVer.h> // Fix _WIN32_WINNT warning
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
    #include <direct.h>
    #include <io.h>
    #include <tchar.h>
    #define WEBUI_GET_CURRENT_DIR _getcwd
    #define WEBUI_FILE_EXIST _access
#endif

// -- Linux -----------------------------------
#ifdef __linux__
    #include <pthread.h> // POSIX threading
    #include <unistd.h>
    #define WEBUI_GET_CURRENT_DIR getcwd
    #define WEBUI_FILE_EXIST _access
#endif

// -- macOS -----------------------------------
// ...

typedef struct webinix_event_t {

    unsigned int window_id;
    unsigned int element_id;
    char* element_name;

} webinix_event_t;

typedef struct webinix_window_core_t {

    unsigned int window_number;
    bool server_running;
    bool connected;
    bool server_handled;
    bool multi_access;
    bool server_root;
    unsigned int server_port;
    bool is_bind_all;
    char* url;
    void (*cb_all[1]) (webinix_event_t e);

    #ifdef _WIN32
        HANDLE server_thread;
    #else
        unsigned int server_thread;
    #endif

    // Pointing to external user data
    const char* html;
    const char* icon;
    const char* icon_type;

    unsigned int CurrentBrowser;
    char* browser_path;
    char* profile_path;
    unsigned int connections;

} webinix_window_core_t;

typedef struct webinix_window_t {

    webinix_window_core_t core;
    char* path;

} webinix_window_t;

typedef struct webinix_javascript_result_t {

    bool error;
    unsigned int length;
    const char* data;

} webinix_javascript_result_t;

typedef struct webinix_javascript_t {

    char* script;
    unsigned int timeout;
    webinix_javascript_result_t result;

} webinix_javascript_t;

typedef struct webinix_cb_t {

    webinix_window_t* win;
    char* element_id;
    char* element_name;

} webinix_cb_t;

typedef struct webinix_custom_browser_t {

    char* app;
    char* arg;
    bool auto_link;

} webinix_custom_browser_t;

typedef struct webinix_browser_t {

    unsigned int any;       // 0
    unsigned int chrome;    // 1
    unsigned int firefox;   // 2
    unsigned int edge;      // 3
    unsigned int safari;    // 4
    unsigned int chromium;  // 5
    unsigned int custom;    // 99

} webinix_browser_t;

typedef struct webinix_t {

    unsigned int servers;
    unsigned int connections;
    webinix_custom_browser_t *custom_browser;
    bool wait_for_socket_window;
    char* html_elements[WEBUI_MAX_ARRAY];
    unsigned int used_ports[WEBUI_MAX_ARRAY];
    unsigned int last_window;
    unsigned int startup_timeout;
    bool use_timeout;
    bool timeout_extra;
    bool exit_now;
    char* run_responses[WEBUI_MAX_ARRAY];
    bool run_done[WEBUI_MAX_ARRAY];
    bool run_error[WEBUI_MAX_ARRAY];
    unsigned int run_last_id;
    struct mg_mgr* mg_mgrs[WEBUI_MAX_ARRAY];
    struct mg_connection* mg_connections[WEBUI_MAX_ARRAY];
    webinix_browser_t browser;
    bool initialized;
    void (*cb[WEBUI_MAX_ARRAY]) (webinix_event_t e);
    void (*cb_py[WEBUI_MAX_ARRAY])(unsigned int, unsigned int, char*);

    // Pointers Tracker
    void *ptr_list[WEBUI_MAX_ARRAY];
    size_t ptr_position;
    size_t ptr_size[WEBUI_MAX_ARRAY];

} webinix_t;

typedef struct webinix_javascript_py_t {

    char* script;
    unsigned int timeout;
    bool error;
    unsigned int length;
    const char* data;

} webinix_javascript_py_t;

// -- Definitions --------------------

EXPORT webinix_t webinix;
EXPORT void webinix_loop();
EXPORT void webinix_exit();
EXPORT bool webinix_any_window_is_open();
EXPORT void webinix_set_timeout(unsigned int second);
EXPORT webinix_window_t* webinix_new_window();
EXPORT bool webinix_show(webinix_window_t* win, const char* html, unsigned int browser);
EXPORT void webinix_set_icon(webinix_window_t* win, const char* icon_s, const char* type_s);
EXPORT void webinix_allow_multi_access(webinix_window_t* win, bool status);
EXPORT bool webinix_set_root_folder(webinix_window_t* win, const char* path);
EXPORT const char* webinix_new_server(webinix_window_t* win, const char* html);
EXPORT void webinix_close(webinix_window_t* win);
EXPORT bool webinix_is_show(webinix_window_t* win);
EXPORT void webinix_run_js(webinix_window_t* win, webinix_javascript_t* javascript);
EXPORT unsigned int webinix_bind(webinix_window_t* win, const char* element, void (*func) (webinix_event_t e));
EXPORT void webinix_bind_all(webinix_window_t* win, void (*func) (webinix_event_t e));
EXPORT bool webinix_open(webinix_window_t* win, char* url, unsigned int browser);
EXPORT void webinix_free_js(webinix_javascript_t* javascript);

// Python Interface
EXPORT unsigned int webinix_bind_py(webinix_window_t* win, const char* element, void (*func)(unsigned int, unsigned int, char*));
EXPORT void webinix_run_js_py(webinix_window_t* win, webinix_javascript_py_t* js_py);

// Core
EXPORT void _webinix_ini();
EXPORT unsigned int _webinix_get_cb_index(char* element);
EXPORT unsigned int _webinix_set_cb_index(char* element);
EXPORT unsigned int _webinix_get_free_port();
EXPORT unsigned int _webinix_get_new_window_number();
EXPORT void _webinix_wait_for_startup();
EXPORT void _webinix_free_port(unsigned int port);
EXPORT void _webinix_set_custom_browser(webinix_custom_browser_t* p);
EXPORT char* _webinix_get_current_path();
EXPORT void _webinix_window_receive(webinix_window_t* win, const char* packet, size_t len);
EXPORT void _webinix_window_send(webinix_window_t* win, char* packet, size_t packets_size);
EXPORT void _webinix_window_event(webinix_window_t* win, char* element_id, char* element);
EXPORT unsigned int _webinix_window_get_window_number(webinix_window_t* win);
EXPORT void _webinix_window_open(webinix_window_t* win, char* link, unsigned int browser);
EXPORT int _webinix_cmd_sync(char* cmd);
EXPORT int _webinix_cmd_async(char* cmd);
EXPORT void _webinix_browser_clean();
EXPORT bool _webinix_browser_exist(webinix_window_t* win, unsigned int browser);
EXPORT char* _webinix_browser_get_temp_path(unsigned int browser);
EXPORT bool _webinix_browser_folder_exist(char* folder);
EXPORT bool _webinix_browser_create_profile_folder(webinix_window_t* win, unsigned int browser);
EXPORT bool _webinix_browser_start_edge(webinix_window_t* win, char* address);
EXPORT bool _webinix_browser_start_firefox(webinix_window_t* win, char* address);
EXPORT bool _webinix_browser_start_custom(webinix_window_t* win, char* address);
EXPORT bool _webinix_browser_start_chrome(webinix_window_t* win, char* address);
EXPORT bool _webinix_browser_start(webinix_window_t* win, char* address, unsigned int browser);
#ifdef _WIN32
    EXPORT DWORD WINAPI _webinix_cb(LPVOID _arg);
#else
    EXPORT void _webinix_cb(void* _arg);
#endif

#endif /* _WEBUI_H */
