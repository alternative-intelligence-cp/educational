# 🔄 Transformer Architecture - Educational Implementation

## 🎯 Overview

Complete Zig implementation of the Transformer architecture focusing on **multi-head attention**, **KV caching**, and **modern optimizations**. This educational implementation demonstrates core concepts with clear, well-documented code.

## 🧠 Key Features

- **Multi-Head Attention**: Parallel attention computation with query, key, value projections
- **KV Caching**: Efficient autoregressive generation with O(1) attention updates
- **Positional Encoding**: RoPE (Rotary Position Embedding) for relative position awareness
- **Layer Normalization**: RMSNorm for improved training stability
- **Feed-Forward Networks**: SwiGLU activation with gating mechanism
- **Educational Design**: Clear separation of concepts with comprehensive documentation

## 🏗️ Architecture Components

### 1. Configuration
```zig
pub const TransformerConfig = struct {
    d_model: usize = 288,        // Model dimension
    n_heads: usize = 6,          // Number of attention heads
    n_layers: usize = 6,         // Number of transformer layers
    n_kv_heads: usize = 6,       // KV heads (for GQA - Grouped Query Attention)
    vocab_size: usize = 32000,   // Vocabulary size
    seq_len: usize = 2048,       // Maximum sequence length
    norm_eps: f32 = 1e-5,        // Layer norm epsilon
    rope_base: f32 = 10000.0,    // RoPE base frequency
};
```

### 2. Runtime State with KV Caching
- **Key Cache**: `[n_layers, seq_len, n_kv_heads, head_dim]` - Cached keys
- **Value Cache**: `[n_layers, seq_len, n_kv_heads, head_dim]` - Cached values
- **Working Buffers**: Temporary computation space for attention and feed-forward
- **Position Tracking**: Current sequence position for cache indexing

### 3. Attention Mechanism
1. **Query/Key/Value Projection**: Linear transformations for attention
2. **Rotary Position Embedding**: Apply relative position encoding
3. **Scaled Dot-Product Attention**: Q·K^T / sqrt(d_k) with causal masking
4. **Multi-Head Fusion**: Concatenate and project attention outputs

## 📚 Educational Concepts

### Self-Attention Formula
```
Attention(Q,K,V) = softmax(Q·K^T / √d_k)·V
```

### Multi-Head Attention
```
MultiHead(Q,K,V) = Concat(head_1, ..., head_h)·W^O
where head_i = Attention(Q·W^Q_i, K·W^K_i, V·W^V_i)
```

### RoPE (Rotary Position Embedding)
```
q_m = q_m · cos(mθ) + rotate(q_m) · sin(mθ)
k_m = k_m · cos(mθ) + rotate(k_m) · sin(mθ)
where θ = 10000^(-2i/d) for dimension i
```

### KV Caching Optimization
- **Training**: Compute all positions in parallel O(N²) memory
- **Inference**: Cache previous K,V, only compute new position O(N) memory
- **Memory Trade-off**: Store KV cache vs recompute each step

## 🚀 Usage Examples

### Basic Model Setup
```zig
const cfg = TransformerConfig{
    .d_model = 288,
    .n_heads = 6,
    .n_layers = 6,
    .seq_len = 2048,
};

var state = try TransformerRunState.init(allocator, cfg);
defer state.deinit();
```

### Autoregressive Generation
```zig
// Generate tokens one by one with KV caching
for (0..max_tokens) |step| {
    // Embed current token
    embed_token(state.x, current_token, embeddings);
    
    // Process through transformer layers
    for (0..cfg.n_layers) |layer| {
        transformer_block(cfg, &state, &weights, layer, step);
    }
    
    // Get next token logits
    get_logits(logits, state.x, output_weights);
    current_token = sample(logits);
    
    // KV cache automatically updated during attention
}
```

### Memory Analysis
```zig
// Analyze KV cache memory usage
const kv_size = cfg.n_layers * cfg.seq_len * cfg.n_kv_heads * cfg.head_dim() * 2; // K + V
print("KV cache size: {} MB\n", .{kv_size * @sizeOf(f32) / 1024 / 1024});
print("Memory scales as: O(layers × seq_len × heads × head_dim)\n");
```

## 🔍 Algorithm Deep Dive

### Multi-Head Attention Implementation
```zig
// Core attention computation with KV caching
fn attention_with_kv_cache(
    cfg: TransformerConfig,
    s: *TransformerRunState,
    w: *TransformerWeights,
    layer: usize,
    pos: usize,
) void {
    const head_dim = cfg.head_dim();
    const n_heads = cfg.n_heads;
    const n_kv_heads = cfg.n_kv_heads;
    
    // 1. Query, Key, Value projections
    matmul(s.q, s.x, w.wq[layer], 1, cfg.d_model, n_heads * head_dim);
    matmul(s.k, s.x, w.wk[layer], 1, cfg.d_model, n_kv_heads * head_dim);
    matmul(s.v, s.x, w.wv[layer], 1, cfg.d_model, n_kv_heads * head_dim);
    
    // 2. Apply RoPE position encoding
    for (0..n_heads) |h| {
        rope(s.q[h * head_dim .. (h + 1) * head_dim], pos, cfg.rope_base, head_dim);
    }
    
    for (0..n_kv_heads) |h| {
        rope(s.k[h * head_dim .. (h + 1) * head_dim], pos, cfg.rope_base, head_dim);
        // No RoPE on values
    }
    
    // 3. Update KV cache (key insight: only current position)
    const kv_cache_layer = layer * cfg.seq_len * n_kv_heads * head_dim;
    const kv_pos_offset = pos * n_kv_heads * head_dim;
    
    // Store current K, V in cache
    std.mem.copy(f32, 
        s.key_cache[kv_cache_layer + kv_pos_offset .. kv_cache_layer + kv_pos_offset + n_kv_heads * head_dim],
        s.k[0 .. n_kv_heads * head_dim]);
    std.mem.copy(f32,
        s.value_cache[kv_cache_layer + kv_pos_offset .. kv_cache_layer + kv_pos_offset + n_kv_heads * head_dim], 
        s.v[0 .. n_kv_heads * head_dim]);
    
    // 4. Compute attention scores with cached K,V
    for (0..n_heads) |h| {
        const kv_head = h / (n_heads / n_kv_heads); // Group mapping for GQA
        const q_head = s.q[h * head_dim .. (h + 1) * head_dim];
        const att_head = s.att[h * cfg.seq_len .. (h + 1) * cfg.seq_len];
        
        // Attention scores: Q · K^T (only up to current position)
        for (0..pos + 1) |t| {
            const k_offset = kv_cache_layer + t * n_kv_heads * head_dim + kv_head * head_dim;
            const k_head = s.key_cache[k_offset .. k_offset + head_dim];
            
            var score: f32 = 0.0;
            for (0..head_dim) |i| {
                score += q_head[i] * k_head[i];
            }
            att_head[t] = score / std.math.sqrt(@as(f32, @floatFromInt(head_dim)));
        }
        
        // Causal mask: set future positions to -inf
        for (pos + 1..cfg.seq_len) |t| {
            att_head[t] = -std.math.inf(f32);
        }
        
        // Softmax attention weights
        softmax(att_head[0..pos + 1]);
        
        // Weighted sum of values: Attention · V
        var x_head = s.xb[h * head_dim .. (h + 1) * head_dim];
        @memset(x_head, 0.0);
        
        for (0..pos + 1) |t| {
            const v_offset = kv_cache_layer + t * n_kv_heads * head_dim + kv_head * head_dim;
            const v_head = s.value_cache[v_offset .. v_offset + head_dim];
            const weight = att_head[t];
            
            for (0..head_dim) |i| {
                x_head[i] += weight * v_head[i];
            }
        }
    }
    
    // 5. Multi-head fusion
    matmul(s.xb2, s.xb, w.wo[layer], 1, cfg.d_model, cfg.d_model);
}
```

### RoPE Implementation
```zig
// Rotary Position Embedding
fn rope(x: []f32, pos: usize, base: f32, dim: usize) void {
    for (0..dim / 2) |i| {
        const freq = 1.0 / std.math.pow(f32, base, @as(f32, @floatFromInt(2 * i)) / @as(f32, @floatFromInt(dim)));
        const val = @as(f32, @floatFromInt(pos)) * freq;
        const cos_val = std.math.cos(val);
        const sin_val = std.math.sin(val);
        
        // Rotate pairs of dimensions
        const x0 = x[2 * i];
        const x1 = x[2 * i + 1];
        x[2 * i] = x0 * cos_val - x1 * sin_val;
        x[2 * i + 1] = x0 * sin_val + x1 * cos_val;
    }
}
```

### Feed-Forward Network (SwiGLU)
```zig
// SwiGLU: Gated Linear Unit with Swish activation
fn feed_forward(cfg: TransformerConfig, s: *TransformerRunState, w: *TransformerWeights, layer: usize) void {
    const ffn_dim = cfg.d_model * 4; // Typical expansion
    
    // Gate and up projections
    matmul(s.h1, s.x, w.w1[layer], 1, cfg.d_model, ffn_dim); // Gate
    matmul(s.h2, s.x, w.w3[layer], 1, cfg.d_model, ffn_dim); // Up
    
    // SwiGLU: swish(gate) * up
    for (0..ffn_dim) |i| {
        s.h1[i] = swish(s.h1[i]) * s.h2[i];
    }
    
    // Down projection
    matmul(s.xb, s.h1, w.w2[layer], 1, ffn_dim, cfg.d_model);
}

// Swish activation: x * sigmoid(x)
inline fn swish(x: f32) f32 {
    return x / (1.0 + std.math.exp(-x));
}
```

## 📊 Performance Analysis

### Computational Complexity
- **Attention**: O(N² × d) for sequence length N
- **KV Cache**: Reduces incremental attention to O(N × d)
- **Feed-Forward**: O(N × d²) where d is model dimension
- **Total per layer**: O(N² × d + N × d²)

### Memory Requirements
| Component | Training | Inference (KV Cache) |
|-----------|----------|---------------------|
| Attention | O(N² × H) | O(L × N × H × d) |
| Activations | O(L × N × d) | O(N × d) |
| Parameters | O(L × d²) | O(L × d²) |
| **Total** | **O(N² × H + L × N × d)** | **O(L × N × H × d)** |

Where: N=seq_len, L=n_layers, H=n_heads, d=head_dim

### KV Cache Analysis
```
Example: d_model=2048, n_layers=32, n_heads=32, seq_len=4096, head_dim=64
KV cache size = 32 × 4096 × 32 × 64 × 2 × 4 bytes = 2.1 GB

Trade-offs:
+ Faster inference (no recomputation)
+ Enables real-time generation
- Large memory usage for long sequences
- Memory scales linearly with sequence length
```

## 🎓 Educational Projects

### Beginner Experiments
1. **Attention Visualization**: Plot attention weights across heads and layers
2. **Position Encoding Study**: Compare absolute vs relative (RoPE) encoding
3. **Scaling Laws**: Study how performance scales with model size
4. **Memory Usage**: Measure actual vs theoretical memory consumption

### Intermediate Projects
1. **Grouped Query Attention**: Implement and analyze GQA efficiency
2. **Attention Patterns**: Study how different heads learn different patterns
3. **KV Cache Optimization**: Implement sliding window or compression
4. **Mixed Precision**: Add FP16/BF16 support for memory efficiency

### Advanced Research
1. **Sparse Attention**: Local, strided, or learned sparsity patterns
2. **Memory Efficient Attention**: FlashAttention or similar optimizations
3. **Alternative Architectures**: RetNet, Mamba hybrid, or other variants
4. **Hardware Optimization**: Custom kernels for specific operations

## 🔬 Key Insights

### Why Transformers Work
1. **Parallelization**: All positions processed simultaneously during training
2. **Long-range Dependencies**: Direct connections between any two positions
3. **Flexible Representations**: Learned attention patterns adapt to data
4. **Compositional**: Stack layers for hierarchical feature learning

### Attention Patterns
- **Early layers**: Local patterns (syntax, neighboring words)
- **Middle layers**: Syntactic relationships (subject-verb agreement)
- **Late layers**: Semantic relationships (coreference, topic modeling)

### Trade-offs
- **Memory vs Computation**: KV cache trades memory for speed
- **Parallelization vs Autoregression**: Training parallel, inference sequential
- **Model Size vs Efficiency**: Larger models more capable but slower
- **Attention Resolution**: Longer sequences enable better context but cost memory

## 🛠️ Build and Test

```bash
# Build the implementation
zig build

# Run educational demo
./zig-out/bin/transformer_demo

# Run tests
zig test src/transformer.zig

# Performance benchmark
zig build -Doptimize=ReleaseFast
./zig-out/bin/transformer_benchmark

# Memory analysis
valgrind ./zig-out/bin/transformer_demo
```

---

## 📚 References and Further Reading

- **Original Paper**: "Attention Is All You Need" (Vaswani et al., 2017)
- **RoPE**: "RoFormer: Enhanced Transformer with Rotary Position Embedding"
- **GQA**: "GQA: Training Generalized Multi-Query Transformer Models"
- **SwiGLU**: "GLU Variants Improve Transformer" (Shazeer, 2020)
- **FlashAttention**: "FlashAttention: Fast and Memory-Efficient Exact Attention"
- **KV Caching**: "Generating Sequences with Recurrent Neural Networks"

*This implementation provides a comprehensive foundation for understanding transformer architecture and modern attention mechanisms with practical optimizations.*