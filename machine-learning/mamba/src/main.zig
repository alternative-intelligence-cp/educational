//! Mamba State-Space Model - Educational Implementation
//! ==================================================
//!
//! High-performance Zig implementation of the Mamba selective state-space model.
//! This implementation demonstrates:
//! - O(1) memory complexity for sequence processing
//! - Selective scan algorithms
//! - Causal convolutions with state management
//! - Educational code structure with comprehensive documentation

const std = @import("std");
const Allocator = std.mem.Allocator;
const print = std.debug.print;

/// Configuration for Mamba state-space model
pub const MambaConfig = struct {
    d_model: usize = 288,         // Model dimension
    n_layers: usize = 6,          // Number of layers
    vocab_size: usize = 32000,    // Vocabulary size
    d_state: usize = 16,          // State dimension (N)
    expand: usize = 2,            // Expansion factor (E)
    d_conv: usize = 4,            // Convolution kernel width
    dt_rank: usize = 0,           // Delta projection rank (auto if 0)

    /// Get inner dimension (expanded)
    pub fn get_d_inner(self: MambaConfig) usize {
        return self.expand * self.d_model;
    }

    /// Get delta projection rank (auto-calculated if not specified)
    pub fn get_dt_rank(self: MambaConfig) usize {
        if (self.dt_rank > 0) return self.dt_rank;
        return (self.d_model + 15) / 16; // Standard calculation
    }

    /// Estimate total parameters
    pub fn estimate_params(self: MambaConfig) usize {
        const d_inner = self.get_d_inner();
        const dt_rank = self.get_dt_rank();
        
        var params: usize = 0;
        
        // Per layer parameters
        for (0..self.n_layers) |_| {
            params += self.d_model;                    // norm_weight
            params += self.d_model * 2 * d_inner;     // in_proj_weight
            params += d_inner * self.d_conv;          // conv_weight
            params += d_inner;                        // conv_bias
            params += d_inner * (dt_rank + 2 * self.d_state); // x_proj_weight
            params += dt_rank * d_inner;              // dt_proj_weight
            params += d_inner;                        // dt_proj_bias
            params += d_inner * self.d_state;         // A_log
            params += d_inner;                        // D
            params += d_inner * self.d_model;         // out_proj_weight
        }
        
        // Embedding and output
        params += self.vocab_size * self.d_model;     // embeddings
        params += self.d_model * self.vocab_size;     // output projection
        
        return params;
    }
};

/// Runtime state for Mamba processing
/// Key insight: Memory allocation is O(1) regardless of sequence length
pub const MambaRunState = struct {
    // Working buffers
    x: []f32,              // Current token representation [d_model]
    xz: []f32,             // Combined projection buffer [2*d_inner]
    dt: []f32,             // Delta time parameters [d_inner]
    B: []f32,              // Input-to-state matrix [d_state]
    C: []f32,              // State-to-output matrix [d_state]

    // The crucial O(1) states - size independent of sequence length
    conv_state: []f32,     // Convolutional state [n_layers, d_inner, d_conv]
    ssm_state: []f32,      // State-space model state [n_layers, d_inner, d_state]

    allocator: Allocator,

    pub fn init(allocator: Allocator, cfg: MambaConfig) !MambaRunState {
        const d_inner = cfg.get_d_inner();
        const conv_total = cfg.n_layers * d_inner * cfg.d_conv;
        const ssm_total = cfg.n_layers * d_inner * cfg.d_state;

        return MambaRunState{
            .x = try allocator.alloc(f32, cfg.d_model),
            .xz = try allocator.alloc(f32, 2 * d_inner),
            .dt = try allocator.alloc(f32, d_inner),
            .B = try allocator.alloc(f32, cfg.d_state),
            .C = try allocator.alloc(f32, cfg.d_state),
            
            // O(1) allocation - constant memory regardless of sequence length
            .conv_state = try allocator.alloc(f32, conv_total),
            .ssm_state = try allocator.alloc(f32, ssm_total),
            .allocator = allocator,
        };
    }
    
    pub fn deinit(self: MambaRunState) void {
        self.allocator.free(self.x);
        self.allocator.free(self.xz);
        self.allocator.free(self.dt);
        self.allocator.free(self.B);
        self.allocator.free(self.C);
        self.allocator.free(self.conv_state);
        self.allocator.free(self.ssm_state);
    }

    /// Get memory usage in bytes
    pub fn getMemoryUsage(self: MambaRunState) usize {
        return (self.x.len + self.xz.len + self.dt.len + self.B.len + self.C.len +
                self.conv_state.len + self.ssm_state.len) * @sizeOf(f32);
    }

    /// Reset all states (useful for new sequences)
    pub fn reset(self: *MambaRunState) void {
        @memset(self.conv_state, 0.0);
        @memset(self.ssm_state, 0.0);
    }
};

/// Weight matrices for Mamba model (educational placeholder)
pub const MambaWeights = struct {
    norm_weight: []const f32,       // [n_layers * d_model]
    in_proj_weight: []const f32,    // [n_layers * d_model * 2*d_inner]
    conv_weight: []const f32,       // [n_layers * d_inner * d_conv]
    conv_bias: []const f32,         // [n_layers * d_inner]
    x_proj_weight: []const f32,     // [n_layers * d_inner * (dt_rank + 2*d_state)]
    dt_proj_weight: []const f32,    // [n_layers * dt_rank * d_inner]
    dt_proj_bias: []const f32,      // [n_layers * d_inner]
    A_log: []const f32,             // [n_layers * d_inner * d_state] (log scale)
    D: []const f32,                 // [n_layers * d_inner] (skip connection)
    out_proj_weight: []const f32,   // [n_layers * d_inner * d_model]
};

/// Core Mamba block processing
/// This implements the selective state-space mechanism
pub fn mamba_block(
    cfg: MambaConfig,
    s: *MambaRunState,
    w: *MambaWeights,
    layer_idx: usize,
) void {
    const d_model = cfg.d_model;
    const d_inner = cfg.get_d_inner();
    const d_state = cfg.d_state;
    const dt_rank = cfg.get_dt_rank();
    const d_conv = cfg.d_conv;

    // 1. RMSNorm (Input normalization)
    rmsnorm(s.x, s.x, w.norm_weight[layer_idx * d_model ..]);

    // 2. Input Projection (Split into x and z branches)
    // Projects d_model -> 2*d_inner, then split
    matmul(s.xz, s.x, w.in_proj_weight, 1, d_model, 2 * d_inner);

    // Split the projection
    const x_ptr = s.xz[0..d_inner];     // Main branch
    const z_ptr = s.xz[d_inner..];      // Gating branch

    // 3. Causal Convolution (Local pattern recognition)
    const conv_offset = layer_idx * d_inner * d_conv;
    
    for (0..d_inner) |i| {
        const state_idx = conv_offset + i * d_conv;
        var channel_state = s.conv_state[state_idx .. state_idx + d_conv];

        // Shift buffer (FIFO for causality)
        std.mem.copyForwards(f32, channel_state[0 .. d_conv - 1], channel_state[1 .. d_conv]);
        channel_state[d_conv - 1] = x_ptr[i];

        // Convolution dot product
        var conv_out: f32 = w.conv_bias[layer_idx * d_inner + i];
        const w_idx = layer_idx * d_inner * d_conv + i * d_conv;
        const w_kernel = w.conv_weight[w_idx .. w_idx + d_conv];

        for (0..d_conv) |k| {
            conv_out += channel_state[k] * w_kernel[k];
        }
        x_ptr[i] = silu(conv_out);
    }

    // 4. Parameter Selection (Key innovation: learnable parameters)
    // Project convolved x to [dt | B | C]
    var params_buf: [256]f32 = undefined; // Temporary buffer
    const param_dim = dt_rank + 2 * d_state;
    matmul(params_buf[0..param_dim], x_ptr, w.x_proj_weight, 1, d_inner, param_dim);

    // Split parameters
    const dt_raw = params_buf[0..dt_rank];
    const B_val = params_buf[dt_rank .. dt_rank + d_state];
    const C_val = params_buf[dt_rank + d_state ..];

    // 5. Delta Projection (Convert to per-channel deltas)
    matmul(s.dt, dt_raw, w.dt_proj_weight, 1, dt_rank, d_inner);

    // 6. Selective Scan (The core algorithm)
    const ssm_offset = layer_idx * d_inner * d_state;

    for (0..d_inner) |i| {
        // Discretize delta (learnable time step)
        const dt_i = softplus(s.dt[i] + w.dt_proj_bias[i]);
        
        // Access state for this channel
        const h_idx = ssm_offset + i * d_state;
        var h = s.ssm_state[h_idx .. h_idx + d_state];
        const A_row = w.A_log[(layer_idx * d_inner + i) * d_state ..];
        const x_val = x_ptr[i];
        
        var y_val: f32 = 0.0;

        // State update loop (the selective scan)
        for (0..d_state) |n| {
             // Discretize A matrix: A_bar = exp(A * dt)
             const A_val = -std.math.exp(A_row[n]); 
             const dA = std.math.exp(A_val * dt_i);

             // Discretize B matrix: B_bar = dt * B
             const dB = dt_i * B_val[n];

             // State update: h = dA * h + dB * x
             h[n] = (dA * h[n]) + (dB * x_val);

             // Output computation: y = C * h
             y_val += h[n] * C_val[n];
        }

        // Skip connection (D matrix)
        y_val += x_val * w.D[layer_idx * d_inner + i];
        
        // 7. Gating (Combine with z branch)
        const z_val = silu(z_ptr[i]);
        x_ptr[i] = y_val * z_val; 
    }

    // 8. Output Projection (Back to model dimension)
    matmul(s.x, x_ptr, w.out_proj_weight, 1, d_inner, d_model);
}

// ============================================================================
// Utility Functions
// ============================================================================

/// Matrix multiplication: out = a @ b
/// Educational implementation (not optimized)
fn matmul(out: []f32, a: []const f32, b: []const f32, m: usize, k: usize, n: usize) void {
    @memset(out, 0);
    for (0..m) |row| {
        for (0..n) |col| {
            var sum: f32 = 0.0;
            for (0..k) |i| {
                sum += a[row * k + i] * b[i * n + col];
            }
            out[row * n + col] = sum;
        }
    }
}

/// RMSNorm: x = x * rsqrt(mean(x^2) + eps) * weight
fn rmsnorm(out: []f32, x: []const f32, weight: []const f32) void {
    const size = x.len;
    const eps = 1e-5;
    var ss: f32 = 0.0;
    
    // Compute sum of squares
    for (x) |val| {
        ss += val * val;
    }
    ss /= @as(f32, @floatFromInt(size));
    ss += eps;
    const inv_ss = 1.0 / std.math.sqrt(ss);
    
    // Apply normalization and scaling
    for (0..size) |i| {
        out[i] = x[i] * inv_ss * weight[i];
    }
}

/// SiLU activation: x * sigmoid(x)
inline fn silu(x: f32) f32 {
    return x / (1.0 + std.math.exp(-x));
}

/// Softplus: log(1 + exp(x))
inline fn softplus(x: f32) f32 {
    return std.math.log1p(std.math.exp(x));
}

// ============================================================================
// Educational Demo
// ============================================================================

/// Demonstrate Mamba's O(1) memory property
fn demonstrateMemoryEfficiency(allocator: Allocator, cfg: MambaConfig) !void {
    print("\n📏 Memory Efficiency Demonstration:\n");
    
    var state = try MambaRunState.init(allocator, cfg);
    defer state.deinit();
    
    const memory_kb = state.getMemoryUsage() / 1024;
    
    print("Configuration: d_model={}, n_layers={}, d_state={}\n", .{cfg.d_model, cfg.n_layers, cfg.d_state});
    print("Total memory usage: {} KB\n", .{memory_kb});
    print("\nMemory breakdown:\n");
    print("- Conv state: {} KB\n", .{(state.conv_state.len * @sizeOf(f32)) / 1024});
    print("- SSM state: {} KB\n", .{(state.ssm_state.len * @sizeOf(f32)) / 1024});
    print("- Working buffers: {} KB\n", .{((state.x.len + state.xz.len + state.dt.len + state.B.len + state.C.len) * @sizeOf(f32)) / 1024});
    
    print("\n✨ Key insight: This memory usage is CONSTANT regardless of sequence length!\n");
    print("Compare to Transformer O(N²) attention memory.\n");
}

/// Educational demo showing architectural concepts
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    print("🐍 Mamba State-Space Model - Educational Demo\n");
    print("===============================================\n");

    const cfg = MambaConfig{
        .d_model = 288,
        .n_layers = 6,
        .d_state = 16,
        .expand = 2,
        .d_conv = 4,
    };

    print("\n📚 Educational Features:\n");
    print("- O(1) memory complexity\n");
    print("- Selective state-space mechanism\n");
    print("- Causal convolution with state\n");
    print("- Educational implementation\n");

    // Configuration analysis
    print("\n🛠️ Configuration Analysis:\n");
    print("d_model: {} (model dimension)\n", .{cfg.d_model});
    print("d_inner: {} (expanded dimension)\n", .{cfg.get_d_inner()});
    print("d_state: {} (state dimension)\n", .{cfg.d_state});
    print("dt_rank: {} (delta projection rank)\n", .{cfg.get_dt_rank()});
    print("Total parameters: ~{} million\n", .{cfg.estimate_params() / 1_000_000});

    // Memory efficiency demonstration
    try demonstrateMemoryEfficiency(allocator, cfg);

    // Architecture comparison
    print("\n🔍 Architecture Comparison:\n");
    print("| Feature          | Mamba    | Transformer |\n");
    print("|------------------|----------|-------------|\n");
    print("| Memory per token | O(1)     | O(N)        |\n");
    print("| Total memory     | O(1)     | O(N²)       |\n");
    print("| Parallelizable   | No       | Yes         |\n");
    print("| Long sequences   | Excellent| Challenging |\n");

    // State evolution simulation
    print("\n🔄 State Evolution Simulation:\n");
    var state = try MambaRunState.init(allocator, cfg);
    defer state.deinit();
    
    // Simulate processing a few tokens
    for (0..5) |token| {
        // Initialize with dummy input
        for (state.x, 0..) |*x, i| {
            x.* = @sin(@as(f32, @floatFromInt(token + i)) * 0.1);
        }
        
        // Show state evolution (first few dimensions)
        print("Token {}: x[0]={d:.3}, conv_state[0]={d:.3}, ssm_state[0]={d:.3}\n", 
              .{token, state.x[0], state.conv_state[0], state.ssm_state[0]});
        
        // Update conv state (simplified)
        if (state.conv_state.len > 0) {
            state.conv_state[0] = state.x[0] * 0.5 + state.conv_state[0] * 0.5;
        }
        
        // Update SSM state (simplified)
        if (state.ssm_state.len > 0) {
            state.ssm_state[0] = state.ssm_state[0] * 0.9 + state.x[0] * 0.1;
        }
    }

    print("\n🎓 Educational Concepts Demonstrated:\n");
    print("- State-space model fundamentals\n");
    print("- Selective mechanism design\n");
    print("- Memory efficiency analysis\n");
    print("- Causal convolution implementation\n");
    print("- Performance vs parallelizability trade-offs\n");

    print("\n✨ Advanced experiments to try:\n");
    print("- Compare with transformer memory usage\n");
    print("- Implement bidirectional processing\n");
    print("- Study state evolution patterns\n");
    print("- Optimize selective scan implementation\n");
    print("- Experiment with different d_state values\n");
}