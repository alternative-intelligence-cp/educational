// ✅ Tests for 9D Wave Processor Educational Framework

const std = @import("std");
const testing = std.testing;

const WaveProcessor = @import("wave_processor_9d.zig").Wave9DProcessor;
const NonaryEmbedder = @import("nonary_embedder.zig").NonaryEmbedder;
const DeviceSpecs = @import("device_specs.zig").Device9DSpecs;

test "9D Wave Processor Initialization" {
    var processor = try WaveProcessor.init(testing.allocator);
    
    // Test initial state
    const state = processor.get_system_state();
    try testing.expect(state.time_step == 0);
    try testing.expect(state.entropy == 0.0);
    try testing.expect(state.coherence == 0.0);
    
    // Test 9D position initialization
    for (processor.dimensional_position) |pos| {
        try testing.expect(pos == 0.1); // Should be initialized to 0.1
    }
}

test "9D Wave Processing Forward Pass" {
    var processor = try WaveProcessor.init(testing.allocator);
    
    // Create test input/output
    var input: [512]f32 = [_]f32{0.0} ** 512;
    var output: [512]f32 = [_]f32{0.0} ** 512;
    
    // Set up simple test signal
    for (input, 0..) |*inp, i| {
        const t = @as(f32, @floatFromInt(i)) / 512.0;
        inp.* = @sin(2.0 * std.math.pi * t);
    }
    
    // Process signal
    try processor.forward_pass(input[0..], output[0..]);
    
    // Check that processing occurred
    const state = processor.get_system_state();
    try testing.expect(state.time_step == 1);
    
    // Check that output was generated
    var output_energy: f32 = 0.0;
    for (output) |out| {
        output_energy += out * out;
    }
    try testing.expect(output_energy > 0.0);
}

test "Nonary Embedding System" {
    var embedder = NonaryEmbedder.init(9, 6);
    
    // Test decimal to nonary conversion
    const test_value: f32 = 3.14159;
    const nonary = try embedder.decimal_to_nonary(test_value);
    
    // Check that we got a valid nonary representation
    for (nonary) |digit| {
        try testing.expect(digit < 9); // All digits must be 0-8 in base-9
    }
    
    // Test round-trip conversion
    const reconstructed = embedder.nonary_to_decimal(nonary);
    const error = @abs(reconstructed - test_value);
    try testing.expect(error < 0.01); // Should be reasonably close
    
    // Test 9D embedding
    const embedding = try embedder.embed_to_9d(test_value);
    
    // Check embedding properties
    var magnitude: f32 = 0.0;
    for (embedding) |val| {
        magnitude += val * val;
    }
    magnitude = @sqrt(magnitude);
    try testing.expect(magnitude > 0.0 and magnitude < 10.0); // Reasonable magnitude
}

test "Device Specifications" {
    var device = DeviceSpecs.standard_device();
    
    // Test basic properties
    try testing.expect(device.processing_units == 9);
    try testing.expect(device.max_dimensions == 9);
    try testing.expect(device.frequency_hz > 3.0 and device.frequency_hz < 4.0); // Should be π
    
    // Test performance calculations
    const metrics = device.calculate_performance_metrics();
    try testing.expect(metrics.operations_per_second > 0.0);
    try testing.expect(metrics.efficiency_ops_per_watt > 0.0);
    
    // Test high-performance variant
    var hp_device = DeviceSpecs.high_performance_device();
    try testing.expect(hp_device.processing_units > device.processing_units);
    
    const hp_metrics = hp_device.calculate_performance_metrics();
    try testing.expect(hp_metrics.operations_per_second > metrics.operations_per_second);
}

test "9D System Integration" {
    // Test that all components work together
    var processor = try WaveProcessor.init(testing.allocator);
    var embedder = NonaryEmbedder.init(9, 6);
    
    // Create a test workflow
    const test_value: f32 = 2.718; // Euler's number
    
    // Convert to 9D embedding
    const embedding = try embedder.embed_to_9d(test_value);
    
    // Create input signal based on embedding
    var input: [512]f32 = undefined;
    var output: [512]f32 = undefined;
    
    for (input, 0..) |*inp, i| {
        const dim_index = i % 9;
        inp.* = embedding[dim_index];
    }
    
    // Process through 9D wave processor
    try processor.forward_pass(input[0..], output[0..]);
    
    // Verify processing succeeded
    const state = processor.get_system_state();
    try testing.expect(state.time_step == 1);
    
    // Check output characteristics
    var output_magnitude: f32 = 0.0;
    for (output) |out| {
        output_magnitude += out * out;
    }
    output_magnitude = @sqrt(output_magnitude);
    
    try testing.expect(output_magnitude > 0.0);
}

test "Mathematical Properties" {
    // Test mathematical properties of the 9D system
    var processor = try WaveProcessor.init(testing.allocator);
    
    // Test π Hz frequency integration
    const pi_freq = 3.141592653589793;
    try testing.expectApproxEqAbs(processor.base_frequency, pi_freq, 0.0001);
    
    // Test state vector properties
    const magnitude = calculate_9d_magnitude(processor.dimensional_position);
    try testing.expect(magnitude > 0.0);
    
    // Test that system maintains stability
    var input: [512]f32 = [_]f32{0.1} ** 512; // Small constant input
    var output: [512]f32 = undefined;
    
    // Multiple processing steps
    for (0..10) |_| {
        try processor.forward_pass(input[0..], output[0..]);
        
        // Check that system doesn't explode
        const current_magnitude = calculate_9d_magnitude(processor.dimensional_position);
        try testing.expect(current_magnitude < 100.0); // Reasonable bound
    }
}

// Helper function for tests
fn calculate_9d_magnitude(vector: [9]f32) f32 {
    var sum: f32 = 0.0;
    for (vector) |val| {
        sum += val * val;
    }
    return @sqrt(sum);
}