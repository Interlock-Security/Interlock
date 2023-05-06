/*
  Webinix Library 2.3.0
  http://webinix.me
  https://github.com/alifcommunity/webinix
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_CORE_H
#define _WEBUI_CORE_H

#include "webinix.h"

#define WEBUI_HEADER_SIGNATURE  0xDD        // All packets should start with this 8bit
#define WEBUI_HEADER_JS         0xFE        // JavaScript result in frontend
#define WEBUI_HEADER_JS_QUICK   0xFD        // JavaScript result in frontend
#define WEBUI_HEADER_CLICK      0xFC        // Click event
#define WEBUI_HEADER_SWITCH     0xFB        // Frontend refresh
#define WEBUI_HEADER_CLOSE      0xFA        // Close window
#define WEBUI_HEADER_CALL_FUNC  0xF9        // Backend function call
#define WEBUI_MAX_ARRAY         (512)       // Max threads, servers, windows, pointers..
#define WEBUI_MIN_PORT          (10000)     // Minimum socket port
#define WEBUI_MAX_PORT          (65500)     // Should be less than 65535
#define WEBUI_MAX_BUF           (1024000)   // 1024 Kb max dynamic memory allocation
#define WEBUI_CMD_STDOUT_BUF    (10240)     // Command STDOUT output buffer size
#define WEBUI_DEFAULT_PATH      "."         // Default root path
#define WEBUI_DEF_TIMEOUT       (30)        // Default startup timeout in seconds
#define WEBUI_MAX_TIMEOUT       (60)        // Maximum startup timeout in seconds the user can set

typedef struct _webinix_timer_t {
    struct timespec start;
    struct timespec now;
} _webinix_timer_t;

typedef struct webinix_event_core_t {
    char* response; // Callback response
} webinix_event_core_t;

typedef struct _webinix_window_t {
    unsigned int window_number;
    bool server_running;
    volatile bool connected;
    bool html_handled;
    bool server_handled;
    bool multi_access;
    bool is_embedded_html;
    unsigned int server_port;
    unsigned int ws_port;
    char* url;
    const char* html;
    const char* icon;
    const char* icon_type;
    unsigned int current_browser;
    char* browser_path;
    char* profile_path;
    unsigned int connections;
    unsigned int runtime;
    bool has_events;
    char* server_root_path;
    bool kiosk_mode;
    webinix_event_core_t* event_core[WEBUI_MAX_ARRAY];
    #ifdef _WIN32
        HANDLE server_thread;
    #else
        pthread_t server_thread;
    #endif
} _webinix_window_t;

typedef struct _webinix_core_t {
    volatile unsigned int servers;
    volatile unsigned int connections;
    char* html_elements[WEBUI_MAX_ARRAY];
    unsigned int used_ports[WEBUI_MAX_ARRAY];
    unsigned int last_window;
    unsigned int startup_timeout;
    volatile bool exit_now;
    const char* run_responses[WEBUI_MAX_ARRAY];
    volatile bool run_done[WEBUI_MAX_ARRAY];
    bool run_error[WEBUI_MAX_ARRAY];
    unsigned char run_last_id;
    bool initialized;
    void (*cb[WEBUI_MAX_ARRAY])(webinix_event_t* e);
    void (*cb_interface[WEBUI_MAX_ARRAY])(size_t, unsigned int, char*, char*, unsigned int);
    char* executable_path;
    void *ptr_list[WEBUI_MAX_ARRAY];
    unsigned int ptr_position;
    size_t ptr_size[WEBUI_MAX_ARRAY];
    unsigned int current_browser;
    struct mg_connection* mg_connections[WEBUI_MAX_ARRAY];
    _webinix_window_t* wins[WEBUI_MAX_ARRAY];
    bool server_handled;
} _webinix_core_t;

typedef struct _webinix_cb_arg_t {
    // Event
    _webinix_window_t* window;
    unsigned int event_type;
    char* element;
    char* data;
    unsigned int event_number;
    // Extras
    char* webinix_internal_id;
} _webinix_cb_arg_t;

typedef struct _webinix_cmd_async_t {
    _webinix_window_t* win;
    char* cmd;
} _webinix_cmd_async_t;

// -- Definitions ---------------------
#ifdef _WIN32
    static const char* webinix_sep = "\\";
    static DWORD WINAPI _webinix_cb(LPVOID _arg);
    static DWORD WINAPI _webinix_run_browser_task(LPVOID _arg);
    static int _webinix_system_win32(char* cmd, bool show);
    static int _webinix_system_win32_out(const char *cmd, char **output, bool show);
    static bool _webinix_socket_test_listen_win32(unsigned int port_num);
    static bool _webinix_get_windows_reg_value(HKEY key, LPCWSTR reg, LPCWSTR value_name, char value[WEBUI_MAX_PATH]);

    #define WEBUI_CB DWORD WINAPI _webinix_cb(LPVOID _arg)
    #define WEBUI_SERVER_START DWORD WINAPI _webinix_server_start(LPVOID arg)
    #define THREAD_RETURN return 0;
#else
    static const char* webinix_sep = "/";
    static void* _webinix_cb(void* _arg);
    static void* _webinix_run_browser_task(void* _arg);

    #define WEBUI_CB void* _webinix_cb(void* _arg)
    #define WEBUI_SERVER_START void* _webinix_server_start(void* arg)
    #define THREAD_RETURN pthread_exit(NULL);
#endif

static void _webinix_init(void);
static bool _webinix_show(_webinix_window_t* window, const char* content, unsigned int browser);
static unsigned int _webinix_get_cb_index(char* webinix_internal_id);
static unsigned int _webinix_set_cb_index(char* webinix_internal_id);
static unsigned int _webinix_get_free_port(void);
static unsigned int _webinix_get_new_window_number(void);
static void _webinix_wait_for_startup(void);
static void _webinix_free_port(unsigned int port);
static char* _webinix_get_current_path(void);
static void _webinix_window_receive(_webinix_window_t* win, const char* packet, size_t len);
static void _webinix_window_send(_webinix_window_t* win, char* packet, size_t packets_size);
static void _webinix_window_event(_webinix_window_t* win, int event_type, char* element, char* data, unsigned int event_number, char* webinix_internal_id);
static unsigned int _webinix_window_get_number(_webinix_window_t* win);
static void _webinix_window_open(_webinix_window_t* win, char* link, unsigned int browser);
static int _webinix_cmd_sync(char* cmd, bool show);
static int _webinix_cmd_async(char* cmd, bool show);
static int _webinix_run_browser(_webinix_window_t* win, char* cmd);
static void _webinix_clean(void);
static bool _webinix_browser_exist(_webinix_window_t* win, unsigned int browser);
static const char* _webinix_browser_get_temp_path(unsigned int browser);
static bool _webinix_folder_exist(char* folder);
static bool _webinix_browser_create_profile_folder(_webinix_window_t* win, unsigned int browser);
static bool _webinix_browser_start_chrome(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_edge(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_epic(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_vivaldi(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_brave(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_firefox(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_yandex(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_chromium(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start(_webinix_window_t* win, const char* address, unsigned int browser);
static long _webinix_timer_diff(struct timespec *start, struct timespec *end);
static void _webinix_timer_start(_webinix_timer_t* t);
static bool _webinix_timer_is_end(_webinix_timer_t* t, unsigned int ms);
static void _webinix_timer_clock_gettime(struct timespec *spec);
static bool _webinix_set_root_folder(_webinix_window_t* win, const char* path);
static const char* _webinix_generate_js_bridge(_webinix_window_t* win);
static void _webinix_print_hex(const char* data, size_t len);
static void _webinix_free_mem(void* ptr);
static bool _webinix_file_exist_mg(struct mg_connection *conn);
static bool _webinix_file_exist(char* file);
static void _webinix_free_all_mem(void);
static bool _webinix_show_window(_webinix_window_t* win, const char* content, bool is_embedded_html, unsigned int browser);
static char* _webinix_generate_internal_id(_webinix_window_t* win, const char* element);
static bool _webinix_is_empty(const char* s);
static size_t _webinix_strlen(const char* s);
static unsigned char _webinix_get_run_id(void);
static void* _webinix_malloc(int size);
static void _webinix_sleep(long unsigned int ms);
static unsigned int _webinix_find_the_best_browser(_webinix_window_t* win);
static bool _webinix_is_process_running(const char* process_name);
static unsigned int _webinix_get_free_event_core_pos(_webinix_window_t* win);
static void _webinix_http_send(struct mg_connection *conn, const char *mime_type, const char* body);
static void _webinix_print_hex(const char* data, size_t len);
static void _webinix_print_ascii(const char* data, size_t len);
static void _webinix_panic(void);

static WEBUI_SERVER_START;
static WEBUI_CB;

#endif /* _WEBUI_CORE_H */
