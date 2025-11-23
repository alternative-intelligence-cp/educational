// 🧮 Demo: Nonary (Base-9) Mathematics

const std = @import("std");
const NonaryEmbedder = @import("nonary_embedder.zig").NonaryEmbedder;

pub fn main() !void {
    std.debug.print("🧮 NONARY (BASE-9) MATHEMATICS DEMO 🧮\n");
    std.debug.print("Exploring number systems beyond binary and decimal!\n\n");
    
    var embedder = NonaryEmbedder.init(9, 6);
    
    // Run the arithmetic demonstration
    try embedder.nonary_arithmetic_demo();
    
    // Interactive exploration of special mathematical constants
    const special_constants = [_]struct { name: []const u8, value: f32 }{
        .{ .name = "Pi (π)", .value = 3.14159265 },
        .{ .name = "Euler's number (e)", .value = 2.71828182 },
        .{ .name = "Golden ratio (φ)", .value = 1.61803398 },
        .{ .name = "Square root of 2", .value = 1.41421356 },
        .{ .name = "Natural log of 2", .value = 0.69314718 },
    };
    
    std.debug.print("\n🌌 SPECIAL CONSTANTS IN NONARY 🌌\n");
    std.debug.print("=" ** 45 ++ "\n");
    
    for (special_constants) |constant| {
        const nonary = try embedder.decimal_to_nonary(constant.value);
        const embedding = try embedder.embed_to_9d(constant.value);
        
        std.debug.print("\n{s}:\n", .{constant.name});
        std.debug.print("  Decimal: {d:.8}\n", .{constant.value});
        std.debug.print("  Nonary:  ");
        for (nonary) |digit| {
            std.debug.print("{}", .{digit});
        }
        std.debug.print("\n");
        
        std.debug.print("  9D Embedding magnitude: {d:.3}\n", .{calculate_magnitude(embedding)});
    }
    
    // Demonstrate nonary arithmetic properties
    std.debug.print("\n🦄 NONARY ARITHMETIC PROPERTIES 🦄\n");
    std.debug.print("=" ** 40 ++ "\n");
    
    // Base-9 multiplication table excerpt  
    std.debug.print("Base-9 Multiplication (first few rows):\n");
    for (1..5) |i| {
        std.debug.print("  {}: ", .{i});
        for (1..9) |j| {
            const product = i * j;
            const nonary_product = try embedder.decimal_to_nonary(@as(f32, @floatFromInt(product)));
            
            // Show just the last few digits for readability
            var significant_digits: u8 = 0;
            for (nonary_product) |digit| {
                if (digit > 0) significant_digits = 1;
                if (significant_digits > 0) std.debug.print("{}", .{digit});
            }
            if (significant_digits == 0) std.debug.print("0");
            std.debug.print(" ");
        }
        std.debug.print("\n");
    }
    
    std.debug.print("\n✨ Nonary system insights:\n");
    std.debug.print("  • Base-9 uses digits 0-8 (no 9!)\n");
    std.debug.print("  • Perfectly aligned with 9D mathematics\n");
    std.debug.print("  • Creates natural harmonic patterns\n");
    std.debug.print("  • Ideal for advanced embedding systems\n");
    
    std.debug.print("\n🚀 Educational applications:\n");
    std.debug.print("  • Alternative number systems research\n");
    std.debug.print("  • Multi-dimensional data representation\n");
    std.debug.print("  • Signal processing and embeddings\n");
    std.debug.print("  • Advanced mathematical modeling\n");
}

// Helper function to calculate 9D vector magnitude
fn calculate_magnitude(vector: [9]f32) f32 {
    var sum: f32 = 0.0;
    for (vector) |val| {
        sum += val * val;
    }
    return @sqrt(sum);
}