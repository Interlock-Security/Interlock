# Webinix C API's

- [New Window](/c_api?id=New-Window)
- [Bind](/c_api?id=Bind)
- [Bind All](/c_api?id=Bind-All)
- [Show Window](/c_api?id=Show-Window)
- [Loop](/c_api?id=Loop)
- [Event](/c_api?id=Event)

### New Window

To create a new window object we use `webinix_new_window()` which return a pointer to a struct `webinix_window_t`. This pointer should never be freed.

```c
webinix_window_t* my_window = webinix_new_window();
```

### Bind

To receive a click event (*function call*) when the user click on an HTML element (*all kind of elements*) thats have an HTML id `MyID` for example `<button id="MyID">Hello</button>`, we use `webinix_bind()`.

```c
void my_function(const webinix_event_t e) {
    ...
}

webinix_bind(my_window, "MyID", my_function);
```

### Bind All

You can also automatically bind all HTML element with one function call.

```c
void all_clicks(const webinix_event_t e) {
    ...
}

webinix_bind_all(my_window, all_clicks);
```

### Show Window

To show a window we use `webinix_show()`. If the window is already shown the UI will get refreshed in the same window.

```c
const char* my_html = "<!DOCTYPE html><html>...</html>";

// Chrome
webinix_show(my_window, my_html, webinix.browser.chrome);

// Firefox
webinix_show(my_window, my_html, webinix.browser.firefox);

// Microsoft Edge
webinix_show(my_window, my_html, webinix.browser.edge);

// Any available browser
webinix_show(my_window, my_html, webinix.browser.any);
```

### Loop

It's very important to call `webinix_wait()` at the end of your main function after you created/shows all your windows.

```c
int main() {

	// Create windows...
	// Bind HTML elements...
    // Show the windows...

    // Wait until all windows get closed
	webinix_wait();

    return 0;
}
```

### Event

When you use [webinix_bind()](/c_api?id=Bind) your application receive an event every time the user click on the HTML element you selected. The event come with the `element_name` which is The HTML ID of the clicked element, example `MyButton`, `MyInput`.., The event come also with the Webinix unique element ID and the unique window ID, those two IDs are not needed in general except if you are written a wrapper for Webinix in other language than C.

```c
void my_function(const webinix_event_t e){

	printf("Hi!, You clicked on %s element\n", e.element_name);
}
```

