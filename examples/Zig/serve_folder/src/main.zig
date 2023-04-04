// Webinix Library 2.1.1
// Serve a Folder Example

pub fn main() void {
    // Create a window
    var my_window: *c.webinix_window_t = c.webinix_new_window();

    // Bind HTML element ID to c function
    _ = c.webinix_bind(my_window, "SwitchToSecondPage", switch_to_second_page);
    _ = c.webinix_bind(my_window, "Exit", exit_app);

    // The root path. Leave it empty to let Webinix
    // automatically select the current working folder
    const root_path = "";

    const url = c.webinix_new_server(my_window, root_path);

    // Show the window using the generated url
    c.webinix_show(my_window, url);

    // Wait until all windows get closed
    c.webinix_wait();
}

// Called every time the user clicks on the SwitchToSecondPage button
export fn switch_to_second_page(e_opt: ?*c.webinix_event_t) callconv(.C) void {
    const e = e_opt orelse {
        std.log.err("switch_to_second_page called with null event", .{});
        return;
    };
    _ = c.webinix_open(e.window, "second.html", c.webinix.browser.any);
}

export fn exit_app(e: ?*c.webinix_event_t) callconv(.C) void {
    _ = e;
    c.webinix_exit();
}

const std = @import("std");
const c = @import("c.zig");
