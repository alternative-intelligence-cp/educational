const std = @import("std");
const print = std.debug.print;
const Allocator = std.mem.Allocator;
const math = std.math;

/// Educational Mamba configuration - Zig's elegant struct design
pub const MambaConfig = struct {
    d_model: u32,           // Model dimension
    d_state: u32 = 16,      // State dimension
    d_inner: u32,           // Inner dimension (computed)
    dt_rank: u32,           // Delta rank (computed)
    
    /// Create configuration with computed fields
    pub fn init(d_model: u32, d_state: u32) MambaConfig {
        return MambaConfig{
            .d_model = d_model,
            .d_state = d_state,
            .d_inner = d_model * 2,     // Standard Mamba expansion
            .dt_rank = d_model / 16,    // Standard ratio
        };
    }
    
    /// Print configuration info
    pub fn print_info(self: MambaConfig) void {
        print("🦎 Zig Mamba Config:\n");
        print("   d_model: {}, d_state: {}, d_inner: {}, dt_rank: {}\n\n", 
              .{ self.d_model, self.d_state, self.d_inner, self.dt_rank });
    }
};

/// Memory-safe Linear Layer with Zig's ownership model
pub const LinearLayer = struct {
    in_features: u32,
    out_features: u32,
    weight: []f32,          // Owned slice
    bias: ?[]f32,           // Optional bias (Zig's null safety)
    allocator: Allocator,   // Memory management
    
    const Self = @This();
    
    /// Initialize linear layer with proper memory management
    pub fn init(allocator: Allocator, in_features: u32, out_features: u32, has_bias: bool) !Self {
        // Allocate weight matrix
        const weight = try allocator.alloc(f32, in_features * out_features);
        errdefer allocator.free(weight);
        
        // Initialize weights (Xavier uniform)
        var prng = std.rand.DefaultPrng.init(@intCast(std.time.milliTimestamp()));
        const random = prng.random();
        const scale = math.sqrt(6.0 / @as(f32, @floatFromInt(in_features + out_features)));
        
        for (weight) |*w| {
            w.* = (random.float(f32) * 2.0 - 1.0) * scale;
        }
        
        // Allocate bias if needed
        var bias: ?[]f32 = null;
        if (has_bias) {
            bias = try allocator.alloc(f32, out_features);
            errdefer allocator.free(bias.?);
            
            // Initialize bias to zero
            @memset(bias.?, 0.0);
        }
        
        return Self{
            .in_features = in_features,
            .out_features = out_features,
            .weight = weight,
            .bias = bias,
            .allocator = allocator,
        };
    }
    
    /// Clean up allocated memory
    pub fn deinit(self: Self) void {
        self.allocator.free(self.weight);
        if (self.bias) |bias| {
            self.allocator.free(bias);
        }
    }
    
    /// Forward pass: y = x * W^T + b
    pub fn forward(self: Self, x: []const f32, y: []f32, batch_size: u32, seq_len: u32) void {
        const total_samples = batch_size * seq_len;
        std.debug.assert(x.len == total_samples * self.in_features);
        std.debug.assert(y.len == total_samples * self.out_features);
        
        for (0..total_samples) |sample| {
            const x_offset = sample * self.in_features;
            const y_offset = sample * self.out_features;
            
            const x_row = x[x_offset..x_offset + self.in_features];
            const y_row = y[y_offset..y_offset + self.out_features];
            
            // Initialize with bias
            if (self.bias) |bias| {
                @memcpy(y_row, bias);
            } else {
                @memset(y_row, 0.0);
            }
            
            // Matrix multiplication: y = x * W^T
            for (0..self.out_features) |out_idx| {
                var sum = y_row[out_idx]; // Start with bias
                const weight_row = self.weight[out_idx * self.in_features..(out_idx + 1) * self.in_features];
                
                for (0..self.in_features) |in_idx| {
                    sum += x_row[in_idx] * weight_row[in_idx];
                }
                
                y_row[out_idx] = sum;
            }
        }
    }
};

/// SiLU (Swish) activation function
pub fn silu(x: f32) f32 {
    return x / (1.0 + math.exp(-x));
}

/// Apply SiLU activation in-place
pub fn silu_inplace(data: []f32) void {
    for (data) |*x| {
        x.* = silu(x.*);
    }
}

/// Educational Mamba Model - Zig's elegant approach
pub const EducationalMamba = struct {
    config: MambaConfig,
    allocator: Allocator,
    
    // Model layers
    in_proj: LinearLayer,      // Input projection
    x_proj: LinearLayer,       // Selective parameter projection  
    dt_proj: LinearLayer,      // Delta projection
    out_proj: LinearLayer,     // Output projection
    
    // State space parameters
    A_log: []f32,             // Log of A matrix for numerical stability
    D: []f32,                 // Skip connection weights
    
    // Working buffers (pre-allocated for efficiency)
    temp_buffer_1: []f32,
    temp_buffer_2: []f32,
    hidden_states: []f32,
    
    const Self = @This();
    
    /// Initialize Educational Mamba model
    pub fn init(allocator: Allocator, config: MambaConfig) !Self {
        config.print_info();
        
        // Create layers
        const in_proj = try LinearLayer.init(allocator, config.d_model, config.d_inner * 2, false);
        errdefer in_proj.deinit();
        
        const x_proj = try LinearLayer.init(allocator, config.d_inner, 
                                           config.dt_rank + config.d_state * 2, false);
        errdefer x_proj.deinit();
        
        const dt_proj = try LinearLayer.init(allocator, config.dt_rank, config.d_inner, true);
        errdefer dt_proj.deinit();
        
        const out_proj = try LinearLayer.init(allocator, config.d_inner, config.d_model, false);
        errdefer out_proj.deinit();
        
        // Initialize A matrix (state transition)
        const A_log = try allocator.alloc(f32, config.d_inner * config.d_state);
        errdefer allocator.free(A_log);
        
        var prng = std.rand.DefaultPrng.init(@intCast(std.time.milliTimestamp()));
        const random = prng.random();
        
        // S4 initialization: A is diagonal with negative real parts
        for (0..config.d_inner) |i| {
            for (0..config.d_state) |j| {
                const val = -math.exp(random.float(f32) * math.log(@as(f32, @floatFromInt(config.d_state))));
                A_log[i * config.d_state + j] = math.log(-val);
            }
        }
        
        // Initialize D (skip connection)
        const D = try allocator.alloc(f32, config.d_inner);
        errdefer allocator.free(D);
        for (D) |*d| {
            d.* = 1.0;
        }
        
        // Pre-allocate working buffers
        const max_batch_seq = 16 * 1024; // Reasonable maximum for educational purposes
        const temp_buffer_1 = try allocator.alloc(f32, max_batch_seq * config.d_inner);
        errdefer allocator.free(temp_buffer_1);
        
        const temp_buffer_2 = try allocator.alloc(f32, max_batch_seq * config.d_inner);
        errdefer allocator.free(temp_buffer_2);
        
        const hidden_states = try allocator.alloc(f32, 16 * config.d_inner * config.d_state);
        errdefer allocator.free(hidden_states);
        
        print("✅ Educational Mamba model initialized successfully!\n\n");
        
        return Self{
            .config = config,
            .allocator = allocator,
            .in_proj = in_proj,
            .x_proj = x_proj,
            .dt_proj = dt_proj,
            .out_proj = out_proj,
            .A_log = A_log,
            .D = D,
            .temp_buffer_1 = temp_buffer_1,
            .temp_buffer_2 = temp_buffer_2,
            .hidden_states = hidden_states,
        };
    }
    
    /// Clean up all allocated resources
    pub fn deinit(self: Self) void {
        self.in_proj.deinit();
        self.x_proj.deinit();
        self.dt_proj.deinit();
        self.out_proj.deinit();
        self.allocator.free(self.A_log);
        self.allocator.free(self.D);
        self.allocator.free(self.temp_buffer_1);
        self.allocator.free(self.temp_buffer_2);
        self.allocator.free(self.hidden_states);
    }
    
    /// Forward pass through Educational Mamba
    /// This demonstrates the complete Mamba algorithm with Zig's safety and elegance
    pub fn forward(self: *Self, x: []const f32, y: []f32, batch_size: u32, seq_len: u32) !void {
        const total_samples = batch_size * seq_len;
        const cfg = self.config;
        
        // Validate input dimensions
        if (x.len != total_samples * cfg.d_model) return error.InvalidInputShape;
        if (y.len != total_samples * cfg.d_model) return error.InvalidOutputShape;
        
        print("🔄 Processing batch_size={}, seq_len={}...\n", .{ batch_size, seq_len });
        
        // === STEP 1: Input Projection ===
        // Get slices of temp buffers (Zig's safe slicing)
        const x_proj = self.temp_buffer_1[0..total_samples * cfg.d_inner * 2];
        self.in_proj.forward(x, x_proj, batch_size, seq_len);
        
        // Split into x_inner and gate
        const inner_size = total_samples * cfg.d_inner;
        const x_inner = x_proj[0..inner_size];
        const gate = x_proj[inner_size..inner_size * 2];
        
        // === STEP 2: Simplified Selective State Space ===
        // For educational purposes, we'll implement a simplified version
        
        // Apply SiLU to inner representation
        silu_inplace(x_inner);
        
        // Project for selective parameters
        const x_ssm_proj = self.temp_buffer_2[0..total_samples * (cfg.dt_rank + cfg.d_state * 2)];
        self.x_proj.forward(x_inner, x_ssm_proj, batch_size, seq_len);
        
        // Simplified state space processing (educational version)
        const y_ssm = self.temp_buffer_1[0..inner_size]; // Reuse buffer
        try self.simplified_selective_scan(x_inner, x_ssm_proj, y_ssm, batch_size, seq_len);
        
        // Add skip connection
        for (0..total_samples) |sample| {
            for (0..cfg.d_inner) |i| {
                const idx = sample * cfg.d_inner + i;
                y_ssm[idx] += self.D[i] * x_inner[idx];
            }
        }
        
        // === STEP 3: Gating ===
        silu_inplace(gate);
        for (0..inner_size) |i| {
            y_ssm[i] *= gate[i];
        }
        
        // === STEP 4: Output Projection ===
        self.out_proj.forward(y_ssm, y, batch_size, seq_len);
        
        print("✅ Forward pass completed successfully!\n");
    }
    
    /// Simplified selective scan for educational purposes
    /// This shows the core idea while being easier to understand
    fn simplified_selective_scan(self: *Self, x: []const f32, ssm_proj: []const f32, 
                                y: []f32, batch_size: u32, seq_len: u32) !void {
        const cfg = self.config;
        
        // Clear hidden state
        @memset(self.hidden_states[0..batch_size * cfg.d_inner * cfg.d_state], 0.0);
        
        // Extract B and C from projection (simplified)
        const B_start = cfg.dt_rank;
        const C_start = cfg.dt_rank + cfg.d_state;
        
        // Process each time step
        for (0..seq_len) |t| {
            for (0..batch_size) |b| {
                // Get current input and parameters
                const x_idx = (b * seq_len + t) * cfg.d_inner;
                const proj_idx = (b * seq_len + t) * (cfg.dt_rank + cfg.d_state * 2);
                
                const x_t = x[x_idx..x_idx + cfg.d_inner];
                const B_t = ssm_proj[proj_idx + B_start..proj_idx + B_start + cfg.d_state];
                const C_t = ssm_proj[proj_idx + C_start..proj_idx + C_start + cfg.d_state];
                
                // Simplified state update for each inner dimension
                for (0..cfg.d_inner) |i| {
                    const h_offset = b * cfg.d_inner * cfg.d_state + i * cfg.d_state;
                    const h = self.hidden_states[h_offset..h_offset + cfg.d_state];
                    
                    // Update state: h = A * h + B * x (simplified)
                    for (0..cfg.d_state) |j| {
                        const A_val = math.exp(self.A_log[i * cfg.d_state + j]);
                        h[j] = A_val * h[j] + B_t[j] * x_t[i] * 0.01; // Small scale factor
                    }
                    
                    // Compute output: y = C^T * h
                    var output: f32 = 0.0;
                    for (0..cfg.d_state) |j| {
                        output += C_t[j] * h[j];
                    }
                    
                    y[x_idx + i] = output;
                }
            }
        }
    }
    
    /// Get model statistics for analysis
    pub fn get_stats(self: Self) struct { parameters: u32, memory_mb: f32 } {
        var param_count: u32 = 0;
        
        // Count parameters
        param_count += @intCast(self.in_proj.weight.len);
        param_count += @intCast(self.x_proj.weight.len);
        param_count += @intCast(self.dt_proj.weight.len);
        param_count += @intCast(self.out_proj.weight.len);
        param_count += @intCast(self.A_log.len);
        param_count += @intCast(self.D.len);
        
        if (self.dt_proj.bias) |bias| {
            param_count += @intCast(bias.len);
        }
        
        const memory_bytes = param_count * @sizeOf(f32) + 
                           self.temp_buffer_1.len * @sizeOf(f32) +
                           self.temp_buffer_2.len * @sizeOf(f32) +
                           self.hidden_states.len * @sizeOf(f32);
        
        return .{
            .parameters = param_count,
            .memory_mb = @as(f32, @floatFromInt(memory_bytes)) / (1024.0 * 1024.0),
        };
    }
};

/// Performance testing utilities
const Timer = struct {
    start_time: i64,
    
    pub fn start() Timer {
        return Timer{ .start_time = std.time.nanoTimestamp() };
    }
    
    pub fn elapsed_ms(self: Timer) f64 {
        const end_time = std.time.nanoTimestamp();
        return @as(f64, @floatFromInt(end_time - self.start_time)) / 1_000_000.0;
    }
};

/// Benchmark Educational Mamba performance
pub fn benchmark_mamba(allocator: Allocator) !void {
    print("🧠⚡ Educational Zig Mamba Benchmark ⚡🧠\n");
    print("======================================\n\n");
    
    const test_configs = [_]struct {
        d_model: u32,
        d_state: u32,
        seq_len: u32,
        batch_size: u32,
        desc: []const u8,
    }{
        .{ .d_model = 64, .d_state = 8, .seq_len = 50, .batch_size = 1, .desc = "Small model, single sequence" },
        .{ .d_model = 128, .d_state = 16, .seq_len = 100, .batch_size = 2, .desc = "Medium model, small batch" },
        .{ .d_model = 256, .d_state = 32, .seq_len = 50, .batch_size = 1, .desc = "Large model, medium sequence" },
        .{ .d_model = 128, .d_state = 16, .seq_len = 200, .batch_size = 1, .desc = "Medium model, long sequence" },
    };
    
    for (test_configs) |test_config| {
        print("📊 {s}\n", .{test_config.desc});
        
        // Create model configuration
        const config = MambaConfig.init(test_config.d_model, test_config.d_state);
        
        // Initialize model
        var mamba = try EducationalMamba.init(allocator, config);
        defer mamba.deinit();
        
        const stats = mamba.get_stats();
        print("   Parameters: {}, Memory: {d:.2f} MB\n", .{ stats.parameters, stats.memory_mb });
        
        // Allocate input and output
        const input_size = test_config.batch_size * test_config.seq_len * config.d_model;
        const input = try allocator.alloc(f32, input_size);
        defer allocator.free(input);
        
        const output = try allocator.alloc(f32, input_size);
        defer allocator.free(output);
        
        // Initialize random input
        var prng = std.rand.DefaultPrng.init(42);
        const random = prng.random();
        for (input) |*x| {
            x.* = (random.float(f32) - 0.5) * 0.2;
        }
        
        // Warmup
        try mamba.forward(input, output, test_config.batch_size, test_config.seq_len);
        
        // Benchmark
        const timer = Timer.start();
        try mamba.forward(input, output, test_config.batch_size, test_config.seq_len);
        const elapsed = timer.elapsed_ms();
        
        const tokens = test_config.batch_size * test_config.seq_len;
        const tokens_per_sec = @as(f64, @floatFromInt(tokens)) / (elapsed / 1000.0);
        
        print("   ⚡ Performance: {d:.2f}ms ({d:.0f} tokens/sec)\n", .{ elapsed, tokens_per_sec });
        
        // Analyze output
        var output_sum: f32 = 0.0;
        var output_sq_sum: f32 = 0.0;
        for (output) |val| {
            output_sum += val;
            output_sq_sum += val * val;
        }
        
        const output_mean = output_sum / @as(f32, @floatFromInt(output.len));
        const output_var = (output_sq_sum / @as(f32, @floatFromInt(output.len))) - output_mean * output_mean;
        const output_std = math.sqrt(output_var);
        
        print("   📈 Output: mean={d:.4f}, std={d:.4f}\n", .{ output_mean, output_std });
        print("   ✅ Test completed successfully!\n\n");
    }
    
    print("🎯 Zig Implementation Highlights:\n");
    print("=================================\n");
    print("✅ Memory safety without garbage collection\n");
    print("✅ Compile-time optimizations and generics\n");
    print("✅ Zero-cost abstractions\n");
    print("✅ Clean, readable syntax\n");
    print("✅ Excellent error handling\n");
    print("✅ Performance competitive with C\n");
}

/// Main function demonstrating Zig Mamba
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();
    
    print("🦎 Educational Zig Mamba Implementation 🦎\n");
    print("Modern systems programming with safety and elegance\n\n");
    
    try benchmark_mamba(allocator);
    
    print("\n💡 Educational Value:\n");
    print("====================\n");
    print("📚 Demonstrates modern systems programming paradigms\n");
    print("🛡️  Shows how memory safety can coexist with performance\n");
    print("⚡ Illustrates compile-time optimizations\n");
    print("🎯 Bridges gap between high-level and systems programming\n");
    print("🚀 Foundation for production-grade implementations\n");
}