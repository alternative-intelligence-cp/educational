# 🧠⚡ Educational Mamba State Space Model ⚡🧠
# Clean, Readable Implementation for Learning
# 
# This implementation prioritizes clarity and understanding over performance.
# It demonstrates the core mathematics of Mamba state space models
# with extensive documentation for educational purposes.

import numpy as np
import math
from typing import Tuple, Optional
from dataclasses import dataclass


@dataclass
class MambaConfig:
    """Configuration for educational Mamba model
    
    This contains all the hyperparameters that define a Mamba model.
    Each parameter is carefully chosen for educational clarity.
    """
    d_model: int = 512        # Model dimension (hidden size)
    d_state: int = 16         # State space dimension (N in papers)
    d_conv: int = 4           # Local convolution width
    expand_factor: int = 2    # Expansion factor for inner dimension
    dt_rank: str = "auto"     # Rank of Δ (discretization parameter)
    dt_min: float = 0.001     # Minimum value of Δ
    dt_max: float = 0.1       # Maximum value of Δ
    dt_init: str = "random"   # How to initialize Δ
    dt_scale: float = 1.0     # Scaling factor for Δ initialization
    bias: bool = False        # Whether to use bias in linear layers
    conv_bias: bool = True    # Whether to use bias in convolution
    
    def __post_init__(self):
        # Auto-compute dt_rank if needed
        if self.dt_rank == "auto":
            self.dt_rank = math.ceil(self.d_model / 16)
        
        # Derived parameters
        self.d_inner = self.expand_factor * self.d_model


class EducationalMamba:
    """Educational Mamba State Space Model
    
    This implementation shows the core mathematics of Mamba in the clearest
    possible way. Every operation is explained and made explicit.
    
    Mamba combines:
    1. Selective state spaces (data-dependent transitions)
    2. Hardware-efficient design (no attention)  
    3. Linear scaling with sequence length
    
    Mathematical Foundation:
    - State equation: h_t = A*h_{t-1} + B*x_t
    - Output equation: y_t = C*h_t + D*x_t
    - Selective mechanism: A, B, C depend on input x_t
    """
    
    def __init__(self, config: MambaConfig):
        self.config = config
        self.reset_parameters()
    
    def reset_parameters(self):
        """Initialize all model parameters
        
        This uses the same initialization strategy as the original Mamba paper
        but with clear educational explanations for each choice.
        """
        cfg = self.config
        
        # === INPUT PROJECTION ===
        # Projects input to expanded dimension for processing
        self.in_proj = self._create_linear_layer(cfg.d_model, cfg.d_inner * 2)
        
        # === CONVOLUTION LAYER ===
        # Local context mixing before state space processing
        self.conv1d = self._create_conv1d(cfg.d_inner, cfg.d_inner, cfg.d_conv)
        
        # === STATE SPACE PARAMETERS ===
        # These are the core of the selective state space
        
        # Linear projections for selective parameters
        self.x_proj = self._create_linear_layer(cfg.d_inner, cfg.dt_rank + cfg.d_state * 2, bias=False)
        
        # Δ (delta) projection - discretization parameter
        self.dt_proj = self._create_linear_layer(cfg.dt_rank, cfg.d_inner, bias=True)
        
        # === STATE SPACE MATRICES ===
        # A matrix: State transition (diagonal form for efficiency)
        self.A_log = self._init_A_matrix(cfg.d_inner, cfg.d_state)
        
        # D matrix: Skip connection (optional)
        self.D = np.ones(cfg.d_inner)
        
        # === OUTPUT PROJECTION ===
        self.out_proj = self._create_linear_layer(cfg.d_inner, cfg.d_model)
        
    def _create_linear_layer(self, in_features: int, out_features: int, bias: bool = False):
        """Create a linear transformation layer"""
        # Xavier initialization for educational clarity
        bound = math.sqrt(6.0 / (in_features + out_features))
        weight = np.random.uniform(-bound, bound, (out_features, in_features))
        
        layer = {"weight": weight}
        if bias:
            layer["bias"] = np.zeros(out_features)
        return layer
    
    def _create_conv1d(self, in_channels: int, out_channels: int, kernel_size: int):
        """Create 1D convolution layer"""
        # He initialization for ReLU-like activations
        fan_in = in_channels * kernel_size
        bound = math.sqrt(2.0 / fan_in)
        
        return {
            "weight": np.random.normal(0, bound, (out_channels, in_channels, kernel_size)),
            "bias": np.zeros(out_channels) if self.config.conv_bias else None
        }
    
    def _init_A_matrix(self, d_inner: int, d_state: int):
        """Initialize A matrix for state space
        
        Uses the S4 initialization strategy:
        - A is diagonal with negative real parts
        - Ensures stability of the dynamical system
        """
        # Create diagonal matrix with values from -1 to -d_state
        A = np.zeros((d_inner, d_state))
        for i in range(d_inner):
            # Each channel gets a different A matrix
            A[i, :] = -np.exp(np.random.uniform(0, math.log(d_state), d_state))
        
        return np.log(-A)  # Store log for numerical stability
    
    def forward(self, x: np.ndarray) -> np.ndarray:
        """Forward pass through Educational Mamba
        
        Args:
            x: Input tensor of shape (batch_size, sequence_length, d_model)
            
        Returns:
            Output tensor of shape (batch_size, sequence_length, d_model)
            
        This function demonstrates the complete Mamba forward pass with
        clear documentation of each mathematical operation.
        """
        batch_size, seq_len, d_model = x.shape
        assert d_model == self.config.d_model, f"Input dimension {d_model} != {self.config.d_model}"
        
        # === STEP 1: INPUT PROJECTION AND GATING ===
        # Project to expanded dimension and split for gating
        x_projected = self._linear_forward(x, self.in_proj)  # (B, L, 2*d_inner)
        x_inner, gate = np.split(x_projected, 2, axis=-1)    # Each (B, L, d_inner)
        
        # === STEP 2: LOCAL CONTEXT MIXING ===
        # Apply 1D convolution for local context
        x_conv = self._conv1d_forward(x_inner, self.conv1d)  # (B, L, d_inner)
        x_conv = self._silu_activation(x_conv)               # Apply SiLU activation
        
        # === STEP 3: SELECTIVE STATE SPACE ===
        # This is the heart of Mamba - the selective mechanism
        y = self._selective_state_space(x_conv)              # (B, L, d_inner)
        
        # === STEP 4: GATING ===
        # Apply gating mechanism (element-wise multiplication)
        y_gated = y * self._silu_activation(gate)            # (B, L, d_inner)
        
        # === STEP 5: OUTPUT PROJECTION ===
        # Project back to model dimension
        output = self._linear_forward(y_gated, self.out_proj)  # (B, L, d_model)
        
        return output
    
    def _selective_state_space(self, x: np.ndarray) -> np.ndarray:
        """The core selective state space computation
        
        This implements the selective mechanism that makes Mamba special:
        - The state space parameters A, B, C become input-dependent
        - Uses discretization to convert continuous to discrete time
        - Implements the selective scan algorithm efficiently
        
        Mathematical Details:
        1. Compute selective parameters: Δ, B, C = f(x)
        2. Discretize: A_bar = exp(Δ * A), B_bar = Δ * B
        3. Selective scan: h_t = A_bar * h_{t-1} + B_bar * x_t
        4. Output: y_t = C * h_t (+ D * x_t)
        """
        batch_size, seq_len, d_inner = x.shape
        d_state = self.config.d_state
        
        # === COMPUTE SELECTIVE PARAMETERS ===
        # Project input to get Δ, B, C parameters
        x_proj = self._linear_forward(x, self.x_proj)  # (B, L, dt_rank + 2*d_state)
        
        # Split into components
        dt_rank = self.config.dt_rank
        delta_proj = x_proj[..., :dt_rank]                    # (B, L, dt_rank)
        B_proj = x_proj[..., dt_rank:dt_rank + d_state]       # (B, L, d_state)
        C_proj = x_proj[..., dt_rank + d_state:]              # (B, L, d_state)
        
        # === DISCRETIZATION PARAMETER Δ ===
        # Δ controls the discretization step size
        delta = self._linear_forward(delta_proj, self.dt_proj)  # (B, L, d_inner)
        delta = np.exp(delta + np.log(self.config.dt_min))      # Ensure positive
        delta = np.minimum(delta, self.config.dt_max)          # Clamp maximum
        
        # === STATE SPACE MATRICES ===
        A = np.exp(self.A_log)  # Convert from log space (d_inner, d_state)
        
        # === DISCRETIZATION ===
        # Convert continuous-time to discrete-time
        # A_bar = exp(Δ * A), B_bar = Δ * B
        delta_A = delta[..., :, None] * A[None, None, :, :]     # (B, L, d_inner, d_state)
        A_bar = np.exp(delta_A)                                 # Discrete A matrix
        
        B_bar = delta[..., :, None] * B_proj[..., None, :]     # (B, L, d_inner, d_state)
        
        # === SELECTIVE SCAN ===
        # This is the core recurrent computation
        # h_t = A_bar_t * h_{t-1} + B_bar_t * x_t
        y = self._selective_scan(x, A_bar, B_bar, C_proj)
        
        # === SKIP CONNECTION ===
        # Add direct input contribution (like residual connection)
        y = y + x * self.D[None, None, :]
        
        return y
    
    def _selective_scan(self, x: np.ndarray, A_bar: np.ndarray, 
                       B_bar: np.ndarray, C: np.ndarray) -> np.ndarray:
        """Selective scan algorithm - the heart of Mamba
        
        This implements the recurrent state space computation:
        h_t = A_bar_t ⊙ h_{t-1} + B_bar_t ⊙ x_t
        y_t = C_t @ h_t
        
        The ⊙ operator represents element-wise multiplication.
        This is more efficient than full matrix multiplication.
        """
        batch_size, seq_len, d_inner = x.shape
        d_state = self.config.d_state
        
        # Initialize hidden state
        h = np.zeros((batch_size, d_inner, d_state))  # (B, d_inner, d_state)
        y = np.zeros((batch_size, seq_len, d_inner))  # (B, L, d_inner)
        
        # Recurrent computation for each time step
        for t in range(seq_len):
            # Current inputs
            x_t = x[:, t, :]                    # (B, d_inner)
            A_t = A_bar[:, t, :, :]             # (B, d_inner, d_state)
            B_t = B_bar[:, t, :, :]             # (B, d_inner, d_state)
            C_t = C[:, t, :]                    # (B, d_state)
            
            # State update: h_t = A_t ⊙ h_{t-1} + B_t ⊙ x_t
            h = A_t * h + B_t * x_t[:, :, None]  # Broadcasting magic
            
            # Output: y_t = sum(C_t * h_t, axis=-1)
            y[:, t, :] = np.sum(C_t[:, None, :] * h, axis=-1)  # (B, d_inner)
        
        return y
    
    def _linear_forward(self, x: np.ndarray, layer: dict) -> np.ndarray:
        """Forward pass through linear layer"""
        # x @ W^T + b
        y = x @ layer["weight"].T
        if "bias" in layer:
            y = y + layer["bias"]
        return y
    
    def _conv1d_forward(self, x: np.ndarray, conv_layer: dict) -> np.ndarray:
        """Forward pass through 1D convolution
        
        Implements causal convolution for autoregressive modeling.
        """
        batch_size, seq_len, d_inner = x.shape
        kernel_size = conv_layer["weight"].shape[-1]
        
        # Pad for causal convolution
        padding = kernel_size - 1
        x_padded = np.pad(x, ((0, 0), (padding, 0), (0, 0)), mode='constant')
        
        # Simple convolution implementation for education
        y = np.zeros_like(x)
        for i in range(seq_len):
            for k in range(kernel_size):
                if i + k < seq_len + padding:
                    y[:, i, :] += (x_padded[:, i + k, :, None] * 
                                  conv_layer["weight"][:, :, kernel_size - 1 - k].T).sum(axis=1)
        
        if conv_layer["bias"] is not None:
            y += conv_layer["bias"][None, None, :]
        
        return y
    
    def _silu_activation(self, x: np.ndarray) -> np.ndarray:
        """SiLU (Swish) activation function: x * sigmoid(x)"""
        return x * (1.0 / (1.0 + np.exp(-x)))


# === EDUCATIONAL DEMONSTRATIONS ===

def create_sample_input(batch_size: int = 2, seq_len: int = 10, d_model: int = 64):
    """Create sample input for testing"""
    return np.random.randn(batch_size, seq_len, d_model) * 0.1


def demonstrate_mamba_forward():
    """Educational demonstration of Mamba forward pass"""
    print("🧠⚡ Educational Mamba Demonstration ⚡🧠")
    print("=" * 50)
    
    # Create small model for demonstration
    config = MambaConfig(
        d_model=64,    # Small for clarity
        d_state=8,     # Small state space
        d_conv=3,      # Small convolution
        expand_factor=2
    )
    
    # Initialize model
    mamba = EducationalMamba(config)
    print(f"✅ Created Mamba model with {config.d_model} dimensions")
    
    # Create sample input
    batch_size, seq_len = 2, 5
    x = create_sample_input(batch_size, seq_len, config.d_model)
    print(f"✅ Created input tensor: {x.shape}")
    
    # Forward pass
    print("\n🔄 Running forward pass...")
    y = mamba.forward(x)
    print(f"✅ Output tensor: {y.shape}")
    
    # Analyze output
    print(f"\n📊 Analysis:")
    print(f"  Input range:  [{x.min():.3f}, {x.max():.3f}]")
    print(f"  Output range: [{y.min():.3f}, {y.max():.3f}]")
    print(f"  Output mean:  {y.mean():.3f}")
    print(f"  Output std:   {y.std():.3f}")
    
    return mamba, x, y


def demonstrate_selective_mechanism():
    """Show how the selective mechanism works"""
    print("\n🎯 Selective Mechanism Demonstration")
    print("=" * 40)
    
    config = MambaConfig(d_model=32, d_state=4, d_conv=3)
    mamba = EducationalMamba(config)
    
    # Create two different inputs to show selectivity
    x1 = np.ones((1, 3, config.d_model)) * 0.1    # Constant input
    x2 = np.random.randn(1, 3, config.d_model) * 0.2  # Random input
    
    print("Input 1 (constant):")
    print(f"  Shape: {x1.shape}, Mean: {x1.mean():.3f}")
    
    print("Input 2 (random):")
    print(f"  Shape: {x2.shape}, Mean: {x2.mean():.3f}")
    
    # Show different outputs
    y1 = mamba.forward(x1)
    y2 = mamba.forward(x2)
    
    print("\nOutputs:")
    print(f"  Y1 mean: {y1.mean():.3f}, std: {y1.std():.3f}")
    print(f"  Y2 mean: {y2.mean():.3f}, std: {y2.std():.3f}")
    print(f"  Difference: {np.abs(y1 - y2).mean():.3f}")
    
    print("✨ The model produces different outputs for different inputs!")
    print("   This demonstrates the selective state space mechanism.")


if __name__ == "__main__":
    print("🎓 Educational Mamba - Learning State Space Models")
    print("=" * 55)
    
    # Basic demonstration
    mamba, x, y = demonstrate_mamba_forward()
    
    # Show selective mechanism
    demonstrate_selective_mechanism()
    
    print("\n🎉 Educational Mamba demonstration complete!")
    print("📚 This implementation prioritizes clarity over performance.")
    print("📊 For production use, see the C and Zig implementations!")