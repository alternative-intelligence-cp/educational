//! ZigML Operations - Educational Implementation
//! ============================================
//!
//! Foundational mathematical operations for machine learning with focus on:
//! - Educational clarity with performance annotations
//! - Numerical stability techniques
//! - Cache-friendly implementations
//! - Clear progression from naive to optimized algorithms

const std = @import("std");
const Allocator = std.mem.Allocator;
const print = std.debug.print;
const math = std.math;

// ============================================================================
// Matrix Operations
// ============================================================================

/// Basic matrix multiplication: C = A × B
/// Educational implementation showing the fundamental algorithm
/// 
/// Complexity: O(m × n × k) time, O(1) additional space
/// Cache behavior: Poor due to B matrix access pattern
pub fn matmul_naive(C: []f32, A: []const f32, B: []const f32, m: usize, k: usize, n: usize) void {
    @memset(C, 0.0);
    
    // Standard triple loop: i-j-k order (cache-unfriendly for B)
    for (0..m) |i| {
        for (0..n) |j| {
            var sum: f32 = 0.0;
            for (0..k) |ki| {
                sum += A[i * k + ki] * B[ki * n + j];  // B access is strided!
            }
            C[i * n + j] = sum;
        }
    }
}

/// Cache-optimized matrix multiplication: C = A × B
/// Reordered loops for better cache locality
/// 
/// Key insight: i-k-j order keeps A[i,k] in register and accesses B row-wise
pub fn matmul(C: []f32, A: []const f32, B: []const f32, m: usize, k: usize, n: usize) void {
    @memset(C, 0.0);
    
    // Cache-friendly i-k-j order
    for (0..m) |i| {
        for (0..k) |ki| {
            const a_val = A[i * k + ki];  // Load once, reuse
            for (0..n) |j| {
                C[i * n + j] += a_val * B[ki * n + j];  // Sequential B access
            }
        }
    }
}

/// Blocked (tiled) matrix multiplication for large matrices
/// Divides computation into cache-sized blocks
/// 
/// Benefits:
/// - Working set fits in L1/L2 cache
/// - Reduces cache misses by 50-80% for large matrices
pub fn matmul_blocked(C: []f32, A: []const f32, B: []const f32, 
                      m: usize, k: usize, n: usize, block_size: usize) void {
    @memset(C, 0.0);
    
    // Iterate over blocks
    var ii: usize = 0;
    while (ii < m) : (ii += block_size) {
        var kk: usize = 0;
        while (kk < k) : (kk += block_size) {
            var jj: usize = 0;
            while (jj < n) : (jj += block_size) {
                // Process current block
                const i_end = @min(ii + block_size, m);
                const k_end = @min(kk + block_size, k);
                const j_end = @min(jj + block_size, n);
                
                // Standard matmul within block
                for (ii..i_end) |i| {
                    for (kk..k_end) |ki| {
                        const a_val = A[i * k + ki];
                        for (jj..j_end) |j| {
                            C[i * n + j] += a_val * B[ki * n + j];
                        }
                    }
                }
            }
        }
    }
}

// ============================================================================
// Activation Functions
// ============================================================================

/// SiLU (Swish) activation: f(x) = x * sigmoid(x) = x / (1 + exp(-x))
/// Properties:
/// - Smooth, differentiable everywhere
/// - Self-gating (output depends on input magnitude)
/// - Better gradient flow than ReLU for deep networks
pub inline fn silu(x: f32) f32 {
    return x / (1.0 + @exp(-x));
}

/// ReLU activation: f(x) = max(0, x)
/// Properties:
/// - Simple, fast computation
/// - Sparse gradients (0 or 1)
/// - Can suffer from "dying ReLU" problem
pub inline fn relu(x: f32) f32 {
    return @max(0.0, x);
}

/// Softplus activation: f(x) = log(1 + exp(x))
/// Properties:
/// - Smooth approximation to ReLU
/// - Always positive output
/// - Derivative is sigmoid
pub inline fn softplus(x: f32) f32 {
    // Numerically stable for large x
    if (x > 20.0) return x;  // exp(x) >> 1, so log(1+exp(x)) ≈ x
    return @log(1.0 + @exp(x));
}

/// Apply activation function to entire array
pub fn silu_array(out: []f32, x: []const f32) void {
    std.debug.assert(out.len == x.len);
    for (x, out) |xi, *yi| {
        yi.* = silu(xi);
    }
}

pub fn relu_array(out: []f32, x: []const f32) void {
    std.debug.assert(out.len == x.len);
    for (x, out) |xi, *yi| {
        yi.* = relu(xi);
    }
}

// ============================================================================
// Normalization Functions
// ============================================================================

/// RMSNorm: y = x / rms(x) * weight
/// where rms(x) = sqrt(mean(x^2) + eps)
/// 
/// Benefits over LayerNorm:
/// - Simpler computation (no mean subtraction)
/// - Equivalent performance in practice
/// - Used in modern models (GPT-3, PaLM, etc.)
pub fn rmsnorm(out: []f32, x: []const f32, weight: []const f32) void {
    std.debug.assert(out.len == x.len);
    std.debug.assert(weight.len == x.len);
    
    const size = x.len;
    const eps = 1e-5;
    
    // Compute sum of squares
    var ss: f32 = 0.0;
    for (x) |val| {
        ss += val * val;
    }
    
    // Root mean square with epsilon for stability
    const rms = math.sqrt(ss / @as(f32, @floatFromInt(size)) + eps);
    const scale = 1.0 / rms;
    
    // Apply normalization and scaling
    for (0..size) |i| {
        out[i] = x[i] * scale * weight[i];
    }
}

/// Layer normalization: y = (x - mean(x)) / std(x) * weight + bias
/// More expensive than RMSNorm but can be more stable
pub fn layernorm(out: []f32, x: []const f32, weight: []const f32, bias: []const f32) void {
    std.debug.assert(out.len == x.len);
    std.debug.assert(weight.len == x.len);
    std.debug.assert(bias.len == x.len);
    
    const size = x.len;
    const eps = 1e-5;
    
    // Compute mean
    var mean: f32 = 0.0;
    for (x) |val| {
        mean += val;
    }
    mean /= @as(f32, @floatFromInt(size));
    
    // Compute variance
    var variance: f32 = 0.0;
    for (x) |val| {
        const diff = val - mean;
        variance += diff * diff;
    }
    variance /= @as(f32, @floatFromInt(size));
    
    // Normalize
    const inv_std = 1.0 / math.sqrt(variance + eps);
    for (0..size) |i| {
        out[i] = (x[i] - mean) * inv_std * weight[i] + bias[i];
    }
}

// ============================================================================
// Softmax and Probability Functions
// ============================================================================

/// Numerically stable softmax: out_i = exp(x_i - max(x)) / sum(exp(x_j - max(x)))
/// Key insight: Subtract maximum for numerical stability
pub fn softmax(x: []f32) void {
    if (x.len == 0) return;
    
    // 1. Find maximum for numerical stability
    var max_val = x[0];
    for (x[1..]) |val| {
        max_val = @max(max_val, val);
    }
    
    // 2. Compute exp(x - max) and sum
    var sum: f32 = 0.0;
    for (x) |*val| {
        val.* = @exp(val.* - max_val);
        sum += val.*;
    }
    
    // 3. Normalize to probabilities
    const inv_sum = 1.0 / sum;
    for (x) |*val| {
        val.* *= inv_sum;
    }
}

/// Log-softmax for numerical stability in loss computation
/// log(softmax(x_i)) = x_i - max(x) - log(sum(exp(x_j - max(x))))
pub fn log_softmax(out: []f32, x: []const f32) void {
    std.debug.assert(out.len == x.len);
    if (x.len == 0) return;
    
    // Find maximum
    var max_val = x[0];
    for (x[1..]) |val| {
        max_val = @max(max_val, val);
    }
    
    // Compute log-sum-exp
    var log_sum_exp: f32 = 0.0;
    for (x) |val| {
        log_sum_exp += @exp(val - max_val);
    }
    log_sum_exp = @log(log_sum_exp);
    
    // Compute log-softmax
    for (x, out) |xi, *yi| {
        yi.* = xi - max_val - log_sum_exp;
    }
}

// ============================================================================
// Vector Operations
// ============================================================================

/// Dot product: sum(a_i * b_i)
/// Optimized for cache locality and potential vectorization
pub fn dot_product(a: []const f32, b: []const f32) f32 {
    std.debug.assert(a.len == b.len);
    
    var sum: f32 = 0.0;
    
    // Unroll for potential vectorization
    const len = a.len;
    const unroll_factor = 4;
    const unrolled_len = (len / unroll_factor) * unroll_factor;
    
    // Unrolled loop
    var i: usize = 0;
    while (i < unrolled_len) : (i += unroll_factor) {
        sum += a[i] * b[i] + a[i+1] * b[i+1] + a[i+2] * b[i+2] + a[i+3] * b[i+3];
    }
    
    // Handle remainder
    while (i < len) : (i += 1) {
        sum += a[i] * b[i];
    }
    
    return sum;
}

/// Element-wise addition: out_i = a_i + b_i
pub fn add_arrays(out: []f32, a: []const f32, b: []const f32) void {
    std.debug.assert(out.len == a.len and a.len == b.len);
    
    for (a, b, out) |ai, bi, *oi| {
        oi.* = ai + bi;
    }
}

/// Element-wise multiplication: out_i = a_i * b_i
pub fn mul_arrays(out: []f32, a: []const f32, b: []const f32) void {
    std.debug.assert(out.len == a.len and a.len == b.len);
    
    for (a, b, out) |ai, bi, *oi| {
        oi.* = ai * bi;
    }
}

/// Scalar multiplication: out_i = scalar * a_i
pub fn scale_array(out: []f32, a: []const f32, scalar: f32) void {
    std.debug.assert(out.len == a.len);
    
    for (a, out) |ai, *oi| {
        oi.* = scalar * ai;
    }
}

// ============================================================================
// Utility Functions
// ============================================================================

/// Compute array sum
pub fn sum(x: []const f32) f32 {
    var result: f32 = 0.0;
    for (x) |val| {
        result += val;
    }
    return result;
}

/// Compute array mean
pub fn mean(x: []const f32) f32 {
    if (x.len == 0) return 0.0;
    return sum(x) / @as(f32, @floatFromInt(x.len));
}

/// Compute array variance
pub fn variance(x: []const f32) f32 {
    if (x.len == 0) return 0.0;
    
    const x_mean = mean(x);
    var var_sum: f32 = 0.0;
    
    for (x) |val| {
        const diff = val - x_mean;
        var_sum += diff * diff;
    }
    
    return var_sum / @as(f32, @floatFromInt(x.len));
}

/// Find maximum value in array
pub fn max_value(x: []const f32) f32 {
    if (x.len == 0) return 0.0;
    
    var max_val = x[0];
    for (x[1..]) |val| {
        max_val = @max(max_val, val);
    }
    return max_val;
}

/// Find minimum value in array
pub fn min_value(x: []const f32) f32 {
    if (x.len == 0) return 0.0;
    
    var min_val = x[0];
    for (x[1..]) |val| {
        min_val = @min(min_val, val);
    }
    return min_val;
}

// ============================================================================
// Educational Demonstrations
// ============================================================================

/// Compare matrix multiplication implementations
fn benchmark_matmul(allocator: Allocator, size: usize) !void {
    const n = size;
    const A = try allocator.alloc(f32, n * n);
    defer allocator.free(A);
    const B = try allocator.alloc(f32, n * n);
    defer allocator.free(B);
    const C1 = try allocator.alloc(f32, n * n);
    defer allocator.free(C1);
    const C2 = try allocator.alloc(f32, n * n);
    defer allocator.free(C2);
    
    // Initialize with random-ish data
    for (A, 0..) |*val, i| {
        val.* = @sin(@as(f32, @floatFromInt(i)) * 0.01);
    }
    for (B, 0..) |*val, i| {
        val.* = @cos(@as(f32, @floatFromInt(i)) * 0.01);
    }
    
    print("\n📊 Matrix Multiplication Comparison ({}x{}):\n", .{n, n});
    
    // Naive implementation
    const start1 = std.time.nanoTimestamp();
    matmul_naive(C1, A, B, n, n, n);
    const time1 = std.time.nanoTimestamp() - start1;
    
    // Optimized implementation
    const start2 = std.time.nanoTimestamp();
    matmul(C2, A, B, n, n, n);
    const time2 = std.time.nanoTimestamp() - start2;
    
    // Verify results match
    var max_diff: f32 = 0.0;
    for (C1, C2) |c1, c2| {
        max_diff = @max(max_diff, @abs(c1 - c2));
    }
    
    print("Naive implementation: {d:.3} ms\n", .{@as(f32, @floatFromInt(time1)) / 1e6});
    print("Cache-optimized:      {d:.3} ms\n", .{@as(f32, @floatFromInt(time2)) / 1e6});
    print("Speedup: {d:.2}x\n", .{@as(f32, @floatFromInt(time1)) / @as(f32, @floatFromInt(time2))});
    print("Max difference: {e}\n", .{max_diff});
}

/// Demonstrate activation functions
fn demonstrate_activations() void {
    print("\n🔥 Activation Functions Demonstration:\n");
    
    const inputs = [_]f32{-3.0, -1.0, 0.0, 1.0, 3.0};
    
    print("|   x   |  SiLU  |  ReLU  | Softplus |\n");
    print("|-------|--------|--------|----------|\n");
    
    for (inputs) |x| {
        print("| {d: >5.1} | {d: >6.3} | {d: >6.3} | {d: >8.3} |\n", .{x, silu(x), relu(x), softplus(x)});
    }
    
    // Softmax demonstration
    var probs = [_]f32{1.0, 2.0, 3.0, 4.0, 5.0};
    print("\nSoftmax example:\n");
    print("Input:  [{d:.1}, {d:.1}, {d:.1}, {d:.1}, {d:.1}]\n", .{probs[0], probs[1], probs[2], probs[3], probs[4]});
    softmax(&probs);
    print("Output: [{d:.3}, {d:.3}, {d:.3}, {d:.3}, {d:.3}]\n", .{probs[0], probs[1], probs[2], probs[3], probs[4]});
    print("Sum: {d:.6} (should be 1.0)\n", .{sum(&probs)});
}

/// Demonstrate normalization
fn demonstrate_normalization(allocator: Allocator) !void {
    print("\n📎 Normalization Demonstration:\n");
    
    var data = [_]f32{1.0, 2.0, 3.0, 4.0, 5.0};
    var weights = [_]f32{1.0, 1.0, 1.0, 1.0, 1.0};
    var bias = [_]f32{0.0, 0.0, 0.0, 0.0, 0.0};
    var output = [_]f32{0.0, 0.0, 0.0, 0.0, 0.0};
    
    print("Original data: [{d:.1}, {d:.1}, {d:.1}, {d:.1}, {d:.1}]\n", .{data[0], data[1], data[2], data[3], data[4]});
    print("Mean: {d:.3}, Variance: {d:.3}\n", .{mean(&data), variance(&data)});
    
    // RMSNorm
    rmsnorm(&output, &data, &weights);
    print("\nAfter RMSNorm:\n");
    print("Data: [{d:.3}, {d:.3}, {d:.3}, {d:.3}, {d:.3}]\n", .{output[0], output[1], output[2], output[3], output[4]});
    print("Mean: {d:.3}, Variance: {d:.3}\n", .{mean(&output), variance(&output)});
    
    // LayerNorm
    layernorm(&output, &data, &weights, &bias);
    print("\nAfter LayerNorm:\n");
    print("Data: [{d:.3}, {d:.3}, {d:.3}, {d:.3}, {d:.3}]\n", .{output[0], output[1], output[2], output[3], output[4]});
    print("Mean: {d:.3}, Variance: {d:.3}\n", .{mean(&output), variance(&output)});
}

/// Performance analysis
fn analyze_performance(allocator: Allocator) !void {
    print("\n⚡ Performance Analysis:\n");
    
    // Vector operations performance
    const n = 10000;
    const a = try allocator.alloc(f32, n);
    defer allocator.free(a);
    const b = try allocator.alloc(f32, n);
    defer allocator.free(b);
    const c = try allocator.alloc(f32, n);
    defer allocator.free(c);
    
    // Initialize
    for (a, 0..) |*val, i| {
        val.* = @as(f32, @floatFromInt(i)) / @as(f32, @floatFromInt(n));
    }
    for (b, 0..) |*val, i| {
        val.* = 1.0 - @as(f32, @floatFromInt(i)) / @as(f32, @floatFromInt(n));
    }
    
    // Dot product timing
    const start = std.time.nanoTimestamp();
    const dot_result = dot_product(a, b);
    const dot_time = std.time.nanoTimestamp() - start;
    
    print("Vector operations (n={}):  \n", .{n});
    print("Dot product: {d:.6} (computed in {d:.3} μs)\n", .{dot_result, @as(f32, @floatFromInt(dot_time)) / 1e3});
    print("Throughput: {d:.1} GFLOPS\n", .{@as(f32, @floatFromInt(n)) / @as(f32, @floatFromInt(dot_time))});
}

/// Educational demo main function
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    print("⚙️ ZigML Operations - Educational Demo\n");
    print("=====================================\n");

    print("\n📚 Educational Features:\n");
    print("- Matrix multiplication optimizations\n");
    print("- Numerical stability techniques\n");
    print("- Modern activation functions\n");
    print("- Efficient normalization methods\n");
    print("- Cache-friendly algorithm design\n");

    // Demonstrate different components
    try benchmark_matmul(allocator, 128);
    demonstrate_activations();
    try demonstrate_normalization(allocator);
    try analyze_performance(allocator);

    print("\n🎓 Educational Concepts Covered:\n");
    print("- Cache optimization in matrix multiplication\n");
    print("- Numerical stability in softmax computation\n");
    print("- Trade-offs between different activation functions\n");
    print("- Modern normalization techniques (RMSNorm vs LayerNorm)\n");
    print("- Performance measurement and analysis\n");

    print("\n🚀 Advanced experiments to try:\n");
    print("- Implement SIMD-optimized operations\n");
    print("- Add quantized (INT8/INT16) operations\n");
    print("- Benchmark against optimized BLAS libraries\n");
    print("- Implement sparse matrix operations\n");
    print("- Study memory access patterns with profilers\n");
}