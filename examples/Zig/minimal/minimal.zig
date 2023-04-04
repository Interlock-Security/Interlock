const c = @import("c.zig");
const std = @import("std");

pub fn main() !void {
    var my_window: *c.webinix_window_t = c.webinix_new_window();
    if (!c.webinix_show(my_window, "<html>Hello</html>")) {
        std.log.err("Unable to show webinix", .{});
        return error.ShowWebui;
    }
    c.webinix_wait();
}
