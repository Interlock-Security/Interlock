/*
    Webinix Library 2.x
    C99 Example

    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

    this program is very simple calculator
    programmed by Kemari Mohammed
*/

#include "webinix.h"

char op = ' ';                              // operation ( + , - , * , / )
int a, b, c ;
bool Is_First_Op[2] = { true , true } ;     // just for calculate one operation ( The two elements because there are two events)
bool begin_second_number = false ;          // for begin writing second number and clear first number
bool end_calculate = false ;                // stop calculate when click "="

void close_the_application(webinix_event_t* e){
    // Close all opened windows
    webinix_exit();
}

void Onclick_numbers(webinix_event_t* e){
    webinix_script_t js;
    if(!end_calculate)
    {
        if(begin_second_number)
        {
            js.script = "Calculator.Input.value = '" ;
            js.timeout = 3;
            begin_second_number = false;
        }
        else
        {
            js.script = "Calculator.Input.value += '" ;
            js.timeout = 3;
        }
        const char* str = webinix_get_string(e);
        asprintf(&js.script, "%s%s%s", js.script, str , "'" );

        // Run the JavaScript on the UI (Web Browser)
        webinix_script(e->window, &js);
    }
}

void operation(webinix_event_t* e){
    if(Is_First_Op[0] )
    {
        begin_second_number = true;
        a = webinix_get_int(e);
        Is_First_Op[0] = false;
    }
}

void operation2(webinix_event_t* e){
    if(Is_First_Op[1])
    {
        op = * webinix_get_string(e);
        Is_First_Op[1] = false;
    }
}

void Result(webinix_event_t* e){
    if(!end_calculate)
    {
        webinix_script_t js = {
            .script = "Calculator.Input.value = '" ,
            .timeout = 3
        };
        b = webinix_get_int(e);
        switch(op)
            {
            case '+':
                c = a + b;
                break;
            case '-':
                c = a - b;
                break;
            case '*':
                c = a * b;
                break;
            case '/':
                c = a / b;
                break;
            };
        asprintf(&js.script, "%s%d%s", js.script, c , "'" );
        webinix_script(e->window, &js);

        begin_second_number  = false ;
        end_calculate        = true  ;
    }
}

void clear(webinix_event_t* e){
        webinix_script_t js = {
        .script = "Calculator.Input.value = ''" ,
        .timeout = 3
    };
    webinix_script(e->window, &js);
    op = ' ';
    a = b = c = 0;

    Is_First_Op[0]       = true  ;
    Is_First_Op[1]       = true  ;
    begin_second_number  = false ;
    end_calculate        = false ;
}

int main() {
    // Create a window
    webinix_set_timeout(20);
    webinix_window_t* my_window = webinix_new_window();

    // Bind HTML elements with functions
    webinix_bind( my_window , "num"   , Onclick_numbers      );
    webinix_bind( my_window , "ope"   , operation            );
    webinix_bind( my_window , "ope2"  , operation2           );
    webinix_bind( my_window , "calc"  , Result               );
    webinix_bind( my_window , "clear" , clear                );
    webinix_bind( my_window , "Exit"  , close_the_application);

    // HTML server
    const char* url = webinix_new_server(my_window, "calc");

    // Show the window
    if(!webinix_show(my_window, url, webinix.browser.chrome))   // Run the window on Chrome
        webinix_show(my_window, url, webinix.browser.any);      // If not, run on any other installed web browser

    // Wait until all windows get closed
    webinix_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
