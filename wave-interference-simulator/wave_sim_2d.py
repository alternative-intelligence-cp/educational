#!/usr/bin/env python3
"""
2D Wave Interference Simulator
Double-slit experiment and circular wave interference

Demonstrates:
- How point sources create circular waves
- Interference patterns in 2D space
- The famous double-slit experiment
- Diffraction and wave fronts
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.animation import FuncAnimation


class Wave2DSimulator:
    def __init__(self):
        # Create 2D grid
        self.grid_size = 200
        self.x = np.linspace(-5, 5, self.grid_size)
        self.y = np.linspace(-5, 5, self.grid_size)
        self.X, self.Y = np.meshgrid(self.x, self.y)

        # Wave parameters
        self.freq = 2.0  # Frequency
        self.wavelength = 1.0  # Wavelength (λ = v/f, assuming v=1)
        self.time = 0.0
        self.damping = 0.1  # Amplitude decay with distance

        # Source positions (two slits)
        self.source1_pos = np.array([-1.0, 0.0])
        self.source2_pos = np.array([1.0, 0.0])
        self.num_sources = 2

        # Simulation control
        self.paused = False

        # Setup figure
        self.setup_figure()

    def setup_figure(self):
        """Create the interactive visualization"""
        self.fig = plt.figure(figsize=(14, 10))
        self.fig.suptitle(
            "2D Wave Interference - Double Slit Experiment",
            fontsize=16,
            fontweight="bold",
        )

        # Main 2D interference pattern
        self.ax_main = plt.subplot(2, 2, (1, 3))
        self.ax_main.set_xlim(-5, 5)
        self.ax_main.set_ylim(-5, 5)
        self.ax_main.set_xlabel("X (m)")
        self.ax_main.set_ylabel("Y (m)")
        self.ax_main.set_title("Interference Pattern (Top View)")
        self.ax_main.set_aspect("equal")

        # Cross-section at y=0
        self.ax_cross = plt.subplot(2, 2, 2)
        self.ax_cross.set_xlim(-5, 5)
        self.ax_cross.set_ylim(-2, 2)
        self.ax_cross.set_xlabel("X (m)")
        self.ax_cross.set_ylabel("Amplitude")
        self.ax_cross.set_title("Cross-Section at Y=0")
        self.ax_cross.grid(True, alpha=0.3)

        # Intensity pattern on screen (x=4)
        self.ax_screen = plt.subplot(2, 2, 4)
        self.ax_screen.set_xlim(0, 2)
        self.ax_screen.set_ylim(-5, 5)
        self.ax_screen.set_xlabel("Intensity")
        self.ax_screen.set_ylabel("Y (m)")
        self.ax_screen.set_title("Detection Screen (X=4m)")
        self.ax_screen.grid(True, alpha=0.3)

        # Initialize plots
        self.im = self.ax_main.imshow(
            np.zeros((self.grid_size, self.grid_size)),
            extent=[-5, 5, -5, 5],
            cmap="RdBu",
            vmin=-2,
            vmax=2,
            origin="lower",
            interpolation="bilinear",
        )

        # Mark source positions
        (self.source_markers,) = self.ax_main.plot(
            [self.source1_pos[0], self.source2_pos[0]],
            [self.source1_pos[1], self.source2_pos[1]],
            "go",
            markersize=10,
            label="Sources",
        )

        # Screen line
        self.ax_main.axvline(
            x=4, color="yellow", linestyle="--", alpha=0.5, label="Screen"
        )
        self.ax_main.legend(loc="upper left")

        # Cross-section line
        (self.line_cross,) = self.ax_cross.plot([], [], "b-", linewidth=2)

        # Screen intensity
        (self.line_screen,) = self.ax_screen.plot([], [], "r-", linewidth=2)

        # Colorbar
        plt.colorbar(self.im, ax=self.ax_main, label="Amplitude")

        # Create controls
        self.create_controls()

    def create_controls(self):
        """Create sliders and buttons"""
        plt.subplots_adjust(left=0.1, bottom=0.25)

        # Frequency slider
        ax_freq = plt.axes([0.15, 0.15, 0.3, 0.02])
        self.slider_freq = Slider(
            ax_freq, "Frequency (Hz)", 0.5, 5.0, valinit=self.freq, valstep=0.1
        )
        self.slider_freq.on_changed(self.update_params)

        # Slit separation slider
        ax_sep = plt.axes([0.15, 0.12, 0.3, 0.02])
        self.slider_sep = Slider(
            ax_sep, "Slit Separation (m)", 0.5, 4.0, valinit=2.0, valstep=0.1
        )
        self.slider_sep.on_changed(self.update_sources)

        # Wavelength slider
        ax_wave = plt.axes([0.15, 0.09, 0.3, 0.02])
        self.slider_wave = Slider(
            ax_wave, "Wavelength (m)", 0.2, 2.0, valinit=self.wavelength, valstep=0.1
        )
        self.slider_wave.on_changed(self.update_params)

        # Number of sources
        ax_nsources = plt.axes([0.15, 0.06, 0.3, 0.02])
        self.slider_nsources = Slider(
            ax_nsources, "Num Sources", 1, 5, valinit=2, valstep=1
        )
        self.slider_nsources.on_changed(self.update_nsources)

        # Pause button
        ax_pause = plt.axes([0.6, 0.12, 0.1, 0.04])
        self.btn_pause = Button(ax_pause, "Pause")
        self.btn_pause.on_clicked(self.toggle_pause)

        # Reset button
        ax_reset = plt.axes([0.72, 0.12, 0.1, 0.04])
        self.btn_reset = Button(ax_reset, "Reset")
        self.btn_reset.on_clicked(self.reset)

    def update_params(self, val):
        """Update wave parameters"""
        self.freq = self.slider_freq.val
        self.wavelength = self.slider_wave.val

    def update_sources(self, val):
        """Update source positions"""
        separation = self.slider_sep.val
        if self.num_sources == 2:
            self.source1_pos = np.array([-separation / 2, 0.0])
            self.source2_pos = np.array([separation / 2, 0.0])
            self.source_markers.set_data(
                [self.source1_pos[0], self.source2_pos[0]],
                [self.source1_pos[1], self.source2_pos[1]],
            )

    def update_nsources(self, val):
        """Update number of sources"""
        self.num_sources = int(self.slider_nsources.val)
        separation = self.slider_sep.val

        if self.num_sources == 1:
            self.source1_pos = np.array([0.0, 0.0])
            self.source_markers.set_data([self.source1_pos[0]], [self.source1_pos[1]])
        else:
            # Evenly space sources
            positions = np.linspace(-separation / 2, separation / 2, self.num_sources)
            x_pos = positions
            y_pos = np.zeros(self.num_sources)
            self.source_markers.set_data(x_pos, y_pos)

    def toggle_pause(self, event):
        """Toggle pause"""
        self.paused = not self.paused
        self.btn_pause.label.set_text("Play" if self.paused else "Pause")

    def reset(self, event):
        """Reset simulation"""
        self.time = 0.0
        self.slider_freq.reset()
        self.slider_sep.reset()
        self.slider_wave.reset()
        self.slider_nsources.reset()

    def circular_wave(self, source_pos, t):
        """Calculate circular wave from a point source"""
        # Distance from source
        dx = self.X - source_pos[0]
        dy = self.Y - source_pos[1]
        r = np.sqrt(dx**2 + dy**2)

        # Avoid division by zero at source
        r = np.maximum(r, 0.1)

        # Wave number
        k = 2 * np.pi / self.wavelength
        omega = 2 * np.pi * self.freq

        # Circular wave with 1/r amplitude decay
        amplitude = 1.0 / (1 + self.damping * r)
        phase = k * r - omega * t

        return amplitude * np.sin(phase)

    def animate(self, frame):
        """Animation update"""
        if not self.paused:
            self.time += 0.03

        # Calculate interference pattern
        wave_field = np.zeros_like(self.X)

        # Get current source positions
        separation = self.slider_sep.val
        if self.num_sources == 1:
            sources = [np.array([0.0, 0.0])]
        else:
            positions = np.linspace(-separation / 2, separation / 2, self.num_sources)
            sources = [np.array([x, 0.0]) for x in positions]

        # Sum waves from all sources
        for source_pos in sources:
            wave_field += self.circular_wave(source_pos, self.time)

        # Update main plot
        self.im.set_array(wave_field)

        # Update cross-section (y=0)
        y_idx = self.grid_size // 2
        self.line_cross.set_data(self.x, wave_field[y_idx, :])

        # Update screen intensity (x=4)
        x_screen_idx = int((4.0 + 5) / 10 * self.grid_size)
        if x_screen_idx < self.grid_size:
            intensity = wave_field[:, x_screen_idx] ** 2  # Intensity = amplitude^2
            self.line_screen.set_data(intensity, self.y)
            self.ax_screen.set_xlim(0, max(1, np.max(intensity) * 1.1))

        return self.im, self.line_cross, self.line_screen

    def run(self):
        """Start simulation"""
        ani = FuncAnimation(
            self.fig, self.animate, interval=50, blit=False, cache_frame_data=False
        )
        plt.show()


def main():
    print("=" * 60)
    print("2D Wave Interference Simulator")
    print("=" * 60)
    print("\nDouble-Slit Experiment Demonstration:")
    print("  • Watch circular waves from point sources")
    print("  • See constructive/destructive interference")
    print("  • Observe the famous interference pattern on the screen")
    print("\nWhat You're Seeing:")
    print("  • Top View: Wave amplitude in 2D space (red=positive, blue=negative)")
    print("  • Cross-Section: Slice through the pattern at Y=0")
    print("  • Screen: Intensity pattern (bright/dark fringes)")
    print("\nControls:")
    print("  • Adjust frequency → change wave speed")
    print("  • Adjust slit separation → change fringe spacing")
    print("  • Adjust wavelength → change interference pattern")
    print("  • Change number of sources → see diffraction grating")
    print("\nTry This:")
    print("  1. Set 2 sources → classic double-slit pattern")
    print("  2. Set 1 source → single circular wave")
    print("  3. Set 3-5 sources → diffraction grating effect")
    print("  4. Increase wavelength → wider fringes")
    print("=" * 60)
    print()

    sim = Wave2DSimulator()
    sim.run()


if __name__ == "__main__":
    main()
