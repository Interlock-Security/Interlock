/*
    Webinix Library 2.0.4
    
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

#define WEBUI_VERSION           "2.0.4"     // Version
#define WEBUI_HEADER_SIGNATURE  0xFF        // All packets should start with this 8bit
#define WEBUI_HEADER_JS         0xFE        // Javascript result in frontend
#define WEBUI_HEADER_CLICK      0xFD        // Click event
#define WEBUI_HEADER_SWITCH     0xFC        // Frontend refresh
#define WEBUI_HEADER_CLOSE      0xFB        // Close window
#define WEBUI_HEADER_CALL_FUNC  0xFA        // Call a backend function
#define WEBUI_MAX_ARRAY         (32)        // Max thread, servers, windows..
#define WEBUI_MIN_PORT          (8080)      // Minimum socket port
#define WEBUI_MAX_PORT          (8335)      // Should be less than 65535
#define WEBUI_MAX_BUF           (512000)    // 512 Kb max dynamic memory allocation
#define WEBUI_DEFAULT_PATH      "."         // Default root path

// -- C STD ---------------------------
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

// -- Windows -------------------------
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    // #include <SDKDDKVer.h> // Fix _WIN32_WINNT warning
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    // #include <iphlpapi.h>
    #include <direct.h>
    #include <io.h>
    #include <tchar.h>
    #define WEBUI_GET_CURRENT_DIR _getcwd
    #define WEBUI_FILE_EXIST _access
    #define WEBUI_POPEN _popen
    #define WEBUI_PCLOSE _pclose
    #define WEBUI_MAX_PATH MAX_PATH
#endif
// -- Linux ---------------------------
#ifdef __linux__
    #include <pthread.h> // POSIX threading
    #include <unistd.h>
    #include <limits.h>
    #include <dirent.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include <poll.h>
    #define WEBUI_GET_CURRENT_DIR getcwd
    #define WEBUI_FILE_EXIST access
    #define WEBUI_POPEN popen
    #define WEBUI_PCLOSE pclose
    #define WEBUI_MAX_PATH PATH_MAX
#endif

// -- Structs -------------------------
struct webinix_event_t;
typedef struct webinix_timer_t {
    struct timespec start;
    struct timespec now;
} webinix_timer_t;
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
    void (*cb_all[1])(struct webinix_event_t* e);
    const char* html;
    const char* html_cpy;
    const char* icon;
    const char* icon_type;
    unsigned int CurrentBrowser;
    char* browser_path;
    char* profile_path;
    unsigned int connections;
    unsigned int runtime;
    bool detect_process_close;
    #ifdef _WIN32
        HANDLE server_thread;
    #else
        pthread_t server_thread;
    #endif
} webinix_window_core_t;
typedef struct webinix_window_t {
    webinix_window_core_t core;
    char* path;
} webinix_window_t;
typedef struct webinix_event_t {
    unsigned int window_id;
    unsigned int element_id;
    char* element_name;
    webinix_window_t* window;
} webinix_event_t;
typedef struct webinix_javascript_result_t {
    bool error;
    unsigned int length;
    const char* data;
} webinix_javascript_result_t;
typedef struct webinix_script_t {
    const char* script;
    unsigned int timeout;
    webinix_javascript_result_t result;
} webinix_script_t;
typedef struct webinix_cb_t {
    webinix_window_t* win;
    char* element_id;
    char* element_name;
} webinix_cb_t;
typedef struct webinix_cmd_async_t {
    webinix_window_t* win;
    char* cmd;
} webinix_cmd_async_t;
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
typedef struct webinix_runtime_t {
    unsigned int none;      // 0
    unsigned int deno;      // 1
    unsigned int nodejs;    // 2
} webinix_runtime_t;
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
    const char* run_responses[WEBUI_MAX_ARRAY];
    bool run_done[WEBUI_MAX_ARRAY];
    bool run_error[WEBUI_MAX_ARRAY];
    unsigned int run_last_id;
    struct mg_mgr* mg_mgrs[WEBUI_MAX_ARRAY];
    struct mg_connection* mg_connections[WEBUI_MAX_ARRAY];
    webinix_browser_t browser;
    webinix_runtime_t runtime;
    bool initialized;
    void (*cb[WEBUI_MAX_ARRAY])(webinix_event_t* e);
    void (*cb_int[WEBUI_MAX_ARRAY])(unsigned int, unsigned int, char*, webinix_window_t*);
    void (*cb_int_all[1])(unsigned int, unsigned int, char*, webinix_window_t*);
    char* executable_path;
    void *ptr_list[WEBUI_MAX_ARRAY];
    unsigned int ptr_position;
    size_t ptr_size[WEBUI_MAX_ARRAY];
} webinix_t;

// -- Definitions ---------------------
EXPORT webinix_t webinix;
EXPORT void webinix_wait();
EXPORT void webinix_exit();
EXPORT bool webinix_is_any_window_running();
EXPORT bool webinix_is_app_running();
EXPORT void webinix_set_timeout(unsigned int second);
EXPORT webinix_window_t* webinix_new_window();
EXPORT bool webinix_show(webinix_window_t* win, const char* html, unsigned int browser);
EXPORT bool webinix_show_cpy(webinix_window_t* win, const char* html, unsigned int browser);
EXPORT bool webinix_refresh(webinix_window_t* win, const char* html);
EXPORT bool webinix_refresh_cpy(webinix_window_t* win, const char* html);
EXPORT void webinix_set_icon(webinix_window_t* win, const char* icon_s, const char* type_s);
EXPORT void webinix_multi_access(webinix_window_t* win, bool status);
EXPORT const char* webinix_new_server(webinix_window_t* win, const char* path);
EXPORT void webinix_close(webinix_window_t* win);
EXPORT bool webinix_is_shown(webinix_window_t* win);
EXPORT void webinix_script(webinix_window_t* win, webinix_script_t* script);
EXPORT unsigned int webinix_bind(webinix_window_t* win, const char* element, void (*func)(webinix_event_t* e));
EXPORT void webinix_bind_all(webinix_window_t* win, void (*func)(webinix_event_t* e));
EXPORT bool webinix_open(webinix_window_t* win, const char* url, unsigned int browser);
EXPORT void webinix_script_cleanup(webinix_script_t* script);
EXPORT void webinix_script_runtime(webinix_window_t* win, unsigned int runtime);

// -- Interface -----------------------
// Used by other languages to create Webinix wrappers
typedef struct webinix_script_interface_t {
    char* script;
    unsigned int timeout;
    bool error;
    unsigned int length;
    const char* data;
} webinix_script_interface_t;
EXPORT unsigned int webinix_bind_interface(webinix_window_t* win, const char* element, void (*func)(unsigned int, unsigned int, char*, webinix_window_t*));
EXPORT void webinix_script_interface(webinix_window_t* win, const char* script, unsigned int timeout, bool* error, unsigned int* length, char* data);
EXPORT void webinix_script_interface_struct(webinix_window_t* win, webinix_script_interface_t* js_int);

// Core
EXPORT void _webinix_init();
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
EXPORT unsigned int _webinix_window_get_number(webinix_window_t* win);
EXPORT void _webinix_window_open(webinix_window_t* win, char* link, unsigned int browser);
EXPORT int _webinix_cmd_sync(char* cmd, bool show);
EXPORT int _webinix_cmd_async(char* cmd, bool show);
EXPORT int _webinix_run_browser(webinix_window_t* win, char* cmd);
EXPORT void _webinix_browser_clean();
EXPORT bool _webinix_browser_exist(webinix_window_t* win, unsigned int browser);
EXPORT const char* _webinix_browser_get_temp_path(unsigned int browser);
EXPORT bool _webinix_folder_exist(char* folder);
EXPORT bool _webinix_browser_create_profile_folder(webinix_window_t* win, unsigned int browser);
EXPORT bool _webinix_browser_start_edge(webinix_window_t* win, const char* address);
EXPORT bool _webinix_browser_start_firefox(webinix_window_t* win, const char* address);
EXPORT bool _webinix_browser_start_custom(webinix_window_t* win, const char* address);
EXPORT bool _webinix_browser_start_chrome(webinix_window_t* win, const char* address);
EXPORT bool _webinix_browser_start(webinix_window_t* win, const char* address, unsigned int browser);
EXPORT long _webinix_timer_diff(struct timespec *start, struct timespec *end);
EXPORT void _webinix_timer_start(webinix_timer_t* t);
EXPORT bool _webinix_timer_is_end(webinix_timer_t* t, unsigned int ms);
EXPORT void _webinix_timer_clock_gettime(struct timespec *spec);
EXPORT bool _webinix_set_root_folder(webinix_window_t* win, const char* path);
EXPORT void _webinix_wait_process(webinix_window_t* win, bool status);
EXPORT const char* _webinix_generate_js_bridge(webinix_window_t* win);
#ifdef _WIN32
    EXPORT DWORD WINAPI _webinix_cb(LPVOID _arg);
    EXPORT DWORD WINAPI _webinix_run_browser_task(LPVOID _arg);
    EXPORT int _webinix_system_win32(char* cmd, bool show);
#else
    EXPORT void* _webinix_cb(void* _arg);
    EXPORT void* _webinix_run_browser_task(void* _arg);
#endif

#endif /* _WEBUI_H */
