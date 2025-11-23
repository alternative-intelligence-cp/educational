/*
🧠⚡ Educational C Mamba - Simplified for Learning ⚡🧠
Focus on clarity and correctness over maximum performance

This version prioritizes:
- Educational clarity
- Memory safety 
- Correct implementation
- Good performance (but not maximum optimization)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Simple configuration
typedef struct {
    int d_model;
    int d_state; 
    int d_inner;
    int seq_len;
    int batch_size;
} SimpleConfig;

// Simplified Mamba for educational purposes
typedef struct {
    SimpleConfig config;
    
    // Model parameters (simplified)
    float* input_weights;   // d_model -> d_inner
    float* state_weights;   // d_inner -> d_state  
    float* output_weights;  // d_inner -> d_model
    float* A_matrix;        // d_state x d_state
    float* B_vector;        // d_state
    float* C_vector;        // d_state
    
    // Working buffers
    float* hidden_state;    // d_state
    float* temp_inner;      // d_inner
} SimpleMamba;

// Forward declaration
void simple_mamba_destroy(SimpleMamba* model);

// Utility functions
float randn() {
    static int has_spare = 0;
    static float spare;
    
    if (has_spare) {
        has_spare = 0;
        return spare;
    }
    
    has_spare = 1;
    float u, v, s;
    do {
        u = 2.0f * (float)rand() / RAND_MAX - 1.0f;
        v = 2.0f * (float)rand() / RAND_MAX - 1.0f;
        s = u * u + v * v;
    } while (s >= 1.0f || s == 0.0f);
    
    s = sqrtf(-2.0f * logf(s) / s);
    spare = v * s;
    return u * s;
}

float silu(float x) {
    return x / (1.0f + expf(-x));
}

// Create simplified Mamba model
SimpleMamba* simple_mamba_create(int d_model, int d_state, int seq_len, int batch_size) {
    SimpleMamba* model = (SimpleMamba*)calloc(1, sizeof(SimpleMamba));
    if (!model) return NULL;
    
    // Set configuration
    model->config.d_model = d_model;
    model->config.d_state = d_state;
    model->config.d_inner = d_model * 2;  // Expansion factor of 2
    model->config.seq_len = seq_len;
    model->config.batch_size = batch_size;
    
    int d_inner = model->config.d_inner;
    
    // Allocate parameters
    model->input_weights = (float*)malloc(d_model * d_inner * sizeof(float));
    model->state_weights = (float*)malloc(d_inner * d_state * sizeof(float));
    model->output_weights = (float*)malloc(d_inner * d_model * sizeof(float));
    model->A_matrix = (float*)malloc(d_state * d_state * sizeof(float));
    model->B_vector = (float*)malloc(d_state * sizeof(float));
    model->C_vector = (float*)malloc(d_state * sizeof(float));
    
    // Allocate working buffers  
    model->hidden_state = (float*)calloc(d_state, sizeof(float));
    model->temp_inner = (float*)malloc(d_inner * sizeof(float));
    
    // Check allocations
    if (!model->input_weights || !model->state_weights || !model->output_weights ||
        !model->A_matrix || !model->B_vector || !model->C_vector ||
        !model->hidden_state || !model->temp_inner) {
        simple_mamba_destroy(model);
        return NULL;
    }
    
    // Initialize parameters
    float input_scale = sqrtf(2.0f / d_model);
    float state_scale = sqrtf(2.0f / d_inner);
    float output_scale = sqrtf(2.0f / d_inner);
    
    for (int i = 0; i < d_model * d_inner; i++) {
        model->input_weights[i] = randn() * input_scale;
    }
    
    for (int i = 0; i < d_inner * d_state; i++) {
        model->state_weights[i] = randn() * state_scale;
    }
    
    for (int i = 0; i < d_inner * d_model; i++) {
        model->output_weights[i] = randn() * output_scale;
    }
    
    // Initialize A matrix (state transition) - diagonal dominance for stability
    for (int i = 0; i < d_state; i++) {
        for (int j = 0; j < d_state; j++) {
            if (i == j) {
                model->A_matrix[i * d_state + j] = 0.9f + 0.1f * randn() * 0.1f;
            } else {
                model->A_matrix[i * d_state + j] = randn() * 0.01f;
            }
        }
    }
    
    // Initialize B and C vectors
    for (int i = 0; i < d_state; i++) {
        model->B_vector[i] = randn() * 0.1f;
        model->C_vector[i] = randn() * 0.1f;
    }
    
    return model;
}

void simple_mamba_destroy(SimpleMamba* model) {
    if (!model) return;
    
    free(model->input_weights);
    free(model->state_weights);
    free(model->output_weights);
    free(model->A_matrix);
    free(model->B_vector);
    free(model->C_vector);
    free(model->hidden_state);
    free(model->temp_inner);
    free(model);
}

// Simple matrix-vector multiplication: y = A * x
void matvec(const float* A, const float* x, float* y, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        y[i] = 0.0f;
        for (int j = 0; j < cols; j++) {
            y[i] += A[i * cols + j] * x[j];
        }
    }
}

// Forward pass for a single time step
void simple_mamba_step(SimpleMamba* model, const float* input, float* output) {
    int d_model = model->config.d_model;
    int d_state = model->config.d_state;
    int d_inner = model->config.d_inner;
    
    // Step 1: Project input to inner dimension
    matvec(model->input_weights, input, model->temp_inner, d_inner, d_model);
    
    // Step 2: Apply activation
    for (int i = 0; i < d_inner; i++) {
        model->temp_inner[i] = silu(model->temp_inner[i]);
    }
    
    // Step 3: Update hidden state (simplified state space)
    // h_new = A * h_old + B * (mean of inner representation)
    float input_summary = 0.0f;
    for (int i = 0; i < d_inner; i++) {
        input_summary += model->temp_inner[i];
    }
    input_summary /= d_inner;
    
    // Update state
    float* new_state = (float*)malloc(d_state * sizeof(float));
    matvec(model->A_matrix, model->hidden_state, new_state, d_state, d_state);
    
    for (int i = 0; i < d_state; i++) {
        new_state[i] += model->B_vector[i] * input_summary;
        model->hidden_state[i] = new_state[i];
    }
    free(new_state);
    
    // Step 4: Compute state contribution
    float state_output = 0.0f;
    for (int i = 0; i < d_state; i++) {
        state_output += model->C_vector[i] * model->hidden_state[i];
    }
    
    // Step 5: Modulate inner representation with state
    for (int i = 0; i < d_inner; i++) {
        model->temp_inner[i] *= (1.0f + 0.1f * state_output);
    }
    
    // Step 6: Project back to output dimension
    matvec(model->output_weights, model->temp_inner, output, d_model, d_inner);
}

// Process full sequence
void simple_mamba_forward(SimpleMamba* model, const float* inputs, float* outputs) {
    int d_model = model->config.d_model;
    int seq_len = model->config.seq_len;
    
    // Reset hidden state
    memset(model->hidden_state, 0, model->config.d_state * sizeof(float));
    
    // Process each time step
    for (int t = 0; t < seq_len; t++) {
        const float* input = inputs + t * d_model;
        float* output = outputs + t * d_model;
        simple_mamba_step(model, input, output);
    }
}

// Performance testing
double get_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}

void benchmark_simple_mamba() {
    printf("🧠⚡ Educational C Mamba Benchmark ⚡🧠\n");
    printf("====================================\n\n");
    
    // Test configurations
    struct {
        int d_model;
        int d_state; 
        int seq_len;
        const char* desc;
    } configs[] = {
        {64, 8, 50, "Small model, medium sequence"},
        {128, 16, 100, "Medium model, long sequence"},
        {256, 32, 50, "Large model, medium sequence"},
        {128, 16, 200, "Medium model, very long sequence"}
    };
    
    int num_configs = sizeof(configs) / sizeof(configs[0]);
    
    for (int config_idx = 0; config_idx < num_configs; config_idx++) {
        int d_model = configs[config_idx].d_model;
        int d_state = configs[config_idx].d_state;
        int seq_len = configs[config_idx].seq_len;
        const char* desc = configs[config_idx].desc;
        
        printf("📊 %s\n", desc);
        printf("   Config: d_model=%d, d_state=%d, seq_len=%d\n", d_model, d_state, seq_len);
        
        // Create model
        SimpleMamba* model = simple_mamba_create(d_model, d_state, seq_len, 1);
        if (!model) {
            printf("   ❌ Failed to create model\n\n");
            continue;
        }
        
        // Allocate input/output
        float* inputs = (float*)malloc(seq_len * d_model * sizeof(float));
        float* outputs = (float*)malloc(seq_len * d_model * sizeof(float));
        
        if (!inputs || !outputs) {
            printf("   ❌ Failed to allocate memory\n\n");
            simple_mamba_destroy(model);
            free(inputs);
            free(outputs);
            continue;
        }
        
        // Initialize random input
        for (int i = 0; i < seq_len * d_model; i++) {
            inputs[i] = ((float)rand() / RAND_MAX - 0.5f) * 0.2f;
        }
        
        // Warmup
        simple_mamba_forward(model, inputs, outputs);
        
        // Benchmark
        double start_time = get_time();
        simple_mamba_forward(model, inputs, outputs);
        double end_time = get_time();
        
        double duration = end_time - start_time;
        double tokens_per_sec = seq_len / duration;
        
        // Analyze output
        float output_mean = 0.0f;
        float output_std = 0.0f;
        
        for (int i = 0; i < seq_len * d_model; i++) {
            output_mean += outputs[i];
        }
        output_mean /= (seq_len * d_model);
        
        for (int i = 0; i < seq_len * d_model; i++) {
            float diff = outputs[i] - output_mean;
            output_std += diff * diff;
        }
        output_std = sqrtf(output_std / (seq_len * d_model));
        
        printf("   ⚡ Performance: %.3fs (%.0f tokens/sec)\n", duration, tokens_per_sec);
        printf("   📈 Output: mean=%.4f, std=%.4f\n", output_mean, output_std);
        
        // Check for reasonable outputs (not NaN or extreme values)
        int valid_output = 1;
        for (int i = 0; i < seq_len * d_model; i++) {
            if (isnan(outputs[i]) || fabsf(outputs[i]) > 1000.0f) {
                valid_output = 0;
                break;
            }
        }
        
        if (valid_output) {
            printf("   ✅ Output validation passed\n");
        } else {
            printf("   ⚠️  Output contains invalid values\n");
        }
        
        printf("\n");
        
        // Cleanup
        simple_mamba_destroy(model);
        free(inputs);
        free(outputs);
    }
    
    printf("🎯 Educational C Implementation Benefits:\n");
    printf("=========================================\n");
    printf("✅ Manual memory management demonstration\n");
    printf("✅ Clear algorithmic structure\n"); 
    printf("✅ Performance optimization opportunities visible\n");
    printf("✅ Direct control over computational flow\n");
    printf("✅ Foundation for SIMD and GPU optimization\n");
}

int main() {
    printf("🔥 Educational C Mamba Implementation 🔥\n");
    printf("Simplified for clarity and learning\n\n");
    
    srand(42);  // Reproducible results
    
    benchmark_simple_mamba();
    
    printf("\n💡 Key Educational Insights:\n");
    printf("===========================\n");
    printf("📚 C shows raw algorithmic performance potential\n");
    printf("🛠️  Manual memory management gives total control\n");
    printf("⚡ Foundation for high-performance optimizations\n");
    printf("🎯 Clear path from algorithm to implementation\n");
    
    return 0;
}