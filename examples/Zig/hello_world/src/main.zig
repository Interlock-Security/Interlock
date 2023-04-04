const c = @import("c.zig");
const std = @import("std");

pub fn main() void {
    const my_html =
        \\ <!DOCTYPE html>
        \\ <html>
        \\     <head>
        \\         <title>Webinix 2 - Zig Example</title>
        \\         <style>
        \\         body{
        \\             color: white; background: #0F2027;
        \\             background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\             background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\             text-align:center;
        \\             font-size: 18px;
        \\             font-family: sans-serif;
        \\         }
        \\         </style>
        \\     </head>
        \\     <body>
        \\         <h1>Webinix 2 - C99 Example</h1><br>
        \\         <input type="password" id="MyInput"><br><br>
        \\         <button id="MyButton1">Check Password</button> - <button id="MyButton2">Exit</button>
        \\     </body>
        \\ </html>;
    ;

    // Create a window
    var my_window: *c.webinix_window_t = c.webinix_new_window();

    // Bind HTML elements with functions
    _ = c.webinix_bind(my_window, "MyButton1", check_the_password);
    _ = c.webinix_bind(my_window, "MyButton2", close_the_application);

    // Show the window
    c.webinix_show(my_window, my_html);

    // Wait until all windows get closed
    c.webinix_wait();
}

export fn close_the_application(e: ?*c.webinix_event_t) callconv(.C) void {
    _ = e;

    // Close all the opened windows
    c.webinix_exit();
}

// check the password function
export fn check_the_password(e_opt: ?*c.webinix_event_t) callconv(.C) void {
    const e = e_opt orelse {
        std.log.err("check_the_pasword passed an empty event", .{});
        return;
    };

    // This function gets called every time the user clicks on "MyButton1"
    var js = c.webinix_script_t{
        .script = " return document.getElementById(\"MyInput\").value; ",
        .timeout = 3,
        .result = undefined,
    };

    // Run the JavaScript in the UI (Web Browser)
    c.webinix_script(e.window, &js);

    // Check if there is a JavaScript error
    if (js.result.@"error") {
        std.log.err("JavaScript Error: {s}\n", .{js.result.data});
        return;
    }

    // Get the password
    const password = std.mem.span(js.result.data);
    std.log.info("Password: {s}", .{password});

    // Check the password
    if (std.mem.eql(u8, password, "123456")) {
        // Correct password
        js.script = "alert('Good, password is correct.')";
        c.webinix_script(e.window, &js);
    } else {
        // Wrong password
        js.script = "alert('Sorry, wrong password.')";
        c.webinix_script(e.window, &js);
    }
}
