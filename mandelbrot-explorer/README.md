# 🌀 Mandelbrot Set Explorer

An interactive terminal-based Mandelbrot set viewer written in C. Pan, zoom, and explore one of mathematics' most beautiful objects — right in your terminal.

![No screenshots needed — just run it!]

## What You'll Learn

| Concept | Where It Appears |
|---------|-----------------|
| **Complex number arithmetic** | The core `z = z² + c` iteration loop |
| **Convergence vs divergence** | Points inside vs outside the set |
| **Smooth coloring algorithms** | `log2(log2(\|z\|))` interpolation to eliminate banding |
| **ANSI 256-color rendering** | Color cube mapping, greyscale ramps |
| **Half-block pixel doubling** | Using `▀` to get 2× vertical resolution |
| **Terminal raw mode** | `termios` manipulation for instant key input |
| **Signal handling** | `SIGWINCH` for live terminal resize |
| **Escape sequence parsing** | Decoding arrow key sequences (`ESC [ A/B/C/D`) |
| **Coordinate mapping** | Screen pixels ↔ complex plane coordinates |
| **Buffered rendering** | Single `write()` per frame to prevent flicker |

## Quick Start

```bash
make
./mandelbrot
```

## Controls

| Key | Action |
|-----|--------|
| `↑` `↓` `←` `→` | Pan around the complex plane |
| `+` / `=` | Zoom in (2×) |
| `-` | Zoom out (2×) |
| `]` | Increase max iterations (+50, more detail) |
| `[` | Decrease max iterations (-50, faster render) |
| `c` | Cycle through 5 color palettes |
| `r` | Reset to default view |
| `q` / `Esc` | Quit |

## Color Palettes

1. **Classic Blue** — Deep ocean blues with cyan highlights
2. **Fire** — Black → red → orange → yellow → white heat map
3. **Rainbow HSV** — Full spectrum psychedelic cycling
4. **Ice** — White → cyan → deep blue frozen palette
5. **Monochrome** — 24-shade greyscale for the minimalist

## The Math

The Mandelbrot set is the set of complex numbers `c` for which the iteration:

```
z₀ = 0
z_{n+1} = zₙ² + c
```

**does not diverge** (stays bounded). That's it — the entire definition.

### How squaring a complex number works

If `z = x + yi`, then:
```
z² = (x + yi)²
   = x² + 2xyi + (yi)²
   = x² - y² + 2xyi        (because i² = -1)

So:  new_x = x² - y² + c_real
     new_y = 2xy      + c_imag
```

### Why escape radius = 2?

It's mathematically proven that if `|z| > 2` at any point during iteration, the sequence **must** diverge to infinity. So we can stop early — the point is definitely NOT in the set.

### Smooth coloring

Without smoothing, colors form harsh "bands" at each integer iteration count. The smooth coloring formula:

```
smooth_iter = iter + 1 - log₂(log₂(|z|))
```

gives a continuous (fractional) iteration value, producing gorgeous gradient transitions.

## Interesting Places to Visit

| Location | Coordinates | Notes |
|----------|-------------|-------|
| Elephant Valley | `(-0.7436, +0.1318)` | Zoom in deep — you'll see elephants |
| Seahorse Valley | `(-0.1011, +0.9563)` | Spiraling seahorse tails |
| Main Bulb Cusp | `(-1.2500, +0.0000)` | Where the main cardioid meets the bulb |
| Spiral Galaxy | `(+0.3000, +0.0220)` | Infinite spiraling arms |
| Double Spiral | `(-0.7463, +0.1102)` | Two intertwined spirals |
| Antenna Tip | `(-1.7491, +0.0000)` | The leftmost point of the set |

Navigate to these coordinates and zoom in with `+`. Increase iterations with `]` when detail gets muddy at high zoom.

## Implementation Notes

### Half-Block Rendering

Normal terminal characters are roughly 2× taller than wide. By using the Unicode upper-half-block character (`▀`), we can color the top and bottom halves of each cell independently:
- **Foreground color** → top pixel
- **Background color** → bottom pixel

This doubles our effective vertical resolution, giving a much better aspect ratio.

### Buffered Output

The entire frame is composited into a single buffer and written with one `write()` syscall. This prevents the "tearing" you'd get from many small writes, because the terminal processes the entire frame atomically.

### Auto-Scaling Iterations

When you zoom deeper than `10⁻³`, the explorer automatically increases max iterations using:
```
max_iter = 100 + 50 × log₂(2 / zoom)
```
This ensures you see detail at every zoom level without manually adjusting.

## Building Blocks for Further Learning

Once you understand this code, try:

1. **Julia sets** — Same formula, but `c` is fixed and `z₀` varies. Each `c` gives a different Julia set. The Mandelbrot set is literally the "map" of all Julia sets!

2. **Multibrot sets** — Replace `z²` with `zⁿ` for different powers. `z³ + c` gives a 3-fold symmetric set.

3. **Burning Ship fractal** — Use `|Re(z)| + i|Im(z)|` instead of `z`. Produces an eerily ship-shaped fractal.

4. **GPU acceleration** — Each pixel is independent → perfectly parallel. Try porting to CUDA or OpenCL.

5. **Arbitrary precision** — Standard `double` limits zoom to ~10¹⁵. Use GMP or implement your own bignum for infinite zoom.

## Requirements

- GCC (or any C11 compiler)
- A terminal with UTF-8 and 256-color support (nearly all modern terminals)
- Linux/macOS (uses POSIX termios)

## License

Educational use. Part of the AILP Educational Repository.
