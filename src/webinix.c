/*
    Webinix Library 2.0.0
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

// -- Third-party ---------------------
#include "mongoose.h"

// -- Webinix ---------------------------
#include "webinix.h"

// -- Heap ----------------------------
#define WEBUI_LOG
webinix_t webinix;

// -- JavaScript Bridge ---------------
// Uncompressed version to allow debugging
// in the browser using the builtin dev-tools
static const char* webinix_javascript_bridge = 
"var _webinix_log = false; \n"
"var _webinix_ws; \n"
"var _webinix_ws_status = false; \n"
"var _webinix_action8 = new Uint8Array(1); \n"
"var _webinix_action_val; \n"
"var _webinix_allow_close = false; \n"
"function _webinix_close(vbyte, v) { \n"
"    _webinix_ws_status = false; \n"
"    _webinix_action8[0] = vbyte; \n"
"    _webinix_action_val = v; \n"
"    _webinix_ws.close(); \n"
"} \n"
"function _webinix_freez_ui() { \n"
"    document.body.style.filter = \"contrast(1%)\"; \n"
"} \n"
"function _webinix_start() { \n"
"    if (\"WebSocket\" in window) { \n"
"        _webinix_ws = new WebSocket(\"ws://localhost:\" + _webinix_port + \"/_ws\"); \n"
"        _webinix_ws.binaryType = \"blob\"; \n"
"        _webinix_ws.onopen = function () { \n"
"            _webinix_ws.binaryType = \"blob\"; \n"
"            _webinix_ws_status = true; \n"
"            if (_webinix_log) console.log(\"Webinix -> Connected\"); \n"
"            _webinix_listener(); \n"
"        }; \n"
"        _webinix_ws.onerror = function () { \n"
"            if (_webinix_log) console.log(\"Webinix -> Connection error\"); \n"
"            _webinix_close(255, \"\"); \n"
"        }; \n"
"        _webinix_ws.onclose = function (evt) { \n"
"            _webinix_ws_status = false; \n"
"            if (_webinix_action8[0] == 252) { \n"
"                if (_webinix_log) console.log(\"Webinix -> Switch URL\"); \n"
"                window.location.replace(_webinix_action_val); \n"
"            } else { \n"
"                if (_webinix_log) console.log(\"Webinix -> Connection lost [\" + evt.code + \"][\" + evt.reason + \"]\"); \n"
"                if (!_webinix_log) window.close(); \n"
"                else _webinix_freez_ui(); \n"
"            } \n"
"        }; \n"
"        _webinix_ws.onmessage = function (evt) { \n"
"            var reader; \n"
"            var buffer8; \n"
"            reader = new FileReader(); \n"
"            reader.readAsArrayBuffer(evt.data); \n"
"            reader.addEventListener(\"loadend\", function (e) { \n"
"                buffer8 = new Uint8Array(e.target.result); \n"
"                if (buffer8.length < 1) return; \n"
"                if (buffer8[0] !== 255) { \n"
"                    if (_webinix_log) console.log(\"Webinix -> Invalid flag -> 0x\" + buffer8[0] + \" 0x\" + buffer8[1] + \" 0x\" + buffer8[2]); \n"
"                    return; \n"
"                } \n"
"                if (_webinix_log) console.log(\"Webinix -> Flag -> 0x\" + buffer8[0] + \" 0x\" + buffer8[1] + \" 0x\" + buffer8[2]); \n"
"                var len = buffer8.length - 3; \n"
"                if (buffer8[buffer8.length - 1] === 0) \n"
"                   len--; // Null terminated byte can break eval() \n"
"                data8 = new Uint8Array(len); \n"
"                for (i = 0; i < len; i++) data8[i] = buffer8[i + 3]; \n"
"                var data8utf8 = new TextDecoder(\"utf-8\").decode(data8); \n"
"                if (buffer8[1] === 252) { \n"
"                    _webinix_close(252, data8utf8); \n"
"                } else if (buffer8[1] === 251) { \n"
"                    _webinix_close(251, \"\"); \n"
"                } else if (buffer8[1] === 254) { \n"
"                    data8utf8 = data8utf8.replace(/(?:\\r\\n|\\r|\\n)/g, \"\\\\n\"); \n"
"                    if (_webinix_log) console.log(\"Webinix -> JS -> Run -> \" + data8utf8); \n"
"                    var FunReturn = \"undefined\"; \n"
"                    var FunError = false; \n"
"                    try { FunReturn = eval('(() => {' + data8utf8 + '})()'); } catch (e) { FunError = true; FunReturn = e.message } \n"
"                    if (typeof FunReturn === \"undefined\" || FunReturn === undefined) FunReturn = \"undefined\"; \n"
"                    if (_webinix_log && !FunError) console.log(\"Webinix -> JS -> Return -> \" + FunReturn); \n"
"                    if (_webinix_log && FunError) console.log(\"Webinix -> JS -> Return Error -> \" + FunReturn); \n"
"                    var FunReturn8 = new TextEncoder(\"utf-8\").encode(FunReturn); \n"
"                    var Return8 = new Uint8Array(4 + FunReturn8.length); \n"
"                    Return8[0] = 255; \n"
"                    Return8[1] = 254; \n"
"                    Return8[2] = buffer8[2]; \n"
"                    if(FunError) Return8[3] = 1; \n"
"                    else Return8[3] = 0; \n"
"                    var p = -1; \n"
"                    for (i = 4; i < FunReturn8.length + 4; i++) Return8[i] = FunReturn8[++p]; \n"
"                    if (Return8[0] !== 255) { \n"
"                        if (_webinix_log) console.log(\"Webinix -> JS -> Generate response failed -> 0x\" + buffer8[0] + \" 0x\" +  \n"
"                                                       buffer8[1] + \" 0x\" + buffer8[2]); \n"
"                        return; \n"
"                    } \n"
"                    if (_webinix_ws_status) _webinix_ws.send(Return8.buffer); \n"
"                    if (_webinix_log) { \n"
"                        var buf = \"[ \"; \n"
"                        for (i = 0; i < Return8.length; i++) buf = buf + \"0x\" + Return8[i] + \" \"; \n"
"                        buf = buf + \"]\"; \n"
"                        console.log(\"Webinix -> JS -> Sent response -> [\" + FunReturn + \"] (\" + buf + \")\"); \n"
"                    } \n"
"                } \n"
"            }); \n"
"        }; \n"
"    } else { \n"
"        alert(\"Sorry. WebSocket not supported by your Browser.\"); \n"
"        if (!_webinix_log) webinix_close_window(); \n"
"    } \n"
"} \n"
"function _webinix_SendEvent(name) { \n"
"    if (_webinix_ws_status && name != \"\") { \n"
"        var Name8 = new TextEncoder(\"utf-8\").encode(name); \n"
"        var Event8 = new Uint8Array(3 + Name8.length); \n"
"        Event8[0] = 255; \n"
"        Event8[1] = 253; \n"
"        Event8[2] = 0; \n"
"        var p = -1; \n"
"        for (i = 3; i < Name8.length + 3; i++) Event8[i] = Name8[++p]; \n"
"        if (_webinix_ws_status) _webinix_ws.send(Event8.buffer); \n"
"        if (_webinix_log) { \n"
"            var buf = \"[ \"; \n"
"            for (i = 0; i < Event8.length; i++) buf = buf + \"0x\" + Event8[i] + \" \"; \n"
"            buf = buf + \"]\"; \n"
"            console.log(\"Webinix -> Event -> Send -> [\" + name + \"] (\" + buf + \")\"); \n"
"        } \n"
"    } \n"
"} \n"
"function _webinix_listener_handler() { \n"
"    var elems = document.getElementsByTagName(\"form\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        _webinix_ws_status = false; \n"
"        alert(\"Incompatible HTML.\\n\\nYour HTML contain <form> elements, wish is not compatible with Webinix. Please remove all those elements.\"); \n"
"        _webinix_close(255, \"\"); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"button\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <Button> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"div\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <Div> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"li\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <LI> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"p\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <P> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"a\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <A> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"p\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <P> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"ul\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <UL> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"footer\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <FOOTER> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"nav\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <NAV> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"    elems = document.getElementsByTagName(\"span\"); \n"
"    for (i = 0; i < elems.length; i++) { \n"
"        if (elems[i].id == \"\") continue; \n"
"        if (_webinix_log) console.log(\"Webinix -> Listen -> <SPAN> -> \" + elems[i].id); \n"
"        elems[i].addEventListener(\"click\", function () { \n"
"            _webinix_SendEvent(this.id); \n"
"        }); \n"
"    } \n"
"} \n"
"function _webinix_listener() { \n"
"    window.addEventListener(\"load\", _webinix_listener_handler()); \n"
"} \n"
"_webinix_start(); \n"
"setTimeout(function () { \n"
"    if (!_webinix_ws_status) { \n"
"        document.body.style.filter = \"contrast(1%)\"; \n"
"        alert(\"Webinix failed to find the background application.\"); \n"
"        if (!_webinix_log) webinix_close_window(); \n"
"    } \n"
"}, 1e3); \n"
"document.addEventListener(\"keydown\", function (e) { \n"
"    if (e.keyCode === 116) { \n"
"        e.preventDefault(); \n"
"        e.returnValue = false; \n"
"        e.keyCode = 0; \n"
"        return false; \n"
"    } \n"
"}); \n"
"window.addEventListener(\"beforeunload\", function (e) { \n"
"    _webinix_close(255, \"\"); \n"
"}); \n"
"window.onbeforeunload = function () { \n"
"    _webinix_close(255, \"\"); \n"
"}; \n"
"document.addEventListener(\"contextmenu\", function (e) {}); \n"
"if (typeof webinix_ready === \"function\") setTimeout(webinix_ready, 1); \n"
"function webinix_debug(status) { \n"
"    if (status) { \n"
"        console.log(\"Webinix -> Debug log enabled.\"); \n"
"        _webinix_log = true; \n"
"    } else { \n"
"        console.log(\"Webinix -> Debug log disabled.\"); \n"
"        _webinix_log = false; \n"
"    } \n"
"} \n"
"function webinix_close_window() { \n"
"    _webinix_freez_ui(); \n"
"    _webinix_allow_close = true; \n"
"    if (_webinix_ws_status) _webinix_close(255, \"\"); \n"
"    else window.close(); \n"
"} \n"
"function webinix_event(event_name) { \n"
"    if (!_webinix_ws_status) { \n"
"        console.log(\"Webinix -> Send Event -> Failed because status is disconnected.\"); \n"
"        return; \n"
"    } \n"
"    var event_name8 = new TextEncoder(\"utf-8\").encode(event_name); \n"
"    var SendEvent8 = new Uint8Array(3 + event_name8.length); \n"
"    SendEvent8[0] = 255; \n"
"    SendEvent8[1] = 250; \n"
"    SendEvent8[2] = 0; \n"
"    var p = -1; \n"
"    for (i = 3; i < event_name8.length + 3; i++) SendEvent8[i] = event_name8[++p]; \n"
"    if (SendEvent8[0] !== 255) { \n"
"        if (_webinix_log) console.log(\"Webinix -> Send Event -> Generate header failed -> 0x\" + SendEvent8[0] + \" 0x\" + SendEvent8[1] + \" 0x\" + SendEvent8[2]); \n"
"        return; \n"
"    } \n"
"    if (_webinix_ws_status) _webinix_ws.send(SendEvent8.buffer); \n"
"    if (_webinix_log) { \n"
"        var buf = \"[ \"; \n"
"        for (i = 0; i < SendEvent8.length; i++) buf = buf + \"0x\" + SendEvent8[i] + \" \"; \n"
"        buf = buf + \"]\"; \n"
"        console.log(\"Webinix -> Send Event -> [\" + event_name + \"] (\" + buf + \")\"); \n"
"    } \n"
"}";

// -- Heap ----------------------------
static const char* webinix_html_served = "<html><head><title>Access Denied</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Access Denied</h2><p>You can't access this window<br>because it's already served.<br><br>The security policy is set to<br>deny multiple requests.</p><br><a href=\"https://www.webinix.me\"><small>Webinix Library<small></a></body></html>";
static const char* webinix_html_res_not_available = "<html><head><title>Resource Not Available</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Resource Not Available</h2><p>The requested resource is not available.</p><br><a href=\"https://www.webinix.me\"><small>Webinix Library<small></a></body></html>";
static const char* webinix_deno_not_found = "<html><head><title>Deno Not Found</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Deno Not Found</h2><p>Deno not found on your system.<br>Please download it from <a href=\"https://github.com/denoland/deno/releases\">https://github.com/denoland/deno/releases</a></p><br><a href=\"https://www.webinix.me\"><small>Webinix Library<small></a></body></html>";
static const char* webinix_nodejs_not_found = "<html><head><title>Node.js Not Found</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Node.js Not Found</h2><p>Node.js not found on your system.<br>Please download it from <a href=\"https://nodejs.org/en/download/\">https://nodejs.org/en/download/</a></p><br><a href=\"https://www.webinix.me\"><small>Webinix Library<small></a></body></html>";
static const char* webinix_def_icon = "<?xml version=\"1.0\" ?><svg height=\"24\" version=\"1.1\" width=\"24\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:cc=\"http://creativecommons.org/ns#\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"><g transform=\"translate(0 -1028.4)\"><path d=\"m3 1030.4c-1.1046 0-2 0.9-2 2v7 2 7c0 1.1 0.8954 2 2 2h9 9c1.105 0 2-0.9 2-2v-7-2-7c0-1.1-0.895-2-2-2h-9-9z\" fill=\"#2c3e50\"/><path d=\"m3 2c-1.1046 0-2 0.8954-2 2v3 3 1 1 1 3 3c0 1.105 0.8954 2 2 2h9 9c1.105 0 2-0.895 2-2v-3-4-2-3-3c0-1.1046-0.895-2-2-2h-9-9z\" fill=\"#34495e\" transform=\"translate(0 1028.4)\"/><path d=\"m4 5.125v1.125l3 1.75-3 1.75v1.125l5-2.875-5-2.875zm5 4.875v1h5v-1h-5z\" fill=\"#ecf0f1\" transform=\"translate(0 1028.4)\"/></g></svg>";
static const char* webinix_def_icon_type = "image/svg+xml";
static const char* webinix_js_empty = "WEBUI_JS_EMPTY";
static const char* webinix_js_timeout = "WEBUI_JS_TIMEOUT";

#ifdef _WIN32
    static const char* webinix_sep = "\\";
#else
    static const char* webinix_sep = "/";
#endif

// -- Functions -----------------------
bool _webinix_ptr_exist(void *p) {

    #ifdef WEBUI_LOG
        // printf("[0] _webinix_ptr_exist()... \n");
    #endif

    if(p == NULL)
        return false;
    
    for(unsigned int i = 0; i < webinix.ptr_position; i++) {

        if(webinix.ptr_list[i] == p)
            return true;
    }

    return false;
}

void _webinix_ptr_add(void *p, size_t size) {

    #ifdef WEBUI_LOG
        // printf("[0] _webinix_ptr_add()... \n");
    #endif

    if(p == NULL)
        return;
    
    if(!_webinix_ptr_exist(p)) {

        for(unsigned int i = 0; i < webinix.ptr_position; i++) {

            if(webinix.ptr_list[i] == NULL) {

                webinix.ptr_list[i] = p;
                webinix.ptr_size[i] = size;
                return;
            }
        }

        webinix.ptr_list[webinix.ptr_position] = p;
        webinix.ptr_size[webinix.ptr_position] = size;
        webinix.ptr_position++;
    }
}

void _webinix_free_mem(void **p) {

    #ifdef WEBUI_LOG
        // printf("[0] _webinix_free_mem()... \n");
    #endif

    if(p == NULL || *p == NULL)
        return;

    for(unsigned int i = 0; i < webinix.ptr_position; i++) {

        if(webinix.ptr_list[i] == *p) {

            memset(*p, 0x00, webinix.ptr_size[i]);
            free(*p);

            webinix.ptr_size[i] = 0;
            webinix.ptr_list[i] = NULL;
        }
    }

    for(unsigned int i = webinix.ptr_position; i >= 0; i--) {

        if(webinix.ptr_list[i] == NULL) {

            webinix.ptr_position = i;
            break;
        }
    }

    *p = NULL;
}

void _webinix_panic() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_panic()... \n");
    #endif

    exit(EXIT_FAILURE);
}

void* _webinix_malloc(size_t size) {

    #ifdef WEBUI_LOG
        // printf("[0] _webinix_malloc([%d])... \n", size);
    #endif

    size++;
    void* block;

    for(unsigned int i = 0; i < 8; i++) {

        if(size > WEBUI_MAX_BUF)
            size = WEBUI_MAX_BUF;

        block = malloc(size);

        if(block == NULL)
            size++;
        else
            break;
    }

    if(block == NULL) {

        _webinix_panic();
        return NULL;
    }

    memset(block, 0x00, size);

    _webinix_ptr_add((void *) block, size);

    return block;
}

void _webinix_sleep(long unsigned int ms) {

    #ifdef WEBUI_LOG
        // printf("[0] _webinix_sleep([%d])... \n", ms);
    #endif

    #ifdef _WIN32
        Sleep(ms);
    #else
        sleep(ms);
    #endif
}

void _webinix_print_hex(const char* data, size_t len) {

    for(size_t i = 0; i < len; i++) {

        printf("0x%02X ", (unsigned char) *data);
        data++;
    }
}

bool _webinix_is_empty(const char* s) {

    #ifdef WEBUI_LOG
        // printf("[0] _webinix_is_empty()... \n");
    #endif

    if ((s != NULL) && (s[0] != '\0'))
        return false;
    return true;
}

bool _webinix_file_exist(char* file) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_file_exist([%s])... \n", file);
    #endif

    if(_webinix_is_empty(file))
        return false;

    if(WEBUI_FILE_EXIST(file, 0) == 0)
        return true;
    return false;
}

const char* _webinix_get_extension(const char *f) {

    if(f == NULL)
        return "";

    const char *ext = strrchr(f, '.');

    if(ext == NULL || !ext || ext == f)
        return "";
    return ext + 1;
}

unsigned int _webinix_get_run_id() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_get_run_id()... \n");
    #endif

    return ++webinix.run_last_id;
}

bool _webinix_socket_test_connect_mg(unsigned int port_num) {

    struct mg_mgr mgr;
    struct mg_connection *c;
    mg_mgr_init(&mgr);

    char url[32];
    sprintf(url, "localhost:%d", port_num);

    c = mg_connect(&mgr, url, NULL, NULL);
    if (c == NULL) {

        mg_close_conn(c);
        mg_mgr_free(&mgr);
        return false;
    }
    
    // Cleaning
    mg_close_conn(c);
    mg_mgr_free(&mgr);

    // Connection Success
    return true;
}

bool _webinix_socket_test_connect(unsigned int port_num) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_socket_test_connect([%d])... \n", port_num);
    #endif

    #ifdef _WIN32
        // -- Win32 ---------------------
        WSADATA wsaData;
        SOCKET ConnectSocket = INVALID_SOCKET;
        struct addrinfo *result = NULL, hints;
        unsigned int iResult;
        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if(iResult != 0)
            return false;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        char the_port[16];
        sprintf(the_port, "%d", port_num);
        iResult = getaddrinfo("localhost", the_port, &hints, &result);
        if(iResult != 0) {
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }
        ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(ConnectSocket == INVALID_SOCKET) {
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }
        iResult = connect(ConnectSocket, result->ai_addr, (unsigned int)result->ai_addrlen);
        if(iResult == SOCKET_ERROR || ConnectSocket == INVALID_SOCKET) {
            closesocket(ConnectSocket);
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }
        
        // Cleaning
        closesocket(ConnectSocket);
        freeaddrinfo(result);
        WSACleanup();

        // Connection Success
        return true;
    #else
        // ...
    #endif
}

bool _webinix_socket_test_listen(unsigned int port_num) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_socket_test_listen([%d])... \n", port_num);
    #endif

    #ifdef _WIN32
        // -- Win32 ---------------------
        WSADATA wsaData;
        unsigned int iResult;
        SOCKET ListenSocket = INVALID_SOCKET;
        struct addrinfo *result = NULL;
        struct addrinfo hints;
        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if(iResult != 0) {
            // WSAStartup failed
            return false;
        }
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;
        // Resolve the server address and port
        char the_port[16];
        sprintf(the_port, "%d", port_num);
        iResult = getaddrinfo("localhost", the_port, &hints, &result);
        if(iResult != 0) {
            WSACleanup();
            return false;
        }
        // Create a SOCKET for the server to listen for client connections.
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(ListenSocket == INVALID_SOCKET) {
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }
        // Setup the TCP listening socket
        iResult = bind(ListenSocket, result->ai_addr, (unsigned int)result->ai_addrlen);
        if(iResult == SOCKET_ERROR) {
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            return false;
        }

        // Clean
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        
        // Listening Success
        return true;
    #else
        // ...
    #endif
}

bool _webinix_port_is_used(unsigned int port_num) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_port_is_used([%d])... \n", port_num);
    #endif

    #ifdef _WIN32
        // -- Win32 ---------------------

        // Connect test
        // if(_webinix_socket_test_connect(port_num))
        //     return true;

        // Connect test MG
        // if(_webinix_socket_test_connect_mg(port_num))
        //     return true;
        
        // Listener test
        if(!_webinix_socket_test_listen(port_num))
            return true; // Port is busy

        // Port is not in use
        return false;
    #else
        // ...
    #endif
}

void _webinix_serve_file(webinix_window_t* win, struct mg_connection *c, void *ev_data) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_serve_file()... \n", win->core.window_number);
    #endif

    // Serve a normal text based file
    // send with HTTP 200 status code

    struct mg_http_serve_opts opts = {

        .root_dir = win->path
    };

    mg_http_serve_dir(c, ev_data, &opts);
}

bool _webinix_deno_exist() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_deno_exist()... \n");
    #endif

    static bool found = false;

    if(found)
        return true;

    #ifdef _WIN32
        if(system("deno --version > nul 2>&1") == 0)
    #else
        if(system("deno --version >>/dev/null 2>>/dev/null") == 0)
    #endif
    {
        found = true;
        return true;
    }
    else
        return false;
}

bool _webinix_nodejs_exist() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_nodejs_exist()... \n");
    #endif

    static bool found = false;

    if(found)
        return true;

    #ifdef _WIN32
        if(system("node -v > nul 2>&1") == 0)
    #else
        if(system("node -v >>/dev/null 2>>/dev/null") == 0)
    #endif
    {
        found = true;
        return true;
    }
    else
        return false;
}

const char* _webinix_interpret_command(const char* cmd) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_interpret_command()... \n");
    #endif

    FILE *runtime = WEBUI_POPEN(cmd, "r");

    if(runtime == NULL)
        return NULL;

    // Get STDOUT length
    // int c;
    // while ((c = fgetc(runtime)) != EOF)
    //     len++;
    int len = 1024 * 8;

    // Read STDOUT
    char* out = (char*) _webinix_malloc(len + 1);
    char* line = (char*) _webinix_malloc(1024);
    while(fgets(line, 1024, runtime) != NULL)
        strcat(out, line);

    WEBUI_PCLOSE(runtime);
    _webinix_free_mem((void *) &line);

    return (const char*) out;
}

void _webinix_interpret_file(webinix_window_t* win, struct mg_connection *c, void *ev_data, char* index) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_interpret_file()... \n", win->core.window_number);
    #endif

    // Run the JavaScript / TypeScript runtime
    // and send back the output with HTTP 200 status code
    // otherwise, send the file as a normal text based one

    char* file;
    char* full_path;

    if(!_webinix_is_empty(index)) {

        // Parse index file
        file = index;
        full_path = index;
    }
    else {

        // Parse other files

        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        // Get file name
        file = (char*) _webinix_malloc(hm->uri.len + 1);
        const char* p = hm->uri.ptr;
        p++; // Skip "/"
        sprintf(file, "%.*s", (int)(hm->uri.len - 1), p);

        // Get full path
        full_path = (char*) _webinix_malloc(strlen(webinix.executable_path) + 1 + strlen(file) + 1);
        sprintf(full_path, "%s%s%s", webinix.executable_path, webinix_sep, file);

        if(!_webinix_file_exist(full_path)) {

            // File not exist - 404
            _webinix_serve_file(win, c, ev_data);

            _webinix_free_mem((void *) &file);
            _webinix_free_mem((void *) &full_path);
            return;
        }
    }

    // Get file extension
    const char* extension = _webinix_get_extension(file);

    if(strcmp(extension, "ts") == 0 || strcmp(extension, "js") == 0) {

        // TypeScript / JavaScript

        if(win->core.runtime == webinix.runtime.deno) {

            // Use Deno

            if(_webinix_deno_exist()) {

                // Set command
                char* cmd = (char*) _webinix_malloc(64 + strlen(full_path) + 1);
                #ifdef _WIN32
                    sprintf(cmd, "Set NO_COLOR=1 & deno run --allow-all 2>&1 \"%s\" ", full_path);
                #else
                    sprintf(cmd, "NO_COLOR=1 & deno run --allow-all 2>&1 \"%s\" ", full_path);
                #endif

                // Run command
                const char* out = _webinix_interpret_command(cmd);

                if(out != NULL) {

                    // Send deno output
                    mg_http_reply(
                        c, 200,
                        "",
                        out
                    );
                }
                else {

                    // Deno failed.
                    // Serve as a normal text-based file
                    _webinix_serve_file(win, c, ev_data);
                }

                _webinix_free_mem((void *) &cmd);
                _webinix_free_mem((void *) &out);
            }
            else {

                // Deno not installed

                mg_http_reply(
                    c, 200,
                    "",
                    webinix_deno_not_found
                );
            }
        }
        else if(win->core.runtime == webinix.runtime.nodejs) {

            // Use Nodejs

            if(_webinix_nodejs_exist()) {

                // Set command
                char* cmd = (char*) _webinix_malloc(64 + strlen(full_path) + 1);
                sprintf(cmd, "node \"%s\" 2>&1 ", full_path);

                // Run command
                const char* out = _webinix_interpret_command(cmd);

                if(out != NULL) {

                    // Send Node.js output
                    mg_http_reply(
                        c, 200,
                        "",
                        out
                    );
                }
                else {

                    // Node.js failed.
                    // Serve as a normal text-based file
                    _webinix_serve_file(win, c, ev_data);
                }

                _webinix_free_mem((void *) &cmd);
                _webinix_free_mem((void *) &out);
            }
            else {

                // Node.js not installed

                mg_http_reply(
                    c, 200,
                    "",
                    webinix_nodejs_not_found
                );
            }
        }
        else {

            // Unknown runtime
            // Serve as a normal text-based file
            _webinix_serve_file(win, c, ev_data);
        }
    }
    else {

        // Unknown file extension
        // Serve as a normal text-based file
        _webinix_serve_file(win, c, ev_data);
    }

    _webinix_free_mem((void *) &file);
    _webinix_free_mem((void *) &full_path);
}

static void _webinix_server_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {

    webinix_window_t* win = (webinix_window_t *) fn_data;

    #ifdef WEBUI_LOG
        // printf("[%d] _webinix_server_event_handler()... \n", win->core.window_number);
    #endif

    if(ev == MG_EV_OPEN) {

        // c->is_hexdumping = 1;
    }
    else if(ev == MG_EV_HTTP_MSG) {

        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if(mg_http_match_uri(hm, "/_ws")) {

            // WebSocket

            #ifdef WEBUI_LOG
                printf("[%d] _webinix_server_event_handler()... HTML Upgrade to WebSocket\n", win->core.window_number);
            #endif

            mg_ws_upgrade(c, hm, NULL);
        } 
        else if(mg_http_match_uri(hm, "/webinix.js")) {

            // Webinix Bridge

            #ifdef WEBUI_LOG
                printf("[%d] _webinix_server_event_handler()... HTML Webinix JS\n", win->core.window_number);
            #endif

            // Generate JavaScript bridge
            size_t len = 64 + strlen(webinix_javascript_bridge);
            char* js = (char *) _webinix_malloc(len);
            sprintf(js, 
                "_webinix_port = %d; \n %s \n",
                win->core.server_port, webinix_javascript_bridge
            );

            // Header
            // Content-Type: text/javascript

            // Send
            mg_http_reply(
                c, 200,
                "",
                js
            );

            _webinix_free_mem((void *) &js);
        }
        else if(mg_http_match_uri(hm, "/favicon.ico") || 
                mg_http_match_uri(hm, "/favicon.svg") || 
                mg_http_match_uri(hm, "/favicon.png")) {

            // Send favicon

            if(win->core.icon) {
                
                // TODO: Add core.icon_type to the header

                // Header
                // ...

                // User icon
                mg_http_reply(
                    c, 200,
                    "",
                    win->core.icon
                );
            }
            else {

                // TODO: Use webinix_def_icon_type

                // Header
                // Content-Type: image/svg+xml
                
                // Default icon
                mg_http_reply(
                    c, 200,
                    "",
                    webinix_def_icon
                );
            }
        }
        else if(mg_http_match_uri(hm, "/")) {

            // [/]

            if(win->core.server_root) {

                // Serve local files

                #ifdef WEBUI_LOG
                    printf("[%d] _webinix_server_event_handler()... HTML Root Index\n", win->core.window_number);
                #endif

                win->core.server_handled = true;

                // Set full path
                // [Path][Sep][Index File Name][Null]
                char* index = (char*) _webinix_malloc(strlen(webinix.executable_path) + 1 + 8 + 1); 

                // Index.ts
                sprintf(index, "%s%sindex.ts", webinix.executable_path, webinix_sep);
                if(_webinix_file_exist(index)) {

                    // TypeScript Index
                    _webinix_interpret_file(win, c, ev_data, index);

                   _webinix_free_mem((void *) &index);
                    return;
                }

                // Index.js
                sprintf(index, "%s%sindex.js", webinix.executable_path, webinix_sep);
                if(_webinix_file_exist(index)) {

                    // JavaScript Index
                    _webinix_interpret_file(win, c, ev_data, index);

                    _webinix_free_mem((void *) &index);
                    return;
                }

                _webinix_free_mem((void *) &index);
                
                // Index.html
                // Serve as a normal text-based file
                _webinix_serve_file(win, c, ev_data);
            }
            else {

                // Main HTML

                if(!win->core.multi_access && win->core.server_handled) {

                    // Main HTML already handled.

                    #ifdef WEBUI_LOG
                        printf("[%d] _webinix_server_event_handler()... HTML Main Already Handled\n", win->core.window_number);
                    #endif

                    // Header
                    // text/html; charset=utf-8

                    mg_http_reply(
                        c, 200,
                        "",
                        webinix_html_served
                    );
                }
                else {

                    // Send main HTML

                    #ifdef WEBUI_LOG
                        printf("[%d] _webinix_server_event_handler()... HTML Main\n", win->core.window_number);
                    #endif

                    win->core.server_handled = true;

                    // Inject JavaScript bridge into HTML
                    size_t len = strlen(win->core.html) + 6 + strlen(webinix_javascript_bridge) + 128;
                    char* html = (char *) _webinix_malloc(len);
                    sprintf(html, 
                        "%s \n <script type = \"text/javascript\"> \n _webinix_port = %d; \n %s \n </script>",
                        win->core.html, win->core.server_port, webinix_javascript_bridge
                    );

                    // // HTTP Header
                    // char header[512];
                    // memset(header, 0x00, 512);
                    // sprintf(header,
                    //     "HTTP/1.1 200 OK\r\n"
                    //     "Content-Type: text/html; charset=utf-8\r\n"
                    //     "Host: localhost:%d\r\n"
                    //     "Cache-Control: no-cache\r\n"
                    //     "Content-Length: %d\r\n"
                    //     "Connection: close\r\n\r\n",
                    //     win->core.server_port, strlen(html)
                    // );

                    // Send
                    mg_http_reply(
                        c, 200,
                        "",
                        html
                    );

                    _webinix_free_mem((void *) &html);
                }
            }
        }
        else {

            // [/file]

            if(win->core.server_root) {

                if(win->core.runtime != webinix.runtime.none) {

                    // Interpret file

                    #ifdef WEBUI_LOG
                        printf("[%d] _webinix_server_event_handler()... HTML Interpret file\n", win->core.window_number);
                    #endif

                    _webinix_interpret_file(win, c, ev_data, NULL);
                }
                else {

                    // Serve local files

                    #ifdef WEBUI_LOG
                        printf("[%d] _webinix_server_event_handler()... HTML Root file\n", win->core.window_number);
                    #endif

                    // Serve as a normal text-based file
                    _webinix_serve_file(win, c, ev_data);
                }
            }
            else {

                // Resource Not Available

                #ifdef WEBUI_LOG
                    printf("[%d] _webinix_server_event_handler()... HTML 404\n", win->core.window_number);
                #endif

                // Header
                // text/html; charset=utf-8

                mg_http_reply(
                    c, 200,
                    "",
                    webinix_html_res_not_available
                );
            }
        }
    }
    else if(ev == MG_EV_WS_MSG) {

        #ifdef WEBUI_LOG
            printf("[%d] _webinix_server_event_handler()... WebSocket Data\n", win->core.window_number);
        #endif

        struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;

        // Parse the packet
        _webinix_window_receive(win, wm->data.ptr, wm->data.len);
    }
    else if(ev == MG_EV_WS_OPEN) {

        #ifdef WEBUI_LOG
            printf("[%d] _webinix_server_event_handler()... WebSocket Connected\n", win->core.window_number);
        #endif

        if(!win->core.connected) {

            // First connection

            webinix.connections++;                                // main loop
            win->core.connected = true;                         // server thread
            webinix.mg_connections[win->core.window_number] = c;  // websocket send func
        }
        else {

            if(win->core.multi_access) {

                // Multi connections
                win->core.connections++;
            }
            else {

                // UNWANTED Multi connections

                #ifdef WEBUI_LOG
                    printf("[%d] _webinix_server_event_handler() -> UNWANTED Multi Connections\n", win->core.window_number);
                #endif

                mg_close_conn(c);
            }
        }
    }
    else if(ev == MG_EV_WS_CTL) {

        #ifdef WEBUI_LOG
            printf("[%d] _webinix_server_event_handler()... WebSocket Closed\n", win->core.window_number);
        #endif

        if(win->core.connected) {

            if(win->core.multi_access && win->core.connections > 0) {

                // Multi connections close
                win->core.connections--;
            }
            else {

                // Main connection close
                webinix.connections--;            // main loop
                win->core.connected = false;    // server thread                
            }
        }
    }
}

#ifdef _WIN32
    DWORD WINAPI webinix_server_start(LPVOID arg) 
#else
    void* webinix_server_start(void* arg)
#endif
{
    webinix_window_t* win = (webinix_window_t*) arg;

    #ifdef WEBUI_LOG
        printf("[%d] [Thread] webinix_server_start(%s)... \n", win->core.window_number, win->core.url);
    #endif

    // Initialization
    webinix.servers++;
    win->core.server_running = true;
    unsigned int timeout = webinix.startup_timeout;
    if(timeout < 1)
        timeout = 1;

    // Start Server
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    webinix.mg_mgrs[win->core.window_number] = &mgr;

    if(mg_http_listen(&mgr, win->core.url, _webinix_server_event_handler, (void *) win) != NULL) {

        if(webinix.use_timeout) {

            bool stop = false;
            bool extra_used = false;

            for(;;) {

                if(!win->core.server_handled) {

                    // Wait for first connection
                    unsigned int n = 0;
                    do {

                        mg_mgr_poll(&mgr, 500);
                        if(win->core.connected)
                            break;
                        
                        if(webinix.timeout_extra && n >= (timeout * 2) && !extra_used) {

                            // At this moment we reached the timeout
                            // but if the browser is started and HTML
                            // is handled, then let's wait more to give
                            // the WebSocket an extra time to connect
                            n = 0;
                            extra_used = true;
                        }
                        else n++;

                    } while(n <= (timeout * 2));

                    if(!win->core.connected)
                        stop = true;
                }
                else {

                    for(;;) {

                        // Wait forever for disconnection
                        mg_mgr_poll(&mgr, 500);
                        if(!win->core.connected)
                            break;
                    }

                    if(win->core.server_handled)
                        stop = true;
                }

                if(stop)
                    break;
            }
        }
        else {

            // Wait forever
            for(;;)
                mg_mgr_poll(&mgr, 500);
        }
    }

    // Stop server
    mg_mgr_free(&mgr);
    webinix.servers--;

    #ifdef WEBUI_LOG
        printf("[%d] [Thread] webinix_server_start()... Server Stop.\n", win->core.window_number);
    #endif

    // Clean
    win->core.server_running = false;
    win->core.server_handled = false;
    win->core.connected = false;
    webinix.mg_mgrs[win->core.window_number] = NULL;
    webinix.mg_connections[win->core.window_number] = NULL;
    _webinix_free_port(win->core.server_port);

    return 0;
}

bool _webinix_browser_create_profile_folder(webinix_window_t* win, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_create_profile_folder(%d)... \n", browser);
    #endif
    
    if(browser == webinix.browser.custom) {

        // Custom Browser

        if(webinix.custom_browser == NULL)
            return false;

        return true;
    }

    char* temp = _webinix_browser_get_temp_path(browser);

    if(browser == webinix.browser.chrome) {

        sprintf(win->core.profile_path, "%s%sWebinixChromeProfile", temp, webinix_sep);
        return true;
    }
    
    if(browser == webinix.browser.edge) {

        sprintf(win->core.profile_path, "%s%sWebinixEdgeProfile", temp, webinix_sep);
        return true;
    }

    char* profile_name = "";

    if(browser == webinix.browser.firefox)
        profile_name = "WebinixFirefoxProfile";

    char parm[1024];
    sprintf(parm, "%s%s%s", temp, webinix_sep, profile_name);
    if(!_webinix_browser_folder_exist(parm)) {

        #ifdef _WIN32
            sprintf(parm, "cmd /c \"%s -CreateProfile \"Webinix %s%s%s\"\" > nul 2>&1", win->core.browser_path, temp, webinix_sep, profile_name);
            _webinix_cmd_sync(parm);
        #else
            sprintf(parm, "%s -CreateProfile \"Webinix %s%s%s\"", win->core.browser_path, temp, webinix_sep, profile_name);
            _webinix_cmd_sync(parm); // we can't do escaping to redirect error .. todo 
        #endif

        char buf[1024];
        sprintf(buf, "%s%s%s", temp, webinix_sep, profile_name);

        // Wait 10 second while slow PC create the folder..
        for(unsigned int n = 0; n <= (webinix.startup_timeout * 4); n++) {

            if(_webinix_browser_folder_exist(buf))
                break;
            
            _webinix_sleep(250);
        }

        if(!_webinix_browser_folder_exist(buf))
            return false;

        // prefs.js
        FILE *file;
        char file_path[1024];
        sprintf(file_path, "%s%s%s%sprefs.js", temp, webinix_sep, profile_name, webinix_sep);
        file = fopen(file_path, "a");
        if(file == NULL)
            return false;
        fputs("user_pref(\"toolkit.legacyUserProfileCustomizations.stylesheets\", true); ", file);
        fputs("user_pref(\"browser.shell.checkDefaultBrowser\", false); ", file);
        fputs("user_pref(\"browser.tabs.warnOnClose\", false); ", file);
        fclose(file);

        // userChrome.css
        sprintf(file_path, "mkdir \"%s%s%s%schrome%s\"", temp, webinix_sep, profile_name, webinix_sep, webinix_sep);
        _webinix_cmd_sync(file_path); // Create directory
        sprintf(file_path, "%s%s%s%schrome%suserChrome.css", temp, webinix_sep, profile_name, webinix_sep, webinix_sep);
        file = fopen(file_path, "a");
        if(file == NULL)
            return false;
        #ifdef _WIN32
            fputs(":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) {--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]) {z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}", file);
        #elif __APPLE__
            fputs(":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) {--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]) {z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}", file);
        #else
            fputs(":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) {--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]) {z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}", file);
        #endif
        fclose(file);

        sprintf(win->core.profile_path, "%s%s%s", temp, webinix_sep, profile_name);
    }

    return true;
}

bool _webinix_browser_folder_exist(char* folder) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_folder_exist([%s])... \n", folder);
    #endif

    #if defined(_MSC_VER)
        if(GetFileAttributes(folder) != INVALID_FILE_ATTRIBUTES)
            return true;
    #else
        DIR* dir = opendir(folder);
        if(dir) {
            closedir(dir);
            return true;
        }
    #endif

    return false;
}

char* _webinix_browser_get_temp_path(unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_get_temp_path([%d])... \n", browser);
    #endif

    #ifdef _WIN32
        // Resolve %USERPROFILE%
        #ifdef _MSC_VER
            char* WinUserProfile = NULL;
            size_t sz = 0;
            if(_dupenv_s(&WinUserProfile, &sz, "USERPROFILE") != 0 || WinUserProfile == NULL)
                return "";
        #else
            char* WinUserProfile = getenv("USERPROFILE"); // _dupenv_s
            if(WinUserProfile == NULL)
                return "";
        #endif
    #endif

    if(browser == webinix.browser.chrome) {

        #ifdef _WIN32
            return WinUserProfile;
        #elif __APPLE__
            char* tmpdir = getenv("TMPDIR"); 
            return tmpdir;
        #else
            return "/var/tmp";
        #endif
    }
    else if(browser == webinix.browser.firefox) {

        #ifdef _WIN32
            return WinUserProfile;
        #elif __APPLE__
            char* tmpdir = getenv("TMPDIR"); 
            return tmpdir;
        #else
            return "/var/tmp";
        #endif
    }
    else if(browser == webinix.browser.edge) {
        
        #ifdef _WIN32
            return WinUserProfile;
        #elif __APPLE__
            char* tmpdir = getenv("TMPDIR");
            return tmpdir;
        #else
            return "/var/tmp";
        #endif
    }
    
    _webinix_panic();
    return "";
}

bool _webinix_browser_exist(webinix_window_t* win, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_exist([%d])... \n", browser);
    #endif

    // Check if a browser exist

    if(browser == webinix.browser.custom) {

        // Custom Browser

        if(webinix.custom_browser == NULL)
            return false;

        return true;
    }

    #ifdef _WIN32
        // Resolve SystemDrive
        #ifdef _MSC_VER
            char* drive = NULL;
            size_t sz = 0;
            if(_dupenv_s(&drive, &sz, "SystemDrive") != 0 || drive == NULL)
                return false;
        #else
            char* drive = getenv("SystemDrive"); // _dupenv_s
            if(drive == NULL)
                return false;
        #endif
        char programs_folder32[1024];
        char programs_folder64[1024];
        sprintf(programs_folder32, "%s%sProgram Files (x86)", drive, webinix_sep);
        sprintf(programs_folder64, "%s%sProgram Files", drive, webinix_sep);
    #endif

    if(browser == webinix.browser.firefox) {

        // Firefox
        
        #ifdef _WIN32
        
            // Firefox 32/64 on Windows

            // TODO: Add support for C:\Program Files\Firefox Nightly\firefox.exe
            char fullpath32[1024];
            char fullpath64[1024];
            sprintf(fullpath32, "%s%sMozilla Firefox\\firefox.exe", programs_folder32, webinix_sep);
            sprintf(fullpath64, "%s%sMozilla Firefox\\firefox.exe", programs_folder64, webinix_sep);

            if(_webinix_file_exist(fullpath64)) {
                
                sprintf(win->core.browser_path, "\"%s\"", fullpath64);
                return true;
            }
            else if(_webinix_file_exist(fullpath32)) {

                sprintf(win->core.browser_path, "\"%s\"", fullpath32);
                return true;
            }
            else
                return false;

        #elif __APPLE__
            
            // Firefox on macOS
            if(_webinix_cmd_sync("open -R -a \"firefox\"") == 0) {

                sprintf(win->core.browser_path, "/Applications/Firefox.app/Contents/MacOS/firefox");
                return true;
            }
            else
                return false;
        #else

            // Firefox on Linux

            if(_webinix_cmd_sync("firefox -v >>/dev/null 2>>/dev/null") == 0) {

                sprintf(win->core.browser_path, "firefox");
                return true;
            }
            else
                return false;

        #endif

    }
    else if(browser == webinix.browser.chrome) {

        // Chrome

        #ifdef _WIN32

            // Chrome on Windows

            char fullpath32[1024];
            char fullpath64[1024];
            sprintf(fullpath32, "%s%sGoogle\\Chrome\\Application\\chrome.exe", programs_folder32, webinix_sep);
            sprintf(fullpath64, "%s%sGoogle\\Chrome\\Application\\chrome.exe", programs_folder64, webinix_sep);

            if(_webinix_file_exist(fullpath64)) {

                sprintf(win->core.browser_path, "\"%s\"", fullpath64);
                return true;
            }
            else if(_webinix_file_exist(fullpath32)) {

                sprintf(win->core.browser_path, "\"%s\"", fullpath32);
                return true;
            }
            else return false;

        #elif __APPLE__

            // Chrome on macOS
            if(_webinix_cmd_sync("open -R -a \"Google Chrome\"") == 0) {

                sprintf(win->core.browser_path, "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome");
                return true;
            }
            else
                return false;
        #else

            // Chrome on Linux
            if(_webinix_cmd_sync("google-chrome --version >>/dev/null 2>>/dev/null") == 0) {

                sprintf(win->core.browser_path, "google-chrome");
                return true;
            }
            else
                return false;

        #endif
    }
    else if(browser == webinix.browser.edge) {

        // Edge

        #ifdef _WIN32

            // Edge on Windows

            char fullpath32[1024];
            char fullpath64[1024];
            sprintf(fullpath32, "%s%sMicrosoft\\Edge\\Application\\msedge.exe", programs_folder32, webinix_sep);
            sprintf(fullpath64, "%s%sMicrosoft\\Edge\\Application\\msedge.exe", programs_folder64, webinix_sep);

            if(_webinix_file_exist(fullpath64)) {

                sprintf(win->core.browser_path, "\"%s\"", fullpath64);
                return true;
            }
            else if(_webinix_file_exist(fullpath32)) {

                sprintf(win->core.browser_path, "\"%s\"", fullpath32);
                return true;
            }
            else return false;

        #elif __APPLE__

            // Edge on macOS
            return false;

        #else

            // Edge on Linux
            return false;

        #endif
    }

    return false;
}

void _webinix_browser_clean() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_clean()... \n");
    #endif

    //TODO: Clean profile folders
    // Remove browser profile folders if needed
    // or keep it to save window positions for
    // future run.
}

#ifdef _WIN32
    DWORD WINAPI _webinix_cmd_async_browser_detect_proc_task(LPVOID _arg)
#else
    void _webinix_cmd_async_browser_detect_proc_task(void* _arg)
#endif
{
    webinix_cmd_async_t* arg = (webinix_cmd_async_t*) _arg;

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_cmd_async_browser_detect_proc_task()... \n", arg->win->core.window_number);
    #endif

    // Run command
    system(arg->cmd);

    // Free memory
    _webinix_free_mem((void *) &arg->cmd);

    // Close app
    webinix_exit();

    return 0;
}

int _webinix_cmd_async_browser(webinix_window_t* win, char* cmd) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_cmd_async_browser([%s])... \n", win->core.window_number, cmd);
    #endif

    if(win->core.detect_process_close) {

        // ASync Command (Background running)
        // and free window connection if process (browser) stop

        webinix_cmd_async_t* arg = (webinix_cmd_async_t*) _webinix_malloc(sizeof(webinix_cmd_async_t));
        arg->win = win;
        arg->cmd = (char*) _webinix_malloc(strlen(cmd) + 1);
        strcpy(arg->cmd, cmd);

        #ifdef _WIN32
            HANDLE user_fun_thread = CreateThread(NULL, 0, _webinix_cmd_async_browser_detect_proc_task, (void *) arg, 0, NULL);
            CloseHandle(user_fun_thread); 
        #else
            // Create posix thread ...
        #endif
    }
    else {

        // ASync Command (Background running)
        
        #ifdef _WIN32
            // char _cmd[1024];
            // sprintf(_cmd, "START \"\" %s", cmd);
            // system(_cmd);
            WinExec(cmd, SW_HIDE);
        #else
            if(!fork()) {
                system(cmd);
                return 0;
            }
        #endif
    }

    return 0;
}

int _webinix_cmd_sync(char* cmd) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_cmd_sync([%s])... \n", cmd);
    #endif

    return system(cmd);
}

bool _webinix_browser_start_chrome(webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_start_chrome([%s])... \n", address);
    #endif
    
    // -- Chrome ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webinix.browser.chrome)
        return false;

    if(!_webinix_browser_exist(win, webinix.browser.chrome))
        return false;
    
    if(!_webinix_browser_create_profile_folder(win, webinix.browser.chrome))
        return false;
    
    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    #ifdef _WIN32
        char parm[1024];
        sprintf(parm, "cmd /c \"%s\" > nul 2>&1", full);
        if(_webinix_cmd_async_browser(win, parm) == 0)
    #else
        char parm[1024];
        sprintf(parm, "%s >>/dev/null 2>>/dev/null", full);
        if(_webinix_cmd_async_browser(win, parm) == 0)
    #endif
    {
        win->core.CurrentBrowser = webinix.browser.chrome;
        return true;
    }
    else
        return false;
}

bool _webinix_browser_start_custom(webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_start_custom([%s])... \n", address);
    #endif
    
    // -- Custom Browser ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webinix.browser.custom)
        return false;

    if(!_webinix_browser_exist(win, webinix.browser.custom))
        return false;
    
    if(!_webinix_browser_create_profile_folder(win, webinix.browser.custom))
        return false;

    char full[1024];
    if(webinix.custom_browser->auto_link)
        sprintf(full, "%s %s%s", webinix.custom_browser->app, webinix.custom_browser->arg, address);
    else
        sprintf(full, "%s %s", webinix.custom_browser->app, webinix.custom_browser->arg);

    #ifdef _WIN32
        char parm[1024];
        sprintf(parm, "cmd /c \"%s\" > nul 2>&1", full);
        if(_webinix_cmd_async_browser(win, parm) == 0)
    #else
        char parm[1024];
        sprintf(parm, "%s >>/dev/null 2>>/dev/null", full);
        if(_webinix_cmd_async_browser(win, parm) == 0)
    #endif
    {
        win->core.CurrentBrowser = webinix.browser.custom;
        return true;
    }
    else
        return false;
}

bool _webinix_browser_start_firefox(webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_start_firefox([%s])... \n", address);
    #endif

    // -- Firefox ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webinix.browser.firefox)
        return false;

    if(!_webinix_browser_exist(win, webinix.browser.firefox))
        return false;

    if(!_webinix_browser_create_profile_folder(win, webinix.browser.firefox))
        return false;

    char full[1024];
    sprintf(full, "%s -P Webinix -private -no-remote -new-instance %s", win->core.browser_path, address);

    #ifdef _WIN32
        char parm[1024];
        sprintf(parm, "cmd /c \"%s\" > nul 2>&1", full);
        if(_webinix_cmd_async_browser(win, parm) == 0)
    #else
        char parm[1024];
        sprintf(parm, "%s >>/dev/null 2>>/dev/null", full);
        if(_webinix_cmd_async_browser(win, parm) == 0)
    #endif
    {
        win->core.CurrentBrowser = webinix.browser.firefox;
        return true;
    }
    else
        return false;
}

bool _webinix_browser_start_edge(webinix_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_start_edge([%s])... \n", address);
    #endif

    // -- Edge ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webinix.browser.edge)
        return false;

    if(!_webinix_browser_exist(win, webinix.browser.edge))
        return false;
    
    if(!_webinix_browser_create_profile_folder(win, webinix.browser.edge))
        return false;

    char full[1024];
    sprintf(full, "%s --user-data-dir=\"%s\" --no-proxy-server --app=%s", win->core.browser_path, win->core.profile_path, address);

    char parm[1024];
    sprintf(parm, "cmd /c \"%s\" > nul 2>&1", full);
    if(_webinix_cmd_async_browser(win, parm) == 0) {

        win->core.CurrentBrowser = webinix.browser.edge;
        return true;
    }
    else
        return false;
}

bool _webinix_browser_start(webinix_window_t* win, const char* address, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_browser_start([%s], [%d])... \n", address, browser);
    #endif

    // Non existing browser
    if(browser > 10)
        return false;
    
    #ifdef __linux__
        if(address[0] == '/') {

            address = "file://" + address;
        }
    #endif

    if(browser != 0) {

        // Specified browser
        if(browser == webinix.browser.chrome)
            return _webinix_browser_start_chrome(win, address);
        else if(browser == webinix.browser.firefox)
            return _webinix_browser_start_firefox(win, address);
        else if(browser == webinix.browser.edge)
            return _webinix_browser_start_edge(win, address);
        else if(browser == webinix.browser.custom)
            return _webinix_browser_start_custom(win, address);
        else
            return false;
    }
    else if(win->core.CurrentBrowser != 0) {

        // Already set browser
        if(win->core.CurrentBrowser == webinix.browser.chrome)
            return _webinix_browser_start_chrome(win, address);
        else if(win->core.CurrentBrowser == webinix.browser.firefox)
            return _webinix_browser_start_firefox(win, address);
        else if(win->core.CurrentBrowser == webinix.browser.edge)
            return _webinix_browser_start_edge(win, address);
        else if(win->core.CurrentBrowser == webinix.browser.custom)
            return _webinix_browser_start_custom(win, address);
        else
            return false;
            //webinix::exit();
    }
    else {

        // Default OS browser

        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
            // Windows
            if(!_webinix_browser_start_chrome(win, address))
                if(!_webinix_browser_start_firefox(win, address))
                    if(!_webinix_browser_start_edge(win, address))
                        if(!_webinix_browser_start_custom(win, address))
                            return false;
                            //webinix::exit();
        #elif __APPLE__
            // macOS
            if(!_webinix_browser_start_chrome(address))
                if(!_webinix_browser_start_firefox(address))
                    if(!_webinix_browser_start_edge(address))
                        if(!_webinix_browser_start_custom(address))
                            return false;
                            //webinix::exit();
        #else
            // Linux
            if(!_webinix_browser_start_chrome(address))
                if(!_webinix_browser_start_firefox(address))
                    if(!_webinix_browser_start_edge(address))
                        if(!_webinix_browser_start_custom(address))
                            return false;
                            //webinix::exit();
        #endif
    }

    return true;
}

void _webinix_window_open(webinix_window_t* win, char* link, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_window_open([%s], [%d])... \n", win->core.window_number, link, browser);
    #endif

    // Just open an app-mode window using this link
    _webinix_browser_start(win, link, browser);
}

void webinix_free_js(webinix_javascript_t* javascript) {

    _webinix_free_mem((void *) &javascript->result.data);
    _webinix_free_mem((void *) &javascript->script);
    memset(javascript, 0x00, sizeof(webinix_javascript_t));
}

void webinix_run_js(webinix_window_t* win, webinix_javascript_t* javascript) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_run_js([%s])... \n", win->core.window_number, javascript->script);
    #endif

    size_t js_len = strlen(javascript->script);

    if(js_len < 1) {

        _webinix_free_mem((void *) &javascript->result.data);
        javascript->result.data = webinix_js_empty;
        javascript->result.length = strlen(webinix_js_empty);
        javascript->result.error = true;
        return;
    }

    // Initializing js result
    _webinix_free_mem((void *) &javascript->result.data);
    javascript->result.data = webinix_js_timeout;
    javascript->result.length = strlen(webinix_js_timeout);
    javascript->result.error = true;
    
    // Initializing pipe
    unsigned int run_id = _webinix_get_run_id();
    webinix.run_done[run_id] = false;
    webinix.run_error[run_id] = false;
    _webinix_free_mem((void *) &webinix.run_responses[run_id]);

    // Prepare the packet
    size_t packet_len = 3 + js_len + 1; // [header][js][null]
    char* packet = (char*) _webinix_malloc(packet_len);
    packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
    packet[1] = WEBUI_HEADER_JS;        // Type
    packet[2] = run_id;                 // ID
    for(unsigned int i = 0; i < js_len; i++)     // Data
        packet[i + 3] = javascript->script[i];
    
    // Send packets
    _webinix_window_send(win, packet, packet_len);
    _webinix_free_mem((void *) &packet);

    // Wait for UI response
    if(javascript->timeout < 1 || javascript->timeout > 86400) {

        for(;;) {

            if(webinix.run_done[run_id])
                break;
            
            _webinix_sleep(1);
        }
    }
    else {

        for(unsigned int n = 0; n <= (javascript->timeout * 1000); n++) {

            if(webinix.run_done[run_id])
                break;
            
            _webinix_sleep(1);
        }
    }

    if(webinix.run_responses[run_id] != NULL) {

        javascript->result.data = webinix.run_responses[run_id];
        javascript->result.length = strlen(webinix.run_responses[run_id]);
        javascript->result.error = webinix.run_error[run_id];
    }
}

webinix_window_t* webinix_new_window() {

    #ifdef WEBUI_LOG
        printf("[0] webinix_new_window()... \n");
    #endif

    _webinix_ini();

    webinix_window_t* win = (webinix_window_t*) _webinix_malloc(sizeof(webinix_window_t));

    // Initialisation
    win->core.window_number = _webinix_get_new_window_number();
    win->core.browser_path = (char*) _webinix_malloc(1024);
    win->core.profile_path = (char*) _webinix_malloc(1024);
    win->path = (char*) _webinix_malloc(MAX_PATH);

    #ifdef WEBUI_LOG
        printf("[0] webinix_new_window() -> New window @ %p\n", win);
    #endif

    return win;
}

void webinix_close(webinix_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_close()... \n", win->core.window_number);
    #endif

    if(win->core.connected) {

        // Prepare packets
        char* packet = (char*) _webinix_malloc(4);
        packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
        packet[1] = WEBUI_HEADER_CLOSE;     // Type
        packet[2] = 0;                      // ID
        packet[3] = 0;                      // Data

        // Send packets
        _webinix_window_send(win, packet, 4);
        _webinix_free_mem((void *) &packet);
    }
}

bool webinix_is_show(webinix_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_is_show()... \n", win->core.window_number);
    #endif

    return win->core.connected;
}

bool webinix_any_window_is_open() {

    #ifdef WEBUI_LOG
        printf("[0] webinix_any_window_is_open()... \n");
    #endif
    
    if(webinix.connections > 0)
        return true;
    
    return false;
}

unsigned int _webinix_window_get_window_number(webinix_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_window_get_window_number()... \n", win->core.window_number);
    #endif
    
    return win->core.window_number;
}

const char* webinix_new_server(webinix_window_t* win, const char* path, const char* index_html) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_new_server()... \n", win->core.window_number);
    #endif

    // Root folder to serve
    webinix_set_root_folder(win, path);
    
    // 99 is a non-existing browser
    // this is to prevent any browser 
    // from running. We want only to
    // run a web-server right now.
    webinix_show(win, index_html, 99);

    // Wait for server to start
    for(unsigned int n = 0; n < 2000; n++) {

        if(win->core.server_running)
            break;
        
        _webinix_sleep(1);
    }

    return (const char*) win->core.url;
}

bool webinix_set_root_folder(webinix_window_t* win, const char* path) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_set_root_folder([%s])... \n", win->core.window_number, path);
    #endif

    if(strlen(path) > MAX_PATH)
        return false;

    win->core.server_root = true;

    if(_webinix_is_empty(path))
        sprintf(win->path, "%s", WEBUI_DEFAULT_PATH);
    else
        sprintf(win->path, "%s", path);
    
    webinix_allow_multi_access(win, true);

    return true;
}

void webinix_allow_multi_access(webinix_window_t* win, bool status) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_allow_multi_access([%d])... \n", win->core.window_number, status);
    #endif

    win->core.multi_access = status;
}

void webinix_set_icon(webinix_window_t* win, const char* icon_s, const char* type_s) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_set_icon([%s], [%s])... \n", win->core.window_number, icon_s, type_s);
    #endif

    win->core.icon = icon_s;
    win->core.icon_type = type_s;
}

bool webinix_show(webinix_window_t* win, const char* html, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_show([%.*s..], [%d])... \n", win->core.window_number, 3, html, browser);
    #endif

    // Initializing
    win->core.html = html;
    win->core.server_handled = false;
    webinix.wait_for_socket_window = true;

    if(!webinix_is_show(win)) {

        // Start a new window

        // Get new port
        unsigned int port = _webinix_get_free_port();
        win->core.server_port = port;
        _webinix_free_mem((void *) &win->core.url);
        win->core.url = (char*) _webinix_malloc(128);
        sprintf(win->core.url, "http://localhost:%d", port);

        // New Server
        HANDLE thread = CreateThread(NULL, 0, webinix_server_start, (void *) win, 0, NULL);
        win->core.server_thread = thread;
        CloseHandle(thread);

        // Run browser
        
        if(!_webinix_browser_start(win, win->core.url, browser))
            return false;
    }
    else {

        // Refresh an existing running window

        // Prepare packets
        size_t packet_len = 3 + strlen(win->core.url) + 1; // [header][url][null]
        char* packet = (char*) _webinix_malloc(packet_len);
        packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
        packet[1] = WEBUI_HEADER_SWITCH;    // Type
        packet[2] = 0;                      // ID
        for(unsigned int i = 0; i < strlen(win->core.url); i++)  // Data
            packet[i + 3] = win->core.url[i];

        // Send the packet
        _webinix_window_send(win, packet, packet_len);
        _webinix_free_mem((void *) &packet);
    }

    return true;
}

void webinix_bind_all(webinix_window_t* win, void (*func) (webinix_event_t e)) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_bind_all([*])... \n", win->core.window_number);
    #endif

    win->core.cb_all[0] = func;
    win->core.is_bind_all = true;
}

unsigned int webinix_bind(webinix_window_t* win, const char* element, void (*func) (webinix_event_t e)) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_bind([%s], [*])... \n", win->core.window_number, element);
    #endif

    char* element_id = _webinix_malloc(strlen(element));
    sprintf(element_id, "%d/%s", win->core.window_number, element);

    unsigned int cb_index = _webinix_get_cb_index(element_id);

    if(cb_index > 0) {

        // Replace a reference
        webinix.cb[cb_index] = func;

        _webinix_free_mem((void *) &element_id);
    }
    else {

        // New reference
        cb_index = _webinix_set_cb_index(element_id);

        if(cb_index > 0)
            webinix.cb[cb_index] = func;
        else
            _webinix_free_mem((void *) &element_id);
    }

    return cb_index;
}

#ifdef _WIN32
    DWORD WINAPI _webinix_cb(LPVOID _arg)
#else
    void _webinix_cb(void* _arg)
#endif
{
    webinix_cb_t* arg = (webinix_cb_t*) _arg;

    #ifdef WEBUI_LOG
        printf("[%d] [Thread] _webinix_cb()... \n", arg->win->core.window_number);
    #endif

    webinix_event_t e;
    e.window_id = arg->win->core.window_number;
    e.element_name = arg->element_name;

    unsigned int cb_index = _webinix_get_cb_index(arg->element_id);

    // Check if cb exist
    if(cb_index > 0 && webinix.cb[cb_index] != NULL) {

        // User cb
        e.element_id = cb_index;
        webinix.cb[cb_index](e);        
    }

    // General user cb
    if(arg->win->core.is_bind_all && arg->win->core.cb_all[0] != NULL) {

        e.element_id = 0;
        arg->win->core.cb_all[0](e);
    }

    #ifdef WEBUI_LOG
        printf("[%d] [Thread] _webinix_cb()... Stop.\n", arg->win->core.window_number);
    #endif    

    // Free
    _webinix_free_mem((void *) &arg->element_id);
    _webinix_free_mem((void *) &arg->element_name);
    _webinix_free_mem((void *) &arg);

    return 0;
}

void _webinix_window_event(webinix_window_t* win, char* element_id, char* element) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_window_event([%s], [%s])... \n", win->core.window_number, element_id, element);
    #endif

    // Create a thread, and call the used cb function
    webinix_cb_t* arg = (webinix_cb_t*) _webinix_malloc(sizeof(webinix_cb_t));
    arg->win = win;
    arg->element_id = element_id;
    arg->element_name = element;

    #ifdef _WIN32
        HANDLE user_fun_thread = CreateThread(NULL, 0, _webinix_cb, (void *) arg, 0, NULL);
        CloseHandle(user_fun_thread); 
    #else
        // Create posix thread ...
    #endif
}

void _webinix_window_send(webinix_window_t* win, char* packet, size_t packets_size) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_window_send([%.*s], [%d])... [ ", win->core.window_number, (int)packets_size, packet, (int)packets_size);
            _webinix_print_hex(packet, packets_size);
        printf("]\n");
    #endif
    
    if(!win->core.connected ||
        webinix.mg_connections[win->core.window_number] == NULL ||
        packet == NULL ||
        packets_size < 4)
        return;

    struct mg_connection* c = webinix.mg_connections[win->core.window_number];
    mg_ws_send(
        c, 
        packet, 
        packets_size, 
        WEBSOCKET_OP_BINARY
    );
}

bool _webinix_get_data(const char* packet, size_t packet_len, unsigned int pos, size_t* data_len, char** data) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_get_data()... \n");
    #endif

    *data = (char*) _webinix_malloc((packet_len - pos) + 1);

    // Copy data part
    char* p = *data;
    for(unsigned int i = pos; i < packet_len; i++) {

        memcpy(p, &packet[i], 1);
        p++;
    }

    // Check data size
    *data_len = strlen(*data);
    if(*data_len < 1) {

        _webinix_free_mem((void *) data);
        return false;
    }

    return true;
}

void _webinix_window_receive(webinix_window_t* win, const char* packet, size_t len) {

    #ifdef WEBUI_LOG
        printf("[%d] _webinix_window_receive([%.*s], [%d])... [ ", win->core.window_number, (int)len, packet, (int)len);
            _webinix_print_hex(packet, len);
        printf("]\n");
    #endif

    if((unsigned char) packet[0] != WEBUI_HEADER_SIGNATURE || len < 4)
        return;

    if((unsigned char) packet[1] == WEBUI_HEADER_CLICK || (unsigned char) packet[1] == WEBUI_HEADER_CALL_FUNC) {

        // Click Event / Call Function

        // 0: [Signature]
        // 1: [Type]
        // 2: [Null]
        // 3: [Data]

        // Get data part
        char* data;
        size_t data_len;
        if(!_webinix_get_data(packet, len, 3, &data_len, &data))
            return;

        size_t element_id_len = 3 + 1 + data_len + 1; // [win num][/][name][null]
        char* element_id = (char*) _webinix_malloc(element_id_len);
        sprintf(element_id, "%d/%s", win->core.window_number, data);

        _webinix_window_event(win, element_id, data);
    }
    else if((unsigned char) packet[1] == WEBUI_HEADER_JS) {

        // JS Result

        // 0: [Signature]
        // 1: [Type]
        // 2: [ID]
        // 3: [Error]
        // 4: [Data]

        // Get data part
        char* data;
        size_t data_len;
        if(!_webinix_get_data(packet, len, 4, &data_len, &data))
            return;

        // Get pipe id
        unsigned char run_id = packet[2];
        if(run_id < 0x01) {

            _webinix_free_mem((void *) &data);
            return;
        }

        // Get js-error
        bool error = true;
        if((unsigned char) packet[3] == 0x00)
            error = false;

        // Set pipe
        _webinix_free_mem((void *) &webinix.run_responses[run_id]);
        webinix.run_error[run_id] = error;
        webinix.run_responses[run_id] = data;

        // Ready signal
        webinix.run_done[run_id] = true;
    }
}

bool webinix_open(webinix_window_t* win, const char* url, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_open()... \n", win->core.window_number);
    #endif

    // Just open an app-mode window using the link
    webinix_set_timeout(0);
    return _webinix_browser_start(win, url, browser);
}

void webinix_detect_process_close(webinix_window_t* win, bool status) {

    #ifdef WEBUI_LOG
        printf("[%d] webinix_detect_process_close()... \n", win->core.window_number);
    #endif

    win->core.detect_process_close = status;
}

char* _webinix_get_current_path() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_get_current_path()... \n");
    #endif

    char* path = (char*) _webinix_malloc(MAX_PATH);
    WEBUI_GET_CURRENT_DIR(path, MAX_PATH);

    return path;
}

void _webinix_set_custom_browser(webinix_custom_browser_t* p) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_set_custom_browser()... \n");
    #endif

    webinix.custom_browser = p;
}

void webinix_exit() {

    #ifdef WEBUI_LOG
        printf("[0] webinix_exit()... \n");
    #endif

    webinix.wait_for_socket_window = false;
    webinix.servers = 0;
    webinix.exit_now = true;
}

void webinix_loop() {

    #ifdef WEBUI_LOG
        printf("[0] webinix_loop()... \n");
    #endif

    if(webinix.use_timeout) {

        // Wait for a specific time

        // TODO: Loop trough all win
        // if(win->core.CurrentBrowser < 1)
        //     return;
        
        // Wait one time while user 
        // browser is starting!
        _webinix_wait_for_startup();

        if(webinix.wait_for_socket_window) {

            while(webinix.servers > 0){
            
                _webinix_sleep(100);
                // printf("[%d/%d]", webinix.servers, webinix.connections);
            }
        }
    }
    else {

        // Infinite wait
        while(!webinix.exit_now)
            _webinix_sleep(100);
    }

    _webinix_browser_clean();
}

void _webinix_free_port(unsigned int port) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_free_port([%d])... \n", port);
    #endif

    for(unsigned int i = 0; i < WEBUI_MAX_ARRAY; i++) {
        if(webinix.used_ports[i] == port) {
            webinix.used_ports[i] = 0;
            break;
        }
    }
}

void _webinix_wait_for_startup() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_wait_for_startup()... \n");
    #endif

    if(webinix.connections > 0)
        return;

    // Wait for a specific time
    for(unsigned int n = 0; n <= (webinix.startup_timeout * 1000); n++) {

        if(webinix.connections > 0)
            break;
        
        _webinix_sleep(1);
    }
}

void webinix_set_timeout(unsigned int second) {

    #ifdef WEBUI_LOG
        printf("[0] webinix_set_timeout([%d])... \n", second);
    #endif

    if(second < 1)
        webinix.use_timeout = false;
    else {

        webinix.use_timeout = true;
        webinix.startup_timeout = second;
        webinix.timeout_extra = false;
    }
}

unsigned int _webinix_get_new_window_number() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_get_new_window_number()... \n");
    #endif

    return ++webinix.last_window;
}

unsigned int _webinix_get_free_port() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_get_free_port()... \n");
    #endif

    unsigned int port = WEBUI_MIN_PORT;

    for(unsigned int i = WEBUI_MIN_PORT; i <= WEBUI_MAX_PORT; i++) {

        // Search [port] in [webinix.used_ports]
        bool found = false;
        for(unsigned int j = 0; j < WEBUI_MAX_ARRAY; j++) {
            if(webinix.used_ports[j] == port) {
                found = true;
                break;
            }
        }

        if(found)
            port++; // Port used by local window
        else {
            if(_webinix_port_is_used(port))
                port++; // Port used by an external app
            else
                break; // Port ready to use
        }
    }

    // Add
    for(unsigned int i = 0; i < WEBUI_MAX_ARRAY; i++) {
        if(webinix.used_ports[i] == 0) {
            webinix.used_ports[i] = port;
            break;
        }
    }

    return port;
}

void webinix_runtime(webinix_window_t* win, unsigned int runtime) {

    if(runtime != webinix.runtime.deno && runtime != webinix.runtime.nodejs)
        win->core.runtime = webinix.runtime.none;
    else
        win->core.runtime = runtime;
}

void _webinix_ini() {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_ini()... \n");
    #endif
    
    if(webinix.initialized)
        return;

    // Initializing
    memset(&webinix, 0x0, sizeof(webinix_t));
    webinix.initialized           = true;
    webinix.use_timeout           = true;
    webinix.startup_timeout       = 10; // Seconds
    webinix.timeout_extra         = true;
    webinix.browser.chrome        = 1;
    webinix.browser.firefox       = 2;
    webinix.browser.edge          = 3;
    webinix.browser.safari        = 4;
    webinix.browser.chromium      = 5;
    webinix.browser.custom        = 99;
    webinix.runtime.deno      = 1;
    webinix.runtime.nodejs    = 2;
    webinix.executable_path       = _webinix_get_current_path();
}

unsigned int _webinix_get_cb_index(char* element_id) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_get_cb_index([%s])... \n", element_id);
    #endif

    if(element_id != NULL) {

        for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {

            if(!_webinix_is_empty(webinix.html_elements[i])) 
                if(strcmp(webinix.html_elements[i], element_id) == 0)
                    return i;
        }
    }

    return 0;
}

unsigned int _webinix_set_cb_index(char* element_id) {

    #ifdef WEBUI_LOG
        printf("[0] _webinix_set_cb_index([%s])... \n", element_id);
    #endif

    // Add
    for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {

        if(_webinix_is_empty(webinix.html_elements[i])) {

            webinix.html_elements[i] = element_id;

            return i;
        }
    }

    return 0;
}

// --[Python Wrapper]---------------

void webinix_bind_py_handler(const webinix_event_t e) {

    unsigned int cb_index = e.element_id;

    if(cb_index > 0 && webinix.cb_py[cb_index] != NULL)
        webinix.cb_py[cb_index](e.element_id, e.window_id, e.element_name);
}

unsigned int webinix_bind_py(webinix_window_t* win, const char* element, void (*func)(unsigned int, unsigned int, char*)) {

    unsigned int cb_index = webinix_bind(win, element, webinix_bind_py_handler);
    webinix.cb_py[cb_index] = func;

    return cb_index;
}

void webinix_run_js_py(webinix_window_t* win, webinix_javascript_py_t* js_py) {

    webinix_javascript_t js = {
		.script = js_py->script,
		.timeout = js_py->timeout
	};

    webinix_run_js(win, &js);
    
    js_py->data = js.result.data;
    js_py->error = js.result.error;
    js_py->length = js.result.length;
}
