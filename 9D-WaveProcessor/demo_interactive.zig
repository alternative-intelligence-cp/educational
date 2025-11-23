// 🎮 Interactive 9D Mathematics Demo

const std = @import("std");
const WaveProcessor = @import("wave_processor_9d.zig").Wave9DProcessor;
const NonaryEmbedder = @import("nonary_embedder.zig").NonaryEmbedder;
const DeviceSpecs = @import("device_specs.zig").Device9DSpecs;

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();
    
    std.debug.print("🎮 INTERACTIVE 9D MATHEMATICS PLAYGROUND 🎮\n\n");
    
    // Initialize systems
    var processor = try WaveProcessor.init(allocator);
    var embedder = NonaryEmbedder.init(9, 6);
    var device = DeviceSpecs.standard_device();
    
    std.debug.print("🎆 Welcome to the 9D Mathematics Educational Playground!\n");
    std.debug.print("This interactive demo showcases advanced mathematical concepts.\n\n");
    
    // Interactive menu system
    var running = true;
    while (running) {
        print_menu();
        
        // For educational purposes, we'll auto-run demonstrations
        // In a real interactive version, this would read user input
        
        std.debug.print("Auto-running demonstrations...\n\n");
        
        // Demo 1: Live 9D wave processing
        try demo_live_wave_processing(&processor);
        std.time.sleep(2_000_000_000); // 2 second pause
        
        // Demo 2: Nonary mathematics exploration
        try demo_nonary_exploration(&embedder);
        std.time.sleep(2_000_000_000); // 2 second pause
        
        // Demo 3: Device performance analysis
        demo_device_analysis(&device);
        std.time.sleep(2_000_000_000); // 2 second pause
        
        // Demo 4: Mathematical harmony analysis
        try demo_mathematical_harmony(&processor, &embedder);
        
        running = false; // End auto-demo
    }
    
    std.debug.print("\n🎉 Thanks for exploring 9D mathematics! 🎉\n");
    std.debug.print("These concepts form the foundation of advanced signal processing\n");
    std.debug.print("and multi-dimensional mathematical systems.\n");
}

fn print_menu() void {
    std.debug.print("┌" ++ "─" ** 50 ++ "┐\n");
    std.debug.print("│  🧠 INTERACTIVE 9D MATHEMATICS MENU 🧠        │\n");
    std.debug.print("├" ++ "─" ** 50 ++ "┤\n");
    std.debug.print("│  1. Live 9D Wave Processing                  │\n");
    std.debug.print("│  2. Nonary Mathematics Explorer              │\n");
    std.debug.print("│  3. Device Performance Analysis              │\n");
    std.debug.print("│  4. Mathematical Harmony Analysis            │\n");
    std.debug.print("│  5. Exit Playground                         │\n");
    std.debug.print("└" ++ "─" ** 50 ++ "┘\n\n");
}

fn demo_live_wave_processing(processor: *WaveProcessor) !void {
    std.debug.print("🌊 LIVE 9D WAVE PROCESSING DEMO 🌊\n");
    std.debug.print("=" ** 40 ++ "\n");
    
    var input: [512]f32 = undefined;
    var output: [512]f32 = undefined;
    
    // Generate dynamic input signal
    for (input, 0..) |*inp, i| {
        const t = @as(f32, @floatFromInt(i)) / 512.0;
        const freq = 3.14159 * (1.0 + 0.1 * @sin(t * 20.0)); // Varying frequency
        inp.* = @sin(freq * t) * (0.5 + 0.3 * @sin(t * 5.0));
    }
    
    std.debug.print("Processing dynamic signal through 9D space...\n");
    
    // Process in real-time steps
    for (0..3) |step| {
        try processor.forward_pass(input[0..], output[0..]);
        
        const state = processor.get_system_state();
        std.debug.print("Step {}: Entropy={d:.3}, Coherence={d:.3}\n", 
            .{step + 1, state.entropy, state.coherence});
        
        std.time.sleep(500_000_000); // 0.5 second
    }
    
    std.debug.print("✓ Real-time 9D processing complete!\n\n");
}

fn demo_nonary_exploration(embedder: *NonaryEmbedder) !void {
    std.debug.print("🧮 NONARY MATHEMATICS EXPLORER 🧮\n");
    std.debug.print("=" ** 40 ++ "\n");
    
    const exploration_values = [_]f32{ 1.0, 2.718, 3.14159, 9.0, 27.0 };
    
    std.debug.print("Exploring mathematical constants in base-9...\n");
    
    for (exploration_values, 0..) |val, i| {
        const nonary = try embedder.decimal_to_nonary(val);
        const embedding = try embedder.embed_to_9d(val);
        
        std.debug.print("\n{}. Value: {d:.3} → Nonary: ", .{i + 1, val});
        for (nonary) |digit| {
            std.debug.print("{}", .{digit});
        }
        
        // Calculate embedding properties
        var magnitude: f32 = 0.0;
        for (embedding) |e| {
            magnitude += e * e;
        }
        magnitude = @sqrt(magnitude);
        
        std.debug.print("\n   9D Embedding magnitude: {d:.3}\n", .{magnitude});
    }
    
    std.debug.print("✓ Nonary exploration complete!\n\n");
}

fn demo_device_analysis(device: *DeviceSpecs) void {
    std.debug.print("🔧 DEVICE PERFORMANCE ANALYSIS 🔧\n");
    std.debug.print("=" ** 40 ++ "\n");
    
    const metrics = device.calculate_performance_metrics();
    
    std.debug.print("Analyzing current device configuration...\n");
    std.debug.print("Processing Units: {}\n", .{device.processing_units});
    std.debug.print("Base Frequency: {d:.6} Hz\n", .{device.frequency_hz});
    std.debug.print("Operations/Second: {d:.0}\n", .{metrics.operations_per_second});
    std.debug.print("Efficiency: {d:.1} ops/watt\n", .{metrics.efficiency_ops_per_watt});
    
    // Performance scaling analysis
    std.debug.print("\nPerformance Scaling Analysis:\n");
    const scale_factors = [_]f32{ 1.0, 2.0, 4.0, 8.0 };
    
    for (scale_factors) |factor| {
        const scaled_ops = metrics.operations_per_second * factor;
        const scaled_power = device.power_consumption_watts * factor;
        const scaled_efficiency = scaled_ops / scaled_power;
        
        std.debug.print("  {d:.0}x scale: {d:.0} ops/s, {d:.1} efficiency\n", 
            .{factor, scaled_ops, scaled_efficiency});
    }
    
    std.debug.print("✓ Performance analysis complete!\n\n");
}

fn demo_mathematical_harmony(processor: *WaveProcessor, embedder: *NonaryEmbedder) !void {
    std.debug.print("✨ MATHEMATICAL HARMONY ANALYSIS ✨\n");
    std.debug.print("=" ** 40 ++ "\n");
    
    std.debug.print("Analyzing harmonic relationships in 9D mathematics...\n");
    
    // Test harmonic frequencies
    const harmonic_freqs = [_]f32{ 1.0, 2.0, 3.0, 5.0, 8.0 };
    
    for (harmonic_freqs) |freq| {
        // Create harmonic input
        var input: [512]f32 = undefined;
        var output: [512]f32 = undefined;
        
        for (input, 0..) |*inp, i| {
            const t = @as(f32, @floatFromInt(i)) / 512.0;
            inp.* = @sin(2.0 * 3.14159 * freq * t);
        }
        
        try processor.forward_pass(input[0..], output[0..]);
        
        // Analyze in nonary
        const nonary = try embedder.decimal_to_nonary(freq);
        
        const state = processor.get_system_state();
        
        std.debug.print("\nFreq: {d:.1} Hz, Nonary: ", .{freq});
        for (nonary[6..]) |digit| {
            std.debug.print("{}", .{digit});
        }
        std.debug.print(", Coherence: {d:.3}\n", .{state.coherence});
    }
    
    std.debug.print("\n🎵 Harmonic analysis reveals deep mathematical patterns\n");
    std.debug.print("connecting frequency, number systems, and 9D processing!\n");
    std.debug.print("✓ Mathematical harmony analysis complete!\n\n");
}