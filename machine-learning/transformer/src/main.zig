//! Transformer Architecture - Educational Implementation
//! ===================================================
//!
//! Complete Zig implementation of the Transformer architecture with:
//! - Multi-head attention with KV caching
//! - Rotary Position Embedding (RoPE)
//! - SwiGLU feed-forward networks
//! - RMSNorm layer normalization
//! - Educational code structure with comprehensive documentation

const std = @import("std");
const Allocator = std.mem.Allocator;
const print = std.debug.print;

/// Configuration for Transformer model
pub const TransformerConfig = struct {
    d_model: usize = 288,         // Model dimension
    n_heads: usize = 6,           // Number of attention heads
    n_layers: usize = 6,          // Number of transformer layers
    n_kv_heads: usize = 6,        // KV heads (for Grouped Query Attention)
    vocab_size: usize = 32000,    // Vocabulary size
    seq_len: usize = 2048,        // Maximum sequence length
    norm_eps: f32 = 1e-5,         // Layer normalization epsilon
    rope_base: f32 = 10000.0,     // RoPE base frequency

    /// Get head dimension
    pub fn head_dim(self: TransformerConfig) usize {
        return self.d_model / self.n_heads;
    }

    /// Get KV head dimension
    pub fn kv_head_dim(self: TransformerConfig) usize {
        return self.d_model / self.n_kv_heads;
    }

    /// Get feed-forward dimension (typically 4x model dim)
    pub fn ffn_dim(self: TransformerConfig) usize {
        return 4 * self.d_model;
    }

    /// Estimate total parameters
    pub fn estimate_params(self: TransformerConfig) usize {
        const head_dim_val = self.head_dim();
        const kv_head_dim_val = self.kv_head_dim();
        const ffn_dim_val = self.ffn_dim();
        
        var params: usize = 0;
        
        // Per layer parameters
        for (0..self.n_layers) |_| {
            params += self.d_model;                           // attention norm
            params += self.d_model * self.n_heads * head_dim_val;      // wq
            params += self.d_model * self.n_kv_heads * kv_head_dim_val; // wk
            params += self.d_model * self.n_kv_heads * kv_head_dim_val; // wv
            params += self.d_model * self.d_model;            // wo (attention output)
            params += self.d_model;                           // ffn norm
            params += self.d_model * ffn_dim_val;             // w1 (gate)
            params += ffn_dim_val * self.d_model;             // w2 (down)
            params += self.d_model * ffn_dim_val;             // w3 (up)
        }
        
        // Embedding and output
        params += self.vocab_size * self.d_model;            // token embeddings
        params += self.d_model * self.vocab_size;            // output projection
        
        return params;
    }

    /// Estimate KV cache memory in bytes
    pub fn estimate_kv_cache_size(self: TransformerConfig) usize {
        // Key and Value caches: [n_layers, seq_len, n_kv_heads, kv_head_dim] each
        return 2 * self.n_layers * self.seq_len * self.n_kv_heads * self.kv_head_dim() * @sizeOf(f32);
    }
};

/// Runtime state for Transformer processing with KV caching
pub const TransformerRunState = struct {
    // Current token representation
    x: []f32,              // [d_model] current input/output
    xb: []f32,             // [d_model] residual buffer
    xb2: []f32,            // [d_model] another residual buffer
    
    // Attention computation buffers
    q: []f32,              // [n_heads * head_dim] queries
    k: []f32,              // [n_kv_heads * kv_head_dim] keys
    v: []f32,              // [n_kv_heads * kv_head_dim] values
    att: []f32,            // [n_heads * seq_len] attention scores
    
    // Feed-forward computation buffers
    h1: []f32,             // [ffn_dim] hidden state 1 (gate)
    h2: []f32,             // [ffn_dim] hidden state 2 (up)
    
    // KV Cache - The key optimization for autoregressive generation
    key_cache: []f32,      // [n_layers, seq_len, n_kv_heads, kv_head_dim]
    value_cache: []f32,    // [n_layers, seq_len, n_kv_heads, kv_head_dim]
    
    // Position tracking
    pos: usize,            // Current position in sequence
    
    allocator: Allocator,

    pub fn init(allocator: Allocator, cfg: TransformerConfig) !TransformerRunState {
        const head_dim_val = cfg.head_dim();
        const kv_head_dim_val = cfg.kv_head_dim();
        const ffn_dim_val = cfg.ffn_dim();
        
        // Calculate KV cache sizes
        const kv_cache_size = cfg.n_layers * cfg.seq_len * cfg.n_kv_heads * kv_head_dim_val;

        return TransformerRunState{
            .x = try allocator.alloc(f32, cfg.d_model),
            .xb = try allocator.alloc(f32, cfg.d_model),
            .xb2 = try allocator.alloc(f32, cfg.d_model),
            
            .q = try allocator.alloc(f32, cfg.n_heads * head_dim_val),
            .k = try allocator.alloc(f32, cfg.n_kv_heads * kv_head_dim_val),
            .v = try allocator.alloc(f32, cfg.n_kv_heads * kv_head_dim_val),
            .att = try allocator.alloc(f32, cfg.n_heads * cfg.seq_len),
            
            .h1 = try allocator.alloc(f32, ffn_dim_val),
            .h2 = try allocator.alloc(f32, ffn_dim_val),
            
            // KV Cache allocation - this is the memory that enables O(1) attention updates
            .key_cache = try allocator.alloc(f32, kv_cache_size),
            .value_cache = try allocator.alloc(f32, kv_cache_size),
            
            .pos = 0,
            .allocator = allocator,
        };
    }
    
    pub fn deinit(self: TransformerRunState) void {
        self.allocator.free(self.x);
        self.allocator.free(self.xb);
        self.allocator.free(self.xb2);
        self.allocator.free(self.q);
        self.allocator.free(self.k);
        self.allocator.free(self.v);
        self.allocator.free(self.att);
        self.allocator.free(self.h1);
        self.allocator.free(self.h2);
        self.allocator.free(self.key_cache);
        self.allocator.free(self.value_cache);
    }

    /// Get total memory usage in bytes
    pub fn getMemoryUsage(self: TransformerRunState) usize {
        return (self.x.len + self.xb.len + self.xb2.len + self.q.len + self.k.len + 
                self.v.len + self.att.len + self.h1.len + self.h2.len + 
                self.key_cache.len + self.value_cache.len) * @sizeOf(f32);
    }

    /// Reset state for new sequence
    pub fn reset(self: *TransformerRunState) void {
        @memset(self.key_cache, 0.0);
        @memset(self.value_cache, 0.0);
        self.pos = 0;
    }
    
    /// Advance position (for autoregressive generation)
    pub fn advance(self: *TransformerRunState) void {
        self.pos += 1;
    }
};

/// Weight matrices for Transformer (educational placeholder)
pub const TransformerWeights = struct {
    // Per-layer weights (arrays of layer weights)
    attn_norm: []const f32,        // [n_layers * d_model] attention layer norm
    wq: []const f32,               // [n_layers * d_model * (n_heads * head_dim)] query weights
    wk: []const f32,               // [n_layers * d_model * (n_kv_heads * kv_head_dim)] key weights  
    wv: []const f32,               // [n_layers * d_model * (n_kv_heads * kv_head_dim)] value weights
    wo: []const f32,               // [n_layers * (n_heads * head_dim) * d_model] output weights
    
    ffn_norm: []const f32,         // [n_layers * d_model] FFN layer norm
    w1: []const f32,               // [n_layers * d_model * ffn_dim] gate weights
    w2: []const f32,               // [n_layers * ffn_dim * d_model] down weights
    w3: []const f32,               // [n_layers * d_model * ffn_dim] up weights
    
    // Global weights
    token_embedding: []const f32,  // [vocab_size * d_model] token embeddings
    output_norm: []const f32,      // [d_model] final layer norm
    output_weight: []const f32,    // [d_model * vocab_size] output projection
};

/// Complete transformer block with attention and feed-forward
pub fn transformer_block(
    cfg: TransformerConfig,
    s: *TransformerRunState,
    w: *TransformerWeights,
    layer: usize,
) void {
    // 1. Attention with residual connection
    std.mem.copy(f32, s.xb, s.x); // Save residual
    
    // Pre-norm: normalize before attention
    const attn_norm_offset = layer * cfg.d_model;
    rmsnorm(s.x, s.x, w.attn_norm[attn_norm_offset .. attn_norm_offset + cfg.d_model]);
    
    // Multi-head attention with KV caching
    multi_head_attention(cfg, s, w, layer);
    
    // Residual connection
    for (0..cfg.d_model) |i| {
        s.x[i] += s.xb[i];
    }
    
    // 2. Feed-forward with residual connection  
    std.mem.copy(f32, s.xb, s.x); // Save residual
    
    // Pre-norm: normalize before FFN
    const ffn_norm_offset = layer * cfg.d_model;
    rmsnorm(s.x, s.x, w.ffn_norm[ffn_norm_offset .. ffn_norm_offset + cfg.d_model]);
    
    // SwiGLU feed-forward
    swiglu_ffn(cfg, s, w, layer);
    
    // Residual connection
    for (0..cfg.d_model) |i| {
        s.x[i] += s.xb[i];
    }
}

/// Multi-head attention with KV caching for efficient autoregressive generation
fn multi_head_attention(
    cfg: TransformerConfig,
    s: *TransformerRunState,
    w: *TransformerWeights,
    layer: usize,
) void {
    const head_dim_val = cfg.head_dim();
    const kv_head_dim_val = cfg.kv_head_dim();
    const pos = s.pos;
    
    // 1. Compute Q, K, V projections
    const wq_offset = layer * cfg.d_model * cfg.n_heads * head_dim_val;
    const wk_offset = layer * cfg.d_model * cfg.n_kv_heads * kv_head_dim_val;
    const wv_offset = layer * cfg.d_model * cfg.n_kv_heads * kv_head_dim_val;
    
    matmul(s.q, s.x, w.wq[wq_offset..], 1, cfg.d_model, cfg.n_heads * head_dim_val);
    matmul(s.k, s.x, w.wk[wk_offset..], 1, cfg.d_model, cfg.n_kv_heads * kv_head_dim_val);
    matmul(s.v, s.x, w.wv[wv_offset..], 1, cfg.d_model, cfg.n_kv_heads * kv_head_dim_val);
    
    // 2. Apply RoPE position encoding to Q and K
    for (0..cfg.n_heads) |h| {
        const q_head = s.q[h * head_dim_val .. (h + 1) * head_dim_val];
        rope(q_head, pos, cfg.rope_base, head_dim_val);
    }
    
    for (0..cfg.n_kv_heads) |h| {
        const k_head = s.k[h * kv_head_dim_val .. (h + 1) * kv_head_dim_val];
        rope(k_head, pos, cfg.rope_base, kv_head_dim_val);
    }
    
    // 3. Store current K, V in cache
    const kv_layer_offset = layer * cfg.seq_len * cfg.n_kv_heads * kv_head_dim_val;
    const kv_pos_offset = pos * cfg.n_kv_heads * kv_head_dim_val;
    const kv_offset = kv_layer_offset + kv_pos_offset;
    
    std.mem.copy(f32, 
        s.key_cache[kv_offset .. kv_offset + cfg.n_kv_heads * kv_head_dim_val],
        s.k);
    std.mem.copy(f32,
        s.value_cache[kv_offset .. kv_offset + cfg.n_kv_heads * kv_head_dim_val],
        s.v);
    
    // 4. Multi-head attention computation
    const heads_per_kv_head = cfg.n_heads / cfg.n_kv_heads; // For grouped query attention
    
    for (0..cfg.n_heads) |h| {
        const kv_head = h / heads_per_kv_head; // Which KV head this query head uses
        const q_head = s.q[h * head_dim_val .. (h + 1) * head_dim_val];
        const att_head = s.att[h * cfg.seq_len .. (h + 1) * cfg.seq_len];
        
        // Compute attention scores: Q · K^T
        for (0..pos + 1) |t| {
            const k_idx = kv_layer_offset + t * cfg.n_kv_heads * kv_head_dim_val + kv_head * kv_head_dim_val;
            const k_head = s.key_cache[k_idx .. k_idx + kv_head_dim_val];
            
            var score: f32 = 0.0;
            for (0..head_dim_val) |i| {
                score += q_head[i] * k_head[i];
            }
            // Scale by sqrt(d_k)
            att_head[t] = score / std.math.sqrt(@as(f32, @floatFromInt(head_dim_val)));
        }
        
        // Causal mask: future positions get -inf (will be 0 after softmax)
        for (pos + 1..cfg.seq_len) |t| {
            att_head[t] = -std.math.inf(f32);
        }
        
        // Softmax to get attention weights
        softmax(att_head[0..pos + 1]);
        
        // Weighted sum of values: Attention · V
        var x_head = s.xb[h * head_dim_val .. (h + 1) * head_dim_val];
        @memset(x_head, 0.0);
        
        for (0..pos + 1) |t| {
            const v_idx = kv_layer_offset + t * cfg.n_kv_heads * kv_head_dim_val + kv_head * kv_head_dim_val;
            const v_head = s.value_cache[v_idx .. v_idx + kv_head_dim_val];
            const weight = att_head[t];
            
            for (0..head_dim_val) |i| {
                x_head[i] += weight * v_head[i];
            }
        }
    }
    
    // 5. Output projection: concatenated heads -> d_model
    const wo_offset = layer * cfg.n_heads * head_dim_val * cfg.d_model;
    matmul(s.x, s.xb, w.wo[wo_offset..], 1, cfg.n_heads * head_dim_val, cfg.d_model);
}

/// SwiGLU feed-forward network
fn swiglu_ffn(
    cfg: TransformerConfig,
    s: *TransformerRunState,
    w: *TransformerWeights,
    layer: usize,
) void {
    const ffn_dim_val = cfg.ffn_dim();
    
    const w1_offset = layer * cfg.d_model * ffn_dim_val;
    const w2_offset = layer * ffn_dim_val * cfg.d_model;
    const w3_offset = layer * cfg.d_model * ffn_dim_val;
    
    // Gate and Up projections
    matmul(s.h1, s.x, w.w1[w1_offset..], 1, cfg.d_model, ffn_dim_val); // Gate
    matmul(s.h2, s.x, w.w3[w3_offset..], 1, cfg.d_model, ffn_dim_val); // Up
    
    // SwiGLU: swish(gate) * up
    for (0..ffn_dim_val) |i| {
        s.h1[i] = swish(s.h1[i]) * s.h2[i];
    }
    
    // Down projection
    matmul(s.x, s.h1, w.w2[w2_offset..], 1, ffn_dim_val, cfg.d_model);
}

// ============================================================================
// Core Mathematical Operations
// ============================================================================

/// Rotary Position Embedding (RoPE)
fn rope(x: []f32, pos: usize, base: f32, dim: usize) void {
    for (0..dim / 2) |i| {
        // Compute frequency for this dimension pair
        const freq = 1.0 / std.math.pow(f32, base, @as(f32, @floatFromInt(2 * i)) / @as(f32, @floatFromInt(dim)));
        const val = @as(f32, @floatFromInt(pos)) * freq;
        const cos_val = std.math.cos(val);
        const sin_val = std.math.sin(val);
        
        // Rotate the dimension pair
        const x0 = x[2 * i];
        const x1 = x[2 * i + 1];
        x[2 * i] = x0 * cos_val - x1 * sin_val;
        x[2 * i + 1] = x0 * sin_val + x1 * cos_val;
    }
}

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
    
    for (x) |val| {
        ss += val * val;
    }
    ss /= @as(f32, @floatFromInt(size));
    ss += eps;
    const inv_ss = 1.0 / std.math.sqrt(ss);
    
    for (0..size) |i| {
        out[i] = x[i] * inv_ss * weight[i];
    }
}

/// Softmax: out_i = exp(x_i) / sum(exp(x_j))
fn softmax(x: []f32) void {
    if (x.len == 0) return;
    
    // Find max for numerical stability
    var max_val = x[0];
    for (x[1..]) |val| {
        max_val = @max(max_val, val);
    }
    
    // Compute exp and sum
    var sum: f32 = 0.0;
    for (x) |*val| {
        val.* = std.math.exp(val.* - max_val);
        sum += val.*;
    }
    
    // Normalize
    for (x) |*val| {
        val.* /= sum;
    }
}

/// Swish/SiLU activation: x * sigmoid(x)
inline fn swish(x: f32) f32 {
    return x / (1.0 + std.math.exp(-x));
}

// ============================================================================
// Educational Demo
// ============================================================================

/// Demonstrate KV caching efficiency
fn demonstrateKVCaching(allocator: Allocator, cfg: TransformerConfig) !void {
    print("\n📊 KV Caching Analysis:\n");
    
    var state = try TransformerRunState.init(allocator, cfg);
    defer state.deinit();
    
    const kv_cache_mb = cfg.estimate_kv_cache_size() / 1024 / 1024;
    const total_memory_mb = state.getMemoryUsage() / 1024 / 1024;
    
    print("Configuration: d_model={}, n_layers={}, seq_len={}\n", .{cfg.d_model, cfg.n_layers, cfg.seq_len});
    print("KV cache size: {} MB\n", .{kv_cache_mb});
    print("Total memory: {} MB\n", .{total_memory_mb});
    
    print("\n💡 KV Cache Benefits:\n");
    print("- Without cache: O(N²) recomputation for each token\n");
    print("- With cache: O(N) computation for each new token\n");
    print("- Memory trade-off: {} MB for ~{}x speedup\n", .{kv_cache_mb, cfg.seq_len});
}

/// Show attention pattern concepts
fn demonstrateAttentionPatterns(allocator: Allocator, cfg: TransformerConfig) !void {
    print("\n🎯 Attention Mechanism Concepts:\n");
    
    var state = try TransformerRunState.init(allocator, cfg);
    defer state.deinit();
    
    // Simulate some attention scores
    print("\nExample attention pattern (head 0, first 5 positions):\n");
    const att_head = state.att[0..cfg.seq_len];
    
    // Mock attention scores for demonstration
    for (0..5) |i| {
        att_head[i] = @exp(-@as(f32, @floatFromInt(i)) * 0.5); // Decay pattern
    }
    softmax(att_head[0..5]);
    
    for (0..5) |i| {
        print("  Position {}: {d:.3}\n", .{i, att_head[i]});
    }
    
    print("\n🔍 Attention Insights:\n");
    print("- Each head learns different patterns\n");
    print("- Early layers: local/syntactic patterns\n");
    print("- Later layers: semantic/global patterns\n");
    print("- Causal mask ensures autoregressive property\n");
}

/// Educational demo
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    print("🔄 Transformer Architecture - Educational Demo\n");
    print("==============================================\n");

    const cfg = TransformerConfig{
        .d_model = 288,
        .n_heads = 6,
        .n_layers = 6,
        .seq_len = 2048,
        .n_kv_heads = 6,
    };

    print("\n📚 Educational Features:\n");
    print("- Multi-head attention with KV caching\n");
    print("- Rotary Position Embedding (RoPE)\n");
    print("- SwiGLU feed-forward networks\n");
    print("- Grouped Query Attention support\n");

    // Configuration analysis
    print("\n🛠️ Configuration Analysis:\n");
    print("d_model: {} (model dimension)\n", .{cfg.d_model});
    print("n_heads: {} (attention heads)\n", .{cfg.n_heads});
    print("head_dim: {} (per-head dimension)\n", .{cfg.head_dim()});
    print("n_layers: {} (transformer layers)\n", .{cfg.n_layers});
    print("seq_len: {} (max sequence length)\n", .{cfg.seq_len});
    print("ffn_dim: {} (feed-forward dimension)\n", .{cfg.ffn_dim()});
    print("Total parameters: ~{} million\n", .{cfg.estimate_params() / 1_000_000});

    // Memory analysis
    try demonstrateKVCaching(allocator, cfg);
    
    // Attention patterns
    try demonstrateAttentionPatterns(allocator, cfg);

    // Complexity comparison
    print("\n⚡ Computational Complexity:\n");
    print("| Operation     | Complexity        | Notes            |\n");
    print("|---------------|-------------------|------------------|\n");
    print("| Attention     | O(N² × d)        | Quadratic in seq |\n");
    print("| Feed-forward  | O(N × d²)        | Linear in seq    |\n");
    print("| KV cache attn | O(N × d)         | With caching     |\n");
    print("| RoPE          | O(N × d)         | Position encode  |\n");

    // Position encoding demo
    print("\n🔄 RoPE Position Encoding Demo:\n");
    var pos_demo = [_]f32{1.0, 0.0, 0.5, 0.0}; // 4D vector
    print("Original: [{d:.3}, {d:.3}, {d:.3}, {d:.3}]\n", .{pos_demo[0], pos_demo[1], pos_demo[2], pos_demo[3]});
    
    rope(&pos_demo, 0, 10000.0, 4); // Position 0
    print("Pos 0:    [{d:.3}, {d:.3}, {d:.3}, {d:.3}]\n", .{pos_demo[0], pos_demo[1], pos_demo[2], pos_demo[3]});
    
    rope(&pos_demo, 5, 10000.0, 4); // Position 5 
    print("Pos 5:    [{d:.3}, {d:.3}, {d:.3}, {d:.3}]\n", .{pos_demo[0], pos_demo[1], pos_demo[2], pos_demo[3]});

    print("\n🎓 Educational Concepts Demonstrated:\n");
    print("- Attention mechanism with causal masking\n");
    print("- KV caching for efficient autoregressive generation\n");
    print("- Position encoding preserves relative positions\n");
    print("- Multi-head attention learns diverse patterns\n");
    print("- Memory vs computation trade-offs\n");

    print("\n🚀 Advanced experiments to try:\n");
    print("- Visualize attention patterns across layers\n");
    print("- Compare different position encoding methods\n");
    print("- Implement sparse attention patterns\n");
    print("- Study scaling laws with model size\n");
    print("- Optimize memory usage with mixed precision\n");
}