// 🧮 Nonary (Base-9) Embedding System
// Advanced number system for 9D mathematical processing

const std = @import("std");

/// Nonary (Base-9) Embedding System for 9D Processing
/// Demonstrates advanced number system concepts beyond binary
pub const NonaryEmbedder = struct {
    embedding_dimension: u32,
    value_range: f32,
    nonary_precision: u8,
    
    const Self = @This();
    
    /// Initialize nonary embedder with specified precision
    pub fn init(embedding_dim: u32, precision: u8) Self {
        return Self{
            .embedding_dimension = embedding_dim,
            .value_range = 1.0,
            .nonary_precision = precision,
        };
    }
    
    /// Convert decimal number to nonary representation
    pub fn decimal_to_nonary(self: *Self, decimal: f32) ![9]u8 {
        _ = self;
        var nonary = [_]u8{0} ** 9;
        var abs_decimal = @abs(decimal);
        
        // Handle fractional part by scaling
        abs_decimal = abs_decimal * 1000.0; // Scale for precision
        var int_decimal = @as(u32, @intFromFloat(abs_decimal));
        
        // Convert to base-9
        for (0..9) |i| {
            const digit = int_decimal % 9;
            nonary[8-i] = @as(u8, @intCast(digit));
            int_decimal /= 9;
            if (int_decimal == 0) break;
        }
        
        return nonary;
    }
    
    /// Convert nonary back to decimal
    pub fn nonary_to_decimal(self: *Self, nonary: [9]u8) f32 {
        _ = self;
        var decimal: f32 = 0.0;
        var power: f32 = 1.0;
        
        // Convert from base-9 to decimal
        var i: usize = 8;
        while (true) : (i -= 1) {
            decimal += @as(f32, @floatFromInt(nonary[i])) * power;
            power *= 9.0;
            if (i == 0) break;
        }
        
        return decimal / 1000.0; // Unscale
    }
    
    /// Embed value into 9D space using nonary structure
    pub fn embed_to_9d(self: *Self, value: f32) ![9]f32 {
        const nonary = try self.decimal_to_nonary(value);
        var embedding = [_]f32{0.0} ** 9;
        
        // Create 9D embedding from nonary digits
        for (nonary, 0..) |digit, i| {
            // Use sine/cosine structure for smooth embedding
            const angle = @as(f32, @floatFromInt(digit)) * std.math.pi / 4.5;
            embedding[i] = @sin(angle);
        }
        
        return embedding;
    }
    
    /// Demonstrate nonary arithmetic operations
    pub fn nonary_arithmetic_demo(self: *Self) !void {
        std.debug.print("\n🧮 NONARY (BASE-9) ARITHMETIC DEMO\n");
        std.debug.print("=" ** 40 ++ "\n");
        
        // Demo values
        const test_values = [_]f32{ 0.5, 1.0, 2.718, 3.14159, 7.389 };
        
        for (test_values) |val| {
            const nonary = try self.decimal_to_nonary(val);
            const reconstructed = self.nonary_to_decimal(nonary);
            const embedding = try self.embed_to_9d(val);
            
            std.debug.print("Value: {d:.3} → Nonary: ", .{val});
            for (nonary) |digit| {
                std.debug.print("{}", .{digit});
            }
            std.debug.print(" → Reconstructed: {d:.3}\n", .{reconstructed});
            
            std.debug.print("  9D Embedding: [");
            for (embedding, 0..) |emb, i| {
                if (i > 0) std.debug.print(", ");
                std.debug.print("{d:.3}", .{emb});
            }
            std.debug.print("]\n\n");
        }
        
        std.debug.print("🧮 Nonary system demonstrates base-9 mathematics\n");
        std.debug.print("  Perfect for 9D processing and advanced embeddings!\n");
    }
};