const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Main executable
    const exe = b.addExecutable(.{
        .name = "educational_mamba",
        .root_source_file = b.path("educational_mamba.zig"),
        .target = target,
        .optimize = optimize,
    });
    
    b.installArtifact(exe);

    // Run command
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the educational Zig Mamba");
    run_step.dependOn(&run_cmd.step);

    // Tests
    const unit_tests = b.addTest(.{
        .root_source_file = b.path("educational_mamba.zig"),
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_unit_tests.step);

    // Benchmark build
    const benchmark_exe = b.addExecutable(.{
        .name = "mamba_benchmark",
        .root_source_file = b.path("educational_mamba.zig"),
        .target = target,
        .optimize = .ReleaseFast, // Maximum optimization for benchmarking
    });
    
    b.installArtifact(benchmark_exe);

    const benchmark_run = b.addRunArtifact(benchmark_exe);
    const benchmark_step = b.step("benchmark", "Run performance benchmarks");
    benchmark_step.dependOn(&benchmark_run.step);
}