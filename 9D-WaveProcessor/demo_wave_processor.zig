// 🌊 Demo: 9D Wave Processor in Action

const std = @import("std");
const WaveProcessor = @import("wave_processor_9d.zig").Wave9DProcessor;

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();
    
    std.debug.print("🌊 9D WAVE PROCESSOR DEMO 🌊\n\n");
    
    // Initialize the processor
    var processor = try WaveProcessor.init(allocator);
    
    // Create sample input data (sine wave)
    var input_data: [512]f32 = undefined;
    var output_data: [512]f32 = undefined;
    
    // Generate demonstration input signal
    std.debug.print("Generating test signal...\n");
    for (input_data, 0..) |*inp, i| {
        const t = @as(f32, @floatFromInt(i)) / 512.0;
        const freq1 = 2.0 * std.math.pi * 5.0;  // 5 Hz component
        const freq2 = 2.0 * std.math.pi * 13.0; // 13 Hz component 
        inp.* = 0.5 * @sin(freq1 * t) + 0.3 * @sin(freq2 * t) + 0.1 * (2.0 * (@sin(freq1 * t * 3.14159) - 0.5));
    }
    
    // Process signal through 9D wave processor
    std.debug.print("\nProcessing through 9D wave mathematics...\n");
    
    for (0..5) |step| {
        try processor.forward_pass(input_data[0..], output_data[0..]);
        
        if (step % 1 == 0) {
            processor.print_debug_state();
        }
        
        // Small pause for demonstration
        std.time.sleep(500_000_000); // 0.5 seconds
    }
    
    // Show final output statistics
    var input_energy: f32 = 0.0;
    var output_energy: f32 = 0.0;
    
    for (input_data) |inp| {
        input_energy += inp * inp;
    }
    
    for (output_data) |out| {
        output_energy += out * out;
    }
    
    std.debug.print("\n📊 SIGNAL ANALYSIS:\n");
    std.debug.print("Input Energy: {d:.3}\n", .{input_energy});
    std.debug.print("Output Energy: {d:.3}\n", .{output_energy});
    std.debug.print("Energy Ratio: {d:.3}\n", .{output_energy / input_energy});
    
    const final_state = processor.get_system_state();
    std.debug.print("\nFinal System State:\n");
    std.debug.print("⚡ Entropy: {d:.4}\n", .{final_state.entropy});
    std.debug.print("⚡ Coherence: {d:.4}\n", .{final_state.coherence});
    std.debug.print("⚡ Time Steps: {}\n", .{final_state.time_step});
    
    std.debug.print("\n🎆 9D Wave Processing Complete! 🎆\n");
}