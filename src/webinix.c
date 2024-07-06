/*
  Webinix Library
  http://webinix.me
  https://github.com/webinix-dev/webinix
  Copyright (c) 2020-2024 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

// 64Mb max dynamic memory allocation
#define WEBUI_MAX_BUF (64000000)

// -- Includes ------------------------
#include "../bridge/webinix_bridge.h" // Webinix Bridge (JavaScript)
#include "webinix.h"                  // Webinix Header

// -- WebView -------------------------
#ifdef _WIN32
    #include "webview/WebView2.h"
#elif __linux__
    #include <dlfcn.h>
#else
    // ...
#endif

// -- Third-party ---------------------
#ifdef WEBUI_TLS
    #ifdef _WIN32
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
    #endif
    // OpenSSL
    #include <openssl/bio.h>
    #include <openssl/evp.h>
    #include <openssl/pem.h>
    #include <openssl/rsa.h>
    #include <openssl/ssl.h>
    #include <openssl/x509.h>
    #ifdef WEBUI_LOG
        #include <openssl/err.h>
    #endif
#endif
#define MG_BUF_LEN (WEBUI_MAX_BUF)
#include "civetweb/civetweb.h"

// -- Disable Non-critical warnings ---
#ifdef _MSC_VER
    #pragma warning(push, 0)
    // Disable spectre warnings
    #pragma warning(disable: 5045)
#elif defined(__clang__)
    // ...
#elif defined(__GNUC__)
    // ...
#endif

// -- Defines -------------------------
#define WEBUI_SIGNATURE      0xDD    // All packets should start with this 8bit
#define WEBUI_CMD_JS         0xFE    // Command: JavaScript result in frontend
#define WEBUI_CMD_JS_QUICK   0xFD    // Command: JavaScript result in frontend
#define WEBUI_CMD_CLICK      0xFC    // Command: Click event
#define WEBUI_CMD_NAVIGATION 0xFB    // Command: Frontend navigation
#define WEBUI_CMD_CLOSE      0xFA    // Command: Close window
#define WEBUI_CMD_CALL_FUNC  0xF9    // Command: Backend function call
#define WEBUI_CMD_SEND_RAW   0xF8    // Command: Send raw binary data to the UI
#define WEBUI_CMD_ADD_ID     0xF7    // Command: Add new bind ID
#define WEBUI_CMD_MULTI      0xF6    // Command: Multi packet data
#define WEBUI_CMD_CHECK_TK   0xF5    // Command: Check validity of a client's token
#define WEBUI_PROTOCOL_SIZE  (8)     // Protocol header size in bytes
#define WEBUI_PROTOCOL_SIGN  (0)     // Protocol byte position: Signature (1 Byte)
#define WEBUI_PROTOCOL_TOKEN (1)     // Protocol byte position: Token (4 Bytes)
#define WEBUI_PROTOCOL_ID    (5)     // Protocol byte position: ID (2 Bytes)
#define WEBUI_PROTOCOL_CMD   (7)     // Protocol byte position: Command (1 Byte)
#define WEBUI_PROTOCOL_DATA  (8)     // Protocol byte position: Data (n Byte)
#define WEBUI_MUTEX_NONE     (0)     // Check boolen mutex without update
#define WEBUI_MUTEX_TRUE     (1)     // Check boolen mutex and update to true
#define WEBUI_MUTEX_FALSE    (2)     // Check boolen mutex and update to false
#define WEBUI_WS_DATA        (1)     // Internal WS Event (Data received)
#define WEBUI_WS_OPEN        (2)     // Internal WS Event (New connection)
#define WEBUI_WS_CLOSE       (3)     // Internal WS Event (Connection close)
#define WEBUI_SHOW_HTML      (1)     // Show window using HTML
#define WEBUI_SHOW_FILE      (2)     // Show window using a local file
#define WEBUI_SHOW_URL       (3)     // Show window using a URL
#define WEBUI_SHOW_FOLDER    (4)     // Show window using a Folder
#define WEBUI_MIN_PORT       (10000) // Minimum socket port
#define WEBUI_MAX_PORT       (65500) // Should be less than 65535
#define WEBUI_STDOUT_BUF     (10240) // Command STDOUT output buffer size
#define WEBUI_DEFAULT_PATH   "."     // Default root path
#define WEBUI_DEF_TIMEOUT    (15)    // Default startup timeout in seconds
#define WEBUI_RELOAD_TIMEOUT (500)   // Default reload page timeout in milliseconds
#define WEBUI_MAX_TIMEOUT    (60)    // Maximum startup timeout in seconds the user can set
#define WEBUI_MIN_WIDTH      (100)   // Minimal window width
#define WEBUI_MIN_HEIGHT     (100)   // Minimal window height
#define WEBUI_MAX_WIDTH      (3840)  // Maximal window width (4K Monitor)
#define WEBUI_MAX_HEIGHT     (2160)  // Maximal window height (4K Monitor)
#define WEBUI_MIN_X          (0)     // Minimal window X
#define WEBUI_MIN_Y          (0)     // Minimal window Y
#define WEBUI_MAX_X          (3000)  // Maximal window X (4K Monitor)
#define WEBUI_MAX_Y          (1800)  // Maximal window Y (4K Monitor)
#define WEBUI_PROFILE_NAME   "Webinix" // Default browser profile name (Used only for Firefox)
#define WEBUI_COOKIES_LEN    (32)    // Authentification cookies len
#define WEBUI_COOKIES_BUF    (64)    // Authentification cookies buffer size

#ifdef WEBUI_TLS
#define WEBUI_SECURE         "TLS-Encryption"
#define WEBUI_SSL_SIZE       (4096)         // SSL Max PEM Size
#define WEBUI_SSL_EXPIRE     (72 * 60 * 60) // SSL Expires (Integer)
#define WEBUI_SSL_EXPIRE_STR "259201"       // SSL Expires (String)
#define WEBUI_HTTP_PROTOCOL  "https://"
#define WEBUI_WS_PROTOCOL    "wss://"
#else
#define WEBUI_SECURE        "Non-Encrypted"
#define WEBUI_HTTP_PROTOCOL "http://"
#define WEBUI_WS_PROTOCOL   "ws://"
#endif

#ifdef _WIN32
#define WEBUI_OS "Microsoft Windows"
#elif __APPLE__
#define WEBUI_OS "Apple macOS"
#else
#define WEBUI_OS "GNU/Linux"
#endif

// Mutex
#ifdef _WIN32
typedef CRITICAL_SECTION webinix_mutex_t;
typedef CONDITION_VARIABLE webinix_condition_t;
#else
typedef pthread_mutex_t webinix_mutex_t;
typedef pthread_cond_t webinix_condition_t;
#endif

// Optimization
#if defined(__clang__) || defined(__GNUC__)
#define WEBUI_DISABLE_OPTIMIZATION_START _Pragma("GCC optimize (\"O0\")")
#define WEBUI_DISABLE_OPTIMIZATION_END
#elif defined(_MSC_VER)
#define WEBUI_DISABLE_OPTIMIZATION_START __pragma(optimize("", off))
#define WEBUI_DISABLE_OPTIMIZATION_END   __pragma(optimize("", on))
#else
#define WEBUI_DISABLE_OPTIMIZATION_START
#define WEBUI_DISABLE_OPTIMIZATION_END
#endif

// Compiler
#if defined(_MSC_VER)
    #define WEBUI_COMPILER "MSVC"
#elif defined(__GNUC__) && !defined(__clang__)
    #define WEBUI_COMPILER "GCC"
#elif defined(__clang__)
    #define WEBUI_COMPILER "Clang"
#else
    #define WEBUI_COMPILER "Unknown"
#endif

// Verbose Log
#ifdef WEBUI_LOG
    // #define WEBUI_LOG_VERBOSE
#endif

// Timer
typedef struct _webinix_timer_t {
    struct timespec start;
    struct timespec now;
} _webinix_timer_t;

// Event data
typedef struct webinix_event_inf_t {
    char* event_data[WEBUI_MAX_ARG + 1]; // Event data (string | num | bool | raw)
    size_t event_size[WEBUI_MAX_ARG + 1]; // Event data size (in bytes)
    char* response; // Event response (string)
    size_t count; // Event arguments count
} webinix_event_inf_t;

// WebView
#ifdef _WIN32
    typedef struct _webinix_wv_win32_t {
        // Win32 WebView
        ICoreWebView2Environment* webviewEnvironment;
        ICoreWebView2Controller* webviewController;
        ICoreWebView2* webviewWindow;
        HWND hwnd;
        void* titleChangedHandler;
        void* titleChangedHandler_lpVtbl;
        void* createWebViewEnvironmentHandler;
        void* createWebViewEnvironmentHandler_lpVtbl;
        void* createWebViewControllerHandler;
        void* createWebViewControllerHandler_lpVtbl;
        // Webinix Window
        wchar_t* url;
        bool navigate;
        bool size;
        bool position;
        int width;
        int height;
        int x;
        int y;
        bool stop;
    } _webinix_wv_win32_t;
#elif __linux__
    void* libgtk;
    void* libwebkit;
    // GTK Symbol Addresses
    typedef void *(*gtk_init_func)(int *argc, char ***argv);
    typedef void (*gtk_widget_show_all_func)(void *);
    typedef void (*gtk_main_iteration_do_func)(int);
    typedef int (*gtk_events_pending_func)(void);
    typedef void (*gtk_container_add_func)(void *, void *);
    typedef void *(*gtk_window_new_func)(int);
    typedef void (*gtk_window_set_default_size_func)(void *, int, int);
    typedef void (*gtk_window_set_title_func)(void *, const char *);
    typedef void (*gtk_window_move_func)(void *, int, int);
    typedef void (*gtk_window_close_func)(void *);
    typedef void (*gtk_window_resize_func)(void *, int, int);
    typedef void (*gtk_window_set_position_func)(void *, int);
    typedef int (*g_idle_add_func)(int (*function)(void*), void*);
    typedef void (*g_signal_connect_data_func)(void *, const char *,
        void (*callback)(void), void *, void *, int);
    gtk_init_func gtk_init = NULL;
    gtk_widget_show_all_func gtk_widget_show_all = NULL;
    gtk_main_iteration_do_func gtk_main_iteration_do = NULL;
    gtk_events_pending_func gtk_events_pending = NULL;
    gtk_container_add_func gtk_container_add = NULL;
    gtk_window_new_func gtk_window_new = NULL;
    gtk_window_set_default_size_func gtk_window_set_default_size = NULL;
    gtk_window_set_title_func gtk_window_set_title = NULL;
    gtk_window_move_func gtk_window_move = NULL;
    gtk_window_close_func gtk_window_close = NULL;
    gtk_window_resize_func gtk_window_resize = NULL;
    gtk_window_set_position_func gtk_window_set_position = NULL;
    g_signal_connect_data_func g_signal_connect_data = NULL;
    g_idle_add_func g_idle_add = NULL;
    // WebKit Symbol Addresses
    typedef void *(*webkit_web_view_new_func)(void);
    typedef void (*webkit_web_view_load_uri_func)(void *, const char *);
    typedef const char *(*webkit_web_view_get_title_func)(void *);
    webkit_web_view_new_func webkit_web_view_new = NULL;
    webkit_web_view_load_uri_func webkit_web_view_load_uri = NULL;
    webkit_web_view_get_title_func webkit_web_view_get_title = NULL;
    
    typedef struct _webinix_wv_linux_t {
        // Linux WebView
        void* gtk_win;
        void* gtk_wv;
        bool open;
        // Webinix Window
        char* url;
        bool navigate;
        bool size;
        bool position;
        unsigned int width;
        unsigned int height;
        unsigned int x;
        unsigned int y;
        bool stop;
    } _webinix_wv_linux_t;
#else
    extern bool _webinix_macos_wv_new(int index);
    extern bool _webinix_macos_wv_show(int index, const char* urlString, int x, int y, int width, int height);
    extern bool _webinix_macos_wv_close(int index);
    extern bool _webinix_macos_wv_set_position(int index, int x, int y);
    extern bool _webinix_macos_wv_set_size(int index, int width, int height);
    extern bool _webinix_macos_wv_navigate(int index, const char* urlString);
    extern void _webinix_macos_wv_process();
    extern void _webinix_macos_wv_stop();
    extern void _webinix_macos_wv_set_close_cb(void (*cb)(int index));
    extern void _webinix_macos_wv_new_thread_safe(int index);

    typedef struct _webinix_wv_macos_t {
        // macOS WebView
        int index;
        // Webinix Window
        char* url;
        bool navigate;
        bool size;
        bool position;
        unsigned int width;
        unsigned int height;
        unsigned int x;
        unsigned int y;
        bool stop;
    } _webinix_wv_macos_t;
#endif

// Window
typedef struct _webinix_window_t {
    // Client
    size_t clients_count;
    // Server
    bool wait; // Let server thread wait more time for websocket
    bool server_running; // Slow check
    bool connected; // Fast check
    size_t server_port;
    char* url;
    const char* html;
    char* server_root_path;
    #ifdef _WIN32
    HANDLE server_thread;
    #else
    pthread_t server_thread;
    #endif
    // Window
    uint32_t token;
    size_t window_number;
    const char* html_elements[WEBUI_MAX_IDS];
    bool has_all_events;
    void(*cb[WEBUI_MAX_IDS])(webinix_event_t* e);
    void(*cb_interface[WEBUI_MAX_IDS])(size_t, size_t, char* , size_t, size_t);
    bool ws_block;
    bool is_embedded_html;
    bool is_closed;
    size_t custom_server_port;
    const char* icon;
    const char* icon_type;
    size_t current_browser;
    char* browser_path;
    bool custom_profile;
    bool default_profile;
    char* profile_path;
    char* profile_name;
    size_t runtime;
    bool kiosk_mode;
    bool disable_browser_high_contrast;
    bool hide;
    int width;
    int height;
    bool size_set;
    int x;
    int y;
    bool position_set;
    size_t process_id;
    const void*(*files_handler)(const char* filename, int* length);
    webinix_event_inf_t* events[WEBUI_MAX_IDS];
    size_t events_count;
    bool is_public;
    bool proxy_set;
    char *proxy_server;
    // WebView
    bool allow_webview;
    bool update_webview;
    #ifdef _WIN32
    _webinix_wv_win32_t* webView;
    #elif __linux__
    _webinix_wv_linux_t* webView;
    #else
    _webinix_wv_macos_t* webView;
    #endif
}
_webinix_window_t;

// Core
typedef struct _webinix_core_t {
    struct {
        bool show_wait_connection;
        bool show_auto_js_inject;
        bool ws_block;
        bool folder_monitor;
        bool multi_client;
        bool use_cookies;
    } config;
    struct mg_connection* clients[WEBUI_MAX_IDS];
    bool clients_token_check[WEBUI_MAX_IDS];
    char* cookies[WEBUI_MAX_IDS];
    bool cookies_single_set[WEBUI_MAX_IDS];
    size_t servers;
    size_t used_ports[WEBUI_MAX_IDS];
    size_t startup_timeout;
    size_t cb_count;
    bool exit_now;
    bool run_done[WEBUI_MAX_IDS]; // 2 Bytes ID
    char* run_userBuffer[WEBUI_MAX_IDS];
    size_t run_userBufferLen[WEBUI_MAX_IDS];
    bool run_error[WEBUI_MAX_IDS];
    uint16_t run_last_id;
    bool initialized;
    char* executable_path;
    void * ptr_list[WEBUI_MAX_IDS * 2];
    size_t ptr_position;
    size_t ptr_size[WEBUI_MAX_IDS * 2];
    size_t current_browser;
    _webinix_window_t* wins[WEBUI_MAX_IDS];
    bool wins_reserved[WEBUI_MAX_IDS];
    size_t last_win_number;
    webinix_mutex_t mutex_server_start;
    webinix_mutex_t mutex_send;
    webinix_mutex_t mutex_receive;
    webinix_mutex_t mutex_wait;
    webinix_mutex_t mutex_bridge;
    webinix_mutex_t mutex_js_run;
    webinix_mutex_t mutex_win_connect;
    webinix_mutex_t mutex_exit_now;
    webinix_mutex_t mutex_webview_stop;
    webinix_mutex_t mutex_http_handler;
    webinix_mutex_t mutex_client;
    webinix_condition_t condition_wait;
    char* default_server_root_path;
    bool ui;
    #ifdef WEBUI_TLS
    uint8_t* root_cert;
    uint8_t* root_key;
    uint8_t* ssl_cert;
    uint8_t* ssl_key;
    #endif
    // WebView
    bool is_browser_main_run;
    bool is_webview;
    #ifdef _WIN32
    char* webview_cacheFolder;
    HMODULE webviewLib;
    #elif __linux__
    bool is_gtk_main_run;
    #else
    bool is_wkwebview_main_run;
    #endif
}
_webinix_core_t;

typedef struct _webinix_cb_arg_t {
    // Event
    _webinix_window_t* window;
    size_t event_type;
    char* element;
    char* data;
    size_t event_number;
}
_webinix_cb_arg_t;

typedef struct _webinix_recv_arg_t {
    _webinix_window_t* win;
    void * ptr;
    size_t len;
    size_t recvNum;
    int event_type;
    struct mg_connection* client;
    size_t connection_id;
}
_webinix_recv_arg_t;

typedef struct _webinix_monitor_arg_t {
    size_t win_num;
    const char *path;
}
_webinix_monitor_arg_t;

typedef struct _webinix_cmd_async_t {
    _webinix_window_t* win;
    char* cmd;
}
_webinix_cmd_async_t;

// -- Definitions ---------------------
#ifdef _WIN32
static const char* webinix_sep = "\\";
static DWORD WINAPI _webinix_run_browser_task(LPVOID _arg);
static int _webinix_system_win32(_webinix_window_t* win, char* cmd, bool show);
static int _webinix_system_win32_out(const char* cmd, char ** output, bool show);
static bool _webinix_socket_test_listen_win32(size_t port_num);
static bool _webinix_get_windows_reg_value(HKEY key, LPCWSTR reg, LPCWSTR value_name, char value[WEBUI_MAX_PATH]);
static bool _webinix_str_to_wide(const char *s, wchar_t **w);
#define WEBUI_THREAD_SERVER_START DWORD WINAPI _webinix_server_thread(LPVOID arg)
#define WEBUI_THREAD_RECEIVE DWORD WINAPI _webinix_ws_process_thread(LPVOID _arg)
#define WEBUI_THREAD_WEBVIEW DWORD WINAPI _webinix_webview_thread(LPVOID arg)
#define WEBUI_THREAD_MONITOR DWORD WINAPI _webinix_folder_monitor_thread(LPVOID arg)
#define WEBUI_THREAD_RETURN return 0;
#else
static const char* webinix_sep = "/";
static void * _webinix_run_browser_task(void * _arg);
#define WEBUI_THREAD_SERVER_START void * _webinix_server_thread(void * arg)
#define WEBUI_THREAD_RECEIVE void * _webinix_ws_process_thread(void * _arg)
#define WEBUI_THREAD_WEBVIEW void * _webinix_webview_thread(void * arg)
#define WEBUI_THREAD_MONITOR void * _webinix_folder_monitor_thread(void * arg)
#define WEBUI_THREAD_RETURN pthread_exit(NULL);
#endif
static void _webinix_init(void);
static bool _webinix_show(_webinix_window_t* win, struct mg_connection* client, const char* content, size_t browser);
static bool _webinix_get_cb_index(_webinix_window_t* win, const char* element, size_t* id);
static size_t _webinix_get_free_port(void);
static void _webinix_free_port(size_t port);
static char* _webinix_get_current_path(void);
static void _webinix_send_client_ws(_webinix_window_t* win, struct mg_connection* client,
    size_t connection_id, char* packet, size_t packets_size);
static void _webinix_window_event(
    _webinix_window_t* win, size_t connection_id, int event_type, char* element, size_t event_number,
    size_t client_id, char* cookies
);
static int _webinix_cmd_sync(_webinix_window_t* win, char* cmd, bool show);
static int _webinix_cmd_async(_webinix_window_t* win, char* cmd, bool show);
static int _webinix_run_browser(_webinix_window_t* win, char* cmd);
static void _webinix_clean(void);
static bool _webinix_browser_exist(_webinix_window_t* win, size_t browser);
static const char* _webinix_get_temp_path();
static bool _webinix_folder_exist(const char* folder);
static void _webinix_delete_folder(char* folder);
static bool _webinix_browser_create_new_profile(_webinix_window_t* win, size_t browser);
static bool _webinix_browser_start_chrome(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_edge(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_epic(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_vivaldi(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_brave(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_firefox(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_yandex(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start_chromium(_webinix_window_t* win, const char* address);
static bool _webinix_browser_start(_webinix_window_t* win, const char* address, size_t _browser);
static long _webinix_timer_diff(struct timespec * start, struct timespec* end);
static void _webinix_timer_start(_webinix_timer_t* t);
static bool _webinix_timer_is_end(_webinix_timer_t* t, size_t ms);
static void _webinix_timer_clock_gettime(struct timespec * spec);
static bool _webinix_set_root_folder(_webinix_window_t* win, const char* path);
static const char* _webinix_generate_js_bridge(_webinix_window_t* win, struct mg_connection* client);
static void _webinix_free_mem(void * ptr);
static size_t _webinix_mb(size_t size);
static bool _webinix_file_exist_mg(_webinix_window_t* win, struct mg_connection* client);
static bool _webinix_file_exist(const char* path);
static void _webinix_free_all_mem(void);
static bool _webinix_show_window(_webinix_window_t* win, struct mg_connection* client,
    const char* content, int type, size_t browser);
static bool _webinix_is_empty(const char* s);
static size_t _webinix_strlen(const char* s);
static uint16_t _webinix_get_run_id(void);
static void * _webinix_malloc(size_t size);
static void _webinix_sleep(long unsigned int ms);
static size_t _webinix_find_the_best_browser(_webinix_window_t* win);
static bool _webinix_is_process_running(const char* process_name);
static void _webinix_panic(char* msg);
static void _webinix_kill_pid(size_t pid);
static _webinix_window_t* _webinix_dereference_win_ptr(void * ptr);
static int _webinix_get_browser_args(_webinix_window_t* win, size_t browser, char* buffer, size_t len);
static void _webinix_mutex_init(webinix_mutex_t* mutex);
static void _webinix_mutex_lock(webinix_mutex_t* mutex);
static void _webinix_mutex_unlock(webinix_mutex_t* mutex);
static void _webinix_mutex_destroy(webinix_mutex_t* mutex);
static bool _webinix_mutex_is_connected(_webinix_window_t* win, int update);
static bool _webinix_mutex_is_exit_now(int update);
static bool _webinix_mutex_is_webview_update(_webinix_window_t* win, int update);
static void _webinix_condition_init(webinix_condition_t* cond);
static void _webinix_condition_wait(webinix_condition_t* cond, webinix_mutex_t* mutex);
static void _webinix_condition_signal(webinix_condition_t* cond);
static void _webinix_condition_destroy(webinix_condition_t* cond);
static void _webinix_http_send(_webinix_window_t* win, struct mg_connection* client,
    const char* mime_type, const char* body, size_t body_len, bool cache);
static void _webinix_http_send_file(_webinix_window_t* win, struct mg_connection* client,
    const char* mime_type, const char* path, bool cache);
static void _webinix_http_send_header(_webinix_window_t* win, struct mg_connection* client,
    const char* mime_type, size_t body_len, bool cache);
static void _webinix_http_send_error(struct mg_connection* client, const char* body, int status);
static int _webinix_http_handler(struct mg_connection* client, void * _win);
static int _webinix_ws_connect_handler(const struct mg_connection* client, void * _win);
static void _webinix_ws_ready_handler(struct mg_connection* client, void * _win);
static int _webinix_ws_data_handler(struct mg_connection* client, int opcode, char* data, size_t datasize, void * _win);
static void _webinix_ws_close_handler(const struct mg_connection* client, void * _win);
static void _webinix_receive(_webinix_window_t* win, struct mg_connection* client, int event_type, void * data, size_t len);
static void _webinix_ws_process(_webinix_window_t* win, struct mg_connection* client, size_t connection_id, 
    void* ptr, size_t len, size_t recvNum, int event_type);
static bool _webinix_connection_save(_webinix_window_t* win, struct mg_connection* client, size_t* connection_id);
static bool _webinix_connection_get_id(_webinix_window_t* win, struct mg_connection* client, size_t* connection_id);
static void _webinix_connection_remove(_webinix_window_t* win, struct mg_connection* client);
static void _webinix_remove_firefox_profile_ini(const char* path, const char* profile_name);
static bool _webinix_is_firefox_ini_profile_exist(const char* path, const char* profile_name);
static void _webinix_send_client(_webinix_window_t* win, struct mg_connection *client, 
    uint16_t id, uint8_t cmd, const char* data, size_t len);
static void _webinix_send_all(_webinix_window_t* win, uint16_t id, uint8_t cmd, const char* data, size_t len);
static uint16_t _webinix_get_id(const char* data);
static uint32_t _webinix_get_token(const char* data);
static uint32_t _webinix_generate_random_uint32();
static const char* _webinix_url_encode(const char* str);
static bool _webinix_open_url_native(const char* url);
static bool _webinix_is_valid_url(const char* url);
static bool _webinix_port_is_used(size_t port_num);
static char* _webinix_str_dup(const char* src);
static void _webinix_bridge_api_handler(webinix_event_t* e);
// static size_t _webinix_hash_djb2(const char* s);
static size_t _webinix_new_event_inf(_webinix_window_t* win, webinix_event_inf_t** event_inf);
static void _webinix_free_event_inf(_webinix_window_t* win, size_t event_num);
static const char* _webinix_get_cookies_full(const struct mg_connection* client);
static void _webinix_get_cookies(const struct mg_connection* client, char* buffer);
static bool _webinix_client_cookies_save(_webinix_window_t* win, const char* cookies, size_t* client_id);
static bool _webinix_client_cookies_get_id(_webinix_window_t* win, const char* cookies, size_t* client_id);
// static void _webinix_client_cookies_free_all(_webinix_window_t* win);
// static void _webinix_client_cookies_free(_webinix_window_t* win, struct mg_connection* client);
static size_t _webinix_client_get_id(_webinix_window_t* win, struct mg_connection* client);
static void _webinix_generate_cookies(char* cookies, size_t length);
static int _webinix_serve_file(_webinix_window_t* win, struct mg_connection* client, size_t client_id);
static int _webinix_interpret_file(_webinix_window_t* win, struct mg_connection* client, char* index, size_t client_id);
// WebView
#ifdef _WIN32
// Microsoft Windows
static void _webinix_wv_free(_webinix_wv_win32_t* webView);
static void _webinix_wv_close(_webinix_wv_win32_t *webView);
static bool _webinix_wv_navigate(_webinix_wv_win32_t* webView, wchar_t* url);
static bool _webinix_wv_set_position(_webinix_wv_win32_t* webView, int x, int y);
static bool _webinix_wv_set_size(_webinix_wv_win32_t* webView, int windowWidth, int windowHeight);
static bool _webinix_wv_show(_webinix_window_t* win, char* url);
static void _webinix_wv_event_closed(_webinix_window_t* win);
#elif __linux__
// Linux
static void _webinix_wv_free();
static void _webinix_wv_close(_webinix_wv_linux_t *webView);
static bool _webinix_wv_navigate(_webinix_wv_linux_t* webView, char* url);
static bool _webinix_wv_set_position(_webinix_wv_linux_t* webView, int x, int y);
static bool _webinix_wv_set_size(_webinix_wv_linux_t* webView, int windowWidth, int windowHeight);
static bool _webinix_wv_show(_webinix_window_t* win, char* url);
static void _webinix_wv_event_closed(void *widget, void *arg);
#else
// macOS
static void _webinix_wv_free(_webinix_wv_macos_t* webView);
static void _webinix_wv_close(_webinix_wv_macos_t *webView);
static bool _webinix_wv_navigate(_webinix_wv_macos_t* webView, char* url);
static bool _webinix_wv_set_position(_webinix_wv_macos_t* webView, int x, int y);
static bool _webinix_wv_set_size(_webinix_wv_macos_t* webView, int windowWidth, int windowHeight);
static bool _webinix_wv_show(_webinix_window_t* win, char* url);
static void _webinix_wv_event_closed(int index);
#endif

#ifdef WEBUI_TLS
static int _webinix_tls_initialization(void * ssl_ctx, void * ptr);
static bool _webinix_tls_generate_self_signed_cert(char* root_cert, char* root_key, char* ssl_cert, char* ssl_key);
static bool _webinix_check_certificate(const char* certificate_pem, const char* private_key_pem);
#endif
#ifdef WEBUI_LOG
static void _webinix_print_hex(const char* data, size_t len);
static void _webinix_print_ascii(const char* data, size_t len);
static int _webinix_http_log(const struct mg_connection* client, const char* message);
#endif
static WEBUI_THREAD_SERVER_START;
static WEBUI_THREAD_RECEIVE;
static WEBUI_THREAD_WEBVIEW;
static WEBUI_THREAD_MONITOR;

// Safe C STD
#ifdef _WIN32
#define WEBUI_STR_TOK(str, delim, context) strtok_s(str, delim, context)
#define WEBUI_FILE_OPEN(file, filename, mode) fopen_s(&file, filename, mode)
#define WEBUI_SN_PRINTF_DYN(buffer, buffer_size, format, ...) snprintf(buffer, _webinix_mb(buffer_size), format, ##__VA_ARGS__)
#define WEBUI_SN_PRINTF_STATIC(buffer, buffer_size, format, ...) snprintf(buffer, buffer_size, format, ##__VA_ARGS__)
#define WEBUI_STR_COPY_DYN(dest, dest_size, src) strcpy_s(dest, _webinix_mb(dest_size), src)
#define WEBUI_STR_COPY_STATIC(dest, dest_size, src) strcpy_s(dest, dest_size, src)
#define WEBUI_STR_CAT_DYN(dest, dest_size, src) strcat_s(dest, _webinix_mb(dest_size), src)
#define WEBUI_STR_CAT_STATIC(dest, dest_size, src) strcat_s(dest, dest_size, src)
#else
#define WEBUI_STR_TOK(str, delim, context) strtok_r(str, delim, context)
#define WEBUI_FILE_OPEN(file, filename, mode) ((file) = fopen(filename, mode))
#define WEBUI_SN_PRINTF_DYN(buffer, buffer_size, format, ...) snprintf(buffer, _webinix_mb(buffer_size), format, ##__VA_ARGS__)
#define WEBUI_SN_PRINTF_STATIC(buffer, buffer_size, format, ...) snprintf(buffer, buffer_size, format, ##__VA_ARGS__)
#define WEBUI_STR_COPY_DYN(dest, dest_size, src) strncpy(dest, src, _webinix_mb(dest_size))
#define WEBUI_STR_COPY_STATIC(dest, dest_size, src) strncpy(dest, src, dest_size)
#define WEBUI_STR_CAT_DYN(dest, dest_size, src) strncat(dest, src, _webinix_mb(dest_size))
#define WEBUI_STR_CAT_STATIC(dest, dest_size, src) strncat(dest, src, dest_size)
#endif

// Assert
#define WEBUI_ASSERT(s) _webinix_panic(s);assert(0 && s);

// -- Heap ----------------------------
static _webinix_core_t _webinix;
static const char* webinix_html_served = "<html><head><title>Access Denied</title><script src=\"/webinix.js\"></script><style>"
"body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-"
"gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style>"
"</head><body><h2>&#9888; Access Denied</h2><p>You can't access this content<br>because it's already in use in<br>another "
"window.<br><br>Note: Multi-client mode is disabled.</p><br><a href=\"https://www.webinix.me\">"
"<small>Webinix v" WEBUI_VERSION "</small></a></body></html>";
static const char* webinix_html_res_not_available = "<html><head><title>Resource Not Available</title><script src=\"/webinix.js\">"
"</script><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-"
"image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}"
"</style></head><body><h2>&#9888; Resource Not Available</h2><p>The requested resource is not available.</p><br><a href=\""
"https://www.webinix.me\"><small>Webinix v" WEBUI_VERSION "</small></a></body></html>";
static const char* webinix_def_icon = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"500\" zoomAndPan=\"magnify\" viewBox=\"0 0 375 374.999991\" height=\"500\" preserveAspectRatio=\"xMidYMid meet\" version=\"1.0\"><defs><clipPath id=\"3530adfd62\"><path d=\"M 22.375 22.558594 L 352.625 22.558594 L 352.625 352.441406 L 22.375 352.441406 Z M 22.375 22.558594 \" clip-rule=\"nonzero\"/></clipPath></defs><g clip-path=\"url(#3530adfd62)\"><path fill=\"#2a6699\" d=\"M 22.375 22.558594 L 352.257812 22.558594 L 352.257812 352.441406 L 22.375 352.441406 Z M 22.375 22.558594 \" fill-opacity=\"1\" fill-rule=\"nonzero\"/></g></svg>"; static const char* webinix_def_icon_type = "image/svg+xml";

// -- Functions -----------------------
void webinix_run_client(webinix_event_t* e, const char* script) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_run_client([%zu])\n", e->window);
    printf("[User] webinix_run_client([%zu]) -> Script: [%s]\n", e->window, script);
    #endif

    // Initialization
    _webinix_init();

    size_t js_len = _webinix_strlen(script);
    if (js_len < 1)
        return;

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
        return;

    // Packet Protocol Format:
    // [...]
    // [CMD]
    // [Script]

    // Send the packet to single client
    _webinix_send_client(win, _webinix.clients[e->connection_id], 0, WEBUI_CMD_JS_QUICK, script, js_len);
}

void webinix_run(size_t window, const char* script) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_run([%zu])\n", window);
    printf("[User] webinix_run([%zu]) -> Script: [%s]\n", window, script);
    #endif

    // Initialization
    _webinix_init();

    size_t js_len = _webinix_strlen(script);
    if (js_len < 1)
        return;

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
        return;

    // Packet Protocol Format:
    // [...]
    // [CMD]
    // [Script]

    // Send the packet to all clients
    _webinix_send_all(win, 0, WEBUI_CMD_JS_QUICK, script, js_len);
}

void webinix_set_file_handler(size_t window, const void*(*handler)(const char* filename, int* length)) {

    if (handler == NULL)
        return;

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    win->files_handler = handler;
}

bool webinix_script_client(webinix_event_t* e, const char* script, size_t timeout,
    char* buffer, size_t buffer_length) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_script_client([%zu])\n", e->window);
    printf("[User] webinix_script_client([%zu]) -> Script [%s] \n", e->window, script);
    printf("[User] webinix_script_client([%zu]) -> Response Buffer @ 0x%p \n", e->window, buffer);
    printf("[User] webinix_script_client([%zu]) -> Response Buffer Size %zu bytes \n", e->window, buffer_length);
    #endif

    // Initializing response buffer
    if (buffer_length > 0) {
        memset(buffer, 0, buffer_length); 
    }

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[e->window];

    if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
        return false;

    size_t js_len = _webinix_strlen(script);

    if (js_len < 1)
        return false;

    // Initializing pipe
    _webinix_mutex_lock(&_webinix.mutex_js_run);
    uint16_t run_id = _webinix_get_run_id();
    _webinix.run_done[run_id] = false;
    _webinix.run_error[run_id] = false;
    _webinix.run_userBuffer[run_id] = buffer;
    _webinix.run_userBufferLen[run_id] = buffer_length;
    _webinix_mutex_unlock(&_webinix.mutex_js_run);

    // Packet Protocol Format:
    // [...]
    // [CMD]
    // [Script]

    // Send the packet
    _webinix_send_client(win, _webinix.clients[e->connection_id], run_id, WEBUI_CMD_JS, script, js_len);

    bool js_status = false;

    // Wait for UI response
    if (timeout < 1 || timeout > 86400) {

        // Wait forever
        for (;;) {
            _webinix_sleep(1);
            _webinix_mutex_lock(&_webinix.mutex_js_run);
            js_status = _webinix.run_done[run_id];
            _webinix_mutex_unlock(&_webinix.mutex_js_run);
            if (js_status)
                break;
        }
    } else {

        // Using timeout
        _webinix_timer_t timer;
        _webinix_timer_start(&timer);
        for (;;) {
            _webinix_sleep(1);
            _webinix_mutex_lock(&_webinix.mutex_js_run);
            js_status = _webinix.run_done[run_id];
            _webinix_mutex_unlock(&_webinix.mutex_js_run);
            if (js_status)
                break;
            if (_webinix_timer_is_end(&timer, (timeout * 1000)))
                break;
        }
    }

    if (js_status) {

        #ifdef WEBUI_LOG
        printf(
            "[User] webinix_script -> Response found. User buffer len: %zu bytes \n",
            _webinix.run_userBufferLen[run_id]
        );
        printf(
            "[User] webinix_script -> Response found. User buffer data: [%s] \n",
            _webinix.run_userBuffer[run_id]
        );
        #endif

        return !_webinix.run_error[run_id];
    } else {

        #ifdef WEBUI_LOG
        printf("[User] webinix_script -> No Response is found. \n");
        #endif
    }

    return false;
}

bool webinix_script(size_t window, const char* script, size_t timeout,
    char* buffer, size_t buffer_length) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_script([%zu])\n", window);
    #endif

    // Stop if multi-client mode is enabled.
    // we can't send and receive from all clients
    if (_webinix.config.multi_client) {
        #ifdef WEBUI_LOG
        printf("[User] webinix_script() -> Multi-client mode is enabled, stop.\n");
        #endif
        // Initializing response buffer
        if (buffer_length > 0) {
            memset(buffer, 0, buffer_length); 
        }
        return false;
    }

    // Event
    webinix_event_t e;
    e.window = window;
    e.connection_id = 0; // Single client

    return webinix_script_client(&e, script, timeout, buffer, buffer_length);
}

WEBUI_DISABLE_OPTIMIZATION_START
static uint32_t _webinix_generate_random_uint32() {
    uint32_t timestamp = (uint32_t) time(NULL);
    // Get the higher 16 bits
    uint32_t high = ((uint32_t) rand()&0xFFFF) + ((timestamp >> 16)&0xFFFF);
    // Get the lower 16 bits
    uint32_t low = ((uint32_t) rand()&0xFFFF) + (timestamp&0xFFFF);
    // Combine
    return (high << 16) | low;
}
WEBUI_DISABLE_OPTIMIZATION_END

size_t webinix_new_window(void) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_new_window()\n");
    #endif

    // Create a new window
    return webinix_new_window_id(webinix_get_new_window_id());
}

size_t webinix_new_window_id(size_t window_number) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_new_window_id([%zu])\n", window_number);
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return 0;

    // Check window ID
    if (window_number < 1 || window_number > WEBUI_MAX_IDS)
        return 0;

    // Destroy the window if already exist
    if (_webinix.wins[window_number] != NULL)
        webinix_destroy(window_number);

    // Create a new window
    _webinix_window_t* win = (_webinix_window_t* ) _webinix_malloc(sizeof(_webinix_window_t));
    _webinix.wins[window_number] = win;

    // Initialisation
    win->ws_block = _webinix.config.ws_block;
    win->window_number = window_number;
    win->browser_path = (char*)_webinix_malloc(WEBUI_MAX_PATH);
    win->server_root_path = (char*)_webinix_malloc(WEBUI_MAX_PATH);
    if (_webinix_is_empty(_webinix.default_server_root_path))
        WEBUI_SN_PRINTF_DYN(win->server_root_path, WEBUI_MAX_PATH, "%s", WEBUI_DEFAULT_PATH);
    else
        WEBUI_SN_PRINTF_DYN(win->server_root_path, WEBUI_MAX_PATH, "%s", _webinix.default_server_root_path);

    // Save window ID
    if (window_number > _webinix.last_win_number)
        _webinix.last_win_number = window_number;

    // Auto bind JavaScript-Bridge Core API Handler
    webinix_bind(window_number, "__webinix_core_api__", _webinix_bridge_api_handler);

    #ifdef WEBUI_LOG
    printf("[User] webinix_new_window_id() -> New window #%zu @ 0x%p\n", window_number, win);
    #endif

    return window_number;
}

size_t webinix_get_new_window_id(void) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_new_window_id()\n");
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return 0;

    for (size_t i = 1; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.wins[i] == NULL && !_webinix.wins_reserved[i]) {
            _webinix.wins_reserved[i] = true;
            if (i > _webinix.last_win_number)
                _webinix.last_win_number = i;
            return i;
        }
    }

    // We should never reach here
    WEBUI_ASSERT("webinix_get_new_window_id() failed");
    return 0;
}

void webinix_set_kiosk(size_t window, bool status) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_kiosk([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    win->kiosk_mode = status;
}

void webinix_set_high_contrast(size_t window, bool status) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_high_contrast([%zu], [%d])\n", window, status);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    win->disable_browser_high_contrast = !status;
}

bool webinix_browser_exist(size_t browser) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_browser_exist([%zu])\n", browser);
    #endif

    return _webinix_browser_exist(NULL, browser);
}

bool webinix_is_high_contrast() {

    #ifdef WEBUI_LOG
    printf("[User] webinix_is_high_contrast()\n");
    #endif

    // Initialization
    _webinix_init();

    bool is_enabled = false;

    #ifdef _WIN32
        char hc[WEBUI_MAX_PATH];
        if (_webinix_get_windows_reg_value(
            HKEY_CURRENT_USER,
            L"Control Panel\\Accessibility\\HighContrast",
            L"Flags",
            hc))
        {
            long flags = strtol(hc, NULL, 10);
            if (flags != LONG_MIN && flags != LONG_MAX) {
                is_enabled = ((flags&0x01) == 1);
            }
        }
    #elif __linux__
        FILE* process_output;
        char buf[128];
        process_output = popen("gsettings get org.gnome.desktop.a11y.interface high-contrast", "r");
        if (process_output != NULL) {
            if (fgets(buf, sizeof(buf), process_output) != NULL) {
                is_enabled = (strstr(buf, "true") != NULL);
            }
            pclose(process_output);
        }
    #else
        FILE* process_output;
        char buf[128];
        process_output = popen("defaults read -g AppleInterfaceStyle", "r");
        if (process_output != NULL) {
            if (fgets(buf, sizeof(buf), process_output) != NULL) {
                is_enabled = (strstr(buf, "Dark") != NULL);
            }
            pclose(process_output);
        }
    #endif

    #ifdef WEBUI_LOG
    printf("[User] webinix_is_high_contrast() -> %d\n", is_enabled);
    #endif

    return is_enabled;
}

void webinix_close_client(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_close_client([%zu])\n", e->window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Remove cookies
    // _webinix_client_cookies_free(win, _webinix.clients[e->connection_id]);

    // Packet Protocol Format:
    // [...]
    // [CMD]
    // Send the packet
    _webinix_send_client(win, _webinix.clients[e->connection_id], 0, WEBUI_CMD_CLOSE, NULL, 0);

    // Forced close
    mg_close_connection(_webinix.clients[e->connection_id]);
}

void webinix_close(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_close([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    // Close
    if (!win->webView) {
        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
            // Packet Protocol Format:
            // [...]
            // [CMD]
            // Send the packet
            _webinix_send_all(win, 0, WEBUI_CMD_CLOSE, NULL, 0);
        }
    }
    else {
        // Stop WebView thread if any
        if (win->webView) {
            win->webView->stop = true;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);
        }
    }
}

void webinix_destroy(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_destroy([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    if (win->server_running) {

        // Freindly close
        webinix_close(window);

        // Wait for server threads to stop
        _webinix_timer_t timer_1;
        _webinix_timer_start(&timer_1);
        for (;;) {
            _webinix_sleep(10);
            if (!win->server_running)
                break;
            if (_webinix_timer_is_end(&timer_1, 2500))
                break;
        }

        if (win->server_running) {

            #ifdef WEBUI_LOG
            printf("[User] webinix_destroy([%zu]) -> Forced close\n", window);
            #endif

            // Forced close
            _webinix_mutex_is_connected(win, WEBUI_MUTEX_FALSE);

            // Wait for server threads to stop
            _webinix_timer_t timer_2;
            _webinix_timer_start(&timer_2);
            for (;;) {
                _webinix_sleep(10);
                if (!win->server_running)
                    break;
                if (_webinix_timer_is_end(&timer_2, 1500))
                    break;
            }
        }
    }

    // Free memory resources
    _webinix_free_mem((void*)win->url);
    _webinix_free_mem((void*)win->html);
    _webinix_free_mem((void*)win->icon);
    _webinix_free_mem((void*)win->icon_type);
    _webinix_free_mem((void*)win->browser_path);
    _webinix_free_mem((void*)win->profile_path);
    _webinix_free_mem((void*)win->profile_name);
    _webinix_free_mem((void*)win->server_root_path);

    // Free events
    for (size_t i = 1; i < WEBUI_MAX_IDS; i++) {
        if (win->events[i] != NULL)
            _webinix_free_mem((void*)win->events[i]);
    }

    // Free window struct
    _webinix_free_mem((void*)_webinix.wins[window]);
    _webinix.wins[window] = NULL;
    _webinix.wins_reserved[window] = false;
}

bool webinix_is_shown(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_is_shown([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[window];

    return _webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE);
}

void webinix_set_icon(size_t window, const char* icon, const char* icon_type) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_icon([%zu], [%s], [%s])\n", window, icon, icon_type);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    // Some wrappers do not guarantee pointers stay valid,
    // so, let's make our copy.

    // Icon
    size_t len = _webinix_strlen(icon);
    const char* icon_cpy = (const char*)_webinix_malloc(len);
    memcpy((char*)icon_cpy, icon, len);

    // Icon Type
    len = _webinix_strlen(icon_type);
    const char* icon_type_cpy = (const char*)_webinix_malloc(len);
    memcpy((char*)icon_type_cpy, icon_type, len);

    // Clean old sets if any
    if (win->icon != NULL)
        _webinix_free_mem((void*)win->icon);
    if (win->icon_type != NULL)
        _webinix_free_mem((void*)win->icon_type);

    win->icon = icon_cpy;
    win->icon_type = icon_type_cpy;
}

void webinix_navigate_client(webinix_event_t* e, const char* url) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_navigate_client([%zu], [%s])\n", e->window, url);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Web-Browser Window
    if (!win->webView) {

        if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
            return;

        // Packet Protocol Format:
        // [...]
        // [CMD]
        // [URL]

        // Send the packet
        _webinix_send_client(win, _webinix.clients[e->connection_id], 0, WEBUI_CMD_NAVIGATION, 
            url, _webinix_strlen(url));
    }
}

void webinix_navigate(size_t window, const char* url) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_navigate([%zu], [%s])\n", window, url);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    // Web-Browser Window
    if (!win->webView) {

        if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
            return;

        // Packet Protocol Format:
        // [...]
        // [CMD]
        // [URL]

        // Send the packet
        _webinix_send_all(win, 0, WEBUI_CMD_NAVIGATION, url, _webinix_strlen(url));
    }
    else {
        // WebView
        _webinix_free_mem((void*) win->webView->url);

        #ifdef _WIN32
        wchar_t* wURL = NULL;
        _webinix_str_to_wide(url, &wURL);
        win->webView->url = wURL;
        #else
        char* url_cp = _webinix_str_dup(url);
        win->webView->url = url_cp;
        #endif

        win->webView->navigate = true;
        _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);
    }
}

void webinix_clean(void) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_clean()\n");
    #endif

    // Initialization
    _webinix_init();

    // Final memory cleaning
    _webinix_clean();
}

void webinix_delete_all_profiles(void) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_delete_all_profiles()\n");
    #endif

    // Initialization
    _webinix_init();

    // Loop trough windows
    for (size_t i = 1; i <= _webinix.last_win_number; i++) {
        if (_webinix.wins[i] != NULL) {
            webinix_delete_profile(i);
        }
    }
}

static bool _webinix_is_firefox_ini_profile_exist(const char* path, const char* profile_name) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_is_firefox_ini_profile_exist([%s], [%s])\n", path, profile_name);
    #endif

    // Parse home environments in the path
    #ifdef _WIN32
    // Windows
    char full_path[WEBUI_MAX_PATH];
    ExpandEnvironmentStringsA(path, full_path, sizeof(full_path));
    #else
    // Linux&macOS
    char full_path[WEBUI_MAX_PATH];
    if (path[0] == '~') {
        const char* home = getenv("HOME");
        if (home) {
            WEBUI_SN_PRINTF_STATIC(full_path, sizeof(full_path), "%s/%s", home,&path[1]);
        } else {
            // If for some reason HOME isn't set
            // fall back to the original path.
            strncpy(full_path, path, sizeof(full_path));
        }
    } else {
        strncpy(full_path, path, sizeof(full_path));
    }
    #endif

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_is_firefox_ini_profile_exist() -> Opening [%s]\n", full_path);
    #endif

    // Open
    FILE* file;
    WEBUI_FILE_OPEN(file, full_path, "r");
    if (!file)
        return false;

    char target[128] = {
        0x00
    };
    WEBUI_SN_PRINTF_STATIC(target, sizeof(target), "Name=%s", profile_name);

    char line[1024];
    while(fgets(line, sizeof(line), file)) {
        if (strstr(line, target) != NULL) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_is_firefox_ini_profile_exist() -> Target found\n");
            #endif
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

static void _webinix_remove_firefox_profile_ini(const char* path, const char* profile_name) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_remove_firefox_profile_ini([%s], [%s])\n", path, profile_name);
    #endif

    // Parse home environments in the path
    #ifdef _WIN32
    // Windows
    char full_path[WEBUI_MAX_PATH];
    ExpandEnvironmentStringsA(path, full_path, sizeof(full_path));
    #else
    // Linux&macOS
    char full_path[WEBUI_MAX_PATH];
    if (path[0] == '~') {
        const char* home = getenv("HOME");
        if (home) {
            WEBUI_SN_PRINTF_STATIC(full_path, sizeof(full_path), "%s/%s", home,&path[1]);
        } else {
            // If for some reason HOME isn't set
            // fall back to the original path.
            strncpy(full_path, path, sizeof(full_path));
        }
    } else {
        strncpy(full_path, path, sizeof(full_path));
    }
    #endif

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_remove_firefox_profile_ini() -> Opening [%s]\n", full_path);
    #endif

    // Open
    FILE * file;
    WEBUI_FILE_OPEN(file, full_path, "r");
    if (!file)
        return;

    char buffer[4096] = {
        0x00
    };
    char output[4096] = {
        0x00
    };

    char target[128] = {
        0x00
    };
    WEBUI_SN_PRINTF_STATIC(target, sizeof(target), "Name=%s", profile_name);

    bool skip = false;
    while(fgets(buffer, sizeof(buffer), file)) {

        if (strncmp(buffer, "[Profile", 8) == 0) {
            if (skip)
                skip = false;
        }

        if (!skip) {

            if (strstr(buffer, target) != NULL) {

                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_remove_firefox_profile_ini() -> Target found\n");
                #endif
                skip = true;
                continue;
            } else
                WEBUI_STR_CAT_DYN(output, sizeof(output), buffer);
        }
    }

    fclose(file);

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_remove_firefox_profile_ini() -> Saving\n");
    #endif

    // Save
    WEBUI_FILE_OPEN(file, full_path, "w");
    if (!file)
        return;
    fputs(output, file);
    fclose(file);
}

void webinix_delete_profile(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_delete_profile([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    if (_webinix_folder_exist(win->profile_path)) {

        if (win->current_browser == Firefox) {

            // Delete Firefox profile

            #ifdef _WIN32
            // Windows
            _webinix_remove_firefox_profile_ini(
                "%APPDATA%\\Mozilla\\Firefox\\profiles.ini", win->profile_name
            );
            _webinix_delete_folder(win->profile_path);
            #elif __linux__
            // Linux
            _webinix_remove_firefox_profile_ini("~/.mozilla/firefox/profiles.ini", win->profile_name);
            _webinix_remove_firefox_profile_ini(
                "~/snap/firefox/common/.mozilla/firefox/profiles.ini", win->profile_name
            );
            _webinix_delete_folder(win->profile_path);
            #else
            // macOS
            _webinix_remove_firefox_profile_ini(
                "~/Library/Application Support/Firefox/profiles.ini", win->profile_name
            );
            _webinix_delete_folder(win->profile_path);
            #endif
        } else {

            // Delete Chromium-based profile
            _webinix_delete_folder(win->profile_path);
        }
    }
}

const char* webinix_start_server(size_t window, const char* content) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_start_server([%zu])\n", window);
    #endif

    if (_webinix_is_empty(content))
        return "";
    
    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return "";
    _webinix_window_t* win = _webinix.wins[window];

    // Check
    if (win->server_running)
        return "";

    // Make `wait()` waits forever
    webinix_set_timeout(0);

    // Start the window without any GUI
    if (webinix_show_browser(window, content, NoBrowser)) {
        return webinix_get_url(window);
    }

    return "";
}

bool webinix_show_client(webinix_event_t* e, const char* content) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_show_client([%zu])\n", e->window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Show the window using WebView or using any browser
    win->allow_webview = true;
    // Single client
    return _webinix_show(win, _webinix.clients[e->connection_id], content, AnyBrowser);
}

bool webinix_show(size_t window, const char* content) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_show([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[window];

    // Show the window using WebView or using any browser
    win->allow_webview = true;
    // All clients
    return _webinix_show(win, NULL, content, AnyBrowser);
}

bool webinix_show_wv(size_t window, const char* content) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_show_wv([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[window];

    // Show the window using WebView only
    win->allow_webview = true;
    return _webinix_show(win, NULL, content, NoBrowser);
}

bool webinix_show_browser(size_t window, const char* content, size_t browser) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_show_browser([%zu], [%zu])\n", window, browser);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[window];

    // Show the window using a specific browser only
    win->allow_webview = (browser == WebView ? true : false);
    return _webinix_show(win, NULL, content, browser);
}

size_t webinix_bind(size_t window, const char* element, void(*func)(webinix_event_t* e)) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_bind([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[window];

    // Search
    size_t cb_index = 0;
    bool exist = _webinix_get_cb_index(win, element, &cb_index);    

    // All events
    if (_webinix_is_empty(element)) {
        win->has_all_events = true;
        size_t index = (exist ? cb_index : _webinix.cb_count++);
        win->html_elements[index] = "";
        win->cb[index] = func;
        #ifdef WEBUI_LOG
        printf("[User] webinix_bind() -> Save bind (all events) at %zu\n", index);
        #endif
        return index;
    }

    // New bind
    const char* element_cpy = (const char*)_webinix_str_dup(element);
    size_t index = (exist ? cb_index : _webinix.cb_count++);
    win->html_elements[index] = element_cpy;
    win->cb[index] = func;
    #ifdef WEBUI_LOG
    printf("[User] webinix_bind() -> Save bind at %zu\n", index);
    #endif

    // Send to all connected clients the new binding ID
    // Packet Protocol Format:

    // [...]
    // [CMD]
    // [NewElement]
    // Send the packet

    _webinix_send_all(
        win, 0, WEBUI_CMD_ADD_ID,
        element, _webinix_strlen(element_cpy)
    );

    return index;
}

size_t webinix_get_best_browser(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_best_browser([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 1; // 1. Default recommended web browser
    _webinix_window_t* win = _webinix.wins[window];
    
    return _webinix_find_the_best_browser(win);
}

const char* webinix_get_string_at(webinix_event_t* e, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_string_at([%zu])\n", index);
    #endif

    // Initialization
    _webinix_init();

    if (index > WEBUI_MAX_ARG)
        return NULL;

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return NULL;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf == NULL)
        return NULL;

    if (event_inf->event_data[index] != NULL) {
        size_t len = _webinix_strlen(event_inf->event_data[index]);
        if (len > 0 && len <= WEBUI_MAX_BUF)
            return (const char*)event_inf->event_data[index];
    }

    return "";
}

size_t webinix_get_count(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_count()\n");
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf == NULL)
        return 0;

    return event_inf->count;
}

long long int webinix_get_int_at(webinix_event_t* e, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_int_at([%zu])\n", index);
    #endif

    // Initialization&Dereference
    // are done by webinix_get_string()

    if (index > WEBUI_MAX_ARG)
        return 0;

    const char* str = webinix_get_string_at(e, index);
    if (str == NULL)
        return 0;

    size_t len = _webinix_strlen(str);
    if (len > 0 && len <= 20) {
        // 64-bit max is -9,223,372,036,854,775,808 (20 character)
        char* endptr;
        return strtoll((const char*)str, &endptr, 10);
    }

    return 0;
}

double webinix_get_float_at(webinix_event_t* e, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_float_at([%zu])\n", index);
    #endif

    // Initialization&Dereference
    // are done by webinix_get_string()

    if (index > WEBUI_MAX_ARG)
        return 0.0;

    const char* str = webinix_get_string_at(e, index);
    if (str == NULL)
        return 0.0;

    size_t len = _webinix_strlen(str);
    if (len > 0 && len <= 20) {
        // 64-bit max is -9,223,372,036,854,775,808 (20 character)
        char* endptr;
        return strtod(str, &endptr);
    }

    return 0.0;
}

bool webinix_get_bool_at(webinix_event_t* e, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_bool_at([%zu])\n", index);
    #endif

    // Initialization&Dereference
    // are done by webinix_get_string()

    if (index > WEBUI_MAX_ARG)
        return false;

    const char* str = webinix_get_string_at(e, index);
    if (str == NULL)
        return false;

    if (str[0] == '0' || str[0] == 'f' || str[0] == 'F') // `0` | `false` | `FALSE`
        return false;

    return true;
}

size_t webinix_get_size_at(webinix_event_t* e, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_size_at([%zu])\n", index);
    #endif

    // Initialization
    _webinix_init();

    if (index > WEBUI_MAX_ARG)
        return 0;

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf == NULL)
        return 0;

    return event_inf->event_size[index];
}

const char* webinix_get_string(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_string()\n");
    #endif

    return webinix_get_string_at(e, 0);
}

long long int webinix_get_int(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_int()\n");
    #endif

    return webinix_get_int_at(e, 0);
}

double webinix_get_float(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_float()\n");
    #endif

    return webinix_get_float_at(e, 0);
}

bool webinix_get_bool(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_bool()\n");
    #endif

    return webinix_get_bool_at(e, 0);
}

size_t webinix_get_size(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_size()\n");
    #endif

    return webinix_get_size_at(e, 0);
}

void webinix_return_int(webinix_event_t* e, long long int n) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_return_int([%lld])\n", n);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf == NULL)
        return;

    // Free
    if (event_inf->response != NULL)
        _webinix_free_mem((void*)event_inf->response);

    // Int to Str
    // 64-bit max is -9,223,372,036,854,775,808 (20 character)
    char* buf = (char*)_webinix_malloc(20);
    WEBUI_SN_PRINTF_DYN(buf, 20, "%lld", n);

    // Set response
    event_inf->response = buf;
}

void webinix_return_float(webinix_event_t* e, double f) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_return_float([%f])\n", f);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf == NULL)
        return;

    // Free
    if (event_inf->response != NULL)
        _webinix_free_mem((void*)event_inf->response);

    // Float to Str
    // 64-bit max is -9,223,372,036,854,775,808 (20 character)
    char* buf = (char*)_webinix_malloc(20);
    WEBUI_SN_PRINTF_DYN(buf, 20, "%lf", f);

    // Set response
    event_inf->response = buf;
}

void webinix_return_string(webinix_event_t* e, const char* s) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_return_string([%s])\n", s);
    #endif

    if (_webinix_is_empty(s))
        return;

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf == NULL)
        return;

    // Free
    if (event_inf->response != NULL)
        _webinix_free_mem((void*)event_inf->response);

    // Copy Str
    size_t len = _webinix_strlen(s);
    char* buf = (char*)_webinix_malloc(len);
    memcpy(buf, s, len);

    // Set response
    event_inf->response = buf;
}

void webinix_return_bool(webinix_event_t* e, bool b) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_return_bool([%d])\n", b);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf == NULL)
        return;

    // Free
    if (event_inf->response != NULL)
        _webinix_free_mem((void*)event_inf->response);

    // Bool to Str
    int len = 1;
    char* buf = (char*)_webinix_malloc(len);
    WEBUI_SN_PRINTF_DYN(buf, len, "%d", b);

    // Set response
    event_inf->response = buf;
}

size_t webinix_get_parent_process_id(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_parent_process_id([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[window];

    return win->process_id;
}

#ifdef WEBUI_TLS
static bool _webinix_check_certificate(const char* certificate_pem, const char* private_key_pem) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_check_certificate()\n");
    #endif

    OpenSSL_add_all_algorithms();

    // SSL Context
    SSL_CTX * ctx;
    SSL_library_init();
    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx)
        return false;

    // Disable security levels. It is The end-user
    // responsability to provide a high encryption
    // level certificate. While Webinix should just
    // use the end-user's certificates.
    SSL_CTX_set_security_level(ctx, 0);

    // Load Certificate
    BIO * bio_cert = BIO_new_mem_buf((void*)certificate_pem, -1);
    X509 * cert = PEM_read_bio_X509(bio_cert, NULL, 0, NULL);
    if (cert == NULL) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_check_certificate() -> PEM_read_bio_X509 failed\n");
        #endif
        BIO_free_all(bio_cert);
        X509_free(cert);
        SSL_CTX_free(ctx);
        return false;
    }

    // Use certificate
    if (SSL_CTX_use_certificate(ctx, cert) <= 0) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_check_certificate() -> SSL_CTX_use_certificate "
            "failed\n");
        #endif
        BIO_free_all(bio_cert);
        X509_free(cert);
        SSL_CTX_free(ctx);
        return false;
    }

    // Load Key
    BIO * bio_key = BIO_new_mem_buf((void*)private_key_pem, -1);
    EVP_PKEY * private_key = PEM_read_bio_PrivateKey(bio_key, NULL, 0, NULL);
    if (private_key == NULL) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_check_certificate() -> PEM_read_bio_PrivateKey "
            "failed\n");
        #endif
        EVP_PKEY_free(private_key);
        BIO_free_all(bio_key);
        BIO_free_all(bio_cert);
        X509_free(cert);
        SSL_CTX_free(ctx);
        EVP_cleanup();
        return false;
    }

    // Use key
    if (SSL_CTX_use_PrivateKey(ctx, private_key) <= 0) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_check_certificate() -> SSL_CTX_use_PrivateKey "
            "failed\n");
        #endif
        EVP_PKEY_free(private_key);
        BIO_free_all(bio_key);
        BIO_free_all(bio_cert);
        X509_free(cert);
        SSL_CTX_free(ctx);
        EVP_cleanup();
        return false;
    }

    // Free
    EVP_PKEY_free(private_key);
    BIO_free_all(bio_key);
    BIO_free_all(bio_cert);
    X509_free(cert);
    SSL_CTX_free(ctx);
    EVP_cleanup();

    return true;
}
#endif

bool webinix_set_tls_certificate(const char* certificate_pem, const char* private_key_pem) {
    (void)certificate_pem;
    (void)private_key_pem;
    #ifdef WEBUI_LOG
    printf("[User] webinix_set_tls_certificate()\n");
    #endif

    // Initialization
    _webinix_init();

    #ifdef WEBUI_TLS
    if (!_webinix_is_empty(certificate_pem) && !_webinix_is_empty(private_key_pem)) {

        // Check size
        size_t certificate_len = _webinix_strlen(certificate_pem);
        size_t private_key_len = _webinix_strlen(private_key_pem);
        if (certificate_len >= WEBUI_SSL_SIZE || private_key_len >= WEBUI_SSL_SIZE)
            return false;

        // Check certificate validity
        if (!_webinix_check_certificate(certificate_pem, private_key_pem)) {
            #ifdef WEBUI_LOG
            unsigned long err = ERR_get_error();
            char err_buf[1024];
            ERR_error_string_n(err, err_buf, sizeof(err_buf));
            printf("[User] webinix_set_tls_certificate() -> Invalid certificate:\n%s\n", err_buf);
            #endif
            return false;
        }

        // Free generated self-signed
        _webinix_free_mem((void*)_webinix.root_cert);
        _webinix_free_mem((void*)_webinix.root_key);
        _webinix_free_mem((void*)_webinix.ssl_cert);
        _webinix_free_mem((void*)_webinix.ssl_key);

        // Set user TLS
        char* ssl_cert = (char*)_webinix_malloc(certificate_len);
        char* ssl_key = (char*)_webinix_malloc(private_key_len);
        WEBUI_SN_PRINTF_DYN(ssl_cert, certificate_len, "%s", certificate_pem);
        WEBUI_SN_PRINTF_DYN(ssl_key, private_key_len, "%s", private_key_pem);
        _webinix.ssl_cert = ssl_cert;
        _webinix.ssl_key = ssl_key;

        #ifdef WEBUI_LOG
        printf("[User] webinix_set_tls_certificate() -> SSL/TLS Certificate:\n");
        printf("%s\n", (const char*)_webinix.ssl_cert);
        printf("%s\n", (const char*)_webinix.ssl_key);
        #endif

        return true;
    }
    #else
        WEBUI_ASSERT("SSL/TLS is not available in this library. "
            "Please use the secure version of Webinix library");
    #endif

    return false;
}

void webinix_set_config(webinix_config option, bool status) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_config([%d], [%d])\n", option, status);
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return;

    switch (option) {
        case show_wait_connection:
            _webinix.config.show_wait_connection = status;
            break;
        case folder_monitor:
            _webinix.config.folder_monitor = status;
            break;
        case multi_client:
            _webinix.config.multi_client = status;
            break;
        case use_cookies:
            _webinix.config.use_cookies = status;
            break;
        case ui_event_blocking:
            _webinix.config.ws_block = status;
            // Update all created windows
            for (size_t i = 1; i <= _webinix.last_win_number; i++) {
                if (_webinix.wins[i] != NULL) {
                    _webinix.wins[i]->ws_block = status;
                }
            }
            break;
        #ifdef WEBUI_LOG
        default:
            printf("[User] webinix_config -> Unknown option [%d]\n", option);
            break;
        #endif
    }
}

void webinix_set_event_blocking(size_t window, bool status) {
    #ifdef WEBUI_LOG
    printf("[User] webinix_set_event_blocking([%zu], [%d])\n", window, status);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    win->ws_block = status;
}

bool webinix_set_port(size_t window, size_t port) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_port([%zu], [%zu])\n", window, port);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[window];

    if (_webinix_port_is_used(port))
        return false;

    win->custom_server_port = port;
    return true;
}

size_t webinix_get_child_process_id(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_child_process_id([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[window];

    #ifdef _WIN32
    DWORD childProcessId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot,&pe32)) {
            do {
                if (pe32.th32ParentProcessID == win->process_id) {
                    childProcessId = pe32.th32ProcessID;
                }
            } while(Process32Next(hSnapshot,&pe32));
        }
        CloseHandle(hSnapshot);
    }
    return childProcessId;
    #elif __linux__
    DIR * dir;
    struct dirent* entry;
    pid_t lastChildPid = (pid_t) win->process_id;
    dir = opendir("/proc");
    if (!dir)
        return win->process_id;
    while((entry = readdir(dir)) != NULL) {
        // Ensure we're looking at a process directory (directories that are just
        // numbers)
        if (entry->d_type == DT_DIR && strspn(entry->d_name, "0123456789") == _webinix_strlen(entry->d_name)) {
            char statFilepath[1024];
            WEBUI_SN_PRINTF_STATIC(statFilepath, sizeof(statFilepath), "/proc/%s/stat", entry->d_name);
            FILE * f;
            WEBUI_FILE_OPEN(f, statFilepath, "r");
            if (f) {
                pid_t pid, ppid;
                char comm[1024];
                char state;
                // Extract data from the stat file;fields are space-delimited
                if (fscanf(f, "%d %s %c %d",&pid, comm,&state,&ppid) == 4) {
                    if ((intmax_t) ppid == (intmax_t) win->process_id) {
                        // Convert directory name (string) to integer PID
                        lastChildPid = atoi(entry->d_name);
                    }
                }
                fclose(f);
            }
        }
    }
    closedir(dir);
    return lastChildPid;
    #else
    pid_t lastChildPid = -1;
    // Get the size required to hold all process information
    int mib[4] = {
        CTL_KERN,
        KERN_PROC,
        KERN_PROC_ALL,
        0
    };
    size_t size;
    if (sysctl(mib, 3, NULL,&size, NULL, 0) < 0) {
        return win->process_id;
    }
    // Allocate memory and get all process information
    struct kinfo_proc * procList = (struct kinfo_proc * ) malloc(size);
    if (!procList) {
        return win->process_id;
    }
    if (sysctl(mib, 3, procList,&size, NULL, 0) < 0) {
        free(procList);
        return win->process_id;
    }
    // Calculate the number of processes from the returned data
    int procCount = size / sizeof(struct kinfo_proc);
    // Search for the last child process
    for (int i = 0; i < procCount; i++) {
        if ((intmax_t) procList[i].kp_eproc.e_ppid == (intmax_t) win->process_id) {
            lastChildPid = procList[i].kp_proc.p_pid;
        }
    }
    free(procList);
    return (size_t) lastChildPid;
    #endif
}

void webinix_set_hide(size_t window, bool status) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_hide(%zu, %d)\n", window, status);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    win->hide = status;
}

void webinix_set_size(size_t window, unsigned int width, unsigned int height) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_size(%zu, %u, %u)\n", window, width, height);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    if (width < WEBUI_MIN_WIDTH || width > WEBUI_MAX_WIDTH || height < WEBUI_MIN_HEIGHT ||
        height > WEBUI_MAX_HEIGHT) {

        win->size_set = false;
        return;
    }

    win->width = width;
    win->height = height;
    win->size_set = true;

    // Resize the current window
    if (!win->webView) {

        // web-browser window
        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
            char script[128];
            WEBUI_SN_PRINTF_STATIC(script, sizeof(script), "window.resizeTo(%u, %u);", width, height);
            webinix_run(window, script);
        }
    }
    else {

        // webView window
        if (win->webView) {
            win->webView->width = width;
            win->webView->height = height;
            win->webView->size = true;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);
        }
    }
}

void webinix_set_position(size_t window, unsigned int x, unsigned int y) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_position(%zu, %u, %u)\n", window, x, y);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    int X = x;
    int Y = y;

    if (X < WEBUI_MIN_X || X > WEBUI_MAX_X || Y < WEBUI_MIN_Y || Y > WEBUI_MAX_Y) {

        win->position_set = false;
        return;
    }

    win->x = X;
    win->y = Y;
    win->position_set = true;

    // Positioning the current window
    if (!win->webView) {

        // web-browser window
        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
            char script[128];
            WEBUI_SN_PRINTF_STATIC(script, sizeof(script), "window.moveTo(%u, %u);", X, Y);
            webinix_run(window, script);
        }
    }
    else {

        // WebView window
        if (win->webView) {
            win->webView->x = X;
            win->webView->y = Y;
            win->webView->position = true;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);
        }
    }
}

void webinix_set_profile(size_t window, const char* name, const char* path) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_profile([%s], [%s])\n", name, path);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    // Some wrappers do not guarantee pointers stay valid,
    // so, let's make our copy.

    char* name_cpy = NULL;
    size_t len = _webinix_strlen(name);
    if (len > 0) {
        name_cpy = (char*)_webinix_malloc(len);
        memcpy((char*)name_cpy, name, len);
    }

    char* path_cpy = NULL;
    len = _webinix_strlen(path);
    if (len > 0) {
        path_cpy = (char*)_webinix_malloc(len);
        memcpy((char*)path_cpy, path, len);
    }

    // Free
    if (win->profile_name != NULL)
        _webinix_free_mem((void*)win->profile_name);
    if (win->profile_path != NULL)
        _webinix_free_mem((void*)win->profile_path);

    // Save
    win->profile_name = name_cpy;
    win->profile_path = path_cpy;
    win->custom_profile = true;

    // Default local machine profile
    if (name_cpy == NULL && path_cpy == NULL)
        win->default_profile = true;
    else
        win->default_profile = false;
}


void webinix_set_proxy(size_t window, const char* proxy_server) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_proxy([%s])\n", proxy_server);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    // Some wrappers do not guarantee pointers stay valid,
    // so, let's make our copy.

    char* proxy_server_cpy = NULL;
    size_t len = _webinix_strlen(proxy_server);
    if (len > 0) {
        proxy_server_cpy = (char*)_webinix_malloc(len);
        memcpy((char*)proxy_server_cpy, proxy_server, len);
    }

    // Free
    if (win->proxy_server != NULL)
        _webinix_free_mem((void*)win->proxy_server);

    // Save
    win->proxy_server = proxy_server_cpy;

    // Set
    if (_webinix_is_empty(win->proxy_server))
        // Disable the proxy because the server
        // address is an empty string
        win->proxy_set = false;
    else
        // Enable proxy
        win->proxy_set = true;
}

void webinix_open_url(const char* url) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_open_url([%s])\n", url);
    #endif

    // Initialization
    _webinix_init();

    _webinix_open_url_native(url);
}

const char* webinix_get_url(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_get_url([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return NULL;
    _webinix_window_t* win = _webinix.wins[window];

    // Check if server is started
    if (_webinix_is_empty(win->url)) {
        // Start server
        webinix_show_browser(window, "<html><head><script src=\"webinix.js\"></script></head></html>", NoBrowser);
    }

    return (const char*) win->url;
}

void webinix_set_public(size_t window, bool status) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_public([%zu])\n", window);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    win->is_public = status;
}

void webinix_send_raw_client(webinix_event_t* e, const char* function, const void* raw, size_t size) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_send_raw_client(%zu bytes)\n", size);
    #endif

    if ((size < 1) || (_webinix_strlen(function) < 1) || (raw == NULL))
        return;

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Generate data
    size_t data_len = _webinix_strlen(function) + 1 + size;
    char* buf = (char*)_webinix_malloc(data_len);

    // Add Function
    size_t p = 0;
    for (size_t i = 0; i < _webinix_strlen(function); i++)
        buf[p++] = function [i];

    // Add Null
    buf[p] = 0x00;
    p++;

    // Add Data
    char* ptr = (char*)raw;
    for (size_t i = 0; i < size; i++) {
        buf[p++] = * ptr;
        ptr++;
    }

    // Packet Protocol Format:
    // [...]
    // [CMD]
    // [Function, Null, RawData]

    // Send the packet
    _webinix_send_client(win, _webinix.clients[e->connection_id], 0, 
        WEBUI_CMD_SEND_RAW, (const char*)buf, data_len);
    _webinix_free_mem((void*)buf);
}

void webinix_send_raw(size_t window, const char* function, const void * raw, size_t size) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_send_raw(%zu bytes)\n", size);
    #endif

    if ((size < 1) || (_webinix_strlen(function) < 1) || (raw == NULL))
        return;

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    // Generate data
    size_t data_len = _webinix_strlen(function) + 1 + size;
    char* buf = (char*)_webinix_malloc(data_len);

    // Add Function
    size_t p = 0;
    for (size_t i = 0; i < _webinix_strlen(function); i++)
        buf[p++] = function [i];

    // Add Null
    buf[p] = 0x00;
    p++;

    // Add Data
    char* ptr = (char*)raw;
    for (size_t i = 0; i < size; i++) {
        buf[p++] = *ptr;
        ptr++;
    }

    // Packet Protocol Format:
    // [...]
    // [CMD]
    // [Function, Null, RawData]

    // Send the packet
    _webinix_send_all(win, 0, WEBUI_CMD_SEND_RAW, (const char*)buf, data_len);
    _webinix_free_mem((void*)buf);
}

char* webinix_encode(const char* str) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_encode()\n");
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return NULL;

    size_t len = _webinix_strlen(str);
    if (len < 1)
        return NULL;

    #ifdef WEBUI_LOG
    printf("[User] webinix_encode() -> Text: [%s]\n", str);
    #endif

    size_t buf_len = (((len + 2) / 3) * 4) + 8;
    char* buf = (char*)_webinix_malloc(buf_len);

    int ret = mg_base64_encode((const unsigned char*)str, len, buf,&buf_len);

    if (ret > (-1)) {

        // Failed
        #ifdef WEBUI_LOG
        printf("[User] webinix_encode() -> Failed (%d).\n", ret);
        #endif
        _webinix_free_mem((void*)buf);
        return NULL;
    }

    #ifdef WEBUI_LOG
    printf("[User] webinix_encode() -> Encoded: [%s]\n", buf);
    #endif

    // Success
    return buf;
}

char* webinix_decode(const char* str) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_decode()\n");
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return NULL;

    size_t len = _webinix_strlen(str);
    if (len < 1)
        return NULL;

    #ifdef WEBUI_LOG
    printf("[User] webinix_decode() -> Encoded: [%s]\n", str);
    #endif

    size_t buf_len = (((len + 2) / 3) * 4) + 8;
    unsigned char* buf = (unsigned char*)_webinix_malloc(buf_len);

    int ret = mg_base64_decode(str, len, buf,&buf_len);

    if (ret > (-1)) {

        // Failed
        #ifdef WEBUI_LOG
        printf("[User] webinix_decode() -> Failed (%d).\n", ret);
        #endif
        _webinix_free_mem((void*)buf);
        return NULL;
    }

    #ifdef WEBUI_LOG
    printf("[User] webinix_decode() -> Decoded: [%s]\n", buf);
    #endif

    // Success
    return (char*)buf;
}

void webinix_free(void * ptr) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_free([0x%p])\n", ptr);
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return;

    _webinix_free_mem(ptr);
}

void * webinix_malloc(size_t size) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_malloc(%zu bytes)\n", size);
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return NULL;

    return _webinix_malloc(size);
}

void webinix_exit(void) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_exit()\n");
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return;

    // Close all windows
    for (size_t i = 1; i <= _webinix.last_win_number; i++) {
        if (_webinix.wins[i] != NULL) {
            if (_webinix_mutex_is_connected(_webinix.wins[i], WEBUI_MUTEX_NONE)) {

                if (!_webinix.wins[i]->webView) {

                    // Web browser

                    // Packet Protocol Format:
                    // [...]
                    // [CMD]

                    // Send the packet
                    _webinix_send_all(
                        _webinix.wins[i], 0, WEBUI_CMD_CLOSE, NULL, 0
                    );
                }
                else {

                    // WebView

                    // Stop WebView thread if any
                    if (_webinix.wins[i]->webView) {
                        _webinix.wins[i]->webView->stop = true;
                        _webinix_mutex_is_webview_update(_webinix.wins[i], WEBUI_MUTEX_TRUE);
                    }        
                }
            }
        }
    }

    // Stop all threads
    _webinix_mutex_is_exit_now(WEBUI_MUTEX_TRUE);

    // Let's give other threads more time to
    // safely exit and finish cleaning up.
    _webinix_sleep(500);

    // Fire the mutex condition for wait()
    _webinix_condition_signal(&_webinix.condition_wait);
    #ifdef __APPLE__
    _webinix_macos_wv_stop();
    #endif
}

void webinix_wait(void) {

    #ifdef WEBUI_LOG
    printf("[Loop] webinix_wait()\n");
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return;

    if (_webinix.startup_timeout > 0) {

        // Check if there is atleast one window (UI)
        // is running. Otherwise the mutex condition
        // signal will never come
        if (!_webinix.ui) {

            #ifdef WEBUI_LOG
            printf("[Loop] webinix_wait() -> No window is found. Stop\n");
            #endif
            return;
        }

        #ifdef WEBUI_LOG
        printf("[Loop] webinix_wait() -> Waiting (Timeout in %zu seconds)\n", 
            _webinix.startup_timeout);
        #endif

        // The mutex conditional signal will
        // be fired when no more UI (servers)
        // is running.
    } else {

        #ifdef WEBUI_LOG
        printf("[Loop] webinix_wait() -> Infinite waiting\n");
        #endif

        // The mutex conditional signal will
        // be fired when `webinix_exit()` is
        // called by the user.
    }

    // Main loop
    #ifdef _WIN32
        if (!_webinix.is_webview) {
            // Windows Web browser main loop

            #ifdef WEBUI_LOG
            printf("[Loop] webinix_wait() -> Windows web browser loop\n");
            #endif

            _webinix.is_browser_main_run = true;
            _webinix_mutex_lock(&_webinix.mutex_wait);
            _webinix_condition_wait(&_webinix.condition_wait,&_webinix.mutex_wait);
            _webinix.is_browser_main_run = false;
        }
        else {
            // Windows WebView main loop

            #ifdef WEBUI_LOG
            printf("[Loop] webinix_wait() -> Windows WebView loop\n");
            #endif

            _webinix_mutex_lock(&_webinix.mutex_wait);
            _webinix_condition_wait(&_webinix.condition_wait,&_webinix.mutex_wait);
        }
    #elif __linux__
        if (!_webinix.is_webview) {
            // Linux Web browser main loop

            #ifdef WEBUI_LOG
            printf("[Loop] webinix_wait() -> Linux web browser loop\n");
            #endif

            _webinix.is_browser_main_run = true;
            _webinix_mutex_lock(&_webinix.mutex_wait);
            _webinix_condition_wait(&_webinix.condition_wait,&_webinix.mutex_wait);
            _webinix.is_browser_main_run = false;
        }
        else {
            // Linux WebView main loop

            #ifdef WEBUI_LOG
            printf("[Loop] webinix_wait() -> Linux WebView loop\n");
            #endif

            _webinix.is_gtk_main_run = true;

            while (true) {

                while (gtk_events_pending()) {
                    gtk_main_iteration_do(0);
                }
                
                if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
                    break;
            }

            _webinix.is_gtk_main_run = false;
        }
    #else
        if (!_webinix.is_webview) {
            // macOS Web browser main loop

            #ifdef WEBUI_LOG
            printf("[Loop] webinix_wait() -> macOS web browser loop\n");
            #endif

            _webinix.is_browser_main_run = true;
            _webinix_mutex_lock(&_webinix.mutex_wait);
            _webinix_condition_wait(&_webinix.condition_wait,&_webinix.mutex_wait);
            _webinix.is_browser_main_run = false;
        }
        else {
            // macOS WebView main loop

            #ifdef WEBUI_LOG
            printf("[Loop] webinix_wait() -> macOS WebView loop\n");
            #endif

            _webinix.is_wkwebview_main_run = true;

            while (true) {

                _webinix_macos_wv_process();

                if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
                    break;
            }

            _webinix.is_wkwebview_main_run = false;
        }
    #endif

    #ifdef WEBUI_LOG
    printf("[Loop] webinix_wait() -> Cleaning\n");
    #endif

    // Clean
    #ifdef _WIN32
        if (!_webinix.is_webview) {
            // Windows Web browser Clean

            // ...
        }
        else {
            // Windows WebView Clean

            PostQuitMessage(0);
            if (_webinix.webview_cacheFolder) {
                _webinix_delete_folder(_webinix.webview_cacheFolder);
                _webinix_free_mem((void*) _webinix.webview_cacheFolder);
                _webinix.webview_cacheFolder = NULL;
            }
            _webinix_sleep(750);
        }
    #elif __linux__
        if (!_webinix.is_webview) {
            // Linux Web browser Clean

            // ...
        }
        else {
            // Linux WebView Clean

            // Process last drawing events if any
            while (gtk_events_pending()) {
                gtk_main_iteration_do(0);
            }
            // Close all windows if any
            for (size_t i = 1; i <= _webinix.last_win_number; i++) {
                if (_webinix.wins[i] != NULL) {
                    if (_webinix.wins[i]->webView) {
                        _webinix.wins[i]->webView->stop = true;
                        _webinix_mutex_is_webview_update(_webinix.wins[i], WEBUI_MUTEX_TRUE);
                    }
                }
            }
            _webinix_wv_free();
            _webinix_sleep(750);
        }
    #else
        if (!_webinix.is_webview) {
            // macOS Web browser Clean

            // ...
        }
        else {
            // macOS WebView Clean

            _webinix_sleep(750);
        }
    #endif

    #ifdef WEBUI_LOG
    printf("[Loop] webinix_wait() -> Main loop exit successfully\n");
    #endif

    _webinix_mutex_unlock(&_webinix.mutex_wait);
}

void webinix_set_timeout(size_t second) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_timeout([%zu])\n", second);
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return;

    if (second > WEBUI_MAX_TIMEOUT)
        second = WEBUI_MAX_TIMEOUT;

    _webinix.startup_timeout = second;
}

void webinix_set_runtime(size_t window, size_t runtime) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_runtime([%zu], [%zu])\n", window, runtime);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    if (runtime != Deno && runtime != NodeJS)
        win->runtime = None;
    else
        win->runtime = runtime;
}

bool webinix_set_root_folder(size_t window, const char* path) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_root_folder([%zu], [%s])\n", window, path);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[window];

    if (win->server_running || _webinix_is_empty(path) || (_webinix_strlen(path) > WEBUI_MAX_PATH) ||
        !_webinix_folder_exist((char*)path)) {

        WEBUI_SN_PRINTF_DYN(win->server_root_path, WEBUI_MAX_PATH, "%s", WEBUI_DEFAULT_PATH);
        #ifdef WEBUI_LOG
        printf("[User] webinix_set_root_folder() -> failed\n");
        #endif
        return false;
    }

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_root_folder() -> Success\n");
    #endif
    WEBUI_SN_PRINTF_DYN(win->server_root_path, WEBUI_MAX_PATH, "%s", path);
    return true;
}

bool webinix_set_default_root_folder(const char* path) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_default_root_folder([%s])\n", path);
    #endif

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return false;

    if (_webinix_is_empty(path) || (_webinix_strlen(path) > WEBUI_MAX_PATH) || !_webinix_folder_exist((char*)path)) {

        _webinix.default_server_root_path[0] = '\0';
        #ifdef WEBUI_LOG
        printf("[User] webinix_set_default_root_folder() -> failed\n");
        #endif
        return false;
    }

    #ifdef WEBUI_LOG
    printf("[User] webinix_set_default_root_folder() -> Success\n");
    #endif
    WEBUI_SN_PRINTF_DYN(_webinix.default_server_root_path, WEBUI_MAX_PATH, "%s", path);

    // Update all windows. This will works only
    // for non-running windows.
    for (size_t i = 1; i <= _webinix.last_win_number; i++) {
        if (_webinix.wins[i] != NULL) {
            WEBUI_SN_PRINTF_DYN(_webinix.wins[i]->server_root_path, WEBUI_MAX_PATH, 
                "%s", _webinix.default_server_root_path);
        }
    }

    return true;
}

// -- Interface's Functions ----------------
static void _webinix_interface_bind_handler(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_interface_bind_handler()\n");
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[e->window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[e->window];

    // Check for all events-bind functions
    if (win->has_all_events) {
        size_t events_cb_index = 0;
        bool exist = _webinix_get_cb_index(win, "", &events_cb_index); 
        if (exist && win->cb_interface[events_cb_index] != NULL) {
            // Call user all-events cb
            #ifdef WEBUI_LOG
            printf(
                "[Core]\t\t_webinix_interface_bind_handler() -> User all-events callback @ 0x%p\n",
                win->cb_interface[events_cb_index]
            );
            printf("[Core]\t\t_webinix_interface_bind_handler() -> User all-events e->event_type [%zu]\n", e->event_type);
            printf("[Core]\t\t_webinix_interface_bind_handler() -> User all-events e->element [%s]\n", e->element);
            printf("[Core]\t\t_webinix_interface_bind_handler() -> User all-events e->event_number %zu\n", e->event_number);
            printf("[Core]\t\t_webinix_interface_bind_handler() -> User all-events e->bind_id %zu\n", e->bind_id);
            #endif
            // Call all-events cb
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_interface_bind_handler() -> Calling user "
                "all-events callback\n[Call]\n");
            #endif
            win->cb_interface[events_cb_index](e->window, e->event_type, e->element, e->event_number, e->bind_id);
        }
    }

    // Check for the regular bind functions
    if (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) && !_webinix_is_empty(e->element)) {
        size_t cb_index = 0;
        bool exist = _webinix_get_cb_index(win, e->element, &cb_index);
        if (exist && win->cb_interface[cb_index] != NULL) {
            #ifdef WEBUI_LOG
            printf(
                "[Core]\t\t_webinix_interface_bind_handler() -> User callback @ 0x%p\n",
                win->cb_interface[cb_index]
            );
            printf("[Core]\t\t_webinix_interface_bind_handler() -> e->event_type [%zu]\n", e->event_type);
            printf("[Core]\t\t_webinix_interface_bind_handler() -> e->element [%s]\n", e->element);
            printf("[Core]\t\t_webinix_interface_bind_handler() -> e->event_number %zu\n", e->event_number);
            printf("[Core]\t\t_webinix_interface_bind_handler() -> e->bind_id %zu\n", e->bind_id);
            #endif
            // Call cb
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_interface_bind_handler() -> Calling user "
                "callback\n[Call]\n");
            #endif
            win->cb_interface[cb_index](e->window, e->event_type, e->element, e->event_number, e->bind_id);
        }
    }

    #ifdef WEBUI_LOG
    // Print cb response
    char* response = NULL;
    // Get event inf
    webinix_event_inf_t* event_inf = win->events[e->event_number];
    if (event_inf != NULL) {
        if (event_inf->response != NULL)
            response = event_inf->response;
    }
    printf(
        "[Core]\t\t_webinix_interface_bind_handler() -> user-callback response [%s]\n",
        response
    );
    #endif
}

const char* webinix_interface_get_string_at(size_t window, size_t event_number, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_get_string_at([%zu], [%zu], [%zu])\n", window, event_number, index);
    #endif

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return NULL;
    _webinix_window_t* win = _webinix.wins[window];

    // New Event (Wrapper)
    webinix_event_t e;
    e.window = window;
    e.event_number = event_number;

    return webinix_get_string_at(&e, index);
}

long long int webinix_interface_get_int_at(size_t window, size_t event_number, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_get_int_at([%zu], [%zu], [%zu])\n", window, event_number, index);
    #endif

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[window];

    // New Event (Wrapper)
    webinix_event_t e;
    e.window = window;
    e.event_number = event_number;

    return webinix_get_int_at(&e, index);
}

double webinix_interface_get_float_at(size_t window, size_t event_number, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_get_float_at([%zu], [%zu], [%zu])\n", window, event_number, index);
    #endif

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return ((double)(0.0));
    _webinix_window_t* win = _webinix.wins[window];

    // New Event (Wrapper)
    webinix_event_t e;
    e.window = window;
    e.event_number = event_number;

    return webinix_get_float_at(&e, index);
}

bool webinix_interface_get_bool_at(size_t window, size_t event_number, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_get_bool_at([%zu], [%zu], [%zu])\n", window, event_number, index);
    #endif

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return false;
    _webinix_window_t* win = _webinix.wins[window];

    // New Event (Wrapper)
    webinix_event_t e;
    e.window = window;
    e.event_number = event_number;

    return webinix_get_bool_at(&e, index);
}

size_t webinix_interface_get_size_at(size_t window, size_t event_number, size_t index) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_get_size_at([%zu], [%zu], [%zu])\n", window, event_number, index);
    #endif

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[window];

    // New Event (Wrapper)
    webinix_event_t e;
    e.window = window;
    e.event_number = event_number;

    return webinix_get_size_at(&e, index);
}

size_t webinix_interface_bind(size_t window, const char* element, void(*func)(size_t, size_t, char* , size_t, size_t)) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_bind([%zu], [%s], [0x%p])\n", window, element, func);
    #endif

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[window];

    // Bind
    size_t cb_index = webinix_bind(window, element, _webinix_interface_bind_handler);
    win->cb_interface[cb_index] = func;
    return cb_index;
}

void webinix_interface_set_response(size_t window, size_t event_number, const char* response) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_set_response()\n");
    printf("[User] webinix_interface_set_response() -> event_number %zu \n", event_number);
    printf("[User] webinix_interface_set_response() -> Response [%s] \n", response);
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return;
    _webinix_window_t* win = _webinix.wins[window];

    // Get event inf
    webinix_event_inf_t* event_inf = win->events[event_number];
    if (event_inf == NULL)
        return;

    // Free
    if (event_inf->response != NULL)
        _webinix_free_mem((void*)event_inf->response);

    // Set the response
    size_t len = _webinix_strlen(response);
    event_inf->response = (char*)_webinix_malloc(len);
    WEBUI_STR_COPY_DYN(event_inf->response, len, response);

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_set_response() -> Internal buffer [%s] \n", event_inf->response);
    #endif
}

bool webinix_interface_is_app_running(void) {

    #ifdef WEBUI_LOG
    // printf("[User] webinix_is_app_running()\n");
    #endif

    // Stop if already flagged
    static bool app_is_running = true;
    if (!app_is_running)
        return false;

    // Initialization
    _webinix_init();
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return false;

    // Get app status
    if (_webinix.startup_timeout > 0) {
        if (_webinix.servers < 1)
            app_is_running = false;
    }

    #ifdef WEBUI_LOG
    if (!app_is_running)
        printf("[User] webinix_is_app_running() -> App Stopped\n");
    #endif

    return app_is_running;
}

size_t webinix_interface_get_window_id(size_t window) {

    #ifdef WEBUI_LOG
    printf("[User] webinix_interface_get_window_id()\n");
    #endif

    // Initialization
    _webinix_init();

    // Dereference
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || _webinix.wins[window] == NULL)
        return 0;
    _webinix_window_t* win = _webinix.wins[window];

    return win->window_number;
}

// -- Core's Functions ----------------
static bool _webinix_ptr_exist(void * ptr) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_ptr_exist()\n");
    #endif

    if (ptr == NULL)
        return false;

    for (size_t i = 0; i < _webinix.ptr_position; i++) {

        if (_webinix.ptr_list[i] == ptr)
            return true;
    }

    return false;
}

static void _webinix_ptr_add(void * ptr, size_t size) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_ptr_add(0x%p)\n", ptr);
    #endif

    if (ptr == NULL)
        return;

    if (!_webinix_ptr_exist(ptr)) {

        for (size_t i = 0; i < _webinix.ptr_position; i++) {

            if (_webinix.ptr_list[i] == NULL) {

                #ifdef WEBUI_LOG_VERBOSE
                printf("[Core]\t\t_webinix_ptr_add(0x%p) -> Allocate %zu bytes\n", ptr, size);
                #endif

                _webinix.ptr_list[i] = ptr;
                _webinix.ptr_size[i] = size;
                return;
            }
        }

        #ifdef WEBUI_LOG_VERBOSE
        printf("[Core]\t\t_webinix_ptr_add(0x%p) -> Allocate %zu bytes\n", ptr, size);
        #endif

        _webinix.ptr_list[_webinix.ptr_position] = ptr;
        _webinix.ptr_size[_webinix.ptr_position] = size;
        _webinix.ptr_position++;
        if (_webinix.ptr_position >= WEBUI_MAX_IDS)
            _webinix.ptr_position = (WEBUI_MAX_IDS - 1);
    }
}

static void _webinix_free_mem(void * ptr) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_free_mem(0x%p)\n", ptr);
    #endif

    if (ptr == NULL)
        return;

    for (size_t i = 0; i < _webinix.ptr_position; i++) {

        if (_webinix.ptr_list[i] == ptr) {

            #ifdef WEBUI_LOG_VERBOSE
            printf("[Core]\t\t_webinix_free_mem(0x%p) -> Free %zu bytes\n", ptr, _webinix.ptr_size[i]);
            #endif

            free(ptr);

            _webinix.ptr_size[i] = 0;
            _webinix.ptr_list[i] = NULL;
        }
    }

    for (int i = _webinix.ptr_position; i >= 0;i--) {

        if (_webinix.ptr_list[i] == NULL) {

            _webinix.ptr_position = i;
            break;
        }
    }
}

static void _webinix_free_all_mem(void) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_free_all_mem()\n");
    #endif

    // Makes sure we run this once
    static bool freed = false;
    if (freed)
        return;
    freed = true;

    void * ptr = NULL;
    for (size_t i = 0; i < _webinix.ptr_position; i++) {

        ptr = _webinix.ptr_list[i];

        if (ptr != NULL) {

            #ifdef WEBUI_LOG
            printf(
                "[Core]\t\t_webinix_free_all_mem() -> Free %zu bytes @ 0x%p\n", _webinix.ptr_size[i], ptr
            );
            #endif

            free(ptr);
        }
    }
}

static void _webinix_panic(char* msg) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_panic() -> %s.\n", msg);
    #endif

    fprintf(stderr, "Webinix Error: %s.\n", msg);
    webinix_exit();
}

static size_t _webinix_mb(size_t size) {

    // Round `size` to fit a memory block
    // 07 -> 8
    // 14 -> 16
    // 29 -> 32
    // 55 -> 64
    // ...

    // Make sure we have the null
    // terminator if it's a string
    size++;

    // If size is negative
    if (size < 4)
        size = 4;

    // If size is already a full block
    // we should return the same block
    size--;

    size_t block_size = 4;
    while(block_size <= size)
        block_size *= 2;

    return block_size;
}

static void * _webinix_malloc(size_t size) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_malloc([%zu])\n", size);
    #endif

    size = _webinix_mb(size);

    void * block = NULL;
    for (size_t i = 0; i < 8; i++) {

        if (size > WEBUI_MAX_BUF)
            size = WEBUI_MAX_BUF;

        block = malloc(size);

        if (block == NULL)
            size++;
        else
            break;
    }

    if (block == NULL) {

        WEBUI_ASSERT("malloc() failed");
        return NULL;
    }

    memset(block, 0, size);

    _webinix_ptr_add((void*)block, size);

    return block;
}

static _webinix_window_t* _webinix_dereference_win_ptr(void * ptr) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_dereference_win_ptr()\n");
    #endif

    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
        return NULL;

    _webinix_window_t* win = (_webinix_window_t* ) ptr;

    for (size_t i = 1; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.wins[i] == win)
            return win;
    }

    // This pointer is not a valid
    // webinix window struct
    return NULL;
}

static void _webinix_sleep(long unsigned int ms) {

    #ifdef WEBUI_LOG_VERBOSE
    // printf("[Core]\t\t_webinix_sleep([%zu])\n", ms);
    #endif

    #ifdef _WIN32
    Sleep(ms);
    #else
    struct timespec req;
    req.tv_sec = ms / 1000; // Convert ms to seconds
    req.tv_nsec = (ms % 1000) * 1000000L; // Convert remainder to nanoseconds
    nanosleep(&req, NULL);
    #endif
}

static long _webinix_timer_diff(struct timespec * start, struct timespec* end) {

    #ifdef WEBUI_LOG_VERBOSE
    // printf("[Core]\t\t_webinix_timer_diff()\n");
    #endif

    return ((long)(end->tv_sec * 1000) + (long)(end->tv_nsec / 1000000)) -
        ((long)(start->tv_sec * 1000) + (long)(start->tv_nsec / 1000000));
}

static void _webinix_timer_clock_gettime(struct timespec * spec) {

    #ifdef WEBUI_LOG_VERBOSE
    // printf("[Core]\t\t_webinix_timer_clock_gettime()\n");
    #endif

    #ifdef _WIN32
    __int64 wintime;
    GetSystemTimeAsFileTime((FILETIME * )&wintime);
    wintime -= ((__int64) 116444736000000000);
    spec->tv_sec = wintime / ((__int64) 10000000);
    spec->tv_nsec = wintime % ((__int64) 10000000) * 100;
    #else
    clock_gettime(CLOCK_MONOTONIC, spec);
    #endif
}

static void _webinix_timer_start(_webinix_timer_t* t) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_timer_start()\n");
    #endif

    _webinix_timer_clock_gettime(&t->start);
}

static bool _webinix_timer_is_end(_webinix_timer_t* t, size_t ms) {

    #ifdef WEBUI_LOG_VERBOSE
    // printf("[Core]\t\t_webinix_timer_is_end()\n");
    #endif

    _webinix_timer_clock_gettime(&t->now);

    size_t def = (size_t) _webinix_timer_diff(&t->start,&t->now);
    if (def > ms)
        return true;
    return false;
}

static bool _webinix_is_empty(const char* s) {

    #ifdef WEBUI_LOG_VERBOSE
    //printf("[Core]\t\t_webinix_is_empty()\n");
    #endif

    if ((s != NULL) && (s[0] != '\0'))
        return false;
    return true;
}

static size_t _webinix_strlen(const char* s) {

    #ifdef WEBUI_LOG_VERBOSE
    //printf("[Core]\t\t_webinix_strlen()\n");
    #endif

    if (_webinix_is_empty(s))
        return 0;

    size_t length = 0;

    while((s[length] != '\0') && (length < WEBUI_MAX_BUF)) {
        length++;
    }

    return length;
}

static bool _webinix_file_exist_mg(_webinix_window_t* win, struct mg_connection* client) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_file_exist_mg()\n");
    #endif

    char* file;
    char* full_path;

    const struct mg_request_info * ri = mg_get_request_info(client);
    const char* url = ri->local_uri;
    size_t url_len = _webinix_strlen(url);

    // Get file name
    file = (char*)_webinix_malloc(url_len);
    const char* p = url;
    p++; // Skip "/"
    WEBUI_SN_PRINTF_DYN(file, url_len, "%.*s", (int)(url_len - 1), p);

    // Get full path
    // [current folder][/][file]
    size_t bf_len = (_webinix_strlen(win->server_root_path) + 1 + _webinix_strlen(file));
    full_path = (char*)_webinix_malloc(bf_len);
    WEBUI_SN_PRINTF_DYN(full_path, bf_len, "%s%s%s", win->server_root_path, webinix_sep, file);

    bool exist = _webinix_file_exist(full_path);

    _webinix_free_mem((void*)file);
    _webinix_free_mem((void*)full_path);

    return exist;
}

static bool _webinix_is_valid_url(const char* url) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_is_valid_url([%.8s...])\n", url);
    #endif

    if ((_webinix_is_empty(url)) || (url[0] != 'h'))
        return false;
    if (strncmp(url, "http://", 7) == 0 || strncmp(url, "https://", 8) == 0)
        return true;
    return false;
}

static bool _webinix_open_url_native(const char* url) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_open_url_native([%s])\n", url);
    #endif

    #if defined(_WIN32)
    HINSTANCE result = ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    return ((INT_PTR) result > 32);
    #elif defined(__APPLE__)
    char command[1024];
    WEBUI_SN_PRINTF_STATIC(command, sizeof(command), "open \"%s\"", url);
    return (system(command) == 0);
    #else
    // Assuming Linux
    char command[1024];
    WEBUI_SN_PRINTF_STATIC(command, sizeof(command), "xdg-open \"%s\"", url);
    return (system(command) == 0);
    #endif
}

static bool _webinix_file_exist(const char* path) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_file_exist([%s])\n", path);
    #endif

    if (_webinix_is_empty(path))
        return false;

    // Parse home environments in the path
    #ifdef _WIN32
    // Windows
    char full_path[WEBUI_MAX_PATH];
    ExpandEnvironmentStringsA(path, full_path, sizeof(full_path));
    #else
    // Linux&macOS
    char full_path[WEBUI_MAX_PATH];
    if (path[0] == '~') {
        const char* home = getenv("HOME");
        if (home) {
            WEBUI_SN_PRINTF_STATIC(full_path, sizeof(full_path), "%s/%s", home,&path[1]);
        } else {
            // If for some reason HOME isn't set
            // fall back to the original path.
            strncpy(full_path, path, sizeof(full_path));
        }
    } else {
        strncpy(full_path, path, sizeof(full_path));
    }
    #endif

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_file_exist() -> Parsed to [%s]\n", full_path);
    #endif

    #if defined(_WIN32)
    // Convert UTF-8 to wide string on Windows
    wchar_t* wfilePath;
    if (!_webinix_str_to_wide(full_path, &wfilePath))
        return false;
    DWORD dwAttrib = GetFileAttributesW(wfilePath);
    _webinix_free_mem((void*)wfilePath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib&FILE_ATTRIBUTE_DIRECTORY));
    #else
    // Linux / macOS
    return (WEBUI_FILE_EXIST(full_path, 0) == 0);
    #endif
}

static const char* _webinix_get_extension(const char* f) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_extension()\n");
    #endif

    if (f == NULL)
        return "";

    const char* ext = strrchr(f, '.');

    if (ext == NULL || !ext || ext == f)
        return "";
    return ext + 1;
}

static uint16_t _webinix_get_run_id(void) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_run_id()\n");
    #endif

    if (_webinix.run_last_id >= WEBUI_MAX_IDS)
        _webinix.run_last_id = 0;
    return _webinix.run_last_id++;
}

static bool _webinix_socket_test_listen_mg(size_t port_num) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_socket_test_listen_mg([%zu])\n", port_num);
    #endif

    // HTTP Port Test
    char* test_port = (char*)_webinix_malloc(64);
    WEBUI_SN_PRINTF_DYN(test_port, 64, "127.0.0.1:%zu", port_num);

    // Start HTTP Server
    const char* http_options[] = {
        "listening_ports",
        test_port,
        NULL,
        NULL
    };
    struct mg_callbacks http_callbacks;
    struct mg_context * http_ctx;
    memset(&http_callbacks, 0, sizeof(http_callbacks));
    http_ctx = mg_start(&http_callbacks, 0, http_options);

    if (http_ctx == NULL) {

        // Cannot listen
        mg_stop(http_ctx);
        return false;
    }

    // Listening success
    mg_stop(http_ctx);

    return true;
}

static bool _webinix_port_is_used(size_t port_num) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_port_is_used([%zu])\n", port_num);
    #endif

    #ifdef _WIN32
    // Listener test
    if (!_webinix_socket_test_listen_win32(port_num))
        return true; // Port is already used
    return false; // Port is not in use
    #else
    // Listener test MG
    if (!_webinix_socket_test_listen_mg(port_num))
        return true; // Port is already used
    return false; // Port is not in use
    #endif
}

static char* _webinix_get_file_name_from_url(const char* url) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_file_name_from_url([%s])\n", url);
    #endif

    if (_webinix_is_empty(url))
        return NULL;

    // Find the position of "://"
    const char* pos = strstr(url, "://");
    if (pos == NULL) {
        pos = url;
    } else {
        // Move the position after "://"
        pos += 3;
    }

    // Find the position of the first '/'
    pos = strchr(pos, '/');
    if (pos == NULL) {
        // Invalid URL
        return NULL;
    } else {
        // Move the position after "/"
        pos++;
    }

    // Copy the path to a new string
    char* file = strdup(pos);

    // Find the position of the first '?'
    char* question_mark = strchr(file, '?');
    if (question_mark != NULL) {
        // Replace '?' with NULL
        * question_mark = '\0';
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_file_name_from_url() -> File name: [%s]\n", file);
    #endif

    return file;
}

static char* _webinix_get_full_path(_webinix_window_t* win, const char* file) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_full_path([%s])\n", file);
    #endif

    if (!file)
        return NULL;

    // Get full path
    // [current folder][/][file]
    size_t bf_len = (_webinix_strlen(win->server_root_path) + 1 + _webinix_strlen(file));
    char* full_path = (char*)_webinix_malloc(bf_len);
    WEBUI_SN_PRINTF_DYN(full_path, bf_len, "%s%s%s", win->server_root_path, webinix_sep, file);

    #ifdef _WIN32
    // Replace `/` by `\`
    for (int i = 0;full_path[i] != '\0'; i++) {
        if (full_path[i] == '/') {
            full_path[i] = * webinix_sep;
        }
    }
    #endif

    // Clean
    _webinix_free_mem((void*)file);

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_get_full_path() -> Full path: [%s]\n", full_path);
    #endif

    return full_path;
}

static size_t _webinix_new_event_inf(_webinix_window_t* win, webinix_event_inf_t** event_inf) {
    (*event_inf) = (webinix_event_inf_t*)_webinix_malloc(sizeof(webinix_event_inf_t));
    if (win->events_count > WEBUI_MAX_ARG)
        win->events_count = 0;
    size_t event_num = win->events_count++;
    win->events[event_num] = (*event_inf);
    return event_num;
}

static void _webinix_free_event_inf(_webinix_window_t* win, size_t event_num) {
    webinix_event_inf_t* event_inf = win->events[event_num];
    for (size_t i = 0; i < (WEBUI_MAX_ARG + 1); i++) {
        if (event_inf->event_data[i] != NULL)
            webinix_free((void*)event_inf->event_data[i]);
    }
    if (event_inf->response != NULL)
        webinix_free((void*)event_inf->response);
    webinix_free((void*)event_inf);
    win->events[event_num] = NULL;
}

static int _webinix_serve_file(_webinix_window_t* win, struct mg_connection* client, size_t client_id) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_serve_file()\n");
    #endif

    // Serve a normal text based file

    int http_status_code = 0;

    const struct mg_request_info * ri = mg_get_request_info(client);
    const char* url = ri->local_uri;

    if (win->files_handler != NULL) {

        // Get file content from the external files handler
        size_t length = 0;

        // This is a breaking changes, should be
        // added in the next major Webinix version.
        // 
        // New Event (HTTP)
        // webinix_event_t e;
        // e.window = win->window_number;
        // e.event_type = WEBUI_EVENT_HTTP_GET;
        // e.element = "";
        // e.event_number = 0;
        // e.bind_id = 0;
        // e.connection_id = 0;
        // e.client_id = client_id;
        // e.cookies = "";

        // Files handler callback
        const void * callback_resp = win->files_handler(url, (int*)&length);

        if (callback_resp != NULL) {

            // File content found (200)

            if (length == 0)
                length = _webinix_strlen(callback_resp);

            // Send user data (200)
            _webinix_http_send(win, client, mg_get_builtin_mime_type(url), 
                callback_resp, length, false);
            
            // Safely free resources if end-user allocated
            // using `webinix_malloc()`. Otherwise just do nothing.
            webinix_free((void*)callback_resp);

            http_status_code = 200;
        }

        // If `data == NULL` thats mean the external handler
        // did not find the requested file. So Webinix will try
        // looking for the file locally.
    }

    if (http_status_code != 200) {

        // Using internal files handler

        // Get full path
        char* file = _webinix_get_file_name_from_url(url);
        char* full_path = _webinix_get_full_path(win, file);

        if (_webinix_file_exist(full_path)) {

            // 200 - File exist
            _webinix_http_send_file(win, client, mg_get_builtin_mime_type(url), full_path, false);
            http_status_code = 200;
        }
        else {

            // 404 - File not exist
            _webinix_http_send_error(client, webinix_html_res_not_available, 404);
            http_status_code = 404;
        }

        _webinix_free_mem((void*)full_path);
        _webinix_free_mem((void*)file);
    }

    return http_status_code;
}

static bool _webinix_deno_exist(_webinix_window_t* win) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_deno_exist()\n");
    #endif

    static bool found = false;

    if (found)
        return true;

    if (_webinix_cmd_sync(win, "deno --version", false) == 0) {

        found = true;
        return true;
    } else
        return false;
}

static bool _webinix_nodejs_exist(_webinix_window_t* win) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_nodejs_exist()\n");
    #endif

    static bool found = false;

    if (found)
        return true;

    if (_webinix_cmd_sync(win, "node -v", false) == 0) {

        found = true;
        return true;
    } else
        return false;
}

static const char* _webinix_interpret_command(const char* cmd) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_interpret_command([%s])\n", cmd);
    #endif

    // Run the command with redirection of errors to stdout
    // and return the output.

    // Output buffer
    char* out = NULL;

    #ifdef _WIN32
    // Redirect stderr to stdout
    char cmd_with_redirection[512] = {0};
    WEBUI_SN_PRINTF_STATIC(cmd_with_redirection, sizeof(cmd_with_redirection), "cmd.exe /c %s 2>&1", cmd);
    _webinix_system_win32_out(cmd_with_redirection,&out, false);
    #else
    // Redirect stderr to stdout
    char cmd_with_redirection[512] = {0};
    WEBUI_SN_PRINTF_STATIC(cmd_with_redirection, sizeof(cmd_with_redirection), "%s 2>&1", cmd);

    FILE * pipe = WEBUI_POPEN(cmd_with_redirection, "r");

    if (pipe == NULL)
        return NULL;

    // Read STDOUT
    out = (char*)_webinix_malloc(WEBUI_STDOUT_BUF);
    char* line = (char*)_webinix_malloc(1024);
    while(fgets(line, 1024, pipe) != NULL)
        WEBUI_STR_CAT_DYN(out, WEBUI_STDOUT_BUF, line);
    WEBUI_PCLOSE(pipe);

    // Clean
    _webinix_free_mem((void*)line);
    #endif

    return (const char*)out;
}

static void _webinix_condition_init(webinix_condition_t* cond) {

    #ifdef _WIN32
    InitializeConditionVariable(cond);
    #else
    pthread_cond_init(cond, NULL);
    #endif
}

static void _webinix_condition_wait(webinix_condition_t* cond, webinix_mutex_t* mutex) {

    #ifdef _WIN32
    SleepConditionVariableCS(cond, mutex, INFINITE);
    #else
    pthread_cond_wait(cond, mutex);
    #endif
}

static void _webinix_condition_signal(webinix_condition_t* cond) {

    #ifdef _WIN32
    WakeConditionVariable(cond);
    #else
    pthread_cond_signal(cond);
    #endif
}

static void _webinix_condition_destroy(webinix_condition_t* cond) {

    #ifdef _WIN32
    // No need
    (void)cond;
    #else
    pthread_cond_destroy(cond);
    #endif
}

static void _webinix_mutex_init(webinix_mutex_t* mutex) {

    #ifdef _WIN32
    InitializeCriticalSection(mutex);
    #else
    pthread_mutex_init(mutex, NULL);
    #endif
}

static void _webinix_mutex_lock(webinix_mutex_t* mutex) {

    #ifdef _WIN32
    EnterCriticalSection(mutex);
    #else
    pthread_mutex_lock(mutex);
    #endif
}

static void _webinix_mutex_unlock(webinix_mutex_t* mutex) {

    #ifdef _WIN32
    LeaveCriticalSection(mutex);
    #else
    pthread_mutex_unlock(mutex);
    #endif
}

static void _webinix_mutex_destroy(webinix_mutex_t* mutex) {

    #ifdef _WIN32
    DeleteCriticalSection(mutex);
    #else
    pthread_mutex_destroy(mutex);
    #endif
}

static int _webinix_interpret_file(_webinix_window_t* win, struct mg_connection* client, char* index, size_t client_id) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_interpret_file()\n");
    #endif

    // Interpret the file using JavaScript/TypeScript runtimes
    // and send back the output. otherwise, send the file as a normal text based

    int interpret_http_stat = 200;
    char* file = NULL;
    char* full_path = NULL;
    const char* query = NULL;

    const struct mg_request_info * ri = mg_get_request_info(client);
    const char* url = ri->local_uri;

    // Get file full path
    if (index != NULL && !_webinix_is_empty(index)) {

        // Parse as index file

        file = index;
        full_path = index;
    } else {

        // Parse as other non-index files

        // Get file name
        file = _webinix_get_file_name_from_url(url);

        // Get full path
        full_path = _webinix_get_full_path(win, file);
    }

    // Get file extension
    const char* extension = _webinix_get_extension(file);

    if (strcmp(extension, "ts") == 0 || strcmp(extension, "js") == 0) {

        // TypeScript / JavaScript

        if (!_webinix_file_exist(full_path)) {

            // File not exist - 404
            _webinix_http_send_error(client, webinix_html_res_not_available, 404);

            _webinix_free_mem((void*)file);
            _webinix_free_mem((void*)full_path);
            return 404;
        }

        // Get query
        query = ri->query_string;

        if (win->runtime == Deno) {

            // Use Deno
            if (_webinix_deno_exist(win)) {

                // Set command
                // [disable coloring][cmd][file][query]
                size_t bf_len = (128 + _webinix_strlen(full_path) + _webinix_strlen(query));
                char* cmd = (char*)_webinix_malloc(bf_len);
                #ifdef _WIN32
                WEBUI_SN_PRINTF_DYN(
                    cmd, bf_len,
                    "Set NO_COLOR=1&Set DENO_NO_UPDATE_CHECK=1&deno run --quiet --allow-all --unstable-ffi --allow-ffi \"%s\" \"%s\"",
                    full_path, query
                );
                #else
                WEBUI_SN_PRINTF_DYN(
                    cmd, bf_len,
                    "NO_COLOR=1;DENO_NO_UPDATE_CHECK=1;deno run --quiet --allow-all --unstable-ffi --allow-ffi \"%s\" \"%s\"",
                    full_path, query
                );
                #endif

                // Run command
                const char* out = _webinix_interpret_command(cmd);

                if (out != NULL) {

                    // Send Deno output 200
                    _webinix_http_send(win, client, "text/plain", out, _webinix_strlen(out), false);
                }
                else {

                    // Deno interpretation failed.
                    // Send back an empty `200 OK`
                    _webinix_http_send(win, client, "text/plain", "", 0, false);
                }

                _webinix_free_mem((void*)cmd);
                _webinix_free_mem((void*)out);
            }
            else {

                // Deno not installed
                // Send back an empty `200 OK`
                _webinix_http_send(win, client, "text/plain", "", 0, false);
            }
        } else if (win->runtime == NodeJS) {

            // Use Nodejs

            if (_webinix_nodejs_exist(win)) {

                // Set command
                // [node][file]
                size_t bf_len = (16 + _webinix_strlen(full_path));
                char* cmd = (char*)_webinix_malloc(bf_len);
                WEBUI_SN_PRINTF_DYN(cmd, bf_len, "node \"%s\" \"%s\"", full_path, query);

                // Run command
                const char* out = _webinix_interpret_command(cmd);

                if (out != NULL) {

                    // Send Node.js output 200
                    _webinix_http_send(win, client, "text/plain", out, _webinix_strlen(out), false);
                }
                else {

                    // Node.js interpretation failed.
                    // Send back an empty `200 OK`
                    _webinix_http_send(win, client, "text/plain", "", 0, false);
                }

                _webinix_free_mem((void*)cmd);
                _webinix_free_mem((void*)out);
            } else {

                // Node.js not installed
                // Send back an empty `200 OK`
                _webinix_http_send(win, client, "text/plain", "", 0, false);
            }
        } else {

            // Unknown runtime
            // Serve as a normal text-based file
            _webinix_http_send_file(win, client, mg_get_builtin_mime_type(url), full_path, false);
        }
    } else {

        // Unknown file extension

        // Serve as a normal text-based file
        interpret_http_stat = _webinix_serve_file(win, client, client_id);
    }

    _webinix_free_mem((void*)file);
    _webinix_free_mem((void*)full_path);

    return interpret_http_stat;
}

static const char* _webinix_generate_js_bridge(_webinix_window_t* win, struct mg_connection* client) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_generate_js_bridge()\n");
    #endif

    _webinix_mutex_lock(&_webinix.mutex_bridge);

    // Token
    uint32_t token = 0x00000000;
    if (!_webinix.config.multi_client) {
        // Single client mode
        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
            // Non-authorized request to `webinix.js` because
            // the single client already connected.
            _webinix_mutex_unlock(&_webinix.mutex_bridge);
            return NULL;
        }
        if (win->token == 0) {
            win->token = _webinix_generate_random_uint32();
            token = win->token;
        }
    }
    else {
        // Multi client mode
        if (win->token == 0) {
            win->token = _webinix_generate_random_uint32();
        }
        token = win->token;
    }

    // Calculate the cb size
    size_t cb_mem_size = 64; // To hold 'const _webinix_bind_list = ["elem1", "elem2",];'
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (!_webinix_is_empty(win->html_elements[i])) {
            cb_mem_size += _webinix_strlen(win->html_elements[i]) + 3;
        }
    }

    // Generate the cb array
    char* event_cb_js_array = (char*)_webinix_malloc(cb_mem_size);
    WEBUI_STR_CAT_DYN(event_cb_js_array, cb_mem_size, "[");
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (!_webinix_is_empty(win->html_elements[i])) {
            WEBUI_STR_CAT_DYN(event_cb_js_array, cb_mem_size, "\"");
            WEBUI_STR_CAT_DYN(event_cb_js_array, cb_mem_size, win->html_elements[i]);
            WEBUI_STR_CAT_DYN(event_cb_js_array, cb_mem_size, "\",");
        }
    }
    WEBUI_STR_CAT_DYN(event_cb_js_array, cb_mem_size, "]");

    // Generate the full Webinix Bridge
    #ifdef WEBUI_LOG
    const char* log = "true";
    #else
    const char* log = "false";
    #endif
    size_t len = 256 + cb_mem_size + _webinix_strlen((const char*)webinix_javascript_bridge);
    char* js = (char*)_webinix_malloc(len);
    #ifdef WEBUI_TLS
    const char* TLS = "true";
    #else
    const char* TLS = "false";
    #endif
    int c = WEBUI_SN_PRINTF_DYN(
        js, len,
        "%s\n document.addEventListener(\"DOMContentLoaded\",function(){ globalThis.webinix = "
        "new WebuiBridge({ secure: %s, token: %" PRIu32 ", port: %zu, winNum: %zu, bindList: %s, log: %s, ",
        webinix_javascript_bridge, TLS, token, win->server_port, win->window_number, event_cb_js_array, log
    );
    // Window Size
    if (win->size_set)
        c += WEBUI_SN_PRINTF_DYN(js + c, len, "winW: %u, winH: %u, ", win->width, win->height);
    // Window Position
    if (win->position_set)
        c += WEBUI_SN_PRINTF_DYN(js + c, len, "winX: %u, winY: %u, ", win->x, win->y);
    // Close
    WEBUI_STR_CAT_DYN(js, len, "});});");

    // Free
    _webinix_free_mem((void*)event_cb_js_array);
    _webinix_mutex_unlock(&_webinix.mutex_bridge);
    return js;
}

static bool _webinix_mutex_is_connected(_webinix_window_t* win, int update) {

    bool status = false;
    _webinix_mutex_lock(&_webinix.mutex_win_connect);
    if (update == WEBUI_MUTEX_TRUE) win->connected = true;
    else if (update == WEBUI_MUTEX_FALSE) win->connected = false;
    status = ((win->clients_count > 0) && (win->connected));
    _webinix_mutex_unlock(&_webinix.mutex_win_connect);
    return status;
}

static bool _webinix_mutex_is_exit_now(int update) {

    bool status = false;
    _webinix_mutex_lock(&_webinix.mutex_exit_now);
    if (update == WEBUI_MUTEX_TRUE) _webinix.exit_now = true;
    else if (update == WEBUI_MUTEX_FALSE) _webinix.exit_now = false;
    status = _webinix.exit_now;
    _webinix_mutex_unlock(&_webinix.mutex_exit_now);
    return status;
}

static bool _webinix_mutex_is_webview_update(_webinix_window_t* win, int update) {

    bool status = false;
    _webinix_mutex_lock(&_webinix.mutex_webview_stop);
    if (update == WEBUI_MUTEX_TRUE) win->update_webview = true;
    else if (update == WEBUI_MUTEX_FALSE) win->update_webview = false;
    status = win->update_webview;
    _webinix_mutex_unlock(&_webinix.mutex_webview_stop);
    return status;
}

static bool _webinix_browser_create_new_profile(_webinix_window_t* win, size_t browser) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_create_new_profile(%zu)\n", browser);
    #endif

    // Default local machine profile
    if (win->default_profile)
        return true;

    if (win->custom_profile) {

        // Custom profile
        if (_webinix_is_empty(win->profile_path) || _webinix_is_empty(win->profile_name))
            return false;
    } else {

        // Webinix profile
        if (win->profile_name != NULL)
            _webinix_free_mem((void*)win->profile_name);
        if (win->profile_path != NULL)
            _webinix_free_mem((void*)win->profile_path);
        win->profile_path = (char*)_webinix_malloc(WEBUI_MAX_PATH);
        win->profile_name = (char*)_webinix_malloc(WEBUI_MAX_PATH);
        if(!win->disable_browser_high_contrast)
            WEBUI_STR_COPY_DYN(win->profile_name, WEBUI_MAX_PATH, WEBUI_PROFILE_NAME);
        else
            WEBUI_STR_COPY_DYN(win->profile_name, WEBUI_MAX_PATH, WEBUI_PROFILE_NAME "-NoHC");
    }

    #ifdef WEBUI_LOG
    printf(
        "[Core]\t\t_webinix_browser_create_new_profile(%zu) -> Generating Webinix profile\n",
        browser
    );
    #endif

    // Temp folder
    const char* temp = _webinix_get_temp_path();

    if (browser == Chrome) {

        // Google Chrome
        if (!win->custom_profile)
            WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixChromeProfile", temp, webinix_sep, webinix_sep);
        return true;
    } else if (browser == Edge) {

        // Edge
        if (!win->custom_profile)
            WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixEdgeProfile", temp, webinix_sep, webinix_sep);
        return true;
    } else if (browser == Epic) {

        // Epic
        if (!win->custom_profile)
            WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixEpicProfile", temp, webinix_sep, webinix_sep);
        return true;
    } else if (browser == Vivaldi) {

        // Vivaldi
        if (!win->custom_profile)
            WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixVivaldiProfile", temp, webinix_sep, webinix_sep);
        return true;
    } else if (browser == Brave) {

        // Brave
        if (!win->custom_profile)
            WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixBraveProfile", temp, webinix_sep, webinix_sep);
        return true;
    } else if (browser == Yandex) {

        // Yandex
        if (!win->custom_profile)
            WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixYandexProfile", temp, webinix_sep, webinix_sep);
        return true;
    } else if (browser == Chromium) {

        // Chromium
        if (!win->custom_profile)
            WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixChromiumProfile", temp, webinix_sep, webinix_sep);
        return true;
    } else if (browser == Firefox) {

        // Firefox (We need to create a folder)

        #ifdef _WIN32
        // Windows
        const char* path_ini = "%APPDATA%\\Mozilla\\Firefox\\profiles.ini";
        #elif __linux__
        // Linux
        const char* path_ini = "";
        if (_webinix_file_exist("~/.mozilla/firefox/profiles.ini"))
            path_ini = "~/.mozilla/firefox/profiles.ini";
        else if (_webinix_file_exist("~/snap/firefox/common/.mozilla/firefox/profiles.ini"))
            path_ini = "~/snap/firefox/common/.mozilla/firefox/profiles.ini";
        else
            return false;
        #else
        // macOS
        const char* path_ini = "~/Library/Application Support/Firefox/profiles.ini";
        #endif
        if (!win->custom_profile){
            if(!win->disable_browser_high_contrast)
                WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixFirefoxProfile", temp, webinix_sep, webinix_sep);
            else
                WEBUI_SN_PRINTF_DYN(win->profile_path, WEBUI_MAX_PATH, "%s%s.Webinix%sWebinixFirefoxProfile-NoHC", temp, webinix_sep, webinix_sep);
        }

        if (!_webinix_folder_exist(win->profile_path) ||
            !_webinix_is_firefox_ini_profile_exist(path_ini, win->profile_name)) {

            char buf[2048] = {
                0
            };

            // There is a possibility that the profile name
            // does not exist in the INI file. or folder does not exist.
            // let's delete the profile name from the ini file, and folder.
            _webinix_remove_firefox_profile_ini(path_ini, win->profile_name);
            _webinix_delete_folder(win->profile_path);

            // Creating the Firefox profile
            WEBUI_SN_PRINTF_STATIC(
                buf, sizeof(buf), "%s -CreateProfile \"%s %s\"", win->browser_path, win->profile_name, win->profile_path
            );
            _webinix_cmd_sync(win, buf, false);

            // Wait for Firefox profile to be created
            _webinix_timer_t timer;
            _webinix_timer_start(&timer);
            for (;;) {
                _webinix_sleep(1000);
                if (_webinix_folder_exist(win->profile_path))
                    break;
                if (_webinix_timer_is_end(&timer, 10000))
                    break;
            }

            // Check if Firefox profile is created
            if (!_webinix_folder_exist(win->profile_path) ||
                !_webinix_is_firefox_ini_profile_exist(path_ini, win->profile_name))
                return false;

            // prefs.js
            FILE * file;
            WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "%s%sprefs.js", win->profile_path, webinix_sep);
            WEBUI_FILE_OPEN(file, buf, "a");
            if (file == NULL)
                return false;
            fputs(
                "user_pref(\"toolkit.legacyUserProfileCustomizations.stylesheets\","
                " true);",
                file
            );
            fputs("user_pref(\"browser.shell.checkDefaultBrowser\", false);", file);
            fputs("user_pref(\"browser.tabs.warnOnClose\", false);", file);
            fputs("user_pref(\"browser.tabs.inTitlebar\", 0);", file);
            if(win->disable_browser_high_contrast){
                fputs("user_pref(\"browser.display.document_color_use\", 1);", file);
            }
            fclose(file);

            // userChrome.css
            WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "\"%s%schrome%s\"", win->profile_path, webinix_sep, webinix_sep);
            if (!_webinix_folder_exist(buf)) {

                WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "mkdir \"%s%schrome%s\"", win->profile_path, webinix_sep, webinix_sep);
                _webinix_cmd_sync(win, buf, false); // Create directory
            }
            WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "%s%schrome%suserChrome.css", win->profile_path, webinix_sep, webinix_sep);
            WEBUI_FILE_OPEN(file, buf, "a");
            if (file == NULL)
                return false;
            fputs(
                "#navigator-toolbox{opacity:0 !important;height:0px !important;max-height:0px !important;"
                "width:0px !important;max-width:0px !important;}"
            , file);
            fclose(file);
        }

        return true;
    }

    return false;
}

static bool _webinix_folder_exist(const char* folder) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_folder_exist([%s])\n", folder);
    #endif

    if (_webinix_file_exist(folder))
        return false; // It's a file not a folder

    #if defined(_MSC_VER)
    if (GetFileAttributesA(folder) != INVALID_FILE_ATTRIBUTES)
        return true;
    #else
    DIR * dir = opendir(folder);
    if (dir) {
        closedir(dir);
        return true;
    }
    #endif

    return false;
}

static void _webinix_delete_folder(char* folder) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_delete_folder([%s])\n", folder);
    #endif

    char command[1024];
    #if defined(_WIN32)
    WEBUI_SN_PRINTF_STATIC(command, sizeof(command), "cmd /c \"rmdir /s /q \"%s\"\" > nul 2>&1", folder);
    #else
    WEBUI_SN_PRINTF_STATIC(command, sizeof(command), "rm -rf \"%s\" >>/dev/null 2>>/dev/null", folder);
    #endif

    // Try 6 times in 3 seconds
    for (size_t i = 0; i < 6; i++) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_delete_folder() -> Running [%s] \n", command);
        #endif

        #if defined(_WIN32)
        _webinix_system_win32(NULL, command, false);
        #else
        system(command);
        #endif
        
        if (!_webinix_folder_exist(folder))
            break;
        _webinix_sleep(500);
    }
}

static uint32_t _webinix_get_token(const char* data) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_token()\n");
    #endif

    uint32_t token = 0;

    // Little-endian
    token |= ((uint32_t) data[WEBUI_PROTOCOL_TOKEN]&0xFF);
    token |= ((uint32_t) data[WEBUI_PROTOCOL_TOKEN + 1]&0xFF) << 8;
    token |= ((uint32_t) data[WEBUI_PROTOCOL_TOKEN + 2]&0xFF) << 16;
    token |= ((uint32_t) data[WEBUI_PROTOCOL_TOKEN + 3]&0xFF) << 24;

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_token() -> 0x%08X\n", token);
    #endif

    return token;
}

static uint16_t _webinix_get_id(const char* data) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_id()\n");
    #endif

    uint16_t id = 0;

    // Little-endian
    id |= ((uint16_t) data[WEBUI_PROTOCOL_ID]&0xFF);
    id |= ((uint16_t) data[WEBUI_PROTOCOL_ID + 1]&0xFF) << 8;

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_id() -> 0x%04X\n", id);
    #endif

    return id;
}

static void _webinix_send_all(_webinix_window_t* win, uint16_t id, uint8_t cmd, const char* data, size_t len) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_send_all()\n");
    #endif

    // Send the WebSocket packet to a all connected clients if
    // `multi_client` mode is enabled, if not then send packet
    // to the only single connected client `_webinix.clients[0]`.
    
    // Send the packet
    if (_webinix.config.multi_client) {
        // Loop trough all connected clients
        for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
            if ((_webinix.clients[i] != NULL) && (_webinix.clients_token_check[i])) {
                _webinix_send_client(win, _webinix.clients[i], 0, cmd, data, len);
            }
        }
    } else {
        // Single client
        if ((_webinix.clients[0] != NULL) && (_webinix.clients_token_check[0])) {
            _webinix_send_client(win, _webinix.clients[0], 0, cmd, data, len);
        }
    }
}

static void _webinix_send_client(
    _webinix_window_t* win, struct mg_connection *client, 
    uint16_t id, uint8_t cmd, const char* data, size_t len) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_send_client()\n");
    #endif

    size_t connection_id = 0;
    if (!_webinix_connection_get_id(win, client, &connection_id))
        return;
    if ((_webinix.clients[connection_id] == NULL) || (!_webinix.clients_token_check[connection_id]))
        return;

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_send_client() -> ID = 0x%04X \n", id);
    printf("[Core]\t\t_webinix_send_client() -> CMD = 0x%02x \n", cmd);
    printf("[Core]\t\t_webinix_send_client() -> Data = %zu bytes \n", len);
    #endif

    // Protocol
    // 0: [SIGNATURE]
    // 1: [TOKEN]
    // 2: [ID]
    // 3: [CMD]
    // 4: [Data]

    // Prepare the packet
    size_t packet_len = WEBUI_PROTOCOL_SIZE + len + 1;
    char* packet = (char*)_webinix_malloc(packet_len);

    // Signature (1 Byte)
    packet[WEBUI_PROTOCOL_SIGN] = WEBUI_SIGNATURE;

    // Little-endian
    // Token (4 Bytes)
    packet[WEBUI_PROTOCOL_TOKEN] = 0xFF;
    packet[WEBUI_PROTOCOL_TOKEN + 1] = 0xFF;
    packet[WEBUI_PROTOCOL_TOKEN + 2] = 0xFF;
    packet[WEBUI_PROTOCOL_TOKEN + 3] = 0xFF;

    // ID (2 Bytes)
    packet[WEBUI_PROTOCOL_ID] = id&0xFF;
    packet[WEBUI_PROTOCOL_ID + 1] = (id >> 8)&0xFF;

    // Command (1 Byte)
    packet[WEBUI_PROTOCOL_CMD] = cmd;

    // Data (n Bytes)
    if (len > 0) {
        size_t j = WEBUI_PROTOCOL_DATA;
        for (size_t i = 0; i < len; i++) {
            packet[j++] = data[i];
        }
    }

    // Send packet
    _webinix_send_client_ws(win, client, connection_id, packet, packet_len);

    // Free
    _webinix_free_mem((void*)packet);
}

static char* _webinix_str_dup(const char* src) {
    size_t len = _webinix_strlen(src);
    char* dst = (char*)_webinix_malloc(len);
    WEBUI_STR_COPY_DYN(dst, len, src);
    return dst;
}

static const char* _webinix_get_temp_path() {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_temp_path()\n");
    #endif

    #ifdef _WIN32
    // Resolve %USERPROFILE%
    #ifdef _MSC_VER
    char* WinUserProfile = NULL;
    size_t sz = 0;
    if (_dupenv_s(&WinUserProfile,&sz, "USERPROFILE") != 0 || WinUserProfile == NULL)
        return "";
    #else
    char* WinUserProfile = getenv("USERPROFILE");
    if (WinUserProfile == NULL)
        return "";
    #endif
    return WinUserProfile;
    #elif __APPLE__
    // Resolve $HOME
    char* MacUserProfile = getenv("HOME");
    if (MacUserProfile == NULL)
        return "";
    return MacUserProfile;
    #else
    // Resolve $HOME
    char* LinuxUserProfile = getenv("HOME");
    if (LinuxUserProfile == NULL)
        return "";
    return LinuxUserProfile;
    #endif
}

static bool _webinix_is_google_chrome_folder(const char* folder) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_is_google_chrome_folder([%s])\n", folder);
    #endif

    char browser_full_path[WEBUI_MAX_PATH];

    // Make sure this folder is Google Chrome setup and not Chromium
    // by checking if `master_preferences` file exist or `initial_preferences`
    // Ref: https://support.google.com/chrome/a/answer/187948?hl=en

    WEBUI_SN_PRINTF_STATIC(browser_full_path, WEBUI_MAX_PATH, "%s\\master_preferences", folder);
    if (!_webinix_file_exist(browser_full_path)) {

        WEBUI_SN_PRINTF_STATIC(browser_full_path, WEBUI_MAX_PATH, "%s\\initial_preferences", folder);
        if (!_webinix_file_exist(browser_full_path))
            return false; // This is Chromium or something else
    }

    // Make sure the browser executable file exist
    WEBUI_SN_PRINTF_STATIC(browser_full_path, WEBUI_MAX_PATH, "%s\\chrome.exe", folder);
    if (!_webinix_file_exist(browser_full_path))
        return false;

    return true;
}

static bool _webinix_browser_exist(_webinix_window_t* win, size_t browser) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_exist([%zu])\n", browser);
    #endif

    // Check if a web browser is installed on this machine

    if (browser == Chrome) {

        // Google Chrome

        static bool ChromeExist = false;
        if(win) {
            if (ChromeExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Google Chrome on Windows

        char browser_folder[WEBUI_MAX_PATH];

        // Search in `HKEY_LOCAL_MACHINE` (If Google Chrome installed for
        // multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\chrome.exe",
                L"Path", browser_folder
            )) {

            // Make sure its Google Chrome and not Chromium
            if (_webinix_is_google_chrome_folder(browser_folder)) {

                // Google Chrome Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\\chrome.exe\"", browser_folder);
                ChromeExist = true;
                return true;
            }
        }

        // Search in `HKEY_CURRENT_USER` (If Google Chrome installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\chrome.exe",
                L"Path", browser_folder
            )) {

            // Make sure its Google Chrome and not Chromium
            if (_webinix_is_google_chrome_folder(browser_folder)) {

                // Google Chrome Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\\chrome.exe\"", browser_folder);
                ChromeExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Google Chrome on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Google Chrome\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Google Chrome.app\" --args");
            ChromeExist = true;
            return true;
        } else
            return false;
        #else

        // Google Chrome on Linux
        if (_webinix_cmd_sync(win, "google-chrome --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "google-chrome");
            ChromeExist = true;
            return true;
        } else if (_webinix_cmd_sync(win, "google-chrome-stable --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "google-chrome-stable");
            ChromeExist = true;
            return true;
        } else
            return false;

        #endif
    } else if (browser == Edge) {

        // Edge

        static bool EdgeExist = false;
        if(win) {
            if (EdgeExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Edge on Windows

        char browser_fullpath[WEBUI_MAX_PATH];

        // Search in `HKEY_LOCAL_MACHINE` (If Edge installed for multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\msedge.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Edge Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                EdgeExist = true;
                return true;
            }
        }

        // Search in `HKEY_CURRENT_USER` (If Edge installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\msedge.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Edge Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                EdgeExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Edge on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Microsoft Edge\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Microsoft Edge.app\" --args");
            EdgeExist = true;
            return true;
        } else
            return false;

        #else

        // Edge on Linux
        if (_webinix_cmd_sync(win, "microsoft-edge-stable --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "microsoft-edge-stable");
            EdgeExist = true;
            return true;
        } else if (_webinix_cmd_sync(win, "microsoft-edge-beta --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "microsoft-edge-beta");
            EdgeExist = true;
            return true;
        } else if (_webinix_cmd_sync(win, "microsoft-edge-dev --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "microsoft-edge-dev");
            EdgeExist = true;
            return true;
        } else
            return false;

        #endif
    } else if (browser == Epic) {

        // Epic Privacy Browser

        static bool EpicExist = false;
        if(win) {
            if (EpicExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Epic on Windows

        char browser_fullpath[WEBUI_MAX_PATH];

        // Search in `HKEY_CURRENT_USER` (If Epic installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\epic.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Epic Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                EpicExist = true;
                return true;
            }
        }

        // Search in `HKEY_LOCAL_MACHINE` (If Epic installed for multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\epic.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Epic Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                EpicExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Epic on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Epic\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Epic.app\" --args");
            EpicExist = true;
            return true;
        } else
            return false;
        #else

        // Epic on Linux
        if (_webinix_cmd_sync(win, "epic --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "epic");
            EpicExist = true;
            return true;
        } else
            return false;
        #endif
    } else if (browser == Vivaldi) {

        // Vivaldi Browser

        static bool VivaldiExist = false;
        if(win) {
            if (VivaldiExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Vivaldi on Windows

        char browser_fullpath[WEBUI_MAX_PATH];

        // Search in `HKEY_LOCAL_MACHINE` (If Vivaldi installed for multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\vivaldi.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Vivaldi Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                VivaldiExist = true;
                return true;
            }
        }

        // Search in `HKEY_CURRENT_USER` (If Vivaldi installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\vivaldi.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Vivaldi Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                VivaldiExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Vivaldi on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Vivaldi\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Vivaldi.app\" --args");
            VivaldiExist = true;
            return true;
        } else
            return false;
        #else

        // Vivaldi on Linux
        if (_webinix_cmd_sync(win, "vivaldi --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "vivaldi");
            VivaldiExist = true;
            return true;
        } else if (_webinix_cmd_sync(win, "vivaldi-stable --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "vivaldi-stable");
            VivaldiExist = true;
            return true;
        } else if (_webinix_cmd_sync(win, "vivaldi-snapshot --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "vivaldi-snapshot");
            VivaldiExist = true;
            return true;
        } else
            return false;
        #endif
    } else if (browser == Brave) {

        // Brave Browser

        static bool BraveExist = false;
        if(win) {
            if (BraveExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Brave on Windows

        char browser_fullpath[WEBUI_MAX_PATH];

        // Search in `HKEY_LOCAL_MACHINE` (If Brave installed for multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\brave.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Brave Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                BraveExist = true;
                return true;
            }
        }

        // Search in `HKEY_CURRENT_USER` (If Brave installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\brave.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Brave Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                BraveExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Brave on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Brave Browser\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Brave Browser.app\" --args");
            BraveExist = true;
            return true;
        } else
            return false;
        #else

        // Brave on Linux
        if (_webinix_cmd_sync(win, "brave --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "brave");
            BraveExist = true;
            return true;
        } else
            return false;
        #endif
    } else if (browser == Firefox) {

        // Firefox

        static bool FirefoxExist = false;
        if(win) {
            if (FirefoxExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Firefox on Windows

        char browser_fullpath[WEBUI_MAX_PATH];

        // Search in `HKEY_LOCAL_MACHINE` (If Firefox installed for multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\firefox.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Firefox Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                FirefoxExist = true;
                return true;
            }
        }

        // Search in `HKEY_CURRENT_USER` (If Firefox installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\firefox.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Firefox Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                FirefoxExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Firefox on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Firefox\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Firefox.app\" --args");
            FirefoxExist = true;
            return true;
        } else
            return false;
        #else

        // Firefox on Linux

        if (_webinix_cmd_sync(win, "firefox -v", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "firefox");
            FirefoxExist = true;
            return true;
        } else
            return false;

        #endif

    } else if (browser == Yandex) {

        // Yandex Browser

        static bool YandexExist = false;
        if(win) {
            if (YandexExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Yandex on Windows

        char browser_fullpath[WEBUI_MAX_PATH];

        // Search in `HKEY_CURRENT_USER` (If Yandex installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\browser.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Yandex Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                YandexExist = true;
                return true;
            }
        }

        // Search in `HKEY_LOCAL_MACHINE` (If Yandex installed for multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\browser.exe",
                L"", browser_fullpath
            )) {

            // Make sure the browser executable file exist
            if (_webinix_file_exist(browser_fullpath)) {

                // Yandex Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\"", browser_fullpath);
                YandexExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Yandex on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Yandex\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Yandex.app\" --args");
            YandexExist = true;
            return true;
        } else
            return false;
        #else

        // Yandex on Linux
        if (_webinix_cmd_sync(win, "yandex-browser --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "yandex-browser");
            YandexExist = true;
            return true;
        } else
            return false;
        #endif
    } else if (browser == Chromium) {

        // The Chromium Projects

        static bool ChromiumExist = false;
        if(win) {
            if (ChromiumExist && !_webinix_is_empty(win->browser_path))
                return true;
        }

        #ifdef _WIN32

        // Chromium on Windows

        char browser_folder[WEBUI_MAX_PATH];

        // Search in `HKEY_CURRENT_USER` (If Chromium installed for one user)
        if (_webinix_get_windows_reg_value(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\chrome.exe",
                L"Path", browser_folder
            )) {

            // Make sure its Chromium and not Google Chrome
            if (!_webinix_is_google_chrome_folder(browser_folder)) {

                // Chromium Found (one user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\\chrome.exe\"", browser_folder);
                ChromiumExist = true;
                return true;
            }
        }

        // Search in `HKEY_LOCAL_MACHINE` (If Chromium installed for multi-user)
        if (_webinix_get_windows_reg_value(
                HKEY_LOCAL_MACHINE,
                L"Software\\Microsoft\\Windows\\CurrentVer"
                L"sion\\App Paths\\chrome.exe",
                L"Path", browser_folder
            )) {

            // Make sure its Chromium and not Google Chrome
            if (!_webinix_is_google_chrome_folder(browser_folder)) {

                // Chromium Found (multi-user)
                if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "\"%s\\chrome.exe\"", browser_folder);
                ChromiumExist = true;
                return true;
            }
        }

        return false;

        #elif __APPLE__

        // Chromium on macOS
        if (_webinix_cmd_sync(win, "open -R -a \"Chromium\"", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "open --new -a \"Chromium.app\" --args");
            ChromiumExist = true;
            return true;
        } else
            return false;
        #else

        // Chromium on Linux
        if (_webinix_cmd_sync(win, "chromium-browser --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "chromium-browser");
            ChromiumExist = true;
            return true;
        } else if (_webinix_cmd_sync(win, "chromium --version", false) == 0) {

            if(win) WEBUI_SN_PRINTF_DYN(win->browser_path, WEBUI_MAX_PATH, "chromium");
            ChromiumExist = true;
            return true;
        } else
            return false;
        #endif
    }

    return false;
}

static void _webinix_clean(void) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_clean()\n");
    #endif

    static bool cleaned = false;
    if (cleaned)
        return;
    cleaned = true;

    // Stop all threads
    _webinix_mutex_is_exit_now(WEBUI_MUTEX_TRUE);

    // Let's give other threads more time to safely exit
    // and finish cleaning up.    
    // _webinix_sleep(500);

    // Clean all servers services
    mg_exit_library();

    // Free all non-freed memory allocations
    _webinix_free_all_mem();

    // Destroy all mutex
    _webinix_mutex_destroy(&_webinix.mutex_server_start);
    _webinix_mutex_destroy(&_webinix.mutex_send);
    _webinix_mutex_destroy(&_webinix.mutex_receive);
    _webinix_mutex_destroy(&_webinix.mutex_wait);
    _webinix_mutex_destroy(&_webinix.mutex_js_run);
    _webinix_mutex_destroy(&_webinix.mutex_win_connect);
    _webinix_mutex_destroy(&_webinix.mutex_exit_now);
    _webinix_mutex_destroy(&_webinix.mutex_webview_stop);
    _webinix_mutex_destroy(&_webinix.mutex_http_handler);
    _webinix_mutex_destroy(&_webinix.mutex_client);
    _webinix_condition_destroy(&_webinix.condition_wait);

    #ifdef WEBUI_LOG
    printf("[Core]\t\tWebinix exit successfully\n");
    #endif
}

static int _webinix_cmd_sync(_webinix_window_t* win, char* cmd, bool show) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_cmd_sync()\n");
    #endif

    // Run sync command and
    // return the exit code

    char buf[2048] = {0};

    // Sync command
    #ifdef _WIN32
    // Using: _CMD_
    WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "cmd /c \"%s\" > nul 2>&1", cmd);
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_cmd_sync() -> Running [%s] \n", buf);
    #endif
    return _webinix_system_win32(win, buf, show);
    #else
    // Using: _CMD_
    WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "%s >>/dev/null 2>>/dev/null", cmd);
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_cmd_sync() -> Running [%s] \n", buf);
    #endif
    int r = system(buf);
    r = (r != -1 && r != 127 && WIFEXITED(r)) ? WEXITSTATUS(r) : -1;
    return r;
    #endif
}

static int _webinix_cmd_async(_webinix_window_t* win, char* cmd, bool show) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_cmd_async()\n");
    #endif

    // Run a async command
    // and return immediately

    char buf[1024] = {0};

    // Asynchronous command
    #ifdef _WIN32
    // Using: START "" _CMD_
    WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "cmd /c \"START \"\" %s\" > nul 2>&1", cmd);
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_cmd_async() -> Running [%s] \n", buf);
    #endif
    return _webinix_system_win32(win, buf, show);
    #else
    // Using: _CMD_ &
    WEBUI_SN_PRINTF_STATIC(buf, sizeof(buf), "%s >>/dev/null 2>>/dev/null &", cmd);
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_cmd_async() -> Running [%s] \n", buf);
    #endif
    int r = system(buf);
    r = (r != -1 && r != 127 && WIFEXITED(r)) ? WEXITSTATUS(r) : -1;
    return r;
    #endif
}

static int _webinix_run_browser(_webinix_window_t* win, char* cmd) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_run_browser()\n");
    #endif

    // Run a async command
    return _webinix_cmd_async(win, cmd, false);
}

static int _webinix_get_browser_args(_webinix_window_t* win, size_t browser, char* buffer, size_t len) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_browser_args([%zu])\n", browser);
    #endif

    const char* chromium_options[] = {
        "--no-first-run",
        // "--no-proxy-server",
        "--safe-mode",
        "--disable-extensions",
        "--disable-background-mode",
        "--disable-plugins",
        "--disable-plugins-discovery",
        "--disable-translate",
        "--disable-features=Translate",
        "--bwsi",
        "--disable-sync",
        "--disable-sync-preferences",
        "--disable-component-update",
        "--allow-insecure-localhost",
        "--auto-accept-camera-and-microphone-capture",
    };

    int c = 0;
    switch(browser) {
        case Chrome:
        case Edge:
        case Epic:
        case Vivaldi:
        case Brave:
        case Yandex:
        case Chromium:
            // Profile
            if (!_webinix_is_empty(win->profile_path))
                c = WEBUI_SN_PRINTF_DYN(buffer, len, " --user-data-dir=\"%s\"", win->profile_path);
            // Basic
            for (int i = 0; i < (int)(sizeof(chromium_options) / sizeof(chromium_options[0])); i++) {
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", chromium_options[i]);
            }
            // Kiosk Mode
            if (win->kiosk_mode)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", "--chrome-frame --kiosk");
            // High Contrast Support
            if (win->disable_browser_high_contrast)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", "--disable-features=ForcedColors");
            // Hide Mode
            if (win->hide)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", "--headless --headless=new");
            // Window Size
            if (win->size_set)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " --window-size=%u,%u", win->width, win->height);
            // Window Position
            if (win->position_set)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " --window-position=%u,%u", win->x, win->y);
            // Proxy
            if (win->proxy_set)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " --proxy-server=%s", win->proxy_server);
            else
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", "--no-proxy-server");

            // URL (END)
            c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", "--app=");
            return c;
        case Firefox:
            // Profile
            if (!_webinix_is_empty(win->profile_name))
                c = WEBUI_SN_PRINTF_DYN(buffer, len, " -P %s", win->profile_name);
            // Basic
            c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " -purgecaches");
            // Kiosk Mode
            if (win->kiosk_mode)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", "-kiosk");
            // Hide Mode
            if (win->hide)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " %s", "-headless");
            // Window Size
            if (win->size_set)
                c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " -width %u -height %u", win->width, win->height);
            // Window Position
            // Firefox does not support window positioning.
            // Proxy
            if (win->proxy_set) {
                // Server: `win->proxy_server`

                // TODO: Add proxy feature to Firefox
                // Method 1: modifying `prefs.js` / user.js
                // Method 2: use Proxy Auto-Configuration (PAC) file
            }

            // URL (END)
            c += WEBUI_SN_PRINTF_DYN(buffer + c, len, " -new-window ");
            return c;
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_browser_args() -> Unknown Browser (%zu)\n", browser);
    #endif
    WEBUI_STR_COPY_DYN(buffer, len, "");
    return 0;
}

static bool _webinix_browser_start_chrome(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_chrome([%s])\n", address);
    #endif

    // -- Google Chrome ----------------------

    if (win->current_browser != 0 && win->current_browser != Chrome)
        return false;

    if (!_webinix_browser_exist(win, Chrome))
        return false;

    if (!_webinix_browser_create_new_profile(win, Chrome))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Chrome, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Chrome;
        _webinix.current_browser = Chrome;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start_edge(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_edge([%s])\n", address);
    #endif

    // -- Microsoft Edge ----------------------

    if (win->current_browser != 0 && win->current_browser != Edge)
        return false;

    if (!_webinix_browser_exist(win, Edge))
        return false;

    if (!_webinix_browser_create_new_profile(win, Edge))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Edge, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Edge;
        _webinix.current_browser = Edge;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start_epic(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_epic([%s])\n", address);
    #endif

    // -- Epic Privacy Browser ----------------------

    if (win->current_browser != 0 && win->current_browser != Epic)
        return false;

    if (!_webinix_browser_exist(win, Epic))
        return false;

    if (!_webinix_browser_create_new_profile(win, Epic))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Epic, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Epic;
        _webinix.current_browser = Epic;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start_vivaldi(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_vivaldi([%s])\n", address);
    #endif

    // -- Vivaldi Browser ----------------------

    if (win->current_browser != 0 && win->current_browser != Vivaldi)
        return false;

    if (!_webinix_browser_exist(win, Vivaldi))
        return false;

    if (!_webinix_browser_create_new_profile(win, Vivaldi))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Vivaldi, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Vivaldi;
        _webinix.current_browser = Vivaldi;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start_brave(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_brave([%s])\n", address);
    #endif

    // -- Brave Browser ----------------------

    if (win->current_browser != 0 && win->current_browser != Brave)
        return false;

    if (!_webinix_browser_exist(win, Brave))
        return false;

    if (!_webinix_browser_create_new_profile(win, Brave))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Brave, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Brave;
        _webinix.current_browser = Brave;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start_firefox(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_firefox([%s])\n", address);
    #endif

    // -- Mozilla Firefox ----------------------

    if (win->current_browser != 0 && win->current_browser != Firefox)
        return false;

    if (!_webinix_browser_exist(win, Firefox))
        return false;

    if (!_webinix_browser_create_new_profile(win, Firefox))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Firefox, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Firefox;
        _webinix.current_browser = Firefox;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start_yandex(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_yandex([%s])\n", address);
    #endif

    // -- Yandex Browser ----------------------

    if (win->current_browser != 0 && win->current_browser != Yandex)
        return false;

    if (!_webinix_browser_exist(win, Yandex))
        return false;

    if (!_webinix_browser_create_new_profile(win, Yandex))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Yandex, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Yandex;
        _webinix.current_browser = Yandex;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start_chromium(_webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start_chromium([%s])\n", address);
    #endif

    // -- The Chromium Projects -------------------

    if (win->current_browser != 0 && win->current_browser != Chromium)
        return false;

    if (!_webinix_browser_exist(win, Chromium))
        return false;

    if (!_webinix_browser_create_new_profile(win, Chromium))
        return false;

    char arg[1024] = {0};
    _webinix_get_browser_args(win, Chromium, arg, sizeof(arg));

    char full[1024] = {0};
    WEBUI_SN_PRINTF_STATIC(full, sizeof(full), "%s%s%s", win->browser_path, arg, address);

    if (_webinix_run_browser(win, full) == 0) {

        win->current_browser = Chromium;
        _webinix.current_browser = Chromium;
        return true;
    } else
        return false;
}

static bool _webinix_browser_start(_webinix_window_t* win, const char* address, size_t _browser) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_browser_start([%s], [%zu])\n", address, _browser);
    #endif

    // Non existing browser
    if (_browser > 12)
        return false;

    // No browser mode
    if (_browser == NoBrowser)
        return true;

    // Find the best web browser to use
    size_t browser = _browser;
    if (browser == AnyBrowser) {
        browser = _webinix.current_browser != 0 ? 
        _webinix.current_browser : _webinix_find_the_best_browser(win);
    }

    // Current browser used in the last opened window
    if (browser == AnyBrowser && _webinix.current_browser != 0)
        browser = _webinix.current_browser;

    // #1 - Chrome - Works perfectly
    // #2 - Edge - Works perfectly like Chrome
    // #3 - Epic - Works perfectly like Chrome
    // #4 - Vivaldi - Works perfectly like Chrome
    // #5 - Brave - Shows a policy notification in the first run
    // #6 - Firefox - Does not support App-Mode like Chrome (Looks not great)
    // #7 - Yandex - Shows a big welcome window in the first run
    // #8 - Chromium - Some Anti-Malware shows a false alert

    if (browser != AnyBrowser) {

        // Open the window using the user specified browser

        if (browser == Chrome)
            return _webinix_browser_start_chrome(win, address);
        else if (browser == Edge)
            return _webinix_browser_start_edge(win, address);
        else if (browser == Epic)
            return _webinix_browser_start_epic(win, address);
        else if (browser == Vivaldi)
            return _webinix_browser_start_vivaldi(win, address);
        else if (browser == Brave)
            return _webinix_browser_start_brave(win, address);
        else if (browser == Firefox)
            return _webinix_browser_start_firefox(win, address);
        else if (browser == Yandex)
            return _webinix_browser_start_yandex(win, address);
        else if (browser == Chromium)
            return _webinix_browser_start_chromium(win, address);
        else if (browser == ChromiumBased) {

            // Open the window using a Chromium-based browser

            if (!_webinix_browser_start_chrome(win, address))
                if (!_webinix_browser_start_edge(win, address))
                    if (!_webinix_browser_start_epic(win, address))
                        if (!_webinix_browser_start_vivaldi(win, address))
                            if (!_webinix_browser_start_brave(win, address))
                                if (!_webinix_browser_start_yandex(win, address))
                                    if (!_webinix_browser_start_chromium(win, address))
                                        return false;
        } else
            return false;
    } else if (win->current_browser != 0) {

        // Open the window using the same web browser used for this current window

        if (win->current_browser == Chrome)
            return _webinix_browser_start_chrome(win, address);
        else if (win->current_browser == Edge)
            return _webinix_browser_start_edge(win, address);
        else if (win->current_browser == Epic)
            return _webinix_browser_start_epic(win, address);
        else if (win->current_browser == Vivaldi)
            return _webinix_browser_start_vivaldi(win, address);
        else if (win->current_browser == Brave)
            return _webinix_browser_start_brave(win, address);
        else if (win->current_browser == Firefox)
            return _webinix_browser_start_firefox(win, address);
        else if (win->current_browser == Yandex)
            return _webinix_browser_start_yandex(win, address);
        else if (win->current_browser == Chromium)
            return _webinix_browser_start_chromium(win, address);
        else
            return false;
    } else {

        // Open the window using the default OS browser

        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        // Windows
        if (!_webinix_browser_start_chrome(win, address))
            if (!_webinix_browser_start_edge(win, address))
                if (!_webinix_browser_start_epic(win, address))
                    if (!_webinix_browser_start_vivaldi(win, address))
                        if (!_webinix_browser_start_brave(win, address))
                            if (!_webinix_browser_start_firefox(win, address))
                                if (!_webinix_browser_start_yandex(win, address))
                                    if (!_webinix_browser_start_chromium(win, address))
                                        return false;
        #elif __APPLE__
        // macOS
        if (!_webinix_browser_start_chrome(win, address))
            if (!_webinix_browser_start_edge(win, address))
                if (!_webinix_browser_start_epic(win, address))
                    if (!_webinix_browser_start_vivaldi(win, address))
                        if (!_webinix_browser_start_brave(win, address))
                            if (!_webinix_browser_start_firefox(win, address))
                                if (!_webinix_browser_start_yandex(win, address))
                                    if (!_webinix_browser_start_chromium(win, address))
                                        return false;
        #else
        // Linux
        if (!_webinix_browser_start_chrome(win, address))
            if (!_webinix_browser_start_edge(win, address))
                if (!_webinix_browser_start_epic(win, address))
                    if (!_webinix_browser_start_vivaldi(win, address))
                        if (!_webinix_browser_start_brave(win, address))
                            if (!_webinix_browser_start_firefox(win, address))
                                if (!_webinix_browser_start_yandex(win, address))
                                    if (!_webinix_browser_start_chromium(win, address))
                                        return false;
        #endif
    }

    return true;
}

static bool _webinix_is_process_running(const char* process_name) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_is_process_running([%s])\n", process_name);
    #endif

    bool isRunning = false;

    #ifdef _WIN32
    // Microsoft Windows
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot,&pe32)) {
        CloseHandle(hSnapshot);
        return false;
    }
    do {
        if (strcmp(pe32.szExeFile, process_name) == 0) {
            isRunning = true;
            break;
        }
    } while(Process32Next(hSnapshot,&pe32));
    CloseHandle(hSnapshot);
    #elif __linux__
    // Linux
    DIR * dir;
    struct dirent* entry;
    char status_path[WEBUI_MAX_PATH];
    char line[WEBUI_MAX_PATH];
    dir = opendir("/proc");
    if (!dir)
        return false; // Unable to open /proc
    while((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            WEBUI_SN_PRINTF_STATIC(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);
            FILE * status_file;
            WEBUI_FILE_OPEN(status_file, status_path, "r");
            if (status_file) {
                while(fgets(line, sizeof(line), status_file)) {
                    if (strncmp(line, "Name:", 5) == 0) {
                        char proc_name[WEBUI_MAX_PATH];
                        sscanf(line, "Name: %s", proc_name);
                        if (strcmp(proc_name, process_name) == 0) {
                            isRunning = true;
                            fclose(status_file);
                            goto _close_dir;
                        }
                        break;
                    }
                }
                fclose(status_file);
            }
        }
    }
    _close_dir:
        closedir(dir);
    #else
    // macOS
    int mib[4] = {
        CTL_KERN,
        KERN_PROC,
        KERN_PROC_ALL,
        0
    };
    struct kinfo_proc * procs = NULL;
    size_t size;
    if (sysctl(mib, 4, NULL,&size, NULL, 0) < 0)
        return false; // Failed to get process list size
    procs = (struct kinfo_proc * ) malloc(size);
    if (!procs)
        return false; // Failed to allocate memory for process list
    if (sysctl(mib, 4, procs,&size, NULL, 0) < 0) {
        free(procs);
        return false; // Failed to get process list
    }
    size_t count = size / sizeof(struct kinfo_proc);
    for (size_t i = 0; i < count; i++) {
        if (strcmp(procs[i].kp_proc.p_comm, process_name) == 0) {
            isRunning = true;
            break;
        }
    }
    free(procs);
    #endif

    return isRunning;
}

static size_t _webinix_find_the_best_browser(_webinix_window_t* win) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_find_the_best_browser()\n");
    #endif

    // #1 - Chrome - Works perfectly
    // #2 - Edge - Works perfectly like Chrome
    // #3 - Epic - Works perfectly like Chrome
    // #4 - Vivaldi - Works perfectly like Chrome
    // #5 - Brave - Shows a policy notification in the first run
    // #6 - Firefox - Does not support App-Mode like Chrome (Looks not great)
    // #7 - Yandex - Shows a big welcome window in the first run
    // #8 - Chromium - Some Anti-Malware shows a false alert

    // To save memory, let's search if a web browser is already running

    #ifdef _WIN32
    // Microsoft Windows
    if (_webinix_is_process_running("chrome.exe") && _webinix_browser_exist(win, Chrome))
        return Chrome;
    else if (_webinix_is_process_running("msedge.exe") && _webinix_browser_exist(win, Edge))
        return Edge;
    else if (_webinix_is_process_running("epic.exe") && _webinix_browser_exist(win, Epic))
        return Epic;
    else if (_webinix_is_process_running("vivaldi.exe") && _webinix_browser_exist(win, Vivaldi))
        return Vivaldi;
    else if (_webinix_is_process_running("brave.exe") && _webinix_browser_exist(win, Brave))
        return Brave;
    else if (_webinix_is_process_running("firefox.exe") && _webinix_browser_exist(win, Firefox))
        return Firefox;
    else if (_webinix_is_process_running("browser.exe") && _webinix_browser_exist(win, Yandex))
        return Yandex;
    // Chromium check is never reached if Google Chrome is installed
    // due to duplicate process name `chrome.exe`
    else if (_webinix_is_process_running("chrome.exe") && _webinix_browser_exist(win, Chromium))
        return Chromium;
    #elif __linux__
    // Linux
    if (_webinix_is_process_running("chrome") && _webinix_browser_exist(win, Chrome))
        return Chrome;
    else if (_webinix_is_process_running("msedge") && _webinix_browser_exist(win, Edge))
        return Edge;
    // Epic...
    else if (_webinix_is_process_running("vivaldi-bin") && _webinix_browser_exist(win, Vivaldi))
        return Vivaldi;
    else if (_webinix_is_process_running("brave") && _webinix_browser_exist(win, Brave))
        return Brave;
    else if (_webinix_is_process_running("firefox") && _webinix_browser_exist(win, Firefox))
        return Firefox;
    else if (_webinix_is_process_running("yandex_browser") && _webinix_browser_exist(win, Yandex))
        return Yandex;
    // Chromium check is never reached if Google Chrome is installed
    // due to duplicate process name `chrome`
    else if (_webinix_is_process_running("chrome") && _webinix_browser_exist(win, Chromium))
        return Chromium;
    #else
    // macOS
    if (_webinix_is_process_running("Google Chrome") && _webinix_browser_exist(win, Chrome))
        return Chrome;
    else if (_webinix_is_process_running("Epic") && _webinix_browser_exist(win, Epic))
        return Epic;
    else if (_webinix_is_process_running("Vivaldi") && _webinix_browser_exist(win, Vivaldi))
        return Vivaldi;
    else if (_webinix_is_process_running("Brave") && _webinix_browser_exist(win, Brave))
        return Brave;
    else if (_webinix_is_process_running("Firefox") && _webinix_browser_exist(win, Firefox))
        return Firefox;
    else if (_webinix_is_process_running("Yandex") && _webinix_browser_exist(win, Yandex))
        return Yandex;
    else if (_webinix_is_process_running("Chromium") && _webinix_browser_exist(win, Chromium))
        return Chromium;
    #endif

    return AnyBrowser;
}

static bool _webinix_show(_webinix_window_t* win, struct mg_connection* client, const char* content, size_t browser) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_show([%zu])\n", browser);
    #endif

    if (_webinix_is_empty(content))
        return false;

    // Some wrappers do not guarantee pointers stay valid,
    // so, let's make our copy.
    size_t content_len = _webinix_strlen(content);
    const char* content_cpy = (const char*)_webinix_malloc(content_len);
    memcpy((char*)content_cpy, content, content_len);

    // URL
    if (_webinix_is_valid_url(content_cpy)) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_show() -> URL: [%s]\n", content_cpy);
        #endif
        return _webinix_show_window(win, client, content_cpy, WEBUI_SHOW_URL, browser);
    }
    // Embedded HTML
    else if (strstr(content_cpy, "<html") || 
            strstr(content_cpy, "<!DOCTYPE") || 
            strstr(content_cpy, "<!doctype") || 
            strstr(content_cpy, "<!Doctype")) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_show() -> Embedded HTML:\n");
        printf("- - -[HTML]- - - - - - - - - -\n%s\n- - - - - - - - - - - - - - - -\n", content_cpy);
        #endif
        return _webinix_show_window(win, client, content_cpy, WEBUI_SHOW_HTML, browser);
    }
    // Folder
    else if (_webinix_folder_exist(content_cpy)) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_show() -> Folder: [%s]\n", content_cpy);
        #endif
        // Set root folder
        if (!webinix_set_root_folder(win->window_number, content_cpy)) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_show() -> Failed to set folder root path\n");
            #endif
            _webinix_free_mem((void*)content_cpy);
            return false;
        }
        return _webinix_show_window(win, client, content_cpy, WEBUI_SHOW_FOLDER, browser);
    }
    // File
    else {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_show() -> File: [%s]\n", content_cpy);
        #endif
        if (content_len > WEBUI_MAX_PATH || strstr(content_cpy, "<"))
            return false;
        return _webinix_show_window(win, client, content_cpy, WEBUI_SHOW_FILE, browser);
    }
}

// TLS
#ifdef WEBUI_TLS
static int _webinix_tls_initialization(void * ssl_ctx, void * ptr) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_tls_initialization()\n");
    #endif

    SSL_CTX * ctx = (SSL_CTX * ) ssl_ctx;

    // Disable security levels. It is The end-user
    // responsability to provide a high encryption
    // level certificate. While Webinix should just
    // use the end-user's certificates.
    SSL_CTX_set_security_level(ctx, 0);

    // Load Certificate
    BIO * bio_cert = BIO_new_mem_buf((void*)_webinix.ssl_cert, -1);
    X509 * cert = PEM_read_bio_X509(bio_cert, NULL, 0, NULL);
    if (cert == NULL) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_tls_initialization() -> PEM_read_bio_X509 failed\n");
        #endif
        WEBUI_ASSERT("PEM_read_bio_X509 failed");
        return -1;
    }
    if (SSL_CTX_use_certificate(ctx, cert) <= 0) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_tls_initialization() -> SSL_CTX_use_certificate failed\n");
        #endif
        WEBUI_ASSERT("SSL_CTX_use_certificate failed");
        return -1;
    }
    X509_free(cert);
    BIO_free(bio_cert);

    // Load Key
    BIO * bio_key = BIO_new_mem_buf((void*)_webinix.ssl_key, -1);
    EVP_PKEY * private_key = PEM_read_bio_PrivateKey(bio_key, NULL, 0, NULL);
    if (private_key == NULL) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_tls_initialization() -> PEM_read_bio_PrivateKey failed\n");
        #endif
        WEBUI_ASSERT("PEM_read_bio_PrivateKey failed");
        return -1;
    }
    if (SSL_CTX_use_PrivateKey(ctx, private_key) <= 0) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_tls_initialization() -> SSL_CTX_use_PrivateKey failed\n");
        #endif
        WEBUI_ASSERT("SSL_CTX_use_PrivateKey failed");
        return -1;
    }
    EVP_PKEY_free(private_key);
    BIO_free(bio_key);

    return 0;
}

static bool _webinix_tls_generate_self_signed_cert(char* root_cert, char* root_key, char* ssl_cert, char* ssl_key) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_tls_generate_self_signed_cert()\n");
    #endif

    int ret = 0;
    int bits = 2048;

    // ----- Create Root Certificate -----
    EVP_PKEY * root_pkey = NULL;
    EVP_PKEY_CTX * root_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!root_ctx)
        return false;

    if (EVP_PKEY_keygen_init(root_ctx) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(root_ctx, bits) <= 0 ||
        EVP_PKEY_keygen(root_ctx,&root_pkey) <= 0) {
        EVP_PKEY_CTX_free(root_ctx);
        return false;
    }

    EVP_PKEY_CTX_free(root_ctx);

    X509 * root_x509 = X509_new();
    X509_set_version(root_x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(root_x509), (long) _webinix_generate_random_uint32());
    X509_gmtime_adj(X509_get_notBefore(root_x509), 0);
    X509_gmtime_adj(X509_get_notAfter(root_x509), (long)(WEBUI_SSL_EXPIRE));

    X509_NAME * root_name = X509_get_subject_name(root_x509);
    X509_NAME_add_entry_by_txt(root_name, "C", MBSTRING_ASC, "CA", -1, -1,
        0); // Country
    X509_NAME_add_entry_by_txt(root_name, "O", MBSTRING_ASC, "Webinix Root Authority", -1, -1, 0); // Organization
    X509_NAME_add_entry_by_txt(root_name, "OU", MBSTRING_ASC, "Webinix", -1, -1,
        0); // Organizational Unit
    X509_NAME_add_entry_by_txt(root_name, "CN", MBSTRING_ASC, "localhost", -1, -1,
        0); // Common Name
    X509_NAME_add_entry_by_txt(
        root_name, "subjectAltName", MBSTRING_ASC, "127.0.0.1", -1, -1,
        0
    ); // Subject Alternative Name
    X509_NAME_add_entry_by_txt(root_name, "ST", MBSTRING_ASC, "Webinix", -1, -1,
        0); // State
    X509_NAME_add_entry_by_txt(root_name, "L", MBSTRING_ASC, "Webinix", -1, -1,
        0); // Locality

    X509_set_issuer_name(root_x509, root_name);
    X509_set_pubkey(root_x509, root_pkey);
    ret = X509_sign(root_x509, root_pkey, EVP_sha256());
    if (ret <= 0) {
        X509_free(root_x509);
        EVP_PKEY_free(root_pkey);
        return false;
    }

    // Write Root Certificate and Key
    BIO * bio_root_cert = BIO_new(BIO_s_mem());
    PEM_write_bio_X509(bio_root_cert, root_x509);
    memset(root_cert, 0, WEBUI_SSL_SIZE);
    BIO_read(bio_root_cert, root_cert, (WEBUI_SSL_SIZE - 1));

    BIO * bio_root_key = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(bio_root_key, root_pkey, NULL, NULL, 0, NULL, NULL);
    memset(root_key, 0, WEBUI_SSL_SIZE);
    BIO_read(bio_root_key, root_key, (WEBUI_SSL_SIZE - 1));

    // ----- Create Server Certificate and sign with Root -----
    EVP_PKEY * pkey = NULL;
    EVP_PKEY_CTX * ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        X509_free(root_x509);
        EVP_PKEY_free(root_pkey);
        return false;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) <= 0 ||
        EVP_PKEY_keygen(ctx,&pkey) <= 0) {
        X509_free(root_x509);
        EVP_PKEY_free(root_pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_PKEY_CTX_free(ctx);

    X509 * x509 = X509_new();
    X509_set_version(x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), (long) _webinix_generate_random_uint32());
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), (long)(WEBUI_SSL_EXPIRE));

    X509_NAME * name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, "CA", -1, -1,
        0); // Country
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, "Webinix", -1, -1,
        0); // Organization
    X509_NAME_add_entry_by_txt(name, "OU", MBSTRING_ASC, "Webinix", -1, -1,
        0); // Organizational Unit
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, "localhost", -1, -1,
        0); // Common Name
    X509_NAME_add_entry_by_txt(
        name, "subjectAltName", MBSTRING_ASC, "127.0.0.1", -1, -1, 0
    ); // Subject Alternative Name
    X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, "Webinix", -1, -1,
        0); // State
    X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, "Webinix", -1, -1,
        0); // Locality

    X509_set_issuer_name(x509, root_name);
    X509_set_pubkey(x509, pkey);
    ret = X509_sign(x509, root_pkey, EVP_sha256());
    if (ret <= 0) {
        X509_free(root_x509);
        EVP_PKEY_free(root_pkey);
        X509_free(x509);
        EVP_PKEY_free(pkey);
        return false;
    }

    // Write the Server Certificate and Key
    BIO * bio_cert = BIO_new(BIO_s_mem());
    PEM_write_bio_X509(bio_cert, x509);
    memset(ssl_cert, 0, WEBUI_SSL_SIZE);
    BIO_read(bio_cert, ssl_cert, (WEBUI_SSL_SIZE - 1));

    BIO * bio_key = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(bio_key, pkey, NULL, NULL, 0, NULL, NULL);
    memset(ssl_key, 0, WEBUI_SSL_SIZE);
    BIO_read(bio_key, ssl_key, (WEBUI_SSL_SIZE - 1));

    // Cleanup
    X509_free(root_x509);
    EVP_PKEY_free(root_pkey);
    BIO_free_all(bio_root_cert);
    BIO_free_all(bio_root_key);
    X509_free(x509);
    EVP_PKEY_free(pkey);
    BIO_free_all(bio_cert);
    BIO_free_all(bio_key);

    return true;
}
#endif

static bool _webinix_show_window(_webinix_window_t* win, struct mg_connection* client, const char* content, int type, size_t browser) {

    #ifdef WEBUI_LOG
    if (type == WEBUI_SHOW_HTML)
        printf("[Core]\t\t_webinix_show_window(HTML, [%zu])\n", browser);
    else if (type == WEBUI_SHOW_URL)
        printf("[Core]\t\t_webinix_show_window(URL, [%zu])\n", browser);
    else if (type == WEBUI_SHOW_FOLDER)
        printf("[Core]\t\t_webinix_show_window(FOLDER, [%zu])\n", browser);
    else
        printf("[Core]\t\t_webinix_show_window(FILE, [%zu])\n", browser);
    #endif

    #ifdef WEBUI_TLS
    // TLS
    if (_webinix_is_empty(_webinix.ssl_cert) || _webinix_is_empty(_webinix.ssl_key)) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_show_window() -> Generating self-signed TLS "
            "certificate\n");
        #endif

        // Generate SSL self-signed certificate once
        char* root_cert = (char*)_webinix_malloc(WEBUI_SSL_SIZE);
        char* root_key = (char*)_webinix_malloc(WEBUI_SSL_SIZE);
        char* ssl_cert = (char*)_webinix_malloc(WEBUI_SSL_SIZE);
        char* ssl_key = (char*)_webinix_malloc(WEBUI_SSL_SIZE);
        if (!_webinix_tls_generate_self_signed_cert(root_cert, root_key, ssl_cert, ssl_key)) {
            #ifdef WEBUI_LOG
            unsigned long err = ERR_get_error();
            char err_buf[1024];
            ERR_error_string_n(err, err_buf, sizeof(err_buf));
            printf(
                "[Core]\t\t_webinix_show_window() -> Generating self-signed TLS "
                "certificate failed:\n%s\n",
                err_buf
            );
            #endif
            _webinix_free_mem((void*)root_cert);
            _webinix_free_mem((void*)root_key);
            _webinix_free_mem((void*)ssl_cert);
            _webinix_free_mem((void*)ssl_key);
            WEBUI_ASSERT("Generating self-signed TLS certificate failed");
            return false;
        }

        _webinix.root_cert = root_cert;
        _webinix.root_key = root_key;
        _webinix.ssl_cert = ssl_cert;
        _webinix.ssl_key = ssl_key;

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_show_window() -> Self-signed SSL/TLS "
            "Certificate:\nRoot:\n");
        printf("%s\n", (const char*)_webinix.root_cert);
        printf("%s\nServer:\n", (const char*)_webinix.root_key);
        printf("%s\n", (const char*)_webinix.ssl_cert);
        printf("%s\n", (const char*)_webinix.ssl_key);
        #endif
    }
    #endif

    // Initialization
    if (win->html != NULL)
        _webinix_free_mem((void*)win->html);
    if (win->url != NULL)
        _webinix_free_mem((void*)win->url);

    // Get network ports
    if (win->custom_server_port > 0) win->server_port = win->custom_server_port;
    else if (win->server_port == 0) win->server_port = _webinix_get_free_port();

    // Generate the server URL
    win->url = (char*)_webinix_malloc(32); // [http][domain][port]
    WEBUI_SN_PRINTF_DYN(win->url, 32, WEBUI_HTTP_PROTOCOL "localhost:%zu", win->server_port);

    // Generate the window URL
    char* window_url = NULL;
    if (type == WEBUI_SHOW_HTML) {

        const char* user_html = content;

        // Show a window using the embedded HTML
        win->is_embedded_html = true;
        win->html = (user_html == NULL ? "" : user_html);

        // Set window URL
        size_t len = _webinix_strlen(win->url);
        window_url = (char*)_webinix_malloc(len);
        WEBUI_STR_COPY_DYN(window_url, len, win->url);
    }
    else if (type == WEBUI_SHOW_URL) {

        const char* user_url = content;

        // Show a window using a specific URL
        win->is_embedded_html = true;
        size_t bf_len = (64 + _webinix_strlen(user_url));
        char* refresh = (char*)_webinix_malloc(bf_len);
        WEBUI_SN_PRINTF_DYN(refresh, bf_len, "<meta http-equiv=\"refresh\" content=\"0;url=%s\">", user_url);
        win->html = refresh;

        // Set window URL
        window_url = (char*)user_url;
    }
    else if (type == WEBUI_SHOW_FOLDER) {

        const char* folder_path = content;

        // Show a window using a local folder
        win->is_embedded_html = false;
        win->html = NULL;

        // Set window URL
        window_url = win->url;
    }
    else {
        const char* user_file = content;

        // Show a window using a local file
        win->is_embedded_html = false;
        win->html = NULL;

        // Generate the URL
        const char* file_url_encoded = _webinix_url_encode(user_file);
        size_t bf_len = (64 + _webinix_strlen(file_url_encoded));
        char* url_encoded = (char*)_webinix_malloc(bf_len); // [http][domain][port] [file_encoded]
        WEBUI_SN_PRINTF_DYN(url_encoded, bf_len, WEBUI_HTTP_PROTOCOL "localhost:%zu/%s", 
            win->server_port, file_url_encoded);
        _webinix_free_mem((void*)file_url_encoded);
        _webinix_free_mem((void*)user_file);

        // Set window URL
        window_url = url_encoded;
    }

    // Run the window
    if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {

        // Start a new window

        // Prioritize the server thread if we
        // knows that there is UIs running
        if (_webinix.ui) {
            
            // New server thread
            #ifdef _WIN32
            HANDLE thread = CreateThread(NULL, 0, _webinix_server_thread, (void*)win, 0, NULL);
            win->server_thread = thread;
            if (thread != NULL)
                CloseHandle(thread);
            #else
            pthread_t thread;
            pthread_create(&thread, NULL,&_webinix_server_thread, (void*)win);
            pthread_detach(thread);
            win->server_thread = thread;
            #endif
        }

        // New WebView
        bool runWebView = false;
        if (win->allow_webview) {
            // Trying to use WebView
            if (_webinix_wv_show(win, window_url)) {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_show_window() -> WebView Found\n");
                #endif
                win->current_browser = WebView;
                runWebView = true;
            }
            else {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_show_window() -> WebView Not Found\n");
                #endif
            }
        }

        // Run browser
        bool runBrowser = false;
        if (!runWebView) {
            if (browser != NoBrowser) {
                if (!_webinix_browser_start(win, window_url, browser)) {
                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webinix_show_window() -> App-mode browser failed\n");
                    #endif
                    // Opening App-mode browser failed
                    // let's try opening UI in native default browser
                    if (browser == AnyBrowser && _webinix_open_url_native(window_url)) {
                        #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webinix_show_window() -> Native browser succeeded\n");
                        #endif
                        runBrowser = true;
                    }
                    else {
                        #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webinix_show_window() -> Native browser failed\n");
                        #endif
                    }
                }
                else {
                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webinix_show_window() -> App-mode browser succeeded\n");
                    #endif
                    runBrowser = true;
                }
            } else {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_show_window() -> Starting server only mode (NoBrowser)\n");
                #endif
                runBrowser = true;
            }
        }

        _webinix_free_mem((void*)window_url);
        if (browser != NoBrowser) {
            if (!runWebView && !runBrowser) {
                // Browser and WebView are not available
                _webinix_free_mem((void*)win->html);
                _webinix_free_mem((void*)win->url);
                _webinix_free_port(win->server_port);
                win->server_port = 0;
                return false;
            }            
        }

        if (!_webinix.ui) {

            // Let the wait() knows that this app
            // has atleast one window to wait for
            _webinix.ui = true;

            // New server thread
            #ifdef _WIN32
            HANDLE thread = CreateThread(NULL, 0, _webinix_server_thread, (void*)win, 0, NULL);
            win->server_thread = thread;
            if (thread != NULL)
                CloseHandle(thread);
            #else
            pthread_t thread;
            pthread_create(&thread, NULL,&_webinix_server_thread, (void*)win);
            pthread_detach(thread);
            win->server_thread = thread;
            #endif
        }

    } else {

        // Refresh an existing running window (All connected clients)

        // Packet Protocol Format:
        // [...]
        // [CMD]
        // [URL]

        // Send the packet
        if (client) {
            // Update single packet
            _webinix_send_client(
                win, client, 0, WEBUI_CMD_NAVIGATION, 
                (const char*)window_url, _webinix_strlen(window_url)
            );
        }
        else {
            // Update all windows
            _webinix_send_all(
                win, 0, WEBUI_CMD_NAVIGATION, 
                (const char*)window_url, _webinix_strlen(window_url)
            );
        }

        // Free
        _webinix_free_mem((void*)window_url);
    }

    // Wait for window connection
    if ((browser != NoBrowser) && _webinix.config.show_wait_connection) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_show_window() -> Waiting for window connection\n");
        #endif

        size_t timeout = (_webinix.startup_timeout > 0 ? 
            _webinix.startup_timeout : WEBUI_DEF_TIMEOUT
        );

        if (_webinix.is_webview) {

            _webinix_timer_t timer;
            _webinix_timer_start(&timer);
            for (;;) {

                _webinix_sleep(10);

                // Process WebView rendering if any
                if (_webinix.is_webview) {
                    #ifdef _WIN32
                    // ...
                    #elif __linux__
                    if (_webinix.is_webview) {
                        while (gtk_events_pending()) {
                            gtk_main_iteration_do(0);
                        }
                    }
                    #else
                    if (!_webinix.is_wkwebview_main_run) {
                        if (_webinix.is_webview) {
                            _webinix_macos_wv_process();
                        }
                    }
                    #endif
                }

                // Stop if window is connected
                if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
                    break;
                
                // Stop if timer is finished
                if (_webinix_timer_is_end(&timer, (timeout * 1000)))
                    break;
            }
        } else {

            // Wait for server thread to start
            _webinix_timer_t timer;
            _webinix_timer_start(&timer);
            for (;;) {

                _webinix_sleep(10);

                // Stop if window is connected
                if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
                    break;
                
                // Stop if timer is finished
                if (_webinix_timer_is_end(&timer, (timeout * 1000)))
                    break;
            }
        }

        // The window is successfully launched and connected.
        return _webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE);
    }

    // The window is successfully launched.
    return true;
}

static void _webinix_window_event(
    _webinix_window_t* win, size_t connection_id, int event_type, char* element,
    size_t event_number, size_t client_id, char* cookies) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_window_event([%zu], [%zu], [%s])\n", 
        win->window_number, connection_id, element);
    #endif

    // New Event (General)
    webinix_event_t e;
    e.window = win->window_number;
    e.event_type = event_type;
    e.element = element;
    e.event_number = event_number;
    e.connection_id = connection_id;
    e.client_id = client_id;
    e.cookies = cookies;

    // Check for all events-bind functions
    if (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) && win->has_all_events) {
        size_t events_cb_index = 0;
        bool exist = _webinix_get_cb_index(win, "", &events_cb_index);
        if (exist && win->cb[events_cb_index] != NULL) {
            // Call user all-events cb
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_window_event() -> Calling all-events user callback\n");
            printf("[Call]\n");
            #endif
            e.bind_id = events_cb_index;
            win->cb[events_cb_index](&e);
        }
    }

    if (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE)) {
        // Check for the regular bind functions
        if (!_webinix_is_empty(element)) {
            size_t cb_index = 0;
            bool exist = _webinix_get_cb_index(win, element, &cb_index);
            if (exist && win->cb[cb_index] != NULL) {
                // Call user cb
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_window_event() -> Calling user callback\n");
                printf("[Call]\n");
                #endif
                e.bind_id = cb_index;
                win->cb[cb_index](&e);
            }
        }
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_window_event() -> Finished\n");
    #endif
}

static void _webinix_send_client_ws(_webinix_window_t* win, struct mg_connection* client,
    size_t connection_id, char* packet, size_t packets_size) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_send_client_ws()\n");
    printf("[Core]\t\t_webinix_send_client_ws() -> Client #%zu\n", connection_id);
    printf("[Core]\t\t_webinix_send_client_ws() -> Packet size: %zu bytes \n", packets_size);
    printf("[Core]\t\t_webinix_send_client_ws() -> Packet hex : [ ");
        _webinix_print_hex(packet, packets_size);
    printf("]\n");
    #endif

    if (win == NULL || client == NULL) {
        WEBUI_ASSERT("_webinix_send_client_ws() null ptr");
    }

    if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE) || packet == NULL ||
        packets_size < WEBUI_PROTOCOL_SIZE)
        return;

    int ret = 0;
    if (win->window_number > 0 && win->window_number < WEBUI_MAX_IDS) {
        if (client != NULL) {
            // Mutex
            _webinix_mutex_lock(&_webinix.mutex_send);
            ret = mg_websocket_write(client, MG_WEBSOCKET_OPCODE_BINARY, packet, packets_size);
            _webinix_mutex_unlock(&_webinix.mutex_send);
        }
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_send_client_ws() -> %d bytes sent.\n", ret);
    #endif
}

static char* _webinix_get_current_path(void) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_current_path()\n");
    #endif

    char* path = (char*)_webinix_malloc(WEBUI_MAX_PATH);
    if (WEBUI_GET_CURRENT_DIR(path, WEBUI_MAX_PATH) == NULL)
        path[0] = 0x00;

    return path;
}

static void _webinix_free_port(size_t port) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_free_port([%zu])\n", port);
    #endif

    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.used_ports[i] == port) {
            _webinix.used_ports[i] = 0;
            break;
        }
    }
}

static size_t _webinix_get_free_port(void) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_free_port()\n");
    #endif

    size_t port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;

    for (size_t i = WEBUI_MIN_PORT; i <= WEBUI_MAX_PORT; i++) {

        // Search [port] in [_webinix.used_ports]
        bool found = false;
        for (size_t j = 0; j < WEBUI_MAX_IDS; j++) {
            if (_webinix.used_ports[j] == port) {
                found = true;
                break;
            }
        }

        if (found)
            // Port used by local window
            port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;
        else {
            if (_webinix_port_is_used(port))
                // Port used by an external app
                port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;
            else
                // Port is free
                break;
        }
    }

    // Add
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.used_ports[i] == 0) {
            _webinix.used_ports[i] = port;
            break;
        }
    }

    return port;
}

static void _webinix_init(void) {

    if (_webinix.initialized)
        return;
    memset(&_webinix, 0, sizeof(_webinix_core_t));
    _webinix.initialized = true;

    #ifdef WEBUI_LOG
    printf("[Core]\t\tWebinix v"
        WEBUI_VERSION " ("
        WEBUI_OS ", "
        WEBUI_COMPILER ", "
        WEBUI_SECURE ")\n");
    printf("[Core]\t\t_webinix_init()\n");
    #endif

    // Random
    #ifdef _WIN32
    srand((unsigned int) time(NULL));
    #else
    srand(time(NULL));
    #endif

    // Initializing core
    _webinix.startup_timeout = WEBUI_DEF_TIMEOUT;
    _webinix.executable_path = _webinix_get_current_path();
    _webinix.default_server_root_path = (char*)_webinix_malloc(WEBUI_MAX_PATH);

    // Initializing configs
    _webinix.config.show_wait_connection = true;
    _webinix.config.use_cookies = true;

    // Initializing server services
    #ifdef WEBUI_TLS
    if (mg_init_library(MG_FEATURES_TLS) != MG_FEATURES_TLS)
        WEBUI_ASSERT("mg_init_library() failed");
    #else
    mg_init_library(0);
    #endif

    // Initializing mutex
    _webinix_mutex_init(&_webinix.mutex_server_start);
    _webinix_mutex_init(&_webinix.mutex_send);
    _webinix_mutex_init(&_webinix.mutex_receive);
    _webinix_mutex_init(&_webinix.mutex_wait);
    _webinix_mutex_init(&_webinix.mutex_bridge);
    _webinix_mutex_init(&_webinix.mutex_js_run);
    _webinix_mutex_init(&_webinix.mutex_win_connect);
    _webinix_mutex_init(&_webinix.mutex_exit_now);
    _webinix_mutex_init(&_webinix.mutex_webview_stop);
    _webinix_mutex_init(&_webinix.mutex_http_handler);
    _webinix_mutex_init(&_webinix.mutex_client);
    _webinix_condition_init(&_webinix.condition_wait);
}

static const char* _webinix_url_encode(const char* str) {

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_url_encode()\n");
    #endif

    const char* hex = "0123456789ABCDEF";
    size_t len = _webinix_strlen(str);
    char* encoded = (char*)_webinix_malloc(4 * len + 1);
    if (!encoded)
        return NULL;

    char* pOutput = encoded;
    while(*str) {
        unsigned char byte = (unsigned char)(*str);
        if (isalnum(byte) || byte == '-' || byte == '_' || byte == '.' || byte == '~') {
            * pOutput++ = byte;
        } else {
            * pOutput++ = '%';
            * pOutput++ = hex[byte >> 4];
            * pOutput++ = hex[byte&15];
        }
        str++;
    }

    return (const char*)encoded;
}

static bool _webinix_get_cb_index(_webinix_window_t* win, const char* element, size_t* id) {

    _webinix_mutex_lock(&_webinix.mutex_bridge);

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_get_cb_index([%zu])\n", win->window_number);
    printf("[Core]\t\t_webinix_get_cb_index() -> Element: [%s]\n", element);
    #endif

    // Search
    if (element != NULL) {
        for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
            if (win->html_elements[i] != NULL) {
                if (strcmp(win->html_elements[i], element) == 0) {
                    #ifdef WEBUI_LOG_VERBOSE
                    printf("[Core]\t\t_webinix_get_cb_index() -> Found at %d\n", i);
                    #endif
                    _webinix_mutex_unlock(&_webinix.mutex_bridge);
                    *id = i;
                    return true;
                }
            }
        }
    }

    #ifdef WEBUI_LOG_VERBOSE
    printf("[Core]\t\t_webinix_get_cb_index() -> Not found\n");
    #endif

    _webinix_mutex_unlock(&_webinix.mutex_bridge);
    return false;
}

#ifdef WEBUI_LOG
static void _webinix_print_hex(const char* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("0x%02X ", (unsigned char)* data);
        data++;
    }
}
static void _webinix_print_ascii(const char* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if ((unsigned char)* data == 0x00)
            putchar(0xCF); // ¤
        else
            printf("%c", (unsigned char)* data);
        data++;
    }
}
#endif

// HTTP Server

static void _webinix_http_send_header(
    _webinix_window_t* win, struct mg_connection* client,
    const char* mime_type, size_t body_len, bool cache) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_http_send_header([%zu])\n", win->window_number);
    #endif

    const char* no_cache = "no-cache, no-store, must-revalidate, private, max-age=0";
    const char* with_cache = "public, max-age=31536000";
    const char* cache_header = (cache ? with_cache : no_cache);

    // Cookies
    bool set_cookies = false;
    size_t new_client_id = 0;
    if (_webinix.config.use_cookies) {
        // Cookies config is enabled
        char cookies[WEBUI_COOKIES_BUF] = {0};
        _webinix_get_cookies(client, cookies);
        bool client_found = false;
        if (!_webinix_is_empty(cookies)) {
            size_t client_id = 0;
            if (_webinix_client_cookies_get_id(win, cookies, &client_id)) {
                client_found = true;
            }
        }
        if (!client_found) {
            // Browser does not have cookies yet, let's set a new cookies
            char new_auth_cookies[WEBUI_COOKIES_BUF];
            _webinix_generate_cookies(new_auth_cookies, WEBUI_COOKIES_LEN);
            if (_webinix_client_cookies_save(win, new_auth_cookies, &new_client_id)) {
                set_cookies = true;
                _webinix.cookies_single_set[win->window_number] = true;
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_http_send() -> New auth cookies [%s]\n",
                    _webinix.cookies[new_client_id]
                );
                #endif
            }
            else {
                // Cookies list is full
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_http_send() -> Cookies list is full\n");
                #endif
            }
        }
    }

    // [header][body]
    size_t buffer_len = (128 + body_len);
    char* buffer = (char*)_webinix_malloc(buffer_len);
    int to_send = 0;
    if (set_cookies) {
        // Header with auth cookies
        to_send = WEBUI_SN_PRINTF_DYN(buffer, buffer_len,
            "HTTP/1.1 200 OK\r\n"
            "Set-Cookie: webinix_auth=%s; Path=/; HttpOnly\r\n"
            "Cache-Control: %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n\r\n",
            _webinix.cookies[new_client_id],
            cache_header, mime_type, body_len
        );
    }
    else {
        // Header without auth cookies
        to_send = WEBUI_SN_PRINTF_DYN(buffer, buffer_len,
            "HTTP/1.1 200 OK\r\n"
            "Cache-Control: %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n\r\n",
            cache_header, mime_type, body_len
        );
    }

    // Send
    mg_write(client, buffer, to_send);
    _webinix_free_mem((void*)buffer);
}

static void _webinix_http_send_file(
    _webinix_window_t* win, struct mg_connection* client,
    const char* mime_type, const char* path, bool cache) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_http_send_file([%zu])\n", win->window_number);
    #endif

    // Open the file
    FILE* file = NULL;
    if (WEBUI_FILE_OPEN(file, path, "rb") != 0 || !file) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_http_send_file() -> Can't open file [%s]\n", path);
        #endif
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Send header
    _webinix_http_send_header(win, client, mime_type, file_size, cache);

    // Send body
    mg_send_file_body(client, path);

    fclose(file);
}

static void _webinix_http_send(
    _webinix_window_t* win, struct mg_connection* client,
    const char* mime_type, const char* body, size_t body_len, bool cache) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_http_send([%zu])\n", win->window_number);
    #endif

    // Send header
    _webinix_http_send_header(win, client, mime_type, body_len, cache);

    // Send body
    mg_write(client, body, body_len);
}

static void _webinix_http_send_error(struct mg_connection* client, const char* body, int status) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_http_send_error()\n");
    #endif

    // [header][body]
    int to_send = 0;
    size_t body_len = _webinix_strlen(body);
    size_t buffer_len = (512 + body_len);
    char* buffer = (char*)_webinix_malloc(buffer_len);
    to_send = WEBUI_SN_PRINTF_DYN(buffer, buffer_len,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Cache-Control: no-cache, no-store, must-revalidate, private, max-age=0\r\n"
        "Pragma: no-cache\r\n"
        "Expires: 0\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n%s",
        body_len, body
    );

    // Send
    mg_write(client, buffer, to_send);
    _webinix_free_mem((void*)buffer);
}

#ifdef WEBUI_LOG
static int _webinix_http_log(const struct mg_connection* client, const char* message) {
    (void)client;
    printf("[Core]\t\t_webinix_http_log()\n");
    printf("[Core]\t\t_webinix_http_log() -> Log: %s.\n", message);
    return 1;
}
#endif

// static size_t _webinix_hash_djb2(const char* s) {
//     // DJB2 Algorithm
//     size_t hash = 5381;
//     int c;
//     while ((c = *s++)) {
//         // hash * 33 + c
//         hash = ((hash << 5) + hash) + c;
//     }
//     return hash;
// }

static void _webinix_generate_cookies(char* cookies, size_t length) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_generate_cookies()\n");
    #endif
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charset_size = sizeof(charset) - 1;
    for (size_t i = 0; i < length - 1; i++) {
        uint32_t random_value = _webinix_generate_random_uint32();
        int key = random_value % charset_size;
        cookies[i] = charset[key];
    }
    cookies[length - 1] = '\0';
}

static bool _webinix_client_cookies_save(_webinix_window_t* win, const char* cookies, size_t* client_id) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_cookies_save()\n");
    #endif
    // [win number][_][cookies]
    char win_cookies[WEBUI_COOKIES_BUF];
    WEBUI_SN_PRINTF_STATIC(win_cookies, sizeof(win_cookies), "%zu_%s", win->window_number, cookies);
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.cookies[i] == NULL) {
            _webinix.cookies[i] = _webinix_str_dup(win_cookies);
            *client_id = i;
            return true;
        }
    }
    // List is full
    return false;
}

static bool _webinix_client_cookies_get_id(_webinix_window_t* win, const char* cookies, size_t* client_id) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_cookies_get_id()\n");
    #endif
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.cookies[i] != NULL) {
            if (strcmp(_webinix.cookies[i], cookies) == 0) {
                *client_id = i;
                return true;
            }
        }
    }
    // Not found
    return false;
}

static size_t _webinix_client_get_id(_webinix_window_t* win, struct mg_connection* client) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_get_id()\n");
    #endif
    size_t client_id = 0;
    if (_webinix.config.use_cookies) {
        char cookies[WEBUI_COOKIES_BUF] = {0};
        _webinix_get_cookies(client, cookies);
        _webinix_client_cookies_get_id(win, cookies, &client_id);
    }
    return client_id;
}

// static void _webinix_client_cookies_free_all(_webinix_window_t* win) {
//     #ifdef WEBUI_LOG
//     printf("[Core]\t\t_webinix_client_cookies_free_all()\n");
//     #endif
//     // [win number]
//     char win_num[24];
//     WEBUI_SN_PRINTF_STATIC(win_num, sizeof(win_num), "%zu_", win->window_number);
//     size_t len = strlen(win_num);
//     for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
//         if (_webinix.cookies[i] != NULL) {
//             // [{win_number}_{cookies}]
//             if (strncmp(_webinix.cookies[i], win_num, len) == 0) {
//                 _webinix_free_mem((void*)_webinix.cookies[i]);
//                 _webinix.cookies[i] = NULL;
//             }
//         }
//     }
//     // Single
//     _webinix.cookies_single_set[win->window_number] = false;
// }

// static void _webinix_client_cookies_free(_webinix_window_t* win, struct mg_connection* client) {
//     #ifdef WEBUI_LOG
//     printf("[Core]\t\t_webinix_client_cookies_free()\n");
//     #endif
//     char cookies[WEBUI_COOKIES_BUF] = {0};
//     _webinix_get_cookies(client, cookies);
//     if (cookies != NULL) {
//         size_t client_id = 0;
//         if (_webinix_client_cookies_get_id(win, cookies, &client_id)) {
//             _webinix_free_mem((void*)_webinix.cookies[client_id]);
//             _webinix.cookies[client_id] = NULL;
//         }
//     }
// }

static const char* _webinix_get_cookies_full(const struct mg_connection* client) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_cookies_full()\n");
    #endif
    const char* header = mg_get_header(client, "Cookie");
    if (header != NULL)
        return header;
    return "";
}

static void _webinix_get_cookies(const struct mg_connection* client, char* buffer) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_cookies()\n");
    #endif
    const char* header = mg_get_header(client, "Cookie");
    if (!_webinix_is_empty(header)) {
        mg_get_cookie(header, "webinix_auth", buffer, WEBUI_COOKIES_BUF);
    }
}

static int _webinix_http_handler(struct mg_connection* client, void * _win) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_http_handler()\n");
    #endif

    // Mutex
    _webinix_mutex_lock(&_webinix.mutex_http_handler);

    // Get the window object
    _webinix_window_t* win = _webinix_dereference_win_ptr(_win);
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || win == NULL) {
        _webinix_mutex_unlock(&_webinix.mutex_http_handler);
        return 500; // Internal Server Error
    }

    // Initializing
    int http_status_code = 200;
    const struct mg_request_info * ri = mg_get_request_info(client);
    const char* url = ri->local_uri;

    if (strcmp(ri->request_method, "GET") == 0) {

        // GET

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_http_handler() -> GET [%s]\n", url);
        #endif

        // Cookies
        size_t client_id = 0;
        bool client_found = false;
        if (_webinix.config.use_cookies) {
            // Cookies config is enabled
            char cookies[WEBUI_COOKIES_BUF] = {0};
            _webinix_get_cookies(client, cookies);
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_http_handler() -> Client cookies [%s]\n", cookies);
            #endif
            // Get client ID based on `webinix_auth` cookies
            if (_webinix_client_cookies_get_id(win, cookies, &client_id)) {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_http_handler() -> Client ID found [%zu]\n", client_id);
                #endif
                client_found = true;
            }
            #ifdef WEBUI_LOG
            else {
                printf("[Core]\t\t_webinix_http_handler() -> Client ID not found\n");
            }
            #endif
        }
        
        // Single client authorisation
        if (!_webinix.config.multi_client) {
            if (!client_found) {
                if ((_webinix.cookies_single_set[win->window_number])) {
                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webinix_http_handler() -> 403 Forbidden\n");
                    #endif
                    _webinix_http_send_error(client, webinix_html_served, 403);
                    _webinix_mutex_unlock(&_webinix.mutex_http_handler);
                    return 403;                    
                }
            } else _webinix.cookies_single_set[win->window_number] = true;
        }

        // Let the server thread waits more time for `webinix.js`
        win->wait = true;

        if (strcmp(url, "/webinix.js") == 0) {

            // Webinix Bridge

            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_http_handler() -> Webinix-Bridge\n");
            #endif

            // Generate JavaScript bridge
            const char* js = _webinix_generate_js_bridge(win, client);

            if (js != NULL) {
                // Send 200
                _webinix_http_send(win, client, "application/javascript", js, _webinix_strlen(js), false);
                _webinix_free_mem((void*)js);
            } else {
                // Non-authorized request to `webinix.js`, like requesting twice
                // Send 200 (Empty)
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_http_handler() -> Non-authorized request to webinix.js\n");
                #endif
                _webinix_http_send(win, client, "application/javascript", "", 0, false);
            }
        }
        else if (strcmp(url, "/") == 0) {

            // [/]

            if (win->is_embedded_html) {

                // Main HTML

                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_http_handler() -> Embedded Index HTML\n");
                #endif

                // Send 200
                _webinix_http_send(win, client, "text/html", win->html, _webinix_strlen(win->html), false);
            }
            else {

                // Looking for index file and redirect

                const char* index_files[] = {"index.ts", "index.js", "index.html", "index.htm"};

                // [Path][Sep][File Name]
                size_t bf_len = (_webinix_strlen(win->server_root_path) + 1 + 24);
                char* index_path = (char*)_webinix_malloc(bf_len);
                for (size_t i = 0; i < (sizeof(index_files) / sizeof(index_files[0])); i++) {
                    WEBUI_SN_PRINTF_DYN(index_path, bf_len, "%s%s%s",
                        win->server_root_path, webinix_sep, index_files[i]
                    );
                    if (_webinix_file_exist(index_path)) {
                        #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webinix_http_handler() -> 302 Redirecting to [%s]\n", index_files[i]);
                        #endif
                        mg_send_http_redirect(client, index_files[i], 302);
                        _webinix_free_mem((void*)index_path);
                        _webinix_mutex_unlock(&_webinix.mutex_http_handler);
                        return 302;
                    }
                }

                // No index file is found in this folder
                _webinix_free_mem((void*)index_path);
                _webinix_http_send_error(client, webinix_html_res_not_available, 404);
                http_status_code = 404;
            }
        }
        else if (strcmp(url, "/favicon.ico") == 0 || strcmp(url, "/favicon.svg") == 0) {

            // Favicon

            if (win->icon != NULL && win->icon_type != NULL) {

                // Custom user icon

                // User icon 200
                _webinix_http_send(win, client, win->icon_type, win->icon, _webinix_strlen(win->icon), false);
            }
            else if (_webinix_file_exist_mg(win, client)) {

                // Local icon file
                http_status_code = _webinix_serve_file(win, client, client_id);
            }
            else {

                // Default embedded icon

                if (strcmp(url, "/favicon.ico") == 0) {

                    mg_send_http_redirect(client, "favicon.svg", 302);
                    http_status_code = 302;
                }
                else {

                    // Default icon 200
                    _webinix_http_send(win, client, webinix_def_icon_type, webinix_def_icon, _webinix_strlen(webinix_def_icon), false);
                }
            }
        } else {

            // [/file] or [/folder]

            // [Path][Sep][folder]
            char* folder_path = _webinix_get_full_path(win, url);

            if (_webinix_folder_exist(folder_path)) {

                // [/folder]
                
                // Looking for index file and redirect

                const char* index_files[] = {"index.ts", "index.js", "index.html", "index.htm"};

                // [Path][Sep][File Name]
                size_t bf_len = (_webinix_strlen(folder_path) + 1 + 24);
                char* index_path = (char*)_webinix_malloc(bf_len);
                for (size_t i = 0; i < (sizeof(index_files) / sizeof(index_files[0])); i++) {
                    WEBUI_SN_PRINTF_DYN(index_path, bf_len, "%s%s%s", folder_path, webinix_sep, index_files[i]);
                    if (_webinix_file_exist(index_path)) {
                        // [URL][/][Index Name]
                        size_t redirect_len = (_webinix_strlen(url) + 1 + 24);
                        char* redirect_url = (char*)_webinix_malloc(bf_len);
                        WEBUI_SN_PRINTF_DYN(redirect_url, redirect_len, "%s%s%s", url, "/", index_files[i]);
                        #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webinix_http_handler() -> 302 Redirecting to [%s]\n", redirect_url);
                        #endif
                        mg_send_http_redirect(client, redirect_url, 302);
                        _webinix_free_mem((void*)redirect_url);
                        _webinix_free_mem((void*)folder_path);
                        _webinix_free_mem((void*)index_path);
                        _webinix_mutex_unlock(&_webinix.mutex_http_handler);
                        return 302;
                    }
                }

                // No index file is found in this folder
                _webinix_free_mem((void*)folder_path);
                _webinix_free_mem((void*)index_path);
                _webinix_http_send_error(client, webinix_html_res_not_available, 404);
                http_status_code = 404;
            }
            else {

                // [/file]

                bool script = false;
                if (win->runtime != None) {
                    const char* extension = _webinix_get_extension(url);
                    const char* index_extensions[] = {
                        "js", "ts"
                    };
                    for (size_t i = 0; i < (sizeof(index_extensions) / sizeof(index_extensions[0])); i++) {
                        if (strcmp(extension, index_extensions[i]) == 0) {
                            script = true;
                            break;
                        }
                    }
                }

                if (script) {

                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webinix_http_handler() -> Interpret local script file\n");
                    #endif

                    // Serve as a script file to be interpreted by
                    // an external interpreter (Deno, Nodejs)
                    http_status_code = _webinix_interpret_file(win, client, NULL, client_id);
                }
                else {

                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webinix_http_handler() -> Local file\n");
                    #endif

                    // Serve as a normal text-based file
                    http_status_code = _webinix_serve_file(win, client, client_id);
                }
            }

            // Clear
            _webinix_free_mem((void*)folder_path);
        }
    } else {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_http_handler() -> Unknown request method [%s]\n", ri->request_method);
        #endif
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_http_handler() -> HTTP Status Code: %d\n", http_status_code);
    #endif

    _webinix_mutex_unlock(&_webinix.mutex_http_handler);
    return http_status_code;
}

static int _webinix_ws_connect_handler(const struct mg_connection* client, void * _win) {
    (void)client;
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_ws_connect_handler()\n");
    #endif

    // Dereference
    _webinix_window_t* win = _webinix_dereference_win_ptr(_win);
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || win == NULL)
        return 1;
    
    // Check connection status
    if (!_webinix.config.multi_client) {
        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
            // Multi-client is disabled, and the single client already connected.
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_ws_connect_handler() -> Single client already connected\n");
            printf("[Core]\t\t_webinix_ws_connect_handler() -> Non-authorized connection\n");
            #endif
            // Block handshake
            return 1;
        }
    }
    
    // Cookies
    size_t client_id = 0;
    bool client_found = false;
    if (_webinix.config.use_cookies) {
        // Cookies config is enabled
        char cookies[WEBUI_COOKIES_BUF] = {0};
        _webinix_get_cookies(client, cookies);
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_ws_connect_handler() -> Client cookies [%s]\n", cookies);
        #endif
        // Get client ID based on `webinix_auth` cookies
        if (_webinix_client_cookies_get_id(win, cookies, &client_id)) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_ws_connect_handler() -> Client ID found [%zu]\n", client_id);
            #endif
            client_found = true;
        }
        #ifdef WEBUI_LOG
        else {
            printf("[Core]\t\t_webinix_ws_connect_handler() -> Client ID not found\n");
        }
        #endif
    }
    
    // Single client authorisation
    if (!_webinix.config.multi_client) {
        if (!client_found) {
            if ((_webinix.cookies_single_set[win->window_number])) {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_ws_connect_handler() -> 403 Forbidden\n");
                #endif
                // Block handshake
                return 1;
            }
        }
    }
    
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_ws_connect_handler() -> Connection authentication OK\n");
    #endif

    // OK. Process handshake
    return 0;
}

static void _webinix_ws_ready_handler(struct mg_connection* client, void * _win) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_ws_ready_handler()\n");
    #endif

    // Dereference
    _webinix_window_t* win = _webinix_dereference_win_ptr(_win);
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || win == NULL)
        return;

    _webinix_receive(win, client, WEBUI_WS_OPEN, NULL, 0);
}

static int _webinix_ws_data_handler(struct mg_connection* client, int opcode, char* data, size_t datasize, void * _win) {
    (void)client;
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_ws_data_handler()\n");
    #endif

    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || datasize < WEBUI_PROTOCOL_SIZE)
        return 1; // OK

    switch(opcode&0xf) {

        case MG_WEBSOCKET_OPCODE_BINARY: {
            _webinix_window_t* win = _webinix_dereference_win_ptr(_win);
            if (win != NULL)
                _webinix_receive(win, client, WEBUI_WS_DATA, data, datasize);
            break;
        }
        case MG_WEBSOCKET_OPCODE_TEXT: {
            break;
        }
        case MG_WEBSOCKET_OPCODE_PING: {
            break;
        }
        case MG_WEBSOCKET_OPCODE_PONG: {
            break;
        }
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_ws_data_handler() -> Finished\n");
    #endif

    // OK
    return 1;
}

static void _webinix_ws_close_handler(const struct mg_connection* client, void * _win) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_ws_close_handler()\n");
    #endif

    // Dereference
    _webinix_window_t* win = _webinix_dereference_win_ptr(_win);
    if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE) || win == NULL || !_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
        return;

    _webinix_receive(win, (struct mg_connection*)client, WEBUI_WS_CLOSE, NULL, 0);
}

static WEBUI_THREAD_SERVER_START {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_server_thread()\n");
    #endif

    // Mutex
    _webinix_mutex_lock(&_webinix.mutex_server_start);

    _webinix_window_t* win = _webinix_dereference_win_ptr(arg);
    if (win == NULL || win->server_running) {
        _webinix_mutex_unlock(&_webinix.mutex_server_start);
        WEBUI_THREAD_RETURN
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_server_thread([%zu]) -> URL: [%s]\n", win->window_number, win->url);
    #endif

    // Folder monitor thread
    bool monitor_created = false;
    #ifdef _WIN32
    HANDLE monitor_thread = NULL;
    #else
    pthread_t monitor_thread;
    #endif

    // Initialization
    _webinix.servers++;
    win->server_running = true;
    if (_webinix.startup_timeout < 1)
        _webinix.startup_timeout = 0;
    if (_webinix.startup_timeout > WEBUI_MAX_TIMEOUT)
        _webinix.startup_timeout = WEBUI_MAX_TIMEOUT;

    // Public host access
    char host[16] = {0};
    if (!win->is_public)
        // Private localhost access
        WEBUI_STR_COPY_STATIC(host, sizeof(host), "127.0.0.1:");

    #ifdef WEBUI_TLS
    // HTTP Secure Port
    char* server_port = (char*)_webinix_malloc(64);
    WEBUI_SN_PRINTF_DYN(server_port, 64, "%s%zus", host, win->server_port);
    #else
    // HTTP Port
    char* server_port = (char*)_webinix_malloc(64);
    WEBUI_SN_PRINTF_DYN(server_port, 64, "%s%zu", host, win->server_port);
    #endif

    // Server Options
    const char* http_options[] = {
        // HTTP
        "listening_ports", server_port,
        "document_root", win->server_root_path,
        "access_control_allow_headers", "*",
        "access_control_allow_methods", "*",
        "access_control_allow_origin", "*",
        #ifdef WEBUI_TLS
        "authentication_domain", "localhost",
        "enable_auth_domain_check", "no",
        "ssl_protocol_version", "4",
        "ssl_cipher_list", "ECDH+AESGCM+AES256:!aNULL:!MD5:!DSS",
        "strict_transport_security_max_age", WEBUI_SSL_EXPIRE_STR,
        #endif
        // WS
        "websocket_timeout_ms", "3600000",
        "enable_websocket_ping_pong", "yes",
        NULL, NULL
    };

    // Server Settings
    struct mg_callbacks http_callbacks;
    struct mg_context * http_ctx = NULL;
    memset(&http_callbacks, 0, sizeof(http_callbacks));
    #ifdef WEBUI_TLS
    http_callbacks.init_ssl = _webinix_tls_initialization;
    #endif
    #ifdef WEBUI_LOG
    http_callbacks.log_message = _webinix_http_log;
    #endif

    // Start Server
    http_ctx = mg_start(&http_callbacks, 0, http_options);
    mg_set_request_handler(http_ctx, "/", _webinix_http_handler, (void*)win);

    if (http_ctx) {

        mg_set_websocket_handler(
            http_ctx, "/_webinix_ws_connect", _webinix_ws_connect_handler, _webinix_ws_ready_handler,
            _webinix_ws_data_handler, _webinix_ws_close_handler, (void*)win
        );

        // Mutex
        _webinix_mutex_unlock(&_webinix.mutex_server_start);

        if (_webinix.startup_timeout > 0) {

            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> Listening Success\n", 
                win->window_number);
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> HTTP/WS Port: %s\n", 
                win->window_number, server_port);
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> Root path: %s\n", 
                win->window_number, win->server_root_path);
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> Timeout is %zu seconds\n", 
                win->window_number, _webinix.startup_timeout);
            #endif

            bool stop = false;
            win->wait = false;

            while(!stop) {

                if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {

                    // UI is not connected

                    #ifdef WEBUI_LOG
                    printf(
                        "[Core]\t\t_webinix_server_thread([%zu]) -> Waiting for connection\n",
                        win->window_number
                    );
                    #endif

                    // Wait for first connection
                    _webinix_timer_t timer_1;
                    _webinix_timer_start(&timer_1);
                    for (;;) {

                        _webinix_sleep(1);

                        // Stop if window is connected
                        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
                            break;

                        // Stop if timer is finished (Default WEBUI_DEF_TIMEOUT)
                        if (_webinix_timer_is_end(&timer_1, (_webinix.startup_timeout * 1000)))
                            break;
                    }

                    if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE) && win->wait) {

                        // At this moment the browser is already started and HTML
                        // files are already handled, let's wait more time to give
                        // the WebSocket an extra seconds to connect. This is helpful
                        // when a web app have too many files to handel before `webinix.js`
                        // get requested.

                        do {
                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_server_thread([%zu]) -> Waiting more for connection\n",
                                win->window_number
                            );
                            #endif

                            win->wait = false;

                            _webinix_timer_t timer_2;
                            _webinix_timer_start(&timer_2);
                            for (;;) {

                                // Stop if window is connected
                                _webinix_sleep(1);
                                if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
                                    break;

                                // Stop if timer is finished
                                if (_webinix_timer_is_end(&timer_2, 5000))
                                    break;
                            }
                        } while(win->wait && !_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE));
                    }

                    if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
                        stop = true; // First run failed
                }
                else {

                    // UI is connected
                    win->is_closed = false;

                    #ifdef WEBUI_LOG
                    printf(
                        "[Core]\t\t_webinix_server_thread([%zu]) -> Window Connected.\n",
                        win->window_number
                    );
                    #endif

                    // Folder monitor thread
                    if (_webinix.config.folder_monitor && !monitor_created) {
                        monitor_created = true;
                        _webinix_monitor_arg_t* arg = (_webinix_monitor_arg_t* ) _webinix_malloc(sizeof(_webinix_monitor_arg_t));
                        arg->win_num = win->window_number;
                        arg->path = win->server_root_path;
                        #ifdef _WIN32
                        monitor_thread = CreateThread(NULL, 0, _webinix_folder_monitor_thread, (void*)arg, 0, NULL);
                        if (monitor_thread != NULL)
                            CloseHandle(monitor_thread);
                        #else
                        pthread_create(&monitor_thread, NULL, &_webinix_folder_monitor_thread, (void*)win);
                        pthread_detach(monitor_thread);
                        #endif
                    }

                    while(!stop) {

                        // Wait forever for disconnection

                        _webinix_sleep(1);

                        // Exit signal
                        if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE)) {
                            stop = true;
                            break;
                        }

                        if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {

                            // The UI is just get disconnected

                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_server_thread([%zu]) -> Window disconnected\n",
                                win->window_number
                            );
                            #endif

                            if (!win->is_closed) {

                                // probably the user did a refresh
                                // let's wait for re-connection...

                                do {
                                    #ifdef WEBUI_LOG
                                    printf(
                                        "[Core]\t\t_webinix_server_thread([%zu]) -> Waiting for reconnection\n",
                                        win->window_number
                                    );
                                    #endif

                                    win->wait = false;

                                    _webinix_timer_t timer_3;
                                    _webinix_timer_start(&timer_3);
                                    for (;;) {

                                        // Stop if window is re-connected
                                        _webinix_sleep(1);
                                        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE))
                                            break;

                                        // Stop if timer is finished
                                        if (_webinix_timer_is_end(&timer_3, WEBUI_RELOAD_TIMEOUT))
                                            break;
                                    }
                                } while(win->wait && !_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE));

                                if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
                                    stop = true;
                                    break;
                                }
                            }
                            else {

                                // Window get closed
                                stop = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Let's check the flag again, there is a change that
        // the flag has ben changed during the first loop for
        // example when set_timeout() get called after show()
        if (_webinix.startup_timeout == 0) {

            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> Listening Success\n", 
                win->window_number);
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> HTTP/WS Port: %s\n", 
                win->window_number, server_port);
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> Root path: %s\n", 
                win->window_number, win->server_root_path);
            printf("[Core]\t\t_webinix_server_thread([%zu]) -> Infinite loop\n", 
                win->window_number);
            #endif

            // Wait forever
            for (;;) {
                _webinix_sleep(1);
                if (_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE))
                    break;
            }
        }
    } else {

        #ifdef WEBUI_LOG
        if (!http_ctx) printf("[Core]\t\t_webinix_server_thread([%zu]) -> Listening failed.\n", win->window_number);
        #endif

        // Mutex
        _webinix_mutex_unlock(&_webinix.mutex_server_start);
    }

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_server_thread([%zu]) -> Cleaning\n", win->window_number);
    #endif

    _webinix_mutex_is_connected(win, WEBUI_MUTEX_FALSE);

    // Clean monitor thread
    #ifdef _WIN32
    TerminateThread(monitor_thread, 0);
    CloseHandle(monitor_thread);
    #else
    pthread_cancel(monitor_thread);
    pthread_join(monitor_thread, NULL);
    #endif

    // Clean WebView
    if (win->webView) {
        win->webView->stop = true;
        _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);
    }

    // Clean
    win->server_running = false;
    _webinix_free_port(win->server_port);
    win->server_port = 0;
    _webinix_free_mem((void*)server_port);
    // _webinix_client_cookies_free_all(win);

    // Kill Process
    // _webinix_kill_pid(win->process_id);
    // win->process_id = 0;

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_server_thread([%zu]) -> Server stopped.\n", win->window_number);
    #endif

    // Let the main wait() know that
    // this server thread is finished
    if (_webinix.servers > 0)
        _webinix.servers--;

    // Stop server services
    // This should be at the
    // end as it may take time
    mg_stop(http_ctx);

    // Fire the mutex condition for wait()
    if (_webinix.startup_timeout > 0 && _webinix.servers < 1) {

        // Stop all threads
        _webinix.ui = false;
        _webinix_mutex_is_exit_now(WEBUI_MUTEX_TRUE);
        // Break main loop
        _webinix_condition_signal(&_webinix.condition_wait);
        #ifdef __APPLE__
        _webinix_macos_wv_stop();
        #endif
    }

    WEBUI_THREAD_RETURN
}

static void _webinix_receive(_webinix_window_t* win, struct mg_connection* client,
    int event_type, void * data, size_t len) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_receive([%zu], [%d], [%zu])\n", win->window_number, event_type, len);
    #endif

    static size_t recvNum = 0;
    static bool multi_packet = false;
    static size_t multi_expect = 0;
    static size_t multi_receive = 0;
    static void * multi_buf = NULL;

    // Get connection id
    size_t connection_id = 0;
    if (event_type != WEBUI_WS_OPEN) {
        if (!_webinix_connection_get_id(win, client, &connection_id)) {
            // Failed to find connection ID
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_receive() -> Failed to find connection ID\n");
            #endif
            _webinix_connection_remove(win, client);
            return;
        }
    }

    // Websocket Open/Close Events
    if (event_type == WEBUI_WS_OPEN) {
        // New connection
        // Autorisation to register
        bool authorization = false;
        if (!_webinix.config.multi_client) {
            if (!_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
                authorization = true;
            }
        } else authorization = true;
        if (!authorization) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_receive(%zu) -> Connection not authorized to register\n",
                recvNum);
            #endif
            _webinix_connection_remove(win, client);
            return;
        }
        else {
            // Register
            if (_webinix_connection_save(win, client, &connection_id)) {
                // Update window connection status
                _webinix_mutex_is_connected(win, WEBUI_MUTEX_TRUE);
                #ifdef WEBUI_LOG
                printf(
                    "[Core]\t\t_webinix_receive(%zu) -> Connection #%zu registered\n",
                    recvNum, connection_id
                );
                #endif
            }
            else {
                // Connection failed to register
                _webinix_connection_remove(win, client);
                return;
            }
        }
    } else if (event_type == WEBUI_WS_CLOSE) {
        // Connection close
        _webinix_connection_remove(win, client);
        #ifdef WEBUI_LOG
        printf(
            "[Core]\t\t_webinix_receive(%zu) -> Connection #%zu Closed\n",
            recvNum, connection_id
        );
        #endif
    }

    // Multi Packet (big data)
    if (multi_packet) {
        if ((multi_receive + len) > multi_expect) {
            // Received more data than expected
            #ifdef WEBUI_LOG
            printf(
                "[Core]\t\t_webinix_receive() -> Multi packet received more data than expected (%zu + %zu > %zu).\n",
                multi_receive, len, multi_expect
            );
            #endif
            multi_packet = false;
            multi_expect = 0;
            multi_receive = 0;
            _webinix_free_mem(multi_buf);
            multi_buf = NULL;
            return;
        }
        // Accumulate packet
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_receive() -> Multi packet accumulate %zu bytes\n", len);
        #endif
        memcpy(((unsigned char*)multi_buf + multi_receive), data, len);
        multi_receive += len;
        // Check if theire is more packets comming
        if (multi_receive < multi_expect)
            return;
    }
    else if (event_type == WEBUI_WS_DATA) {
        if (((unsigned char*)data)[WEBUI_PROTOCOL_CMD] == WEBUI_CMD_MULTI) {
            if (len >= WEBUI_PROTOCOL_SIZE && ((unsigned char*)data)[WEBUI_PROTOCOL_SIGN] == WEBUI_SIGNATURE) {
                size_t expect_len = (size_t) strtoul(&((const char*)data)[WEBUI_PROTOCOL_DATA], NULL, 10);
                if (expect_len > 0 && expect_len <= WEBUI_MAX_BUF) {
                    #ifdef WEBUI_LOG
                    printf(
                        "[Core]\t\t_webinix_receive() -> Multi packet started, Expecting %zu bytes\n",
                        expect_len
                    );
                    #endif
                    multi_buf = _webinix_malloc(expect_len);
                    memcpy(multi_buf, data, len);
                    multi_receive = 0;
                    multi_expect = expect_len;
                    multi_packet = true;
                }
            }
            return;
        }
    }

    // Generate args
    void * arg_ptr = NULL;
    size_t arg_len = 0;
    if (multi_packet) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_receive() -> Processing multi packet\n");
        #endif
        // Get data from accumulated multipackets
        arg_len = multi_receive;
        arg_ptr = multi_buf;
        // Reset
        multi_packet = false;
        multi_expect = 0;
        multi_receive = 0;
        multi_buf = NULL;
    } else {
        arg_len = len;
        if (len > 0) {
            if (win->ws_block) {
                // This event has data. And it will be processed
                // in this current thread, no need to copy data
                arg_ptr = data;
            }
            else {
                // This event has data. And it will be processed
                // in a new thread, we should copy data once
                void * data_cpy = (void*)_webinix_malloc(len);
                memcpy((char*)data_cpy, data, len);
                arg_ptr = data_cpy;
            }
        } else {
            // This is a WS connect/disconnect event
            arg_ptr = NULL;
        }
    }

    // Process
    if (win->ws_block) {
        // Process the packet in this current thread
        _webinix_ws_process(win, client, connection_id, arg_ptr, arg_len, ++recvNum, event_type);
        _webinix_free_mem((void*)arg_ptr);
    }
    else {
        // Process the packet in a new thread
        _webinix_recv_arg_t* arg = (_webinix_recv_arg_t* ) _webinix_malloc(sizeof(_webinix_recv_arg_t));
        arg->win = win;
        arg->ptr = arg_ptr;
        arg->len = arg_len;
        arg->recvNum = ++recvNum;
        arg->event_type = event_type;
        arg->client = client;
        arg->connection_id = connection_id;
        #ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, _webinix_ws_process_thread, (void*)arg, 0, NULL);
        if (thread != NULL)
            CloseHandle(thread);
        #else
        pthread_t thread;
        pthread_create(&thread, NULL,&_webinix_ws_process_thread, (void*)arg);
        pthread_detach(thread);
        #endif
    }
}

static bool _webinix_connection_save(_webinix_window_t* win, struct mg_connection* client, size_t* connection_id) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_save([%zu])\n", win->window_number);
    #endif

    // Save new ws client
    _webinix_mutex_lock(&_webinix.mutex_client);
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.clients[i] == NULL) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_client_save() -> Registering client #%zu \n", i);
            #endif
            // Save
            _webinix.clients[i] = client;
            _webinix.clients_token_check[i] = false;
            win->clients_count++;
            _webinix_mutex_unlock(&_webinix.mutex_client);
            *connection_id = i;
            return true;
        }
    }
    
    // List is full
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_save() -> Clients list is full\n");
    #endif
    _webinix_mutex_unlock(&_webinix.mutex_client);
    return false;
}

static void _webinix_connection_remove(_webinix_window_t* win, struct mg_connection* client) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_remove([%zu])\n", win->window_number);
    #endif

    _webinix_mutex_lock(&_webinix.mutex_client);

    // Remove a ws client
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.clients[i] == client) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_client_remove() -> Removing client #%zu \n", i);
            #endif
            // Reset Token
            if (!_webinix.config.multi_client) {
                if (_webinix.clients_token_check[i]) {
                    win->token = 0;
                }
            }
            // Clear
            _webinix.clients[i] = NULL;
            _webinix.clients_token_check[i] = false;
            if (win->clients_count > 0)
                win->clients_count--;
            // Close
            _webinix_mutex_unlock(&_webinix.mutex_client);
            mg_close_connection(client);
            return;
        }
    }

    // Client not found
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_remove() -> Client not found\n");
    #endif
    _webinix_mutex_unlock(&_webinix.mutex_client);
    mg_close_connection(client);
}

static bool _webinix_connection_get_id(_webinix_window_t* win, struct mg_connection* client, size_t* connection_id) {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_get_id([%zu], [%p])\n", win->window_number, client);
    #endif

    // Find a ws client
    _webinix_mutex_lock(&_webinix.mutex_client);
    for (size_t i = 0; i < WEBUI_MAX_IDS; i++) {
        if (_webinix.clients[i] == client) {
            *connection_id = i;
            _webinix_mutex_unlock(&_webinix.mutex_client);
            return true;
        }
    }

    // Client not found
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_client_get_id() -> Client not found\n");
    #endif
    _webinix_mutex_unlock(&_webinix.mutex_client);
    return false;
}

static void _webinix_ws_process(
    _webinix_window_t* win, struct mg_connection* client, size_t connection_id, 
    void* ptr, size_t len, size_t recvNum, int event_type) {
    
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_ws_process(%zu)\n", recvNum);
    #endif

    if (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE)) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_ws_process(%zu) -> Start\n", recvNum);
        #endif

        if (event_type == WEBUI_WS_DATA) {

            const char* packet = (const char*)ptr;
            uint32_t packet_token = _webinix_get_token(packet);
            uint16_t packet_id = _webinix_get_id(packet);

            #ifdef WEBUI_LOG
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> Data received\n",
                recvNum
            );
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> Packet Size : %zu bytes\n",
                recvNum, len
            );
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> Packet Header : [ ",
                recvNum
            );
            _webinix_print_hex(packet, WEBUI_PROTOCOL_SIZE);
            printf("]\n");
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> Packet Token: 0x%08X (%" PRIu32 ")\n",
                recvNum, packet_token, packet_token
            );
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> Packet ID: 0x%04X (%u)\n",
                recvNum, packet_id, packet_id
            );
            printf("[Core]\t\t_webinix_ws_process(%zu) -> Packet Data: [", recvNum);
                // _webinix_print_ascii(&packet[WEBUI_PROTOCOL_DATA], (len -
                // WEBUI_PROTOCOL_SIZE));
            printf("]\n");
            #endif

            if ((len >= WEBUI_PROTOCOL_SIZE) &&
                ((unsigned char)packet[WEBUI_PROTOCOL_SIGN] == WEBUI_SIGNATURE) &&
                (packet_token == win->token)) {

                // Mutex
                if (_webinix.config.ws_block) {
                    // wait for previous event to finish
                    if ((unsigned char)packet[WEBUI_PROTOCOL_CMD] != WEBUI_CMD_JS) {
                        _webinix_mutex_lock(&_webinix.mutex_receive);
                    }
                }

                if (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE)) { // Check if previous event called exit()

                    if ((unsigned char)packet[WEBUI_PROTOCOL_CMD] == WEBUI_CMD_CLICK) {

                        // Click Event

                        // Protocol
                        // [Header]
                        // [Element]

                        // Get html element id
                        char* element = (char*)&packet[WEBUI_PROTOCOL_DATA];
                        size_t element_len = _webinix_strlen(element);

                        #ifdef WEBUI_LOG
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> WEBUI_CMD_CLICK \n",
                            recvNum
                        );
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> Element size: %zu bytes \n",
                            recvNum, element_len
                        );
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> Element : [%s] \n",
                            recvNum, element
                        );
                        #endif

                        // New event inf (Click)
                        // webinix_event_inf_t* event_inf = NULL;
                        // size_t event_num = _webinix_new_event_inf(win, &event_inf);

                        // Set arguments
                        // ...

                        _webinix_window_event(
                            win, // Event -> Window
                            connection_id, // Event -> Client Unique ID
                            WEBUI_EVENT_MOUSE_CLICK, // Event -> Type of this event
                            element, // Event -> HTML Element
                            0, // Event -> Event Number
                            _webinix_client_get_id(win, client), // Event -> Client ID
                            _webinix_get_cookies_full(client) // Event -> Full cookies
                        );

                        // Free event
                        // _webinix_free_event_inf(win, event_num);
                    } else if ((unsigned char)packet[WEBUI_PROTOCOL_CMD] == WEBUI_CMD_JS) {

                        // JS Result

                        // Protocol
                        // [Header]
                        // [Error, ScriptResponse]

                        // Get pipe id
                        if (packet_id < WEBUI_MAX_IDS) {

                            if (_webinix.run_userBuffer[packet_id] != NULL) {

                                _webinix_mutex_lock(&_webinix.mutex_js_run);
                                _webinix.run_done[packet_id] = false;
                                _webinix_mutex_unlock(&_webinix.mutex_js_run);

                                // Get js-error
                                bool error = true;
                                if ((unsigned char)packet[WEBUI_PROTOCOL_DATA] == 0x00)
                                    error = false;

                                // Get data part
                                char* data = (char*)&packet[WEBUI_PROTOCOL_DATA + 1];
                                size_t data_len = _webinix_strlen(data);

                                #ifdef WEBUI_LOG
                                printf(
                                    "[Core]\t\t_webinix_ws_process(%zu) -> WEBUI_CMD_JS \n",
                                    recvNum
                                );
                                printf(
                                    "[Core]\t\t_webinix_ws_process(%zu) -> run_id = 0x%02x (%u) \n",
                                    recvNum, packet_id, packet_id
                                );
                                printf(
                                    "[Core]\t\t_webinix_ws_process(%zu) -> error = %s \n",
                                    recvNum, error ? "true" : "false"
                                );
                                printf(
                                    "[Core]\t\t_webinix_ws_process(%zu) -> %zu bytes of data\n",
                                    recvNum, data_len
                                );
                                printf(
                                    "[Core]\t\t_webinix_ws_process(%zu) -> data = [%s] @ 0x%p\n",
                                    recvNum, data, data
                                );
                                #endif

                                // Set pipe
                                _webinix.run_error[packet_id] = error;
                                if (data_len > 0) {

                                    // Copy response to the user's response buffer
                                    // directly
                                    size_t response_len = data_len + 1;
                                    size_t bytes_to_cpy =
                                        (response_len <=
                                            _webinix
                                            .run_userBufferLen[packet_id] ?
                                            response_len :
                                            _webinix
                                            .run_userBufferLen[packet_id]);
                                    memcpy(
                                        _webinix.run_userBuffer[packet_id], data,
                                        bytes_to_cpy
                                    );
                                } else {

                                    // Empty Result
                                    _webinix.run_userBuffer[packet_id] = 0x00;
                                }

                                // Send ready signal to webinix_script()
                                _webinix_mutex_lock(&_webinix.mutex_js_run);
                                _webinix.run_done[packet_id] = true;
                                _webinix_mutex_unlock(&_webinix.mutex_js_run);
                            }
                        }
                    } else if ((unsigned char)packet[WEBUI_PROTOCOL_CMD] == WEBUI_CMD_NAVIGATION) {

                        // Navigation Event

                        // Protocol
                        // [Header]
                        // [URL]

                        // Events
                        if (win->has_all_events) {

                            // Get URL
                            char* url = (char*)&packet[WEBUI_PROTOCOL_DATA];
                            size_t url_len = _webinix_strlen(url);

                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> WEBUI_CMD_NAVIGATION \n",
                                recvNum
                            );
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> URL size: %zu bytes \n",
                                recvNum, url_len
                            );
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> URL: [%s] \n",
                                recvNum, url
                            );
                            #endif

                            // New event inf (Navigation)
                            webinix_event_inf_t* event_inf = NULL;
                            size_t event_num = _webinix_new_event_inf(win, &event_inf);

                            // Set arguments
                            event_inf->event_data[0] = url;
                            event_inf->event_size[0] = url_len;

                            _webinix_window_event(
                                win, // Event -> Window
                                connection_id, // Event -> Client Unique ID
                                WEBUI_EVENT_NAVIGATION, // Event -> Type of this event
                                "", // Event -> HTML Element
                                event_num, // Event -> Event Number
                                _webinix_client_get_id(win, client), // Event -> Client ID
                                _webinix_get_cookies_full(client) // Event -> Full cookies
                            );

                            // Free event
                            _webinix_free_event_inf(win, event_num);
                        }
                    } else if ((unsigned char)packet[WEBUI_PROTOCOL_CMD] == WEBUI_CMD_CALL_FUNC) {

                        // Function Call

                        // Protocol
                        // [Header]
                        // [Fn, Null, {Len;Len;...}, Null, {Data,Null,Data,Null...}]

                        // Get html element id
                        char* element = (char*)&packet[WEBUI_PROTOCOL_DATA];
                        size_t element_len = _webinix_strlen(element);

                        #ifdef WEBUI_LOG
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> WEBUI_CMD_CALL_FUNC \n",
                            recvNum
                        );
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> Call ID: [%u] \n",
                            recvNum, packet_id
                        );
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> Element: [%s] \n",
                            recvNum, element
                        );
                        #endif

                        // New event inf (Function Call)
                        webinix_event_inf_t* event_inf = NULL;
                        size_t event_num = _webinix_new_event_inf(win, &event_inf);

                        // Loop trough args
                        size_t data_size_expected = 0;
                        char* args_lens = (char*)&packet[WEBUI_PROTOCOL_DATA + element_len + 1];
                        size_t args_len = _webinix_strlen(args_lens);
                        char* args_ptr = (char*)&packet[WEBUI_PROTOCOL_DATA + element_len + 1 + args_len + 1];
                        char* context;
                        char* tk = WEBUI_STR_TOK(args_lens, ";", &context);
                        size_t tk_num = 0;
                        while(tk != NULL && tk_num < WEBUI_MAX_ARG + 1) {

                            size_t arg_len = (size_t) strtoul(tk, NULL, 10);
                            data_size_expected = data_size_expected + arg_len + 1;

                            if (arg_len > 0) {

                                // Set argument
                                event_inf->event_size[tk_num] = arg_len;
                                event_inf->event_data[tk_num] = args_ptr;
                            }

                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> Argument %zu: %zu bytes\n",
                                recvNum, tk_num, arg_len
                            );
                            #endif

                            args_ptr = args_ptr + (arg_len + 1);
                            tk = WEBUI_STR_TOK(NULL, ";", &context);

                            // Save total found arguments
                            event_inf->count = ++tk_num;
                        }

                        // Check data validity
                        size_t data_size_recv = len - (WEBUI_PROTOCOL_SIZE + // [Header]
                            element_len + // [Fn]
                            1 + // [Null]
                            args_len + // [{Len;Len;...}]
                            1); // [Null]

                        if (data_size_expected == data_size_recv) {

                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> Expected and received %zu bytes of data.\n",
                                recvNum, data_size_expected
                            );
                            #endif

                            // Create new event (Function Call)
                            webinix_event_t e;
                            e.window = win->window_number;
                            e.event_type = WEBUI_EVENT_CALLBACK;
                            e.element = element;
                            e.event_number = event_num;
                            e.connection_id = connection_id;
                            e.client_id = _webinix_client_get_id(win, client);
                            e.cookies = _webinix_get_cookies_full(client); // Full cookies

                            // Call user function
                            size_t cb_index = 0;
                            bool exist = _webinix_get_cb_index(win, element, &cb_index);
                            if (exist && win->cb[cb_index] != NULL) {

                                // Call user cb
                                #ifdef WEBUI_LOG
                                printf(
                                    "[Core]\t\t_webinix_ws_process(%zu) -> Calling user callback\n[Call]\n",
                                    recvNum
                                );
                                #endif
                                e.bind_id = cb_index;
                                win->cb[cb_index](&e);
                            }

                            // Check the response
                            if (_webinix_is_empty(event_inf->response))
                                event_inf->response = (char*)"";

                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> user-callback response [%s]\n",
                                recvNum, event_inf->response
                            );
                            #endif

                            // Packet Protocol Format:
                            // [...]
                            // [CMD]
                            // [CallResponse]

                            // Send the packet
                            _webinix_send_client(
                                win, client, packet_id, WEBUI_CMD_CALL_FUNC,
                                event_inf->response, _webinix_strlen(event_inf->response)
                            );

                            // Free event
                            _webinix_free_event_inf(win, event_num);
                        } else {

                            // WebSocket/Civetweb did not send all the data as expected.
                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> No enough data received. "
                                "Expected %zu bytes, received %zu bytes.\n",
                                recvNum, data_size_expected, data_size_recv
                            );
                            #endif

                            // Send a void response to solve `.call()` promise

                            // Packet Protocol Format:
                            // [...]
                            // [CMD]
                            // [CallResponse]

                            // Send the packet
                            _webinix_send_client(
                                win, client, packet_id, WEBUI_CMD_CALL_FUNC, NULL, 0
                            );
                        }

                        // Free event
                        _webinix_free_mem((void*)event_inf);
                    } else if ((unsigned char)packet[WEBUI_PROTOCOL_CMD] == WEBUI_CMD_CHECK_TK) {

                        // Check Token Event

                        // Protocol
                        // [Header]

                        #ifdef WEBUI_LOG
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> WEBUI_CMD_CHECK_TK \n",
                            recvNum
                        );
                        #endif

                        unsigned char status = 0x00;

                        size_t connection_id = 0;
                        if (_webinix_connection_get_id(win, client, &connection_id)) {
                            _webinix.clients_token_check[connection_id] = true;
                            status = 0x01;
                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> Check token accepted.\n",
                                recvNum
                            );
                            #endif
                        }
                        else {
                            #ifdef WEBUI_LOG
                            printf(
                                "[Core]\t\t_webinix_ws_process(%zu) -> Check token not accepted.\n",
                                recvNum
                            );
                            #endif
                        }

                        // Packet Protocol Format:
                        // [...]
                        // [CMD]
                        // [Check Token Status]

                        // Send the packet
                        _webinix_send_client(
                            win, client, packet_id, WEBUI_CMD_CHECK_TK, &status, 1
                        );

                        if (status == 0x01) {
                            // New Event
                            if (win->has_all_events) {

                                // New event inf (Connected)
                                // webinix_event_inf_t* event_inf = NULL;
                                // size_t event_num = _webinix_new_event_inf(win, &event_inf);

                                // Set arguments
                                // ...

                                _webinix_window_event(
                                    win, // Event -> Window
                                    connection_id, // Event -> Client Unique ID
                                    WEBUI_EVENT_CONNECTED, // Event -> Type of this event
                                    "", // Event -> HTML Element
                                    0, // Event -> Event Number
                                    _webinix_client_get_id(win, client), // Event -> Client ID
                                    _webinix_get_cookies_full(client) // Event -> Full cookies
                                );

                                // Free event
                                // _webinix_free_event_inf(win, event_num);
                            }
                        }
                    }
                    #ifdef WEBUI_LOG
                    else {
                        printf(
                            "[Core]\t\t_webinix_ws_process(%zu) -> Unknown command "
                            "[0x%02x]\n",
                            recvNum, (unsigned char)packet[WEBUI_PROTOCOL_CMD]
                        );
                    }
                    #endif
                }
                #ifdef WEBUI_LOG
                else {
                    printf(
                        "[Core]\t\t_webinix_ws_process(%zu) -> Window is not connected.\n",
                        recvNum
                    );
                }
                #endif

                // Unlock Mutex
                if (_webinix.config.ws_block) {
                    if ((unsigned char)packet[WEBUI_PROTOCOL_CMD] != WEBUI_CMD_JS) {
                        _webinix_mutex_unlock(&_webinix.mutex_receive);
                    }
                }
            } else {

                #ifdef WEBUI_LOG
                printf(
                    "[Core]\t\t_webinix_ws_process(%zu) -> Invalid Packet.\n",
                    recvNum
                );
                #endif

                // Forced close
                _webinix_connection_remove(win, client);
            }
        } else if (event_type == WEBUI_WS_OPEN) {

            // New connection

            #ifdef WEBUI_LOG
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> WEBUI_WS_OPEN \n",
                recvNum
            );
            #endif

            /*
            // New Event
            if (win->has_all_events) {

                // New event inf (Connected)
                // webinix_event_inf_t* event_inf = NULL;
                // size_t event_num = _webinix_new_event_inf(win, &event_inf);

                // Set arguments
                // ...

                _webinix_window_event(
                    win, // Event -> Window
                    connection_id, // Event -> Client Unique ID
                    WEBUI_EVENT_CONNECTED, // Event -> Type of this event
                    "", // Event -> HTML Element
                    0, // Event -> Event Number
                    _webinix_client_get_id(win, client), // Event -> Client ID
                    _webinix_get_cookies_full(client) // Event -> Full cookies
                );

                // Free event
                // _webinix_free_event_inf(win, event_num);
            }
            */
        } else if (event_type == WEBUI_WS_CLOSE) {

            // Connection close

            #ifdef WEBUI_LOG
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> WEBUI_WS_CLOSE \n",
                recvNum
            );
            #endif

            // Events
            if (win->has_all_events) {

                // New event inf (Disconnected)
                // webinix_event_inf_t* event_inf = NULL;
                // size_t event_num = _webinix_new_event_inf(win, &event_inf);

                // Set arguments
                // ...

                _webinix_window_event(
                    win, // Event -> Window
                    connection_id, // Event -> Client Unique ID
                    WEBUI_EVENT_DISCONNECTED, // Event -> Type of this event
                    "", // Event -> HTML Element
                    0, // Event -> Event Number
                    _webinix_client_get_id(win, client), // Event -> Client ID
                    _webinix_get_cookies_full(client) // Event -> Full cookies
                );

                // Free event
                // _webinix_free_event_inf(win, event_num);
            }
        }
        #ifdef WEBUI_LOG
        else {
            printf(
                "[Core]\t\t_webinix_ws_process(%zu) -> UNKNOWN EVENT "
                "TYPE (%zu)\n",
                recvNum, event_type
            );
        }
        #endif

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_ws_process(%zu) -> Finished.\n", recvNum);
        #endif
    }
}

static WEBUI_THREAD_RECEIVE {
    #ifdef WEBUI_LOG
    printf("[Core]\t\t[Thread .] _webinix_ws_process_thread()\n");
    #endif

    // Get arguments
    _webinix_recv_arg_t* arg = (_webinix_recv_arg_t* ) _arg;

    // Process
    _webinix_ws_process(arg->win, arg->client, arg->connection_id, arg->ptr, arg->len, arg->recvNum, arg->event_type);

    // Free
    _webinix_free_mem((void*)arg->ptr);
    _webinix_free_mem((void*)arg);

    WEBUI_THREAD_RETURN
}

#ifdef _WIN32
static void _webinix_kill_pid(size_t pid) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_kill_pid(%zu)\n", pid);
    #endif

    if (pid < 1)
        return;
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, (DWORD) pid);
    if (hProcess != NULL) {
        TerminateProcess(hProcess, 1);
        CloseHandle(hProcess);
    }
}
#else
static void _webinix_kill_pid(size_t pid) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_kill_pid(%zu)\n", pid);
    #endif

    if (pid < 1)
        return;
    kill((pid_t) pid, SIGTERM);
}
#endif

#ifdef _WIN32

static bool _webinix_str_to_wide(const char *s, wchar_t **w) {
    int wlen = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (wlen < 1)
        return false;
    wchar_t *wide = (wchar_t *)_webinix_malloc(wlen * sizeof(wchar_t));
    if (!wide)
        return false;
    MultiByteToWideChar(CP_UTF8, 0, s, -1, wide, wlen);
    *w = wide;
    return true;
}

static bool _webinix_socket_test_listen_win32(size_t port_num) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_socket_test_listen_win32([%zu])\n", port_num);
    #endif

    WSADATA wsaData;
    size_t iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo * result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2),&wsaData);
    if (iResult != 0) {
        // WSAStartup failed
        return false;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    char the_port[16] = {0};
    WEBUI_SN_PRINTF_STATIC(&the_port[0], sizeof(the_port), "%zu", port_num);
    iResult = getaddrinfo("127.0.0.1",&the_port[0],&hints,&result);
    if (iResult != 0) {
        // WSACleanup();
        return false;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        // WSACleanup();
        return false;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == (size_t)SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(ListenSocket);
        shutdown(ListenSocket, SD_BOTH);
        // WSACleanup();
        return false;
    }

    // Clean
    freeaddrinfo(result);
    closesocket(ListenSocket);
    shutdown(ListenSocket, SD_BOTH);
    // WSACleanup();

    // Listening Success
    return true;
}

static int _webinix_system_win32_out(const char* cmd, char ** output, bool show) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_system_win32_out()\n");
    #endif

        // Ini
        *
        output = NULL;
    if (cmd == NULL)
        return -1;

    // Return
    DWORD Return = 0;

    // Flags
    DWORD CreationFlags = CREATE_NO_WINDOW;
    if (show)
        CreationFlags = SW_SHOW;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    HANDLE stdout_read, stdout_write;
    if (!CreatePipe(&stdout_read,&stdout_write,&sa, 0)) {
        return -1;
    }
    if (!SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(stdout_read);
        CloseHandle(stdout_write);
        return -1;
    }

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = stdout_write;
    si.hStdError = stdout_write;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    if (!CreateProcessA(
            NULL, // No module name (use cmd line)
            (LPSTR) cmd, // Command line
            NULL, // Process handle not inheritable
            NULL, // Thread handle not inheritable
            TRUE, // Set handle inheritance to FALSE
            CreationFlags, // Creation flags
            NULL, // Use parent's environment block
            NULL, // Use parent's starting directory
            &
            si, // Pointer to STARTUP INFO structure
            &
            pi
        )) // Pointer to PROCESS_INFORMATION structure
    {
        CloseHandle(stdout_read);
        CloseHandle(stdout_write);
        return -1;
    }
    CloseHandle(stdout_write);

    SetFocus(pi.hProcess);
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess,&Return);

    DWORD bytes_read;
    char buffer[WEBUI_STDOUT_BUF];
    size_t output_size = 0;

    while(ReadFile(stdout_read, buffer, WEBUI_STDOUT_BUF,&bytes_read, NULL) && bytes_read > 0) {

        char* new_output = realloc(*output, output_size + bytes_read + 1);
        if (new_output == NULL) {
            free(*output);
            CloseHandle(stdout_read);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return -1;
        }

        * output = new_output;
        memcpy(*output + output_size, buffer, bytes_read);
        output_size += bytes_read;
    }

    if (*output != NULL)
        (*output)[output_size] = '\0';

    CloseHandle(stdout_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (Return == 0)
        return 0;
    else
        return -1;
}

/*
    static BOOL CALLBACK _webinix_enum_windows_proc_win32(HWND hwnd, LPARAM
    targetProcessId) {

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_enum_windows_proc_win32()\n");
        #endif

        DWORD windowProcessId;
        GetWindowThreadProcessId(hwnd, &windowProcessId);

        if (windowProcessId == targetProcessId) {

            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_enum_windows_proc_win32() -> Bring the"
                    "process (%lu) to the front\n", windowProcessId);
            #endif

            SetFocus(hwnd);
            SetForegroundWindow(hwnd);
            return FALSE;
        }

        return TRUE;
    }
*/

static int _webinix_system_win32(_webinix_window_t* win, char* cmd, bool show) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_system_win32()\n");
    #endif

    // Convert UTF-8 to wide string
    wchar_t* wcmd;
    if (!_webinix_str_to_wide(cmd, &wcmd))
        return -1;

    /*
    We should not kill this process, because may had many child
    process of other Webinix app instances. Unfortunately, this is
    how modern browsers save memory by combine all windows into one
    single parent process, and we can't control this behavior.

    // Automatically close the browser process when the
    // parent process (this app) get closed. If this fail
    // webinix.js will try to close the window.
    HANDLE JobObject = CreateJobObject(NULL, NULL);
    JOB_OBJECT_EXTENDED_LIMIT_INFORMATION ExtendedInfo = { 0 };
    ExtendedInfo.BasicLimitInformation.LimitFlags =
    JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION |
    JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;SetInformationJobObject(JobObject,
    JobObjectExtendedLimitInformation, &ExtendedInfo, sizeof(ExtendedInfo));
    */

    DWORD Return = 0;
    DWORD CreationFlags = CREATE_NO_WINDOW;

    if (show)
        CreationFlags = SW_SHOW;

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessW(
            NULL, // No module name (use command line)
            wcmd, // Command line
            NULL, // Process handle not inheritable
            NULL, // Thread handle not inheritable
            FALSE, // Set handle inheritance to FALSE
            CreationFlags, // Creation flags
            NULL, // Use parent's environment block
            NULL, // Use parent's starting directory
            &
            si, // Pointer to STARTUP INFO structure
            &
            pi
        )) // Pointer to PROCESS_INFORMATION structure
    {
        // CreateProcess failed
        _webinix_free_mem((void*)wcmd);
        return -1;
    }

    if (win) {
        win->process_id = (size_t)pi.dwProcessId;
    }
    
    SetFocus(pi.hProcess);
    // EnumWindows(_webinix_enum_windows_proc_win32, (LPARAM)(pi.dwProcessId));
    // AssignProcessToJobObject(JobObject, pi.hProcess);
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess,&Return);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    _webinix_free_mem((void*)wcmd);

    if (Return == 0)
        return 0;
    else
        return -1;
}

static bool _webinix_get_windows_reg_value(HKEY key, LPCWSTR reg, LPCWSTR value_name, char value[WEBUI_MAX_PATH]) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_get_windows_reg_value([%Ls])\n", reg);
    #endif

    HKEY hKey;

    if (RegOpenKeyExW(key, reg, 0, KEY_READ,&hKey) == ERROR_SUCCESS) {

        DWORD VALUE_TYPE;
        BYTE VALUE_DATA[WEBUI_MAX_PATH];
        DWORD VALUE_SIZE = sizeof(VALUE_DATA);

        // If `value_name` is empty then it will read the "(default)" reg-key
        if (RegQueryValueExW(hKey, value_name, NULL,&VALUE_TYPE, VALUE_DATA,&VALUE_SIZE) == ERROR_SUCCESS) {

            if (VALUE_TYPE == REG_SZ)
                WEBUI_SN_PRINTF_STATIC(value, WEBUI_MAX_PATH, "%S", (LPCWSTR) VALUE_DATA);
            else if (VALUE_TYPE == REG_DWORD)
                WEBUI_SN_PRINTF_STATIC(value, WEBUI_MAX_PATH, "%lu", *((DWORD * ) VALUE_DATA));

            RegCloseKey(hKey);
            return true;
        }
    }

    return false;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    (void)hinstDLL;
    (void)fdwReason;
    (void)lpReserved;
    return true;
}

#endif

// -- WebView -------------------------

#ifdef _WIN32
    // Microsoft Windows

    typedef HRESULT (__stdcall *CreateCoreWebView2EnvironmentWithOptionsFunc)(
        PCWSTR browserExecutableFolder, PCWSTR userDataFolder, ICoreWebView2EnvironmentOptions* environmentOptions,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environment_created_handler
    );

    typedef struct {
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl* lpVtbl;
        ULONG refCount;
        _webinix_wv_win32_t* webView;
    } CreateWebViewEnvironmentHandler;

    typedef struct {
        ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl* lpVtbl;
        ULONG refCount;
        _webinix_wv_win32_t* webView;
    } CreateWebViewControllerHandler;

    typedef struct {
        ICoreWebView2DocumentTitleChangedEventHandlerVtbl* lpVtbl;
        ULONG refCount;
        _webinix_wv_win32_t* webView;
    } TitleChangedHandler;

    HRESULT STDMETHODCALLTYPE CreateWebViewEnvironmentHandler_Invoke(
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This, 
        HRESULT result, 
        ICoreWebView2Environment* env
    );

    HRESULT STDMETHODCALLTYPE CreateWebViewControllerHandler_Invoke(
        ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This, 
        HRESULT result, 
        ICoreWebView2Controller* controller
    );

    HRESULT STDMETHODCALLTYPE QueryInterfaceEnvironment(
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This, 
        REFIID riid, 
        void** ppvObject
    );

    HRESULT STDMETHODCALLTYPE QueryInterfaceController(
        ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This, 
        REFIID riid, 
        void** ppvObject
    );

    ULONG STDMETHODCALLTYPE AddRefEnvironment(
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This
    );

    ULONG STDMETHODCALLTYPE AddRefController(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This);
    ULONG STDMETHODCALLTYPE ReleaseEnvironment(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This);
    ULONG STDMETHODCALLTYPE ReleaseController(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This);

    HRESULT STDMETHODCALLTYPE TitleChanged_Invoke(
        ICoreWebView2DocumentTitleChangedEventHandler* This, ICoreWebView2* sender, IUnknown* args) {
        (void)args;
        TitleChangedHandler* handler = (TitleChangedHandler*)This;
        _webinix_wv_win32_t* webView = handler->webView;
        LPWSTR newTitle = NULL;
        sender->lpVtbl->get_DocumentTitle(sender, &newTitle);
        SetWindowTextW(webView->hwnd, newTitle);
        CoTaskMemFree(newTitle);
        return S_OK;
    };

    ULONG STDMETHODCALLTYPE TitleChanged_AddRef(ICoreWebView2DocumentTitleChangedEventHandler* This) {
        TitleChangedHandler* handler = (TitleChangedHandler*)This;
        return ++handler->refCount;
    };

    ULONG STDMETHODCALLTYPE TitleChanged_Release(ICoreWebView2DocumentTitleChangedEventHandler* This) {
        TitleChangedHandler* handler = (TitleChangedHandler*)This;
        if (--handler->refCount == 0) {
            _webinix_free_mem((void*) handler->lpVtbl);
            _webinix_free_mem((void*) handler);
            return 0;
        }
        return handler->refCount;
    };

    TitleChangedHandler* CreateTitleChangedHandler(_webinix_wv_win32_t* webView) {
        TitleChangedHandler* handler = _webinix_malloc(sizeof(TitleChangedHandler));
        handler->lpVtbl = _webinix_malloc(sizeof(ICoreWebView2DocumentTitleChangedEventHandlerVtbl));
        handler->lpVtbl->Invoke = TitleChanged_Invoke;
        handler->lpVtbl->AddRef = TitleChanged_AddRef;
        handler->lpVtbl->Release = TitleChanged_Release;
        handler->refCount = 1;
        handler->webView = webView;

        // Save pointers to be freed by `_webinix_wv_free()`
        webView->titleChangedHandler = handler;
        webView->titleChangedHandler_lpVtbl = handler->lpVtbl;
        return handler;
    };

    CreateWebViewEnvironmentHandler* CreateEnvironmentHandler(_webinix_wv_win32_t* webView) {
        CreateWebViewEnvironmentHandler* handler = _webinix_malloc(sizeof(CreateWebViewEnvironmentHandler));
        if (!handler) return NULL;
        handler->lpVtbl = _webinix_malloc(sizeof(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl));
        if (!handler->lpVtbl) {
            _webinix_free_mem((void*) handler);
            return NULL;
        }
        handler->lpVtbl->QueryInterface = QueryInterfaceEnvironment;
        handler->lpVtbl->AddRef = AddRefEnvironment;
        handler->lpVtbl->Release = ReleaseEnvironment;
        handler->lpVtbl->Invoke = CreateWebViewEnvironmentHandler_Invoke;
        handler->refCount = 1;
        handler->webView = webView;

        // Save pointers to be freed by `_webinix_wv_free()`
        webView->createWebViewEnvironmentHandler = handler;
        webView->createWebViewEnvironmentHandler_lpVtbl = handler->lpVtbl;
        return handler;
    };

    CreateWebViewControllerHandler* CreateControllerHandler(_webinix_wv_win32_t* webView) {
        CreateWebViewControllerHandler* handler = _webinix_malloc(sizeof(CreateWebViewControllerHandler));
        if (!handler) return NULL;
        handler->lpVtbl = _webinix_malloc(sizeof(ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl));
        if (!handler->lpVtbl) {
            _webinix_free_mem((void*) handler);
            return NULL;
        }
        handler->lpVtbl->QueryInterface = QueryInterfaceController;
        handler->lpVtbl->AddRef = AddRefController;
        handler->lpVtbl->Release = ReleaseController;
        handler->lpVtbl->Invoke = CreateWebViewControllerHandler_Invoke;
        handler->refCount = 1;
        handler->webView = webView;

        // Save pointers to be freed by `_webinix_wv_free()`
        webView->createWebViewControllerHandler = handler;
        webView->createWebViewControllerHandler_lpVtbl = handler->lpVtbl;
        return handler;
    };

    HRESULT STDMETHODCALLTYPE CreateWebViewEnvironmentHandler_Invoke(
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This, HRESULT result, 
        ICoreWebView2Environment* env
    ) {
        CreateWebViewEnvironmentHandler* handler = (CreateWebViewEnvironmentHandler*)This;
        _webinix_wv_win32_t* webView = handler->webView;
        if (SUCCEEDED(result)) {
            CreateWebViewControllerHandler* controllerHandler = CreateControllerHandler(webView);
            if (controllerHandler) {
                env->lpVtbl->CreateCoreWebView2Controller(env, webView->hwnd, 
                (ICoreWebView2CreateCoreWebView2ControllerCompletedHandler*)controllerHandler);
            }
        }
        return S_OK;
    };

    HRESULT STDMETHODCALLTYPE CreateWebViewControllerHandler_Invoke(
        ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This, HRESULT result, 
        ICoreWebView2Controller* controller) {
        CreateWebViewControllerHandler* handler = (CreateWebViewControllerHandler*)This;
        _webinix_wv_win32_t* webView = handler->webView;
        if (SUCCEEDED(result) && controller != NULL) {
            webView->webviewController = controller;
            webView->webviewController->lpVtbl->get_CoreWebView2(webView->webviewController, 
            &webView->webviewWindow);
            webView->webviewController->lpVtbl->AddRef(webView->webviewController);
            ICoreWebView2Settings* settings;
            webView->webviewWindow->lpVtbl->get_Settings(webView->webviewWindow, &settings);
            settings->lpVtbl->put_IsScriptEnabled(settings, TRUE);
            settings->lpVtbl->put_AreDefaultScriptDialogsEnabled(settings, TRUE);
            settings->lpVtbl->put_IsWebMessageEnabled(settings, TRUE);
            RECT bounds = {0, 0, webView->width, webView->height};
            webView->webviewController->lpVtbl->put_Bounds(webView->webviewController, bounds);
            TitleChangedHandler* titleChangedHandler = CreateTitleChangedHandler(webView);
            EventRegistrationToken tk;
            webView->webviewWindow->lpVtbl->add_DocumentTitleChanged(webView->webviewWindow, 
            (ICoreWebView2DocumentTitleChangedEventHandler*)titleChangedHandler, &tk);
            webView->webviewWindow->lpVtbl->Navigate(webView->webviewWindow, webView->url);
            // Microsoft WebView2 Auto JS Inject
            if (_webinix.config.show_auto_js_inject) {
                // HRESULT AutoInject = webView->webviewWindow->lpVtbl->AddScriptToExecuteOnDocumentCreated(
                //     webView->webviewWindow, L"var script = document.createElement('script');"
                //     "script.src = 'webinix.js';document.head.appendChild(script);", 
                //     NULL
                // );
                // if (FAILED(AutoInject)) {
                //     #ifdef WEBUI_LOG
                //     printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> Auto Inject creation failed\n");
                //     #endif
                // }
                // else {
                //     #ifdef WEBUI_LOG
                //     printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> Auto Inject creation succeeds\n");
                //     #endif
                // }
            }
        } else return S_FALSE;
        return S_OK;
    };

    HRESULT STDMETHODCALLTYPE QueryInterfaceEnvironment(
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This, REFIID riid, void** ppvObject) {
        (void)This;
        (void)riid;
        *ppvObject = NULL;
        return E_NOINTERFACE;
    };

    HRESULT STDMETHODCALLTYPE QueryInterfaceController(
        ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This, REFIID riid, void** ppvObject) {
        (void)This;
        (void)riid;
        *ppvObject = NULL;
        return E_NOINTERFACE;
    };

    ULONG STDMETHODCALLTYPE AddRefEnvironment(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This) {
        CreateWebViewEnvironmentHandler* handler = (CreateWebViewEnvironmentHandler*)This;
        return ++handler->refCount;
    };

    ULONG STDMETHODCALLTYPE AddRefController(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This) {
        CreateWebViewControllerHandler* handler = (CreateWebViewControllerHandler*)This;
        return ++handler->refCount;
    };

    ULONG STDMETHODCALLTYPE ReleaseEnvironment(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This) {
        CreateWebViewEnvironmentHandler* handler = (CreateWebViewEnvironmentHandler*)This;
        if (--handler->refCount == 0) {
            _webinix_free_mem((void*) handler->lpVtbl);
            _webinix_free_mem((void*) handler);
            return 0;
        }
        return handler->refCount;
    };

    ULONG STDMETHODCALLTYPE ReleaseController(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This) {
        CreateWebViewControllerHandler* handler = (CreateWebViewControllerHandler*)This;
        if (--handler->refCount == 0) {
            _webinix_free_mem((void*) handler->lpVtbl);
            _webinix_free_mem((void*) handler);
            return 0;
        }
        return handler->refCount;
    };

    LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        void* ptr = (void*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        _webinix_window_t* win = _webinix_dereference_win_ptr(ptr);

        switch (uMsg) {
            case WM_SIZE:
                if (win) {
                    if (win->webView && win->webView->webviewController) {
                        RECT bounds;
                        GetClientRect(hwnd, &bounds);
                        win->webView->webviewController->lpVtbl->put_Bounds(win->webView->webviewController, bounds);
                    }
                }
                break;
            case WM_CLOSE:
                if (win) {
                    // Stop the WebView thread, close the window
                    // and free resources.
                    if (win->webView) {
                        win->webView->stop = true;
                        _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);
                    }                    
                    _webinix_wv_event_closed(win);
                }
                break;
            case WM_DESTROY:
                if (win) {
                    // Destroy message will be
                    // sent by `webinix_wait()`
                    // Nothing to do here.
                }
                break;
            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    };

    // Close Event
    static void _webinix_wv_event_closed(_webinix_window_t* win) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_event_closed([%zu])\n", win->window_number);
        #endif
        win->is_closed = true;
    }

    static bool _webinix_wv_show(_webinix_window_t* win, char* url) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_show([%s])\n", url);
        #endif

        // Microsoft Windows WebView2

        // Free old WebView
        if (win->webView) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
        }

        // Get wide URL
        wchar_t* wURL = NULL;
        _webinix_str_to_wide(url, &wURL);

        // Initializing the Win32 WebView struct
        _webinix_wv_win32_t* webView = (_webinix_wv_win32_t*) _webinix_malloc(sizeof(_webinix_wv_win32_t));
        webView->url = wURL;
        webView->width = (win->width > 0 ? win->width : 800);
        webView->height = (win->height > 0 ? win->height : 600);
        webView->x = (win->x > 0 ? win->x : (int)((GetSystemMetrics(SM_CXSCREEN) - 800) / 2));
        webView->y = (win->y > 0 ? win->y : (int)((GetSystemMetrics(SM_CYSCREEN) - 600) / 2));
        win->webView = webView;

        // Note: To garantee all Microsoft WebView's operations ownership we should
        // process all the WebView's operations in one single thread for each window.

        // Initializing
        // Expecting `_webinix_webview_thread` to change `mutex_is_webview_update` 
        // to `false` when initialization is done, and `_webinix.is_webview`
        // to `true` if loading the WebView is succeeded.
        _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);

        // Win32 WebView thread
        #ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, _webinix_webview_thread, (void*)win, 0, NULL);
        if (thread != NULL)
            CloseHandle(thread);
        #else
        pthread_t thread;
        pthread_create(&thread, NULL, &_webinix_webview_thread, (void*)win);
        pthread_detach(thread);
        #endif
        
        // Wait for WebView thread to start
        _webinix_timer_t timer;
        _webinix_timer_start(&timer);
        for (;;) {
            _webinix_sleep(10);
            if (!_webinix_mutex_is_webview_update(win, WEBUI_MUTEX_NONE)) {
                // WebView thread just started
                // and loaded WebView successfully
                break;
            }
            if (_webinix_timer_is_end(&timer, 2500)) {
                // Timeout. WebView thread failed.
                break;
            }
        }

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_show() -> Return [%d]\n", (_webinix.is_webview == true));
        #endif

        return (_webinix.is_webview);
    };

    static bool _webinix_wv_set_size(_webinix_wv_win32_t* webView, int windowWidth, int windowHeight) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_set_size(%d. %d)\n", windowWidth, windowHeight);
        #endif
        // if (webView && webView->webviewController) {
        //     RECT bounds = {0, 0, windowWidth, windowHeight};
        //     HRESULT hr = webView->webviewController->lpVtbl->put_Bounds(webView->webviewController, bounds);
        //     return SUCCEEDED(hr);
        // }
        if (webView) {
            return (SetWindowPos(webView->hwnd, NULL, 0, 0, windowWidth, windowHeight, SWP_NOMOVE| SWP_NOREPOSITION));
        }
        return false;
    };

    static bool _webinix_wv_set_position(_webinix_wv_win32_t* webView, int x, int y) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_set_position(%d. %d)\n", x, y);
        #endif
        if (webView && webView->webviewController) {
            RECT bounds;
            webView->webviewController->lpVtbl->get_Bounds(webView->webviewController, &bounds);
            HRESULT hr = MoveWindow(webView->hwnd, x, y, bounds.right - bounds.left, bounds.bottom - bounds.top, TRUE);
            return hr != 0;
        }
        return false;
    };

    static bool _webinix_wv_navigate(_webinix_wv_win32_t* webView, wchar_t* url) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_navigate([%ls])\n", url);
        #endif
        if (webView && webView->webviewWindow) {
            HRESULT hr = webView->webviewWindow->lpVtbl->Navigate(webView->webviewWindow, url);
            return SUCCEEDED(hr);
        }
        return false;
    };

    static void _webinix_wv_free(_webinix_wv_win32_t* webView) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_free()\n");
        #endif
        if (webView != NULL) {
            if (webView->webviewWindow) {
                webView->webviewWindow->lpVtbl->Release(webView->webviewWindow);
            }
            if (webView->webviewController) {
                webView->webviewController->lpVtbl->Release(webView->webviewController);
            }
            if (webView->webviewEnvironment) {
                webView->webviewEnvironment->lpVtbl->Release(webView->webviewEnvironment);
            }
        }

        _webinix_free_mem((void*) webView->titleChangedHandler_lpVtbl);
        _webinix_free_mem((void*) webView->titleChangedHandler);
        _webinix_free_mem((void*) webView->createWebViewEnvironmentHandler_lpVtbl);
        _webinix_free_mem((void*) webView->createWebViewEnvironmentHandler);
        _webinix_free_mem((void*) webView->createWebViewControllerHandler_lpVtbl);
        _webinix_free_mem((void*) webView->createWebViewControllerHandler);

        _webinix_free_mem((void*) webView->url);
        _webinix_free_mem((void*) webView);
    };

    static void _webinix_wv_close(_webinix_wv_win32_t *webView) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_close()\n");
        #endif
        (void)webView;
        // ...
    }

    static WEBUI_THREAD_WEBVIEW {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread()\n");
        #endif

        _webinix_window_t* win = _webinix_dereference_win_ptr(arg);
        if (win == NULL) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
            WEBUI_THREAD_RETURN
        }

        // WebView Dynamic Library
        if (!_webinix.webviewLib) {
            _webinix.webviewLib = LoadLibraryA("WebView2Loader.dll");
            if (!_webinix.webviewLib) {
                _webinix_wv_free(win->webView);
                win->webView = NULL;
                _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
                WEBUI_THREAD_RETURN
            }
        }

        // Window class
        const char wvClass[] = "WebViewWindow";
        WNDCLASSA wc;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = wvClass;
        wc.style = 0;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

        if (!RegisterClassA(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
            WEBUI_THREAD_RETURN
        }

        win->webView->hwnd = CreateWindowExA(
            0, wvClass, "", WS_OVERLAPPEDWINDOW,
            win->webView->x, win->webView->y, 
            win->webView->width, win->webView->height,
            NULL, NULL, GetModuleHandle(NULL), NULL
        );

        if (!win->webView->hwnd) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
            WEBUI_THREAD_RETURN
        }

        SetWindowLongPtr(win->webView->hwnd, GWLP_USERDATA, (LONG_PTR)win);
        ShowWindow(win->webView->hwnd, SW_SHOW);
        static CreateCoreWebView2EnvironmentWithOptionsFunc createEnv = NULL;
        createEnv = (CreateCoreWebView2EnvironmentWithOptionsFunc)(void*)GetProcAddress(
            _webinix.webviewLib,
            "CreateCoreWebView2EnvironmentWithOptions"
        );

        if (!createEnv) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
            WEBUI_THREAD_RETURN
        }

        CreateWebViewEnvironmentHandler* environmentHandler = CreateEnvironmentHandler(win->webView);
        if (!environmentHandler) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
            WEBUI_THREAD_RETURN
        }

        // Get temp chache folder path
        if (!_webinix.webview_cacheFolder) {
            const char* temp = _webinix_get_temp_path();
            _webinix.webview_cacheFolder = (char*)_webinix_malloc(WEBUI_MAX_PATH);
            WEBUI_SN_PRINTF_DYN(_webinix.webview_cacheFolder, WEBUI_MAX_PATH, 
                "%s%s.Webinix%sWebinixWebViewCache_%"PRIu32, temp, webinix_sep, webinix_sep,
                _webinix_generate_random_uint32());
        }

        // Convert chache folder path to wide
        wchar_t* cacheFolderW = NULL;
        _webinix_str_to_wide(_webinix.webview_cacheFolder, &cacheFolderW);

        HRESULT hr = createEnv(NULL, cacheFolderW, NULL, 
            (ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*)environmentHandler
        );

        if (SUCCEEDED(hr)) {

            // Success
            // Let `wait()` use safe main-thread WebView2 loop
            _webinix.is_webview = true;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);

            MSG msg;
            while (true) {

                // Win32 Messages
                if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) > 0) {

                    // Process
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);

                    // Check if window manually closed
                    if (msg.message == WM_QUIT) {
                        if (win->webView) {
                            DestroyWindow(win->webView->hwnd);
                        }
                        break;
                    }
                }

                if (_webinix_mutex_is_webview_update(win, WEBUI_MUTEX_NONE)) {

                    _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);

                    if (win->webView) {

                        // Stop this thread
                        if (win->webView->stop) {
                            DestroyWindow(win->webView->hwnd);
                            break;
                        }

                        // Window Size
                        if (win->webView->size) {
                            win->webView->size = false;
                            _webinix_wv_set_size(win->webView, win->webView->width, win->webView->height);
                        }

                        // Window Position
                        if (win->webView->position) {
                            win->webView->position = false;
                            _webinix_wv_set_position(win->webView, win->webView->x, win->webView->y);
                        }

                        // Navigation
                        if (win->webView->navigate) {
                            win->webView->navigate = false;
                            _webinix_wv_navigate(win->webView, win->webView->url);
                        }
                    }
                }
            }
        }

        // Clean
        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> Cleaning\n");
        #endif
        _webinix_wv_free(win->webView);
        _webinix_free_mem((void*) cacheFolderW);
        win->webView = NULL;

        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> End\n");
        #endif

        WEBUI_THREAD_RETURN
    };

#elif __linux__
    // Linux

    #define G_CALLBACK(f) ((void (*)(void)) (f))
    #define GTK_RUNTIME_ARR { "libgtk-3.so.0" } // TODO: Add GTK v4 APIs "libgtk-4.so.1"
    #define WEBKIT_RUNTIME_ARR { "libwebkit2gtk-4.1.so.0", "libwebkit2gtk-4.0.so.37" }

    // Title Event
    static void _webinix_wv_event_title(void *web_view, void *pspec, void *arg) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_event_title()\n");
        #endif
        _webinix_window_t* win = (_webinix_window_t *)arg;
        webkit_web_view_get_title_func webkit_web_view_get_title = (
            webkit_web_view_get_title_func)dlsym(libwebkit, "webkit_web_view_get_title");
        const char *title = webkit_web_view_get_title(web_view);
        if (title) {
            gtk_window_set_title_func gtk_window_set_title = (
                gtk_window_set_title_func)dlsym(libgtk, "gtk_window_set_title");
            gtk_window_set_title(win->webView->gtk_win, title);
        }
    }

    // Close Event
    static void _webinix_wv_event_closed(void *widget, void *arg) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_event_closed()\n");
        #endif
        _webinix_window_t* win = _webinix_dereference_win_ptr(arg);
        if (win) {
            win->is_closed = true;
            if (win->webView) {
                win->webView->open = false;
            }
        }
    }

    static bool _webinix_wv_set_size(_webinix_wv_linux_t* webView, int windowWidth, int windowHeight) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_set_size(%d. %d)\n", windowWidth, windowHeight);
        #endif
        if (webView) {
            if (webView->gtk_win) {
                gtk_window_resize(webView->gtk_win, webView->width, webView->height);
                return true;
            }
        }
        return false;
    };

    static bool _webinix_wv_set_position(_webinix_wv_linux_t* webView, int x, int y) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_set_position(%d. %d)\n", x, y);
        #endif
        if (webView) {
            if (webView->gtk_win) {
                // Note:
                // This API does not work under Wayland, and it has been removed
                // in GTK v4, alongside all the APIs that relies on a global 
                // coordinates system. So, `gtk_window_move` may have no effect.
                gtk_window_move(webView->gtk_win, webView->x, webView->y);
                return true;
            }
        }
        return false;
    };

    static bool _webinix_wv_navigate(_webinix_wv_linux_t* webView, char* url) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_navigate([%s])\n", url);
        #endif
        if (webView) {
            if (webView->gtk_win) {
                webkit_web_view_load_uri(webView->gtk_wv, webView->url);
                return true;
            }
        }
        return false;
    };

    static void _webinix_wv_close(_webinix_wv_linux_t* webView) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_close()\n");
        #endif

        if (webView) {

            if (webView->open) {

                webView->open = false;

                #ifdef WEBUI_LOG
                printf("[Core]\t\t_webinix_wv_close() -> Closing WebView window\n");
                #endif
                gtk_window_close(webView->gtk_win);
            }
        }

        _webinix_free_mem((void*) webView->url);
        _webinix_free_mem((void*) webView);
    };

    static void _webinix_wv_free() {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_free()\n");
        #endif

        if (libwebkit) {

            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_wv_free() -> Unload WebKit\n");
            #endif
            dlclose(libwebkit);
        }

        if (libgtk) {

            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_wv_free() -> Unload GTK\n");
            #endif
            dlclose(libgtk);
        }

        _webinix.is_webview = false;
        libgtk = NULL;
        libwebkit = NULL;
    };

    static void _webinix_wv_create(_webinix_window_t* win) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_create()\n");
        #endif

        // Initialize GTK Window
        win->webView->gtk_win = gtk_window_new(0);

        // Initialize WebView
        win->webView->gtk_wv = webkit_web_view_new();

        // Window Settings
        gtk_window_set_default_size(win->webView->gtk_win, win->webView->width, win->webView->height);
        gtk_container_add(win->webView->gtk_win, win->webView->gtk_wv);

        // Window position
        // Note: The new positioning system it's not GTK's toolkit job anymore.
        // if ((win->x > 0) && (win->y > 0)) {
        //     gtk_window_move(win->webView->gtk_win, win->webView->x, win->webView->y);
        // }
        // else {
        //     gtk_window_set_position(win->webView->gtk_wv, 1);
        // }

        // Events
        g_signal_connect_data(win->webView->gtk_wv, "notify::title", G_CALLBACK(
            _webinix_wv_event_title), (void *)win, NULL, 0);
        g_signal_connect_data(win->webView->gtk_win, "destroy", G_CALLBACK(
            _webinix_wv_event_closed), (void *)win, NULL, 0);
        
        // Linux GTK WebView Auto JS Inject
        if (_webinix.config.show_auto_js_inject) {
            // ...
        }

        // Show
        webkit_web_view_load_uri(win->webView->gtk_wv, win->webView->url);
        gtk_widget_show_all(win->webView->gtk_win);
        win->webView->open = true;

        // Note: All the GTK WebView's operations should be
        // processed in one single thread for each window.

        // Linux WebView thread
        #ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, _webinix_webview_thread, (void*)win, 0, NULL);
        if (thread != NULL)
            CloseHandle(thread);
        #else
        pthread_t thread;
        pthread_create(&thread, NULL, &_webinix_webview_thread, (void*)win);
        pthread_detach(thread);
        #endif
    }

    static int _webinix_wv_create_schedule(void* arg) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_create_schedule()\n");
        #endif

        // This callback is fired by GTK. so it's safe
        // to create the new WebView window right now.

        _webinix_window_t* win = _webinix_dereference_win_ptr(arg);
        if (win) {
            _webinix_wv_create(win);
        }

        return 0;
    }

    static bool _webinix_wv_show(_webinix_window_t* win, char* url) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_show([%s])\n", url);
        #endif

        // Linux GTK WebView

        if (_webinix.is_browser_main_run)
            return false;

        // Dynamic Load
        if (!libgtk || !libwebkit) {

            _webinix.is_webview = false;

            // GTK Dynamic Load
            const char *gtk_libs[] = GTK_RUNTIME_ARR;
            for (size_t i = 0; i < (sizeof(gtk_libs) / sizeof(gtk_libs[0]));++i) {
                libgtk = dlopen(gtk_libs[i], RTLD_LAZY);
                if (libgtk) {
                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webinix_wv_show() -> GTK loaded [%s]\n", 
                    gtk_libs[i]);
                    #endif
                    break;
                }
            }

            if (!libgtk) {
                _webinix_wv_free();
                return false;
            }
    
            // WebView Dynamic Load
            const char *webkit_libs[] = WEBKIT_RUNTIME_ARR;
            for (size_t i = 0; i < (sizeof(webkit_libs) / sizeof(webkit_libs[0]));++i) {
                libwebkit = dlopen(webkit_libs[i], RTLD_LAZY);
                if (libwebkit) {
                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webinix_wv_show() -> WebKit loaded [%s]\n", 
                    webkit_libs[i]);
                    #endif
                    break;
                }
            }

            if (!libwebkit) {
                _webinix_wv_free();
                return false;
            }

            // GTK Symbol Addresses
            gtk_init = (gtk_init_func)dlsym(
                libgtk, "gtk_init");
            gtk_widget_show_all = (gtk_widget_show_all_func)dlsym(
                libgtk, "gtk_widget_show_all");
            gtk_main_iteration_do = (gtk_main_iteration_do_func)dlsym(
                libgtk, "gtk_main_iteration_do");
            gtk_events_pending = (gtk_events_pending_func)dlsym(
                libgtk, "gtk_events_pending");
            gtk_container_add = (gtk_container_add_func)dlsym(
                libgtk, "gtk_container_add");
            gtk_window_new = (gtk_window_new_func)dlsym(
                libgtk, "gtk_window_new");
            gtk_window_set_default_size = (gtk_window_set_default_size_func)dlsym(
                libgtk, "gtk_window_set_default_size");
            gtk_window_set_title = (gtk_window_set_title_func)dlsym(
                libgtk, "gtk_window_set_title");
            gtk_window_move = (gtk_window_move_func)dlsym(
                libgtk, "gtk_window_move");
            gtk_window_close = (gtk_window_close_func)dlsym(
                libgtk, "gtk_window_close");
            gtk_window_resize = (gtk_window_resize_func)dlsym(
                libgtk, "gtk_window_resize");
            gtk_window_set_position = (gtk_window_set_position_func)dlsym(
                libgtk, "gtk_window_set_position");
            g_signal_connect_data = (g_signal_connect_data_func)dlsym(
                libgtk, "g_signal_connect_data");
            g_idle_add = (g_idle_add_func)dlsym(
                libgtk, "g_idle_add");
            
            // WebView Symbol Addresses
            webkit_web_view_new = (webkit_web_view_new_func)dlsym(
                libwebkit, "webkit_web_view_new");
            webkit_web_view_load_uri = (webkit_web_view_load_uri_func)dlsym(
                libwebkit, "webkit_web_view_load_uri");
            webkit_web_view_get_title = (webkit_web_view_get_title_func)dlsym(
                libwebkit, "webkit_web_view_get_title");

            // Check GTK
            if (
                // GTK Commun
                !gtk_init || !gtk_window_new || !gtk_window_set_default_size
                || !gtk_window_set_title || !g_signal_connect_data
                // GTK v3
                || !gtk_widget_show_all || !gtk_main_iteration_do
                || !gtk_events_pending || !gtk_container_add
                || !gtk_window_move
                // GTK v4
                // ...
                )
            {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> GTK symbol addresses failed\n");
                #endif
                _webinix_wv_free();
                return false;
            }

            // Check WebView
            if (!webkit_web_view_new || !webkit_web_view_load_uri || !webkit_web_view_get_title) {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> WebKit symbol addresses failed\n");
                #endif
                _webinix_wv_free();
                return false;
            }

            // Initialize GTK
            gtk_init(NULL, NULL);
        }

        // Free old WebView
        if (win->webView) {
            win->webView->stop = true;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);
        }

        // Copy URL
        char* url_copy = _webinix_str_dup(url);

        // Initializing the Linux WebView struct
        _webinix_wv_linux_t* webView = (_webinix_wv_linux_t*) _webinix_malloc(sizeof(_webinix_wv_linux_t));
        webView->url = url_copy;
        webView->width = (win->width > 0 ? win->width : 800);
        webView->height = (win->height > 0 ? win->height : 600);
        webView->x = (win->x > 0 ? win->x : 0);
        webView->y = (win->y > 0 ? win->y : 0);
        win->webView = webView;

        // New WebView window
        if (_webinix.is_gtk_main_run) {

            // Schedule the creation of the new WebView
            // window in the main thread `webinix_wait()`

            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_wv_show() -> Schedule the creation of the new WebView window\n");
            #endif

            g_idle_add(_webinix_wv_create_schedule, (void*)win);
        }
        else {

            // The main thread `webinix_wait()` is not running
            // so it's safe to create the new WebView window
            // from this thread, which is should be fired from
            // the back-end main thread.

            #ifdef WEBUI_LOG
            printf("[Core]\t\t_webinix_wv_show() -> New WebView window\n");
            #endif

            _webinix_wv_create(win);
        }

        // Initializing
        // Expecting `_webinix_webview_thread` to change `mutex_is_webview_update` 
        // to `false` when initialization is done, and `_webinix.is_webview`
        // to `true` if loading the WebView is succeeded.
        _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);

        // Wait for WebView thread to get
        // started by `_webinix_wv_create()`
        
        _webinix_timer_t timer;
        _webinix_timer_start(&timer);
        for (;;) {
            _webinix_sleep(100);
            if (!_webinix_mutex_is_webview_update(win, WEBUI_MUTEX_NONE)) {
                // WebView thread just started
                // and loaded window successfully
                break;
            }
            if (_webinix_timer_is_end(&timer, 2500)) {
                // Timeout. WebView thread failed.
                break;
            }
        }

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_show() -> Return [%d]\n", (_webinix.is_webview == true));
        #endif

        return (_webinix.is_webview);
    };

    static WEBUI_THREAD_WEBVIEW {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread()\n");
        #endif

        _webinix_window_t* win = _webinix_dereference_win_ptr(arg);
        if (win == NULL) {
            _webinix_wv_close(win->webView);
            win->webView = NULL;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
            WEBUI_THREAD_RETURN
        }

        // Check GTK is loaded
        if (!libgtk || !libwebkit) {
            _webinix_wv_close(win->webView);
            win->webView = NULL;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);
            WEBUI_THREAD_RETURN
        }

        if (win->webView->gtk_win && win->webView->gtk_wv) {

            #ifdef WEBUI_LOG
            printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> Started\n");
            #endif

            // Success
            // Let `wait()` use safe main-thread GTK WebView loop
            _webinix.is_webview = true;
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);

            while (true) {

                if (_webinix_mutex_is_webview_update(win, WEBUI_MUTEX_NONE)) {

                    _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);

                    if (win->webView) {

                        // Stop this thread
                        if (win->webView->stop) {
                            break;
                        }

                        // Window Size
                        if (win->webView->size) {
                            win->webView->size = false;
                            _webinix_wv_set_size(win->webView, win->webView->width, win->webView->height);
                        }

                        // Window Position
                        if (win->webView->position) {
                            win->webView->position = false;
                            _webinix_wv_set_position(win->webView, win->webView->x, win->webView->y);
                        }

                        // Navigation
                        if (win->webView->navigate) {
                            win->webView->navigate = false;
                            _webinix_wv_navigate(win->webView, win->webView->url);
                        }
                    }
                }
            }
        }

        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> Cleaning\n");
        #endif

        // Clean
        _webinix_wv_close(win->webView);
        win->webView = NULL;

        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> End\n");
        #endif

        WEBUI_THREAD_RETURN
    }
#else
    // macOS

    static bool _webinix_wv_set_size(_webinix_wv_macos_t* webView, int windowWidth, int windowHeight) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_set_size(%d. %d)\n", windowWidth, windowHeight);
        #endif
        _webinix_macos_wv_set_size(webView->index, windowWidth, windowHeight);
        return false;
    };

    static bool _webinix_wv_set_position(_webinix_wv_macos_t* webView, int x, int y) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_set_position(%d. %d)\n", x, y);
        #endif
        _webinix_macos_wv_set_position(webView->index, x, y);
        return false;
    };

    static bool _webinix_wv_navigate(_webinix_wv_macos_t* webView, char* url) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_navigate([%s])\n", url);
        #endif
        _webinix_macos_wv_navigate(webView->index, (const char*)url);
        return false;
    };

    static void _webinix_wv_free(_webinix_wv_macos_t* webView) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_free()\n");
        #endif
        // ...
        _webinix_free_mem((void*) webView->url);
        _webinix_free_mem((void*) webView);
    };

    static void _webinix_wv_close(_webinix_wv_macos_t *webView) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_close()\n");
        #endif
        // ...
    }

    // Close Event
    static void _webinix_wv_event_closed(int index) {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_event_closed([%d])\n", index);
        #endif
        if (_webinix.wins[index] != NULL) {
            _webinix.wins[index]->is_closed = true;
            if (_webinix.wins[index]->webView) {
                // Close window
                if (_webinix_mutex_is_connected(_webinix.wins[index], WEBUI_MUTEX_NONE)) {
                    _webinix_send_all(
                        _webinix.wins[index], 0, WEBUI_CMD_CLOSE, NULL, 0
                    );
                }
                // Stop WebView thread if any
                if (_webinix.wins[index]->webView) {
                    _webinix.wins[index]->webView->stop = true;
                    _webinix_mutex_is_webview_update(_webinix.wins[index], WEBUI_MUTEX_TRUE);
                }
            }
        }
    }

    static bool _webinix_wv_show(_webinix_window_t* win, char* url) {

        #ifdef WEBUI_LOG
        printf("[Core]\t\t_webinix_wv_show([%s])\n", url);
        #endif

        // Apple macOS WKWebView

        if (_webinix.is_browser_main_run)
            return false;

        if (!_webinix.is_wkwebview_main_run) {
            if (_webinix_macos_wv_new(win->window_number)) {
                if (!_webinix.is_webview) {
                    // Let `wait()` use safe main-thread WKWebView loop
                    _webinix.is_webview = true;
                    // Set close callback once
                    _webinix_macos_wv_set_close_cb(_webinix_wv_event_closed);
                }
            } else return false;
        }
        else {

            _webinix_macos_wv_new_thread_safe(win->window_number);
            _webinix_sleep(250);
        }

        // Free old WebView
        if (win->webView) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
        }

        // Copy URL
        char* url_copy = _webinix_str_dup(url);

        // Initializing the macOS WebView struct
        _webinix_wv_macos_t* webView = (_webinix_wv_macos_t*) _webinix_malloc(sizeof(_webinix_wv_macos_t));
        webView->url = url_copy;
        webView->width = (win->width > 0 ? win->width : 800);
        webView->height = (win->height > 0 ? win->height : 600);
        webView->x = (win->x > 0 ? win->x : 100);
        webView->y = (win->y > 0 ? win->y : 100);
        webView->index = win->window_number;
        win->webView = webView;

        // Show window
        _webinix_macos_wv_show(
            webView->index, webView->url,
            webView->x, webView->y,
            webView->width, webView->height);

        // Initializing
        // Expecting `_webinix_webview_thread` to change
        // `mutex_is_webview_update` to false when success
        _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_TRUE);

        // macOS WebView thread
        #ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, _webinix_webview_thread, (void*)win, 0, NULL);
        if (thread != NULL)
            CloseHandle(thread);
        #else
        pthread_t thread;
        pthread_create(&thread, NULL, &_webinix_webview_thread, (void*)win);
        pthread_detach(thread);
        #endif
        
        // Wait for WebView thread to start
        _webinix_timer_t timer;
        _webinix_timer_start(&timer);
        for (;;) {
            _webinix_sleep(10);
            if (!_webinix_mutex_is_webview_update(win, WEBUI_MUTEX_NONE)) {
                // WebView thread just started
                // and loaded WebView successfully
                break;
            }
            if (_webinix_timer_is_end(&timer, 2500)) {
                // Timeout. WebView thread failed.
                break;
            }
        }

        return (_webinix_mutex_is_webview_update(win, WEBUI_MUTEX_NONE) == false);
    };

    static WEBUI_THREAD_WEBVIEW {
        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread()\n");
        #endif

        _webinix_window_t* win = _webinix_dereference_win_ptr(arg);
        if (win == NULL) {
            _webinix_wv_free(win->webView);
            win->webView = NULL;
            WEBUI_THREAD_RETURN
        }

        // ...

        if (true) {

            // Success
            _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);

            while (true) {

                // Messages ...

                if (_webinix_mutex_is_webview_update(win, WEBUI_MUTEX_NONE)) {

                    _webinix_mutex_is_webview_update(win, WEBUI_MUTEX_FALSE);

                    if (win->webView) {

                        // Stop this thread
                        if (win->webView->stop) {
                            _webinix_macos_wv_close(win->webView->index);
                            break;
                        }

                        // Window Size
                        if (win->webView->size) {
                            win->webView->size = false;
                            _webinix_wv_set_size(win->webView, win->webView->width, win->webView->height);
                        }

                        // Window Position
                        if (win->webView->position) {
                            win->webView->position = false;
                            _webinix_wv_set_position(win->webView, win->webView->x, win->webView->y);
                        }

                        // Navigation
                        if (win->webView->navigate) {
                            win->webView->navigate = false;
                            _webinix_wv_navigate(win->webView, win->webView->url);
                        }
                    }
                }
            }
        }

        // Close window in case WKWebView did
        // not fire the close event.
        if (_webinix_mutex_is_connected(win, WEBUI_MUTEX_NONE)) {
            _webinix_send_all(
                win, 0, WEBUI_CMD_CLOSE, NULL, 0
            );
        }

        // Clean
        _webinix_wv_free(win->webView);
        win->webView = NULL;

        #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread .] _webinix_webview_thread() -> End\n");
        #endif

        WEBUI_THREAD_RETURN
    }
#endif

static WEBUI_THREAD_MONITOR {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread()\n");
    #endif

    // Get arguments
    _webinix_monitor_arg_t* args = (_webinix_monitor_arg_t*)arg;

    #ifdef _WIN32
        // Windows
        HANDLE hDir = CreateFile(
            args->path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL
        );
        if (hDir == INVALID_HANDLE_VALUE) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> Failed to open folder: %s\n", args->path);
            #endif
            WEBUI_THREAD_RETURN
        }
        char buffer[1024];
        DWORD bytesReturned;
        while (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE)) {
            if (ReadDirectoryChangesW(
                    hDir, buffer, sizeof(buffer), TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                    FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL
                ))
            {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> Folder updated\n");
                #endif
                webinix_run(args->win_num, "location.reload();");
            } else {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> Failed to read folder changes\n");
                #endif
                break;
            }
        }
        CloseHandle(hDir);
    #elif __linux__
        // Linux
        int fd = inotify_init();
        if (fd < 0) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> inotify_init error\n");
            #endif
            WEBUI_THREAD_RETURN
        }
        int wd = inotify_add_watch(fd, args->path, IN_MODIFY | IN_CREATE | IN_DELETE);
        if (wd < 0) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> inotify_add_watch error\n");
            #endif
            close(fd);
            WEBUI_THREAD_RETURN
        }
        char buffer[1024];
        while (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE)) {
            int length = read(fd, buffer, sizeof(buffer));
            if (length < 0) {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> read error\n");
                #endif
                break;
            }
            int i = 0;
            while (i < length) {
                struct inotify_event *event = (struct inotify_event *)&buffer[i];
                if (event->len) {
                    if (event->mask&(IN_CREATE | IN_DELETE | IN_MODIFY)) {
                        #ifdef WEBUI_LOG
                        printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> Folder updated\n");
                        #endif
                        webinix_run(args->win_num, "location.reload();");
                    }
                }
                i += sizeof(struct inotify_event) + event->len;
            }
        }
        inotify_rm_watch(fd, wd);
        close(fd);
    #else
        // macOS
        int kq = kqueue();
        if (kq == -1) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> kqueue error\n");
            #endif
            WEBUI_THREAD_RETURN
        }
        int fd = open(args->path, O_RDONLY);
        if (fd == -1) {
            #ifdef WEBUI_LOG
            printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> open error\n");
            #endif
            close(kq);
            WEBUI_THREAD_RETURN
        }
        struct kevent change;
        EV_SET(&change, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_WRITE, 0, NULL);
        while (!_webinix_mutex_is_exit_now(WEBUI_MUTEX_NONE)) {
            struct kevent event;
            int nev = kevent(kq, &change, 1, &event, 1, NULL);
            if (nev == -1) {
                #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> kevent error\n");
                #endif
                break;
            } else if (nev > 0) {
                if (event.fflags&NOTE_WRITE) {
                    #ifdef WEBUI_LOG
                    printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> Folder updated\n");
                    #endif
                    webinix_run(args->win_num, "location.reload();");
                }
            }
        }
        close(fd);
        close(kq);
    #endif
    #ifdef WEBUI_LOG
    printf("[Core]\t\t[Thread .] _webinix_folder_monitor_thread() -> Exit\n");
    #endif
    WEBUI_THREAD_RETURN
}

static void _webinix_bridge_api_handler(webinix_event_t* e) {

    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_bridge_api_handler()\n");
    #endif

	// This function gets called by `webinix.js` to reach
    // an internal core API. This is not a public user API.

    const char* api_name = webinix_get_string(e);
    #ifdef WEBUI_LOG
    printf("[Core]\t\t_webinix_bridge_api_handler() -> api_name: [%s]\n", api_name);
    #endif

    if (strcmp(api_name, "high_contrast") == 0) {
        webinix_return_bool(e, webinix_is_high_contrast());
    }
}
