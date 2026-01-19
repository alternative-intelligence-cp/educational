# Wave Physics - Theory and Mathematics

This document provides the theoretical foundation for understanding the wave phenomena demonstrated in the simulators.

## Table of Contents
1. [Wave Fundamentals](#wave-fundamentals)
2. [Superposition Principle](#superposition-principle)
3. [Interference](#interference)
4. [Standing Waves](#standing-waves)
5. [2D Wave Propagation](#2d-wave-propagation)
6. [Quantum Wave Packets](#quantum-wave-packets)
7. [Heisenberg Uncertainty Principle](#heisenberg-uncertainty-principle)

---

## Wave Fundamentals

### The Wave Equation

A traveling wave can be described by:

```
ψ(x,t) = A sin(kx - ωt + φ)
```

Where:
- **ψ(x,t)** = wave displacement at position x and time t
- **A** = amplitude (maximum displacement)
- **k** = wave number = 2π/λ (spatial frequency)
- **ω** = angular frequency = 2πf (temporal frequency)
- **φ** = phase offset
- **λ** = wavelength
- **f** = frequency

### Key Relationships

```
v = λf           (wave velocity)
ω = 2πf          (angular frequency)
k = 2π/λ         (wave number)
v = ω/k          (phase velocity)
```

### Wave Types

**Transverse waves**: Displacement perpendicular to propagation
- Light, water waves, strings

**Longitudinal waves**: Displacement parallel to propagation
- Sound, compression waves

---

## Superposition Principle

**Core Principle**: When two or more waves overlap, the resulting displacement is the sum of individual displacements.

```
ψ_total(x,t) = ψ₁(x,t) + ψ₂(x,t) + ψ₃(x,t) + ...
```

This is a **linear** property - waves pass through each other without permanent interaction.

### Mathematical Example

Two waves:
```
ψ₁ = A sin(kx - ωt)
ψ₂ = A sin(kx - ωt + φ)
```

Superposition:
```
ψ_total = A[sin(kx - ωt) + sin(kx - ωt + φ)]
        = 2A cos(φ/2) sin(kx - ωt + φ/2)
```

Result: A wave with amplitude **2A cos(φ/2)** that depends on phase difference φ!

---

## Interference

### Constructive Interference

Occurs when **φ = 0, 2π, 4π, ...** (even multiples of π)

```
cos(φ/2) = cos(0) = 1
Amplitude = 2A (maximum)
```

**Physical meaning**: Wave peaks align, creating a larger wave.

### Destructive Interference

Occurs when **φ = π, 3π, 5π, ...** (odd multiples of π)

```
cos(φ/2) = cos(π/2) = 0
Amplitude = 0 (cancellation)
```

**Physical meaning**: Peaks meet troughs, waves cancel.

### Path Difference

For two sources separated by distance d:

```
Phase difference: φ = (2π/λ) × path difference

Constructive: path difference = nλ (n = 0, 1, 2, ...)
Destructive: path difference = (n + 1/2)λ
```

---

## Standing Waves

Created when two identical waves travel in opposite directions.

### Mathematical Derivation

Forward wave: `ψ₁ = A sin(kx - ωt)`
Backward wave: `ψ₂ = A sin(kx + ωt)`

Superposition:
```
ψ_standing = ψ₁ + ψ₂
           = A[sin(kx - ωt) + sin(kx + ωt)]
           = 2A sin(kx) cos(ωt)
```

**Key insight**: Separates into spatial and temporal parts!

### Nodes and Antinodes

**Nodes** (no motion): `sin(kx) = 0`
- Locations: `x = nλ/2` (n = 0, 1, 2, ...)

**Antinodes** (maximum motion): `sin(kx) = ±1`
- Locations: `x = (2n+1)λ/4`

### Boundary Conditions

For a string fixed at both ends (length L):
```
Allowed wavelengths: λₙ = 2L/n  (n = 1, 2, 3, ...)
Allowed frequencies: fₙ = nv/(2L)
```

These are the **harmonics** or **normal modes**.

---

## 2D Wave Propagation

### Circular Waves from Point Source

```
ψ(r,t) = (A/r) sin(kr - ωt)
```

Note the **1/r amplitude decay** - energy spreads over larger circles.

### Two-Source Interference

Sources at positions **r₁** and **r₂**:

```
ψ_total = (A/r₁) sin(kr₁ - ωt) + (A/r₂) sin(kr₂ - ωt)
```

### Double-Slit Interference Pattern

For small angles θ from center:

```
Path difference = d sin(θ)

Bright fringes: d sin(θ) = nλ
Dark fringes: d sin(θ) = (n + 1/2)λ
```

On a screen at distance D:
```
Fringe spacing: Δy = λD/d
```

**Key insight**: Smaller wavelength or larger slit separation → narrower fringes.

---

## Quantum Wave Packets

### Gaussian Wave Packet

A localized quantum particle is described by:

```
ψ(x,t) = A exp[-(x-x₀)²/(4σ²)] exp[i(kx - ωt)]
```

Where:
- **σ** = spatial width (position uncertainty)
- **k** = central wave number (momentum)
- Gaussian envelope × plane wave

### Fourier Relationship

Position and momentum representations are **Fourier transforms**:

```
ψ(x) = (1/√2π) ∫ φ(k) e^(ikx) dk

φ(k) = (1/√2π) ∫ ψ(x) e^(-ikx) dx
```

**Consequence**: Narrow in position → wide in momentum, and vice versa.

### Dispersion

Wave packets **spread** over time due to different momentum components traveling at different velocities:

```
σ(t) = σ₀ √[1 + (ℏt/2mσ₀²)²]
```

Where:
- **σ₀** = initial width
- **m** = particle mass
- **ℏ** = reduced Planck constant

**Key insight**: Narrower initial packets spread faster!

---

## Heisenberg Uncertainty Principle

### Position-Momentum Uncertainty

```
Δx · Δp ≥ ℏ/2
```

Where:
- **Δx** = standard deviation of position
- **Δp** = standard deviation of momentum
- **ℏ** = h/(2π) ≈ 1.055 × 10⁻³⁴ J·s

### For Gaussian Wave Packet

```
Δx = σ (width parameter)
Δp = ℏ/(2σ) (momentum spread)

Product: Δx·Δp = ℏ/2 (minimum uncertainty!)
```

Gaussian packets achieve the **minimum possible uncertainty**.

### Physical Interpretation

1. **Not measurement limitation**: This is fundamental to reality
2. **Wave nature**: Particles ARE waves; localization requires wavelength spread
3. **Fourier principle**: Mathematical consequence of wave-particle duality

### Energy-Time Uncertainty

Related principle:
```
ΔE · Δt ≥ ℏ/2
```

Allows "virtual particles" to exist briefly if ΔE is large enough.

---

## Connecting to Classical Physics

### Wave-Particle Duality

**de Broglie relation**:
```
λ = h/p
k = p/ℏ
```

Every particle has an associated wavelength!

### Correspondence Principle

As quantum numbers get large (macroscopic objects):
- Wave packets become very localized (Δx → 0 classically)
- Quantum mechanics → classical mechanics

### Examples

**Electron** (m ≈ 10⁻³⁰ kg, v ≈ 10⁶ m/s):
- λ ≈ 10⁻⁹ m (nanometer scale - significant!)

**Baseball** (m ≈ 0.1 kg, v ≈ 40 m/s):
- λ ≈ 10⁻³⁴ m (unmeasurably small)

---

## Further Reading

### Historical Papers
1. Young (1801): "On the theory of light and colours" - Double-slit experiment
2. de Broglie (1924): "Recherches sur la théorie des quanta" - Matter waves
3. Heisenberg (1927): "Über den anschaulichen Inhalt..." - Uncertainty principle
4. Schrödinger (1926): "Quantisierung als Eigenwertproblem" - Wave equation

### Modern Textbooks
- Griffiths, "Introduction to Quantum Mechanics"
- French, "Vibrations and Waves"
- Feynman Lectures, Volume 1, Chapters 29-31

### Experimental Demonstrations
- Single-photon interference (Grangier et al., 1986)
- Matter wave interference with C₆₀ molecules (Arndt et al., 1999)
- Delayed-choice quantum eraser (Kim et al., 2000)

---

## Practice Problems

### Problem 1: Wave Addition
Two waves: ψ₁ = sin(2x - 3t), ψ₂ = sin(2x - 3t + π/3)
Calculate: (a) resultant amplitude, (b) phase

### Problem 2: Standing Wave on String
A 1-meter guitar string vibrates at 440 Hz (A note).
Calculate: (a) wavelength, (b) wave velocity, (c) 3rd harmonic frequency

### Problem 3: Double-Slit
Light λ=500 nm, slits separated by 0.1 mm, screen at 2 m.
Calculate: (a) fringe spacing, (b) angle to 5th bright fringe

### Problem 4: Quantum Particle
Electron confined to Δx = 1 Å (10⁻¹⁰ m).
Calculate: (a) minimum Δp, (b) minimum kinetic energy

### Problem 5: Wave Packet
ψ(x) has width σ = 10 nm at t=0.
Calculate: (a) Δp, (b) width after t = 1 ns for an electron

**Solutions and detailed explanations available in SOLUTIONS.md**

---

*"The underlying physical laws necessary for the mathematical theory of a large part of physics and the whole of chemistry are thus completely known, and the difficulty is only that the exact application of these laws leads to equations much too complicated to be soluble."* 
— Paul Dirac (1929)

The beauty of computational physics is that we can now solve these equations visually!
