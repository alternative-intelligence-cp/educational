#!/usr/bin/env python3
"""
1D Wave Interference Simulator
Educational tool for understanding wave superposition and interference

Features:
- Real-time parameter adjustment with sliders
- Multiple wave sources
- Phase relationship visualization
- Constructive/destructive interference demonstration
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, RadioButtons
from matplotlib.animation import FuncAnimation


class Wave1DSimulator:
    def __init__(self):
        # Spatial domain
        self.x = np.linspace(0, 10, 1000)

        # Initial wave parameters
        self.freq1 = 2.0  # Hz
        self.freq2 = 2.0  # Hz
        self.amp1 = 1.0  # Amplitude
        self.amp2 = 1.0  # Amplitude
        self.phase1 = 0.0  # radians
        self.phase2 = 0.0  # radians
        self.time = 0.0  # Current time

        # Simulation mode
        self.mode = "two_waves"  # 'two_waves', 'beats', 'standing'
        self.paused = False

        # Setup the figure and axes
        self.setup_figure()

    def setup_figure(self):
        """Create the interactive figure with subplots and controls"""
        self.fig = plt.figure(figsize=(14, 10))
        self.fig.suptitle(
            "1D Wave Interference Simulator", fontsize=16, fontweight="bold"
        )

        # Main plot area - individual waves and superposition
        self.ax_main = plt.subplot(3, 1, 1)
        self.ax_main.set_xlim(0, 10)
        self.ax_main.set_ylim(-3, 3)
        self.ax_main.set_xlabel("Position (m)")
        self.ax_main.set_ylabel("Amplitude")
        self.ax_main.set_title("Wave Superposition")
        self.ax_main.grid(True, alpha=0.3)

        # Phase diagram
        self.ax_phase = plt.subplot(3, 2, 3)
        self.ax_phase.set_xlim(-1.5, 1.5)
        self.ax_phase.set_ylim(-1.5, 1.5)
        self.ax_phase.set_aspect("equal")
        self.ax_phase.set_title("Phase Relationship")
        self.ax_phase.grid(True, alpha=0.3)

        # Interference pattern (time evolution at center)
        self.ax_interference = plt.subplot(3, 2, 4)
        self.ax_interference.set_xlim(0, 10)
        self.ax_interference.set_ylim(-3, 3)
        self.ax_interference.set_xlabel("Time (s)")
        self.ax_interference.set_ylabel("Amplitude at x=5m")
        self.ax_interference.set_title("Interference Over Time")
        self.ax_interference.grid(True, alpha=0.3)

        # Initialize plots
        (self.line_wave1,) = self.ax_main.plot(
            [], [], "b-", alpha=0.5, label="Wave 1", linewidth=2
        )
        (self.line_wave2,) = self.ax_main.plot(
            [], [], "r-", alpha=0.5, label="Wave 2", linewidth=2
        )
        (self.line_sum,) = self.ax_main.plot(
            [], [], "g-", label="Superposition", linewidth=3
        )
        self.ax_main.legend(loc="upper right")

        # Phase vectors
        (self.phase_vec1,) = self.ax_phase.plot(
            [0, 0], [0, 0], "b-", linewidth=3, label="Wave 1"
        )
        (self.phase_vec2,) = self.ax_phase.plot(
            [0, 0], [0, 0], "r-", linewidth=3, label="Wave 2"
        )
        (self.phase_sum,) = self.ax_phase.plot(
            [0, 0], [0, 0], "g--", linewidth=2, label="Sum"
        )
        circle = plt.Circle((0, 0), 1, fill=False, linestyle="--", alpha=0.3)
        self.ax_phase.add_patch(circle)
        self.ax_phase.legend(loc="upper right", fontsize=8)

        # Time history
        self.time_history = []
        self.amplitude_history = []
        (self.line_history,) = self.ax_interference.plot([], [], "g-", linewidth=2)

        # Create sliders
        self.create_sliders()

        # Create buttons
        self.create_buttons()

    def create_sliders(self):
        """Create interactive sliders for wave parameters"""
        plt.subplots_adjust(left=0.1, bottom=0.35)

        # Frequency 1
        ax_freq1 = plt.axes([0.15, 0.25, 0.3, 0.02])
        self.slider_freq1 = Slider(
            ax_freq1, "Freq 1 (Hz)", 0.1, 5.0, valinit=self.freq1, valstep=0.1
        )
        self.slider_freq1.on_changed(self.update_params)

        # Frequency 2
        ax_freq2 = plt.axes([0.15, 0.22, 0.3, 0.02])
        self.slider_freq2 = Slider(
            ax_freq2, "Freq 2 (Hz)", 0.1, 5.0, valinit=self.freq2, valstep=0.1
        )
        self.slider_freq2.on_changed(self.update_params)

        # Amplitude 1
        ax_amp1 = plt.axes([0.15, 0.19, 0.3, 0.02])
        self.slider_amp1 = Slider(
            ax_amp1, "Amp 1", 0.0, 2.0, valinit=self.amp1, valstep=0.1
        )
        self.slider_amp1.on_changed(self.update_params)

        # Amplitude 2
        ax_amp2 = plt.axes([0.15, 0.16, 0.3, 0.02])
        self.slider_amp2 = Slider(
            ax_amp2, "Amp 2", 0.0, 2.0, valinit=self.amp2, valstep=0.1
        )
        self.slider_amp2.on_changed(self.update_params)

        # Phase 1
        ax_phase1 = plt.axes([0.15, 0.13, 0.3, 0.02])
        self.slider_phase1 = Slider(
            ax_phase1, "Phase 1 (rad)", 0, 2 * np.pi, valinit=self.phase1
        )
        self.slider_phase1.on_changed(self.update_params)

        # Phase 2
        ax_phase2 = plt.axes([0.15, 0.10, 0.3, 0.02])
        self.slider_phase2 = Slider(
            ax_phase2, "Phase 2 (rad)", 0, 2 * np.pi, valinit=self.phase2
        )
        self.slider_phase2.on_changed(self.update_params)

    def create_buttons(self):
        """Create control buttons and mode selector"""
        # Play/Pause button
        ax_pause = plt.axes([0.15, 0.05, 0.1, 0.04])
        self.btn_pause = Button(ax_pause, "Pause")
        self.btn_pause.on_clicked(self.toggle_pause)

        # Reset button
        ax_reset = plt.axes([0.26, 0.05, 0.1, 0.04])
        self.btn_reset = Button(ax_reset, "Reset")
        self.btn_reset.on_clicked(self.reset)

        # Mode selector
        ax_mode = plt.axes([0.6, 0.10, 0.15, 0.15])
        self.radio_mode = RadioButtons(
            ax_mode, ("Two Waves", "Beat Frequency", "Standing Wave")
        )
        self.radio_mode.on_clicked(self.change_mode)

    def wave_function(self, x, t, freq, amp, phase):
        """Calculate wave displacement at position x and time t"""
        k = 2 * np.pi * freq  # Wave number (assuming v=1 m/s for simplicity)
        omega = 2 * np.pi * freq  # Angular frequency
        return amp * np.sin(k * x - omega * t + phase)

    def update_params(self, val):
        """Update wave parameters from sliders"""
        self.freq1 = self.slider_freq1.val
        self.freq2 = self.slider_freq2.val
        self.amp1 = self.slider_amp1.val
        self.amp2 = self.slider_amp2.val
        self.phase1 = self.slider_phase1.val
        self.phase2 = self.slider_phase2.val

    def toggle_pause(self, event):
        """Toggle animation pause"""
        self.paused = not self.paused
        self.btn_pause.label.set_text("Play" if self.paused else "Pause")

    def reset(self, event):
        """Reset simulation to initial state"""
        self.time = 0.0
        self.time_history = []
        self.amplitude_history = []
        self.slider_freq1.reset()
        self.slider_freq2.reset()
        self.slider_amp1.reset()
        self.slider_amp2.reset()
        self.slider_phase1.reset()
        self.slider_phase2.reset()

    def change_mode(self, label):
        """Change simulation mode"""
        if label == "Two Waves":
            self.mode = "two_waves"
            # Free control
        elif label == "Beat Frequency":
            self.mode = "beats"
            # Set frequencies close together
            self.slider_freq1.set_val(2.0)
            self.slider_freq2.set_val(2.2)
            self.slider_amp1.set_val(1.0)
            self.slider_amp2.set_val(1.0)
            self.slider_phase1.set_val(0.0)
            self.slider_phase2.set_val(0.0)
        elif label == "Standing Wave":
            self.mode = "standing"
            # Set equal frequencies, opposite direction (phase = π)
            self.slider_freq1.set_val(2.0)
            self.slider_freq2.set_val(2.0)
            self.slider_amp1.set_val(1.0)
            self.slider_amp2.set_val(1.0)
            self.slider_phase1.set_val(0.0)
            self.slider_phase2.set_val(np.pi)

        self.reset(None)

    def animate(self, frame):
        """Animation update function"""
        if not self.paused:
            self.time += 0.05  # Time step

        # Calculate waves
        wave1 = self.wave_function(
            self.x, self.time, self.freq1, self.amp1, self.phase1
        )
        wave2 = self.wave_function(
            self.x, self.time, self.freq2, self.amp2, self.phase2
        )
        wave_sum = wave1 + wave2

        # Update main plot
        self.line_wave1.set_data(self.x, wave1)
        self.line_wave2.set_data(self.x, wave2)
        self.line_sum.set_data(self.x, wave_sum)

        # Update phase diagram (at x=5m)
        x_center = 5.0
        phase1_val = self.wave_function(
            x_center, self.time, self.freq1, self.amp1, self.phase1
        )
        phase2_val = self.wave_function(
            x_center, self.time, self.freq2, self.amp2, self.phase2
        )

        # Convert to unit circle representation
        angle1 = (2 * np.pi * self.freq1 * self.time - self.phase1) % (2 * np.pi)
        angle2 = (2 * np.pi * self.freq2 * self.time - self.phase2) % (2 * np.pi)

        x1, y1 = self.amp1 * np.cos(angle1), self.amp1 * np.sin(angle1)
        x2, y2 = self.amp2 * np.cos(angle2), self.amp2 * np.sin(angle2)

        self.phase_vec1.set_data([0, x1], [0, y1])
        self.phase_vec2.set_data([0, x2], [0, y2])
        self.phase_sum.set_data([0, x1 + x2], [0, y1 + y2])

        # Update time history
        if len(self.time_history) > 200:
            self.time_history.pop(0)
            self.amplitude_history.pop(0)
        self.time_history.append(self.time)
        self.amplitude_history.append(phase1_val + phase2_val)
        self.line_history.set_data(self.time_history, self.amplitude_history)

        # Auto-scale time axis
        if len(self.time_history) > 0:
            self.ax_interference.set_xlim(max(0, self.time - 10), self.time + 0.1)

        return (
            self.line_wave1,
            self.line_wave2,
            self.line_sum,
            self.phase_vec1,
            self.phase_vec2,
            self.phase_sum,
            self.line_history,
        )

    def run(self):
        """Start the interactive simulation"""
        ani = FuncAnimation(
            self.fig, self.animate, interval=50, blit=True, cache_frame_data=False
        )
        plt.show()


def main():
    print("=" * 60)
    print("Wave Interference Simulator - 1D")
    print("=" * 60)
    print("\nEducational Tool for Understanding:")
    print("  • Wave superposition principle")
    print("  • Constructive and destructive interference")
    print("  • Phase relationships")
    print("  • Beat frequencies")
    print("  • Standing waves")
    print("\nControls:")
    print("  • Adjust sliders to change wave parameters")
    print("  • Select preset modes from radio buttons")
    print("  • Pause/Play to freeze the animation")
    print("  • Reset to start over")
    print("\nTry This:")
    print("  1. Set both frequencies equal, vary phase → see interference")
    print("  2. Select 'Beat Frequency' → see amplitude modulation")
    print("  3. Select 'Standing Wave' → see nodes and antinodes")
    print("=" * 60)
    print()

    sim = Wave1DSimulator()
    sim.run()


if __name__ == "__main__":
    main()
