const c = @import("c.zig");
const std = @import("std");

pub fn main() void {
    const my_html =
        \\<!DOCTYPE html>
        \\<html>
        \\    <head>
        \\        <title>Webinix 2 - Zig Example</title>
        \\        <style>
        \\            body {
        \\                 color: white;
        \\                 background: #0F2027;
        \\                 background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\                 background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\                 text-align:center; font-size: 18px; font-family: sans-serif;
        \\            }
        \\        </style>
        \\    </head>
        \\    <body>
        \\        <h2>Webinix 2 - C99 Example</h2>
        \\        <p>Call function with arguments (See log in the Windows console)</p><br>
        \\        <button OnClick="webinix_fn('One', 'Hello');"">Call function one</button><br><br>
        \\        <button OnClick="webinix_fn('Two', 2022);"">Call function two</button><br><br>
        \\        <button OnClick="webinix_fn('Three', true);"">Call function three</button><br><br>
        \\        <p>Call function four, and wait for the result</p><br>
        \\        <button OnClick="MyJS();"">Call function four</button><br><br>
        \\        <input type="text" id="MyInput" value="2">
        \\        <script>
        \\            function MyJS() {
        \\                const number = document.getElementById('MyInput').value;
        \\                var result = webinix_fn('Four', number);
        \\                document.getElementById('MyInput').value = result;
        \\            }
        \\        </script>
        \\    </body>
        \\</html>
    ;

    // Create a window
    var my_window: *c.webinix_window_t = c.webinix_new_window();

    // Bind HTML elements with functions
    _ = c.webinix_bind(my_window, "One", function_one);
    _ = c.webinix_bind(my_window, "Two", function_two);
    _ = c.webinix_bind(my_window, "Three", function_three);
    _ = c.webinix_bind(my_window, "Four", function_four);

    // Show the window
    if (!c.webinix_show(my_window, my_html, c.webinix.browser.chrome)) {
        _ = c.webinix_show(my_window, my_html, c.webinix.browser.any);
    }

    // Wait until all windows get closed
    c.webinix_wait();
}

export fn function_one(e: ?*c.webinix_event_t) callconv(.C) void {
    // JavaScript: webinix_fn('One', 'Hello');
    const str = c.webinix_get_string(e);
    std.debug.print("function_one: {s}\n", .{str});
}

export fn function_two(e: ?*c.webinix_event_t) callconv(.C) void {
    // JavaScript: webinix_fn('Two', 2022);
    const number = c.webinix_get_int(e);
    std.debug.print("function_two: {}\n", .{number});
}

export fn function_three(e: ?*c.webinix_event_t) callconv(.C) void {
    // JavaScript: webinix_fn('Three', true);
    const status = c.webinix_get_bool(e);
    if (status) {
        std.debug.print("function_three: True\n", .{});
    } else {
        std.debug.print("function_three: False\n", .{});
    }
}

export fn function_four(e: ?*c.webinix_event_t) callconv(.C) void {
    // JavaScript: webinix_fn('Four', 2);
    var number = c.webinix_get_int(e);
    number = number * 2;
    std.debug.print("function_four: {}\n", .{number});

    c.webinix_return_int(e, number);
}
