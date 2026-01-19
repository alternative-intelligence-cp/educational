#!/usr/bin/env python3
"""
Quantum Wave Packet Simulator
Demonstrates wave-particle duality and the uncertainty principle

Shows:
- Gaussian wave packets
- How localized particles emerge from wave superposition
- Heisenberg uncertainty principle (Δx · Δp ≥ ℏ/2)
- Wave packet dispersion over time
- Fourier relationship between position and momentum
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.animation import FuncAnimation
from scipy.fft import fft, fftfreq, fftshift


class QuantumWavePacket:
    def __init__(self):
        # Position space
        self.x = np.linspace(-10, 10, 1000)
        self.dx = self.x[1] - self.x[0]

        # Wave packet parameters
        self.x0 = 0.0  # Initial position
        self.k0 = 5.0  # Initial wave number (momentum)
        self.sigma = 1.0  # Initial width (position uncertainty)
        self.time = 0.0

        # Physical constants (using convenient units where ℏ=m=1)
        self.hbar = 1.0
        self.mass = 1.0

        self.paused = False

        # Setup figure
        self.setup_figure()

    def setup_figure(self):
        """Create interactive visualization"""
        self.fig = plt.figure(figsize=(14, 11))
        self.fig.suptitle(
            "Quantum Wave Packet - Wave-Particle Duality",
            fontsize=16,
            fontweight="bold",
        )

        # Position space - real and imaginary parts
        self.ax_position = plt.subplot(3, 2, 1)
        self.ax_position.set_xlim(-10, 10)
        self.ax_position.set_ylim(-1.5, 1.5)
        self.ax_position.set_xlabel("Position x (m)")
        self.ax_position.set_ylabel("ψ(x)")
        self.ax_position.set_title("Wave Function (Position Space)")
        self.ax_position.grid(True, alpha=0.3)

        # Probability density |ψ|²
        self.ax_prob = plt.subplot(3, 2, 2)
        self.ax_prob.set_xlim(-10, 10)
        self.ax_prob.set_ylim(0, 1)
        self.ax_prob.set_xlabel("Position x (m)")
        self.ax_prob.set_ylabel("|ψ(x)|²")
        self.ax_prob.set_title("Probability Density (Where Particle Is)")
        self.ax_prob.grid(True, alpha=0.3)

        # Momentum space
        self.ax_momentum = plt.subplot(3, 2, 3)
        self.ax_momentum.set_xlim(-15, 15)
        self.ax_momentum.set_ylim(0, 1)
        self.ax_momentum.set_xlabel("Wave Number k (1/m)")
        self.ax_momentum.set_ylabel("|φ(k)|²")
        self.ax_momentum.set_title("Momentum Distribution")
        self.ax_momentum.grid(True, alpha=0.3)

        # Uncertainty principle
        self.ax_uncertainty = plt.subplot(3, 2, 4)
        self.ax_uncertainty.set_xlim(0, 10)
        self.ax_uncertainty.set_ylim(0, 10)
        self.ax_uncertainty.set_xlabel("Position Uncertainty Δx")
        self.ax_uncertainty.set_ylabel("Momentum Uncertainty Δp")
        self.ax_uncertainty.set_title("Heisenberg Uncertainty Principle")
        self.ax_uncertainty.grid(True, alpha=0.3)

        # Draw uncertainty bound
        x_unc = np.linspace(0.1, 10, 100)
        y_unc = self.hbar / (2 * x_unc)
        self.ax_uncertainty.plot(
            x_unc, y_unc, "r--", linewidth=2, label="ΔxΔp = ℏ/2 (minimum)"
        )
        self.ax_uncertainty.fill_between(x_unc, 0, y_unc, alpha=0.2, color="red")
        self.ax_uncertainty.text(
            5, 0.5, "Forbidden\nby QM", ha="center", fontsize=10, color="red"
        )
        self.ax_uncertainty.legend()

        # Time evolution
        self.ax_time = plt.subplot(3, 1, 3)
        self.ax_time.set_xlim(-10, 10)
        self.ax_time.set_ylim(0, 5)
        self.ax_time.set_xlabel("Position x (m)")
        self.ax_time.set_ylabel("Time (s)")
        self.ax_time.set_title("Wave Packet Spreading Over Time")

        # Initialize plots
        (self.line_real,) = self.ax_position.plot(
            [], [], "b-", label="Re(ψ)", linewidth=2
        )
        (self.line_imag,) = self.ax_position.plot(
            [], [], "r-", label="Im(ψ)", alpha=0.5, linewidth=2
        )
        self.ax_position.legend()

        (self.line_prob,) = self.ax_prob.plot([], [], "g-", linewidth=3)
        self.fill_prob = self.ax_prob.fill_between([], [], 0, alpha=0.3, color="green")

        (self.line_momentum,) = self.ax_momentum.plot([], [], "purple", linewidth=2)

        (self.uncertainty_point,) = self.ax_uncertainty.plot(
            [], [], "bo", markersize=15, label="Current State"
        )
        self.ax_uncertainty.legend()

        # Time evolution image
        self.time_data = []
        self.im_time = None

        # Create controls
        self.create_controls()

    def create_controls(self):
        """Create sliders and buttons"""
        plt.subplots_adjust(left=0.1, bottom=0.25, top=0.95, hspace=0.4)

        # Initial position
        ax_x0 = plt.axes([0.15, 0.15, 0.3, 0.02])
        self.slider_x0 = Slider(
            ax_x0, "Position x₀", -5, 5, valinit=self.x0, valstep=0.5
        )
        self.slider_x0.on_changed(self.update_params)

        # Initial momentum
        ax_k0 = plt.axes([0.15, 0.12, 0.3, 0.02])
        self.slider_k0 = Slider(
            ax_k0, "Momentum k₀", 0, 10, valinit=self.k0, valstep=0.5
        )
        self.slider_k0.on_changed(self.update_params)

        # Width (uncertainty)
        ax_sigma = plt.axes([0.15, 0.09, 0.3, 0.02])
        self.slider_sigma = Slider(
            ax_sigma, "Width σ (Δx)", 0.3, 3.0, valinit=self.sigma, valstep=0.1
        )
        self.slider_sigma.on_changed(self.update_params)

        # Pause button
        ax_pause = plt.axes([0.6, 0.12, 0.1, 0.04])
        self.btn_pause = Button(ax_pause, "Pause")
        self.btn_pause.on_clicked(self.toggle_pause)

        # Reset button
        ax_reset = plt.axes([0.72, 0.12, 0.1, 0.04])
        self.btn_reset = Button(ax_reset, "Reset")
        self.btn_reset.on_clicked(self.reset)

    def update_params(self, val):
        """Update parameters"""
        self.x0 = self.slider_x0.val
        self.k0 = self.slider_k0.val
        self.sigma = self.slider_sigma.val
        self.time = 0.0
        self.time_data = []

    def toggle_pause(self, event):
        """Toggle pause"""
        self.paused = not self.paused
        self.btn_pause.label.set_text("Play" if self.paused else "Pause")

    def reset(self, event):
        """Reset simulation"""
        self.time = 0.0
        self.time_data = []
        self.slider_x0.reset()
        self.slider_k0.reset()
        self.slider_sigma.reset()

    def wave_packet(self, x, t):
        """Calculate Gaussian wave packet with time evolution

        ψ(x,t) = (2πσ²)^(-1/4) * exp(-(x-x₀)²/(4σ²(1+it/τ))) * exp(ik₀x - iω₀t)

        where τ = 2mσ²/ℏ is the spreading time scale
        """
        # Spreading time scale
        tau = 2 * self.mass * self.sigma**2 / self.hbar

        # Time-dependent width
        sigma_t = self.sigma * np.sqrt(1 + (t / tau) ** 2)

        # Normalization
        norm = (2 * np.pi * sigma_t**2) ** (-0.25)

        # Gaussian envelope (spreading)
        x_shifted = x - self.x0 - self.hbar * self.k0 * t / self.mass
        gaussian = np.exp(-((x_shifted) ** 2) / (4 * sigma_t**2))

        # Phase evolution
        omega = self.hbar * self.k0**2 / (2 * self.mass)
        phase = self.k0 * x - omega * t

        # Additional phase from spreading
        phase -= (x_shifted) ** 2 * t / (4 * self.mass * sigma_t**2 * tau)

        # Complete wave function
        psi = norm * gaussian * np.exp(1j * phase)

        return psi

    def fourier_transform(self, psi):
        """Calculate momentum space wave function"""
        # FFT to get momentum distribution
        psi_k = fftshift(fft(psi)) * self.dx / np.sqrt(2 * np.pi)
        k = fftshift(fftfreq(len(self.x), self.dx)) * 2 * np.pi

        return k, psi_k

    def calculate_uncertainties(self, x, psi):
        """Calculate position and momentum uncertainties"""
        # Probability density
        prob = np.abs(psi) ** 2
        prob = prob / np.sum(prob)  # Normalize

        # Position expectation and uncertainty
        x_mean = np.sum(x * prob)
        x2_mean = np.sum(x**2 * prob)
        delta_x = np.sqrt(x2_mean - x_mean**2)

        # Momentum space
        k, psi_k = self.fourier_transform(psi)
        prob_k = np.abs(psi_k) ** 2
        prob_k = prob_k / np.sum(prob_k)

        k_mean = np.sum(k * prob_k)
        k2_mean = np.sum(k**2 * prob_k)
        delta_k = np.sqrt(k2_mean - k_mean**2)

        # Convert to momentum uncertainty (p = ℏk)
        delta_p = self.hbar * delta_k

        return delta_x, delta_p

    def animate(self, frame):
        """Animation update"""
        if not self.paused:
            self.time += 0.05

        # Calculate wave packet
        psi = self.wave_packet(self.x, self.time)

        # Update position space plots
        self.line_real.set_data(self.x, np.real(psi))
        self.line_imag.set_data(self.x, np.imag(psi))

        # Probability density
        prob = np.abs(psi) ** 2
        self.line_prob.set_data(self.x, prob)

        # Update fill - remove old fills and add new one
        for coll in self.ax_prob.collections:
            coll.remove()
        self.ax_prob.fill_between(self.x, prob, 0, alpha=0.3, color="green")

        # Auto-scale
        self.ax_prob.set_ylim(0, max(np.max(prob) * 1.1, 0.1))

        # Momentum space
        k, psi_k = self.fourier_transform(psi)
        prob_k = np.abs(psi_k) ** 2

        # Only plot relevant k range
        k_mask = (k >= -15) & (k <= 15)
        self.line_momentum.set_data(k[k_mask], prob_k[k_mask])
        if np.max(prob_k[k_mask]) > 0:
            self.ax_momentum.set_ylim(0, np.max(prob_k[k_mask]) * 1.1)

        # Uncertainty principle
        delta_x, delta_p = self.calculate_uncertainties(self.x, psi)
        self.uncertainty_point.set_data([delta_x], [delta_p])

        # Add text showing product - remove old text first
        for txt in self.ax_uncertainty.texts[:]:
            if txt.get_text() != "Forbidden\nby QM" or "ΔxΔp" in txt.get_text():
                txt.remove()

        self.ax_uncertainty.text(
            5, 0.5, "Forbidden\nby QM", ha="center", fontsize=10, color="red"
        )
        product = delta_x * delta_p
        ratio = product / (self.hbar / 2)
        self.ax_uncertainty.text(
            8,
            8,
            f"ΔxΔp = {product:.2f}\n= {ratio:.2f} × (ℏ/2)",
            fontsize=10,
            bbox=dict(boxstyle="round", facecolor="wheat", alpha=0.8),
        )

        # Time evolution visualization
        if len(self.time_data) < 100:
            self.time_data.append(prob)
        else:
            self.time_data.pop(0)
            self.time_data.append(prob)

        if len(self.time_data) > 1:
            time_array = np.array(self.time_data)
            if self.im_time is not None:
                self.im_time.remove()
            self.im_time = self.ax_time.imshow(
                time_array,
                aspect="auto",
                extent=[-10, 10, max(0, self.time - 5), self.time],
                cmap="hot",
                origin="lower",
                interpolation="bilinear",
            )
            self.ax_time.set_ylim(max(0, self.time - 5), self.time + 0.1)

        return (
            self.line_real,
            self.line_imag,
            self.line_prob,
            self.line_momentum,
            self.uncertainty_point,
        )

    def run(self):
        """Start simulation"""
        ani = FuncAnimation(
            self.fig, self.animate, interval=50, blit=False, cache_frame_data=False
        )
        plt.show()


def main():
    print("=" * 60)
    print("Quantum Wave Packet Simulator")
    print("=" * 60)
    print("\nWave-Particle Duality Demonstration:")
    print("  • See how a localized 'particle' emerges from waves")
    print("  • Watch the Heisenberg uncertainty principle in action")
    print("  • Observe wave packet spreading over time")
    print("\nWhat You're Seeing:")
    print("  • Position Space: The quantum wave function ψ(x)")
    print("  • Probability: Where the particle is likely to be found")
    print("  • Momentum: The distribution of possible momenta")
    print("  • Uncertainty: ΔxΔp must be ≥ ℏ/2 (quantum limit)")
    print("  • Time Evolution: How the packet spreads out")
    print("\nKey Insights:")
    print("  • Narrow position → wide momentum (high uncertainty)")
    print("  • Wide position → narrow momentum (low uncertainty)")
    print("  • Wave packets ALWAYS spread over time (quantum mechanics!)")
    print("  • The uncertainty product is ALWAYS above ℏ/2")
    print("\nControls:")
    print("  • Position x₀: Where packet starts")
    print("  • Momentum k₀: How fast it moves")
    print("  • Width σ: Position uncertainty (try small vs large!)")
    print("\nTry This:")
    print("  1. Set σ very small → see momentum spread wide")
    print("  2. Set σ large → see momentum localized")
    print("  3. Watch the packet spread over time")
    print("  4. Change k₀ → see packet move left/right")
    print("=" * 60)
    print()

    sim = QuantumWavePacket()
    sim.run()


if __name__ == "__main__":
    main()
