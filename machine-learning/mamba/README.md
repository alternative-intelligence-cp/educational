# 🐍 Mamba State-Space Model - Educational Implementation

## 🎯 Overview

Advanced Zig implementation of the Mamba selective state-space model architecture. This educational implementation demonstrates modern sequence modeling with **O(1) memory complexity**, making it highly efficient for very long sequences.

## 📊 Key Features

- **O(1) Memory Complexity**: Constant memory usage regardless of sequence length
- **Selective Scan Algorithm**: Efficient state updates with learnable selection
- **Convolutional Processing**: Local pattern recognition with 1D convolutions
- **SiLU Gating**: Modern activation functions for improved gradient flow
- **Educational Design**: Clear, well-documented implementation for learning

## 🦬 Architecture Components

### 1. Configuration
```zig
pub const MambaConfig = struct {
    d_model: usize = 288,        // Model dimension
    n_layers: usize = 6,         // Number of layers
    vocab_size: usize = 32000,   // Vocabulary size
    d_state: usize = 16,         // State dimension (N)
    expand: usize = 2,           // Expansion factor (E) 
    d_conv: usize = 4,           // Convolution kernel width
    dt_rank: usize = 0,          // Delta projection rank (auto-calculated)
};
```

### 2. Runtime State
- **Fixed Allocation**: Memory allocated once regardless of sequence length
- **Convolutional State**: `[n_layers, d_inner, d_conv]` for local patterns
- **SSM State**: `[n_layers, d_inner, d_state]` for long-range dependencies
- **Working Buffers**: Temporary computation space

### 3. Selective Scan Process
1. **Input Projection**: Split into x and z branches
2. **Convolution**: 1D causal conv for local patterns
3. **Parameter Selection**: Learn Δt, B, C from convolved input
4. **State Update**: Selective scan with learned parameters
5. **Gating**: Combine with z branch using SiLU activation
6. **Output Projection**: Final linear transformation

## 📚 Educational Concepts

### State-Space Models
```
State Update: h[t] = A • h[t-1] + B • x[t]
Output: y[t] = C • h[t] + D • x[t]
```

### Selective Mechanism
- **Δt (Delta)**: Controls state update rate
- **B**: Input-to-state transformation
- **C**: State-to-output transformation
- **Selection**: Parameters depend on input (learned, not fixed)

### Memory Efficiency
- **Traditional Attention**: O(N²) memory for N-length sequence
- **Mamba State-Space**: O(1) memory regardless of sequence length
- **Trade-off**: Sequential processing vs parallel attention

## 🚀 Usage Examples

### Basic Model Setup
```zig
const cfg = MambaConfig{
    .d_model = 288,
    .n_layers = 6,
    .d_state = 16,
    .expand = 2,
    .d_conv = 4,
};

var state = try MambaRunState.init(allocator, cfg);
defer state.deinit();
```

### Processing Sequence
```zig
// Process token by token with constant memory
for (tokens) |token| {
    // Embed token
    embed_token(state.x, token, embeddings);
    
    // Process through layers
    for (0..cfg.n_layers) |layer| {
        mamba_block(cfg, &state, &weights, layer);
    }
    
    // Get predictions
    get_logits(logits, state.x, output_weights);
}
```

### Memory Analysis
```zig
// Memory usage is independent of sequence length
print("Conv state size: {} bytes\n", .{cfg.n_layers * cfg.get_d_inner() * cfg.d_conv * @sizeOf(f32)});
print("SSM state size: {} bytes\n", .{cfg.n_layers * cfg.get_d_inner() * cfg.d_state * @sizeOf(f32)});
print("Total state: {} KB\n", .{(conv_size + ssm_size) / 1024});
```

## 🔬 Algorithm Deep Dive

### Selective Scan Implementation
```zig
// The core selective scan loop
for (0..d_inner) |i| {
    // Discretize delta (learnable time step)
    const dt_i = softplus(s.dt[i] + w.dt_proj_bias[i]);
    
    // Access state for this channel
    var h = s.ssm_state[h_idx .. h_idx + d_state];
    const A_row = w.A_log[(layer * d_inner + i) * d_state ..];
    const x_val = x_ptr[i];
    
    var y_val: f32 = 0.0;
    
    // Update each state dimension
    for (0..d_state) |n| {
        // Compute discretized A and B
        const A_val = -std.math.exp(A_row[n]);
        const dA = std.math.exp(A_val * dt_i);
        const dB = dt_i * B_val[n];
        
        // State update: h = dA * h + dB * x
        h[n] = (dA * h[n]) + (dB * x_val);
        
        // Output: y = C * h  
        y_val += h[n] * C_val[n];
    }
    
    // Skip connection and gating
    y_val += x_val * w.D[layer * d_inner + i];
    x_ptr[i] = y_val * silu(z_ptr[i]);
}
```

### Convolutional State Management
```zig
// Causal 1D convolution with state buffer
for (0..d_inner) |i| {
    var channel_state = s.conv_state[state_idx .. state_idx + d_conv];
    
    // Shift buffer (FIFO)
    std.mem.copyForwards(f32, channel_state[0 .. d_conv - 1], channel_state[1 .. d_conv]);
    channel_state[d_conv - 1] = x_ptr[i];
    
    // Convolution dot product
    var conv_out = w.conv_bias[layer * d_inner + i];
    for (0..d_conv) |k| {
        conv_out += channel_state[k] * w_kernel[k];
    }
    x_ptr[i] = silu(conv_out);
}
```

## 📊 Performance Analysis

### Computational Complexity
- **Per Token**: O(d_model × d_inner + d_inner × d_state)
- **Per Sequence**: Linear in sequence length
- **Memory**: Constant regardless of sequence length

### Comparison with Attention
| Metric | Mamba | Transformer |
|--------|-------|-------------|
| Memory per token | O(1) | O(N) |
| Memory total | O(1) | O(N²) |
| Computation per token | O(d) | O(N × d) |
| Parallelizable | No (sequential) | Yes (attention) |
| Long sequences | Excellent | Challenging |

### Memory Usage Example
```
Configuration: d_model=288, n_layers=6, d_state=16, expand=2
- d_inner = expand * d_model = 576
- Conv state: 6 * 576 * 4 = 13,824 floats = 55 KB
- SSM state: 6 * 576 * 16 = 55,296 floats = 221 KB
- Total: ~276 KB (constant for any sequence length)
```

## 🧪 Educational Projects

### Beginner Experiments
1. **Sequence Length Analysis**: Compare memory usage vs transformer
2. **State Visualization**: Plot state evolution over time
3. **Parameter Sensitivity**: Study effect of d_state, expand factor
4. **Convergence Analysis**: Training dynamics on toy problems

### Intermediate Projects
1. **Hybrid Models**: Combine Mamba layers with attention
2. **Bidirectional Mamba**: Process sequences forward and backward
3. **Multi-scale Processing**: Different d_conv for different layers
4. **Compression Studies**: Quantization effects on state precision

### Advanced Research
1. **Novel Selection Mechanisms**: Alternative to Δt, B, C selection
2. **Hardware Optimization**: Custom kernels for selective scan
3. **Structured Matrices**: Efficient A matrix parameterizations
4. **Multi-dimensional States**: 2D/3D extensions for images/video

## 🔍 Key Insights

### Why Mamba Works
1. **Selectivity**: Parameters depend on input, not fixed
2. **Efficiency**: O(1) memory enables very long sequences
3. **Expressiveness**: State-space models are universal approximators
4. **Practicality**: Real-world performance competitive with transformers

### Trade-offs
- **Sequential Processing**: Cannot parallelize like attention
- **Training Speed**: Slower than parallel transformer training
- **Inference**: Excellent for autoregressive generation
- **Implementation**: More complex than standard attention

## 🛠️ Build and Test

```bash
# Build the implementation
zig build

# Run educational demo
./zig-out/bin/mamba_demo

# Run tests
zig test src/mamba.zig

# Performance benchmark
zig build -Doptimize=ReleaseFast
./zig-out/bin/mamba_benchmark
```

---

## 📚 References and Further Reading

- **Mamba Paper**: "Mamba: Linear-Time Sequence Modeling with Selective State Spaces"
- **State-Space Models**: Foundations of control theory and signal processing
- **Selective Mechanisms**: Attention-like selection in state-space models
- **Efficient Transformers**: Survey of memory-efficient transformer variants

*This implementation provides a clear, educational foundation for understanding modern state-space models and their advantages for long sequence processing.*