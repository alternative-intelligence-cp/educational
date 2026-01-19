# Educational Examples and Experiments

This guide provides step-by-step experiments to help you understand wave phenomena through hands-on exploration.

## Experiment 1: Constructive vs Destructive Interference

**Goal**: Understand how waves can add to make bigger waves or cancel each other out.

**Tool**: `wave_sim_1d.py`

**Steps**:
1. Run the simulator
2. Set both frequencies to 2.0 Hz
3. Set both amplitudes to 1.0
4. **Constructive interference**: Set both phases to 0.0
   - Watch: The waves add together to make amplitude 2.0!
5. **Destructive interference**: Set phase 2 to π (3.14)
   - Watch: The waves cancel completely! Amplitude ≈ 0.0
6. **Partial interference**: Slowly adjust phase 2 from 0 to 2π
   - Watch: See the superposition smoothly transition from constructive to destructive

**What You Learned**:
- When wave peaks align → constructive interference (louder sound, brighter light)
- When peaks meet troughs → destructive interference (silence, darkness)
- Phase difference controls interference type

---

## Experiment 2: Beat Frequencies

**Goal**: Discover why slightly out-of-tune instruments create that "wah-wah" sound.

**Tool**: `wave_sim_1d.py`

**Steps**:
1. Select "Beat Frequency" mode from radio buttons
2. Default: freq1=2.0 Hz, freq2=2.2 Hz
3. Watch the "Interference Over Time" plot
   - See the slow amplitude variation? That's the beat!
4. Calculate beat frequency: |f2 - f1| = |2.2 - 2.0| = 0.2 Hz
   - The amplitude maxima occur 5 times per second
5. Try making the frequencies closer: freq1=2.0, freq2=2.05
   - Beat frequency = 0.05 Hz (slower wah-wah)
6. Try frequencies farther apart: freq1=2.0, freq2=3.0
   - Beat frequency = 1.0 Hz (faster beating)

**Real-World Applications**:
- Musicians tune instruments by listening for beats to disappear
- Radio receivers use "heterodyning" (beat frequencies) to tune stations
- Astronomers detect binary stars by observing light beats

---

## Experiment 3: Standing Waves

**Goal**: See how reflected waves create stable patterns (like guitar strings).

**Tool**: `wave_sim_1d.py`

**Steps**:
1. Select "Standing Wave" mode
2. Both frequencies = 2.0 Hz, opposite phases (phase difference = π)
3. Watch the main plot carefully
   - Notice some positions NEVER move (nodes)
   - Other positions oscillate maximally (antinodes)
4. Change frequency to 3.0 Hz
   - More nodes appear! (Higher harmonic)
5. Try 1.0 Hz, then 4.0 Hz, then 5.0 Hz
   - Count the nodes: frequency = (number of wavelengths)

**Why This Matters**:
- Guitar strings vibrate in standing wave patterns
- Organ pipes create standing sound waves
- Microwave ovens have standing wave patterns (that's why food heats unevenly!)
- Quantum mechanics: electrons in atoms exist in standing wave patterns

---

## Experiment 4: The Double-Slit Experiment

**Goal**: Recreate one of the most important experiments in physics history.

**Tool**: `wave_sim_2d.py`

**Steps**:
1. Run the 2D simulator
2. Default: 2 sources (slits) separated by 2.0 m
3. Look at the "Detection Screen" plot on the right
   - See the bright and dark fringes? That's the interference pattern!
4. Increase slit separation to 4.0 m
   - Fringes get closer together
5. Decrease to 1.0 m
   - Fringes spread out
6. Change wavelength to 0.5 m
   - Fringes get closer (smaller wavelength → smaller pattern)
7. Change wavelength to 1.5 m
   - Fringes spread out (larger wavelength → larger pattern)

**The Mind-Blowing Part**:
In quantum mechanics, this works with **single photons** or **electrons**. Each particle somehow "interferes with itself" and creates the pattern. This proved that particles are waves!

**Formula**: 
- Fringe spacing ∝ wavelength / slit separation
- d·sin(θ) = m·λ (where m = 0, 1, 2, ... for bright fringes)

---

## Experiment 5: Diffraction Grating

**Goal**: See how multiple slits create sharper interference patterns.

**Tool**: `wave_sim_2d.py`

**Steps**:
1. Set number of sources to 3
   - Pattern becomes sharper with secondary fringes
2. Set to 4 sources
   - Even sharper main peaks
3. Set to 5 sources
   - Very sharp peaks with multiple secondary fringes between them

**Real-World Use**:
- Spectrometers use diffraction gratings to separate light into colors
- CD/DVD surfaces act as diffraction gratings (rainbow effect)
- X-ray crystallography (how we discovered DNA structure)

---

## Experiment 6: Heisenberg Uncertainty Principle

**Goal**: See why you can't know both position AND momentum precisely.

**Tool**: `wave_quantum.py`

**Steps**:
1. Run the quantum simulator
2. **Experiment A - Narrow packet**:
   - Set width σ = 0.5 (small)
   - Look at "Probability Density": Very localized (know position well)
   - Look at "Momentum Distribution": Very spread out (don't know momentum)
   - Check uncertainty plot: Point is above the red line (allowed)
3. **Experiment B - Wide packet**:
   - Set width σ = 2.5 (large)
   - Probability: Spread out (position uncertain)
   - Momentum: Narrow peak (momentum well-known)
   - Uncertainty plot: Still above red line (always!)
4. **Try to break physics** (you can't!):
   - Try any combination of parameters
   - The uncertainty point NEVER goes below the red line
   - This is a fundamental law of nature!

**Why This Happens**:
- Position and momentum are **Fourier transform pairs**
- Narrow in one domain = wide in the other domain
- This isn't measurement error - it's built into reality itself

---

## Experiment 7: Wave Packet Spreading

**Goal**: See why quantum particles "spread out" over time.

**Tool**: `wave_quantum.py`

**Steps**:
1. Set width σ = 1.0, momentum k₀ = 5.0
2. Watch the "Wave Packet Spreading Over Time" plot
   - The packet starts narrow and gets wider
3. Set σ = 0.5 (narrower initial packet)
   - Spreads FASTER! (Narrower → more momentum uncertainty → faster spreading)
4. Set σ = 2.0 (wider initial packet)
   - Spreads slower (wider → less momentum uncertainty)

**Deep Insight**:
This is why we can't build perfect "particle beams" in quantum mechanics. Even if you start with a perfectly localized particle, it spreads out due to its inherent momentum uncertainty. This is fundamental to quantum mechanics, not engineering limitations!

---

## Experiment 8: Phase Velocity vs Group Velocity

**Goal**: Understand the difference between wave motion and energy motion.

**Tool**: `wave_sim_1d.py`

**Steps**:
1. Set freq1 = 2.0 Hz, freq2 = 2.2 Hz
2. Set amp1 = amp2 = 1.0
3. Watch the main plot carefully:
   - **Phase velocity**: Watch individual wave crests - they move fast
   - **Group velocity**: Watch the envelope (beat pattern) - it moves slower!
4. Watch the "Interference Over Time" plot
   - The beat pattern shows the group velocity

**Why This Matters**:
- Information travels at group velocity, not phase velocity
- In some materials, phase velocity can exceed the speed of light (but information still can't)
- This is crucial for understanding fiber optics and signal transmission

---

## Challenge Problems

### Challenge 1: Mystery Frequency
Your friend plays two tuning forks together and you hear a beat frequency of 3 Hz. One fork is labeled 440 Hz (concert A). What are the two possible frequencies of the second fork?

**Answer**: Either 437 Hz or 443 Hz (440 ± 3)

### Challenge 2: Guitar String
A guitar string vibrates in its 3rd harmonic (3 half-wavelengths fit on the string). If the string is 65 cm long, what is the wavelength?

**Answer**: 3λ/2 = 65 cm, so λ = 43.3 cm

### Challenge 3: Double-Slit
In the double-slit experiment, slits are 0.1 mm apart. Using red light (λ = 650 nm), how far apart are the bright fringes on a screen 2 meters away?

**Answer**: θ ≈ λ/d = 6.5×10⁻³ rad, so spacing ≈ 13 mm

### Challenge 4: Quantum Electron
An electron is confined to a box of width 1 nanometer. Estimate its minimum kinetic energy using the uncertainty principle.

**Hint**: Δx ≈ 1 nm, so Δp ≥ ℏ/(2Δx), then E = p²/(2m)

---

## Next Steps

After mastering these experiments:
1. Try combining multiple concepts in one simulation
2. Predict what will happen before adjusting parameters
3. Connect to real-world phenomena you've observed
4. Read about the history: Young's double-slit (1801), de Broglie matter waves (1924), Heisenberg uncertainty (1927)

**Remember**: Physics is learned by doing, not just reading. Play with these simulations until the behavior becomes intuitive!
