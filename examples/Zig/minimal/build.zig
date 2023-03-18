const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const webinix = b.dependency("webinix", .{});

    const exe = b.addExecutable(.{
        .name = "minimal",
        .root_source_file = .{ .path = "minimal.zig" },
        .target = target,
        .optimize = optimize,
    });

    exe.linkLibrary(webinix.artifact("webinix"));
    exe.install();

    const run = exe.run();
    run.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Runs the minimal zig webinix example");
    run_step.dependOn(&run.step);
}
