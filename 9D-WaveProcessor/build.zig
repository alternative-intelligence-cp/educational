const std = @import("std");

const Build = std.Build;
const Step = std.Build.Step;

pub fn build(b: *Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Main 9D Wave Processor executable
    const wave_processor_exe = b.addExecutable(.{
        .name = "9d_wave_processor",
        .root_source_file = .{ .path = "demo_wave_processor.zig" },
        .target = target,
        .optimize = optimize,
    });
    b.installArtifact(wave_processor_exe);

    // Nonary embedding demo
    const nonary_demo_exe = b.addExecutable(.{
        .name = "nonary_demo",
        .root_source_file = .{ .path = "demo_nonary.zig" },
        .target = target,
        .optimize = optimize,
    });
    b.installArtifact(nonary_demo_exe);

    // Device specifications demo
    const device_specs_exe = b.addExecutable(.{
        .name = "device_specs",
        .root_source_file = .{ .path = "demo_device_specs.zig" },
        .target = target,
        .optimize = optimize,
    });
    b.installArtifact(device_specs_exe);

    // Interactive 9D demo
    const interactive_demo_exe = b.addExecutable(.{
        .name = "interactive_9d",
        .root_source_file = .{ .path = "demo_interactive.zig" },
        .target = target,
        .optimize = optimize,
    });
    b.installArtifact(interactive_demo_exe);

    // Run commands
    const run_wave_cmd = b.addRunArtifact(wave_processor_exe);
    const run_nonary_cmd = b.addRunArtifact(nonary_demo_exe);
    const run_device_cmd = b.addRunArtifact(device_specs_exe);
    const run_interactive_cmd = b.addRunArtifact(interactive_demo_exe);

    // Run steps
    const run_wave_step = b.step("wave", "Run 9D wave processor demo");
    run_wave_step.dependOn(&run_wave_cmd.step);

    const run_nonary_step = b.step("nonary", "Run nonary embedding demo");
    run_nonary_step.dependOn(&run_nonary_cmd.step);

    const run_device_step = b.step("device", "Run device specifications demo");
    run_device_step.dependOn(&run_device_cmd.step);

    const run_interactive_step = b.step("interactive", "Run interactive 9D demo");
    run_interactive_step.dependOn(&run_interactive_cmd.step);

    // Test step
    const tests = b.addTest(.{
        .root_source_file = .{ .path = "test_9d.zig" },
        .target = target,
        .optimize = optimize,
    });

    const test_step = b.step("test", "Run 9D wave processor tests");
    test_step.dependOn(&tests.step);

    // Install all demos
    const install_all_step = b.step("install-all", "Install all demo executables");
    install_all_step.dependOn(&wave_processor_exe.step);
    install_all_step.dependOn(&nonary_demo_exe.step);
    install_all_step.dependOn(&device_specs_exe.step);
    install_all_step.dependOn(&interactive_demo_exe.step);
}