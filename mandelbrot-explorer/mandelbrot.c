/*
 * mandelbrot.c — Interactive Terminal Mandelbrot Set Explorer
 * ============================================================
 *
 * WHAT THIS TEACHES:
 *   - Complex number arithmetic (z = z² + c iteration)
 *   - The Mandelbrot set: boundary between convergence and divergence
 *   - ANSI 256-color terminal rendering
 *   - Terminal raw mode (non-canonical input, no echo)
 *   - Escape sequence parsing for arrow keys
 *   - Coordinate mapping (screen pixels ↔ complex plane)
 *   - Adaptive iteration depth for detail at high zoom
 *
 * CONTROLS:
 *   Arrow keys  — Pan around the complex plane
 *   +/=         — Zoom in (2×)
 *   -           — Zoom out (2×)
 *   ]           — Increase max iterations (more detail, slower)
 *   [           — Decrease max iterations (less detail, faster)
 *   r           — Reset to default view
 *   c           — Cycle color palette
 *   q / Esc     — Quit
 *
 * BUILD:
 *   gcc -O2 -o mandelbrot mandelbrot.c -lm
 *   # or just: make
 *
 * THE MATH:
 *   For each pixel (mapped to complex number c = a + bi):
 *     Start with z = 0
 *     Repeatedly compute z = z² + c
 *     If |z| > 2 (escape radius), the point ESCAPES → color by iteration count
 *     If we hit max_iterations without escaping → point is IN the set → color black
 *
 *   z² where z = x + yi:
 *     z² = (x + yi)² = x² - y² + 2xyi
 *     So: new_x = x² - y²  + c_real
 *         new_y = 2xy       + c_imag
 *
 *   The "escape radius" of 2 works because if |z| > 2, the sequence
 *   is mathematically guaranteed to diverge to infinity.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

/* ─── Terminal State ─────────────────────────────────────────────── */

static struct termios orig_termios;
static int term_rows, term_cols;
static volatile sig_atomic_t needs_resize = 1;

/* Suppress warn_unused_result for terminal writes (non-recoverable) */
static inline void term_write(const char *buf, size_t len)
{
    if (write(STDOUT_FILENO, buf, len)) { /* intentionally ignored */ }
}

/*
 * Raw mode: disable canonical input (line buffering) and echo.
 * This lets us read individual keystrokes immediately.
 *
 * WHY: Normal terminals wait for Enter before sending input.
 * For an interactive explorer, we need instant key response.
 */
static void enable_raw_mode(void)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;

    /* Input: no break signal, no CR→NL, no parity, no strip, no flow ctrl */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* Output: keep output processing (we need \n → \r\n) */
    /* raw.c_oflag &= ~(OPOST); — we leave this ON */
    /* Control: 8-bit chars */
    raw.c_cflag |= CS8;
    /* Local: no echo, no canonical mode, no signals, no extended */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    /* Read returns after 1 byte, with 100ms timeout */
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 1;  /* 100ms timeout — allows responsive resize */

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/*
 * Query terminal dimensions using ioctl.
 * We subtract 2 from rows to leave room for the status bar.
 */
static void update_term_size(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        term_cols = ws.ws_col;
        term_rows = ws.ws_row - 2;  /* Reserve 2 lines for status */
        if (term_rows < 4) term_rows = 4;
        if (term_cols < 10) term_cols = 10;
    } else {
        term_cols = 80;
        term_rows = 22;
    }
    needs_resize = 0;
}

static void handle_sigwinch(int sig)
{
    (void)sig;
    needs_resize = 1;
}

/* ─── Color Palettes ─────────────────────────────────────────────── */

/*
 * ANSI 256-color: codes 16-231 form a 6×6×6 color cube.
 * Formula: 16 + 36*r + 6*g + b  where r,g,b ∈ [0,5]
 *
 * We define several palettes that map iteration count → color.
 * Each palette is a function: (iteration, max_iter) → ANSI color code.
 */

#define NUM_PALETTES 5
static int current_palette = 0;

static const char *palette_names[] = {
    "Classic Blue",
    "Fire",
    "Rainbow HSV",
    "Ice",
    "Monochrome"
};

/*
 * Map a normalized value t ∈ [0,1] to an ANSI 256 color code.
 * Uses the 6×6×6 color cube (codes 16-231).
 */
static int rgb_to_ansi(int r, int g, int b)
{
    /* Clamp to [0,5] range for the ANSI color cube */
    if (r < 0) r = 0;
    if (r > 5) r = 5;
    if (g < 0) g = 0;
    if (g > 5) g = 5;
    if (b < 0) b = 0;
    if (b > 5) b = 5;
    return 16 + 36 * r + 6 * g + b;
}

static int palette_classic(int iter, int max_iter)
{
    /* Deep blue → cyan → white progression */
    double t = (double)iter / max_iter;
    int r = (int)(t * 2 * 5) % 6;
    int g = (int)(t * 3 * 5) % 6;
    int b = (int)((0.5 + t * 0.5) * 5);
    return rgb_to_ansi(r, g, b);
}

static int palette_fire(int iter, int max_iter)
{
    /* Black → red → orange → yellow → white */
    double t = (double)iter / max_iter;
    int r, g, b;
    if (t < 0.33) {
        double s = t / 0.33;
        r = (int)(s * 5); g = 0; b = 0;
    } else if (t < 0.66) {
        double s = (t - 0.33) / 0.33;
        r = 5; g = (int)(s * 5); b = 0;
    } else {
        double s = (t - 0.66) / 0.34;
        r = 5; g = 5; b = (int)(s * 5);
    }
    return rgb_to_ansi(r, g, b);
}

static int palette_rainbow(int iter, int max_iter)
{
    /*
     * HSV color wheel: cycle hue through full spectrum.
     * This creates the classic "psychedelic" Mandelbrot look.
     *
     * HSV→RGB with S=1, V=1:
     *   H in [0,6), divide into 6 sectors
     */
    double h = fmod((double)iter / max_iter * 6.0, 6.0);
    int hi = (int)h;
    double f = h - hi;
    int v = 5;
    int p = 0;
    int q = (int)((1.0 - f) * 5);
    int t_val = (int)(f * 5);

    switch (hi % 6) {
        case 0: return rgb_to_ansi(v, t_val, p);
        case 1: return rgb_to_ansi(q, v, p);
        case 2: return rgb_to_ansi(p, v, t_val);
        case 3: return rgb_to_ansi(p, q, v);
        case 4: return rgb_to_ansi(t_val, p, v);
        case 5: return rgb_to_ansi(v, p, q);
    }
    return 16; /* fallback: black */
}

static int palette_ice(int iter, int max_iter)
{
    /* White → cyan → blue → dark blue */
    double t = (double)iter / max_iter;
    int r = (int)((1.0 - t) * 5);
    int g = (int)((1.0 - t * 0.5) * 5);
    int b = 5;
    return rgb_to_ansi(r, g, b);
}

static int palette_mono(int iter, int max_iter)
{
    /* Greyscale using the 24-step grey ramp (codes 232-255) */
    int shade = (int)((double)iter / max_iter * 23);
    if (shade < 0) shade = 0;
    if (shade > 23) shade = 23;
    return 232 + shade;
}

typedef int (*palette_fn)(int iter, int max_iter);
static palette_fn palettes[] = {
    palette_classic,
    palette_fire,
    palette_rainbow,
    palette_ice,
    palette_mono
};

/* ─── Mandelbrot Computation ─────────────────────────────────────── */

/*
 * The core algorithm. For a point c on the complex plane:
 *   z₀ = 0
 *   z_{n+1} = z_n² + c
 *
 * Returns iteration count at which |z| > 2 (escaped),
 * or max_iter if it never escapes (point is in the set).
 *
 * We also return a smooth iteration count for better coloring:
 *   smooth = iter + 1 - log2(log2(|z|))
 * This eliminates the harsh "banding" between iteration levels.
 */
typedef struct {
    int iterations;
    double smooth;
} mandelbrot_result;

static mandelbrot_result compute_mandelbrot(double cr, double ci, int max_iter)
{
    mandelbrot_result result;
    double zr = 0.0, zi = 0.0;
    double zr2, zi2;
    int i;

    for (i = 0; i < max_iter; i++) {
        zr2 = zr * zr;
        zi2 = zi * zi;

        /* |z|² > 4 means |z| > 2  — escaped! */
        if (zr2 + zi2 > 4.0) {
            /*
             * Smooth coloring: instead of harsh integer bands,
             * we interpolate using the escape magnitude.
             *
             * The formula log2(log2(|z|)) gives us a fractional
             * iteration count that varies smoothly.
             */
            double log_zn = log(zr2 + zi2) / 2.0;  /* log(|z|)  */
            double nu = log(log_zn / log(2.0)) / log(2.0);
            result.iterations = i;
            result.smooth = (double)i + 1.0 - nu;
            return result;
        }

        /* z = z² + c
         * Remember: (a+bi)² = a²-b² + 2abi */
        zi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
    }

    /* Never escaped — this point is IN the Mandelbrot set */
    result.iterations = max_iter;
    result.smooth = (double)max_iter;
    return result;
}

/* ─── Rendering ──────────────────────────────────────────────────── */

/*
 * We use a large output buffer to minimize write() syscalls.
 * Writing the entire frame at once prevents flickering.
 *
 * Each cell: up to ~20 bytes for ANSI color + char + reset
 * Max buffer: rows × cols × 30 bytes (generous estimate)
 */
#define MAX_BUF (300 * 400 * 30)
static char render_buf[MAX_BUF];

/*
 * Block characters for "half-pixel" rendering:
 * Each terminal cell shows TWO rows of the fractal using the
 * Unicode upper-half-block character (▀). The foreground color
 * represents the top pixel, background color the bottom pixel.
 * This doubles our effective vertical resolution!
 */
static const char *HALF_BLOCK = "\xe2\x96\x80"; /* UTF-8 for ▀ */

static void render_frame(double center_r, double center_i, double zoom,
                         int max_iter)
{
    int buf_pos = 0;
    palette_fn get_color = palettes[current_palette];

    /*
     * Coordinate mapping:
     * The "zoom" value represents the half-width of the view in the
     * complex plane. A zoom of 2.0 shows c_real ∈ [center-2, center+2].
     *
     * We use half-block rendering, so each terminal row = 2 fractal rows.
     * pixel_height = term_rows × 2 (doubled vertical resolution)
     */
    int pixel_height = term_rows * 2;
    double aspect = (double)term_cols / (double)pixel_height * 0.5;
    double x_range = zoom * 2.0;
    double y_range = x_range / aspect;

    double x_min = center_r - x_range / 2.0;
    double y_min = center_i - y_range / 2.0;
    double x_step = x_range / term_cols;
    double y_step = y_range / pixel_height;

    /* Move cursor to top-left */
    buf_pos += snprintf(render_buf + buf_pos, MAX_BUF - buf_pos,
                        "\033[H");

    for (int row = 0; row < term_rows; row++) {
        /* Two fractal rows per terminal row */
        int py_top = row * 2;
        int py_bot = row * 2 + 1;

        for (int col = 0; col < term_cols; col++) {
            double cr = x_min + col * x_step;
            double ci_top = y_min + py_top * y_step;
            double ci_bot = y_min + py_bot * y_step;

            mandelbrot_result res_top = compute_mandelbrot(cr, ci_top, max_iter);
            mandelbrot_result res_bot = compute_mandelbrot(cr, ci_bot, max_iter);

            int color_top, color_bot;

            if (res_top.iterations >= max_iter) {
                /* In the set: black */
                color_top = 16;  /* ANSI black */
            } else {
                /* Use smooth value for palette lookup */
                int smooth_iter = (int)res_top.smooth % max_iter;
                color_top = get_color(smooth_iter, max_iter);
            }

            if (res_bot.iterations >= max_iter) {
                color_bot = 16;
            } else {
                int smooth_iter = (int)res_bot.smooth % max_iter;
                color_bot = get_color(smooth_iter, max_iter);
            }

            /*
             * ▀ (upper half block):
             *   Foreground color → top half of cell
             *   Background color → bottom half of cell
             *
             * \033[38;5;Xm  = set foreground to 256-color X
             * \033[48;5;Xm  = set background to 256-color X
             */
            buf_pos += snprintf(render_buf + buf_pos, MAX_BUF - buf_pos,
                                "\033[38;5;%dm\033[48;5;%dm%s",
                                color_top, color_bot, HALF_BLOCK);

            if (buf_pos >= MAX_BUF - 100) break;
        }
        /* Reset colors at end of line */
        buf_pos += snprintf(render_buf + buf_pos, MAX_BUF - buf_pos,
                            "\033[0m\r\n");
    }

    /* ─── Status Bar ─────────────────────────────────────────── */

    /* Line 1: coordinates and zoom */
    buf_pos += snprintf(render_buf + buf_pos, MAX_BUF - buf_pos,
        "\033[0m\033[1;36m"  /* bold cyan */
        " Center: %.14f %+.14fi  Zoom: %.2e  Iter: %d  "
        "\033[0m\r\n",
        center_r, center_i, 2.0 / zoom, max_iter);

    /* Line 2: controls */
    buf_pos += snprintf(render_buf + buf_pos, MAX_BUF - buf_pos,
        "\033[0m\033[1;33m"  /* bold yellow */
        " [Arrows]Pan [+/-]Zoom []/[]Iter [c]Palette:%s [r]Reset [q]Quit"
        "\033[0m",
        palette_names[current_palette]);

    /* Write entire frame at once — no flicker */
    term_write(render_buf, buf_pos);
}

/* ─── Input Handling ─────────────────────────────────────────────── */

typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ZOOM_IN,
    KEY_ZOOM_OUT,
    KEY_ITER_UP,
    KEY_ITER_DOWN,
    KEY_RESET,
    KEY_PALETTE,
    KEY_QUIT,
} key_action;

/*
 * Read a keypress. Arrow keys come as escape sequences:
 *   ESC [ A = Up,  ESC [ B = Down,  ESC [ C = Right,  ESC [ D = Left
 *
 * We use non-blocking reads (VMIN=0, VTIME=1) so we can
 * detect resize events between keypresses.
 */
static key_action read_key(void)
{
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return KEY_NONE;

    if (c == 27) {  /* ESC */
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return KEY_QUIT;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return KEY_QUIT;
        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
        return KEY_NONE;
    }

    switch (c) {
        case '+': case '=': return KEY_ZOOM_IN;
        case '-': case '_': return KEY_ZOOM_OUT;
        case ']':           return KEY_ITER_UP;
        case '[':           return KEY_ITER_DOWN;
        case 'r': case 'R': return KEY_RESET;
        case 'c': case 'C': return KEY_PALETTE;
        case 'q': case 'Q': return KEY_QUIT;
    }
    return KEY_NONE;
}

/* ─── Interesting Locations ──────────────────────────────────────── */

/*
 * A few beautiful spots in the Mandelbrot set to explore.
 * Try zooming into these coordinates!
 */
static const struct { double r, i; const char *name; } poi[] = {
    { -0.7436447860,  0.1318252536,  "Elephant Valley"       },
    { -0.1011,        0.9563,        "Seahorse Valley"       },
    { -1.2500,        0.0000,        "Main Bulb Cusp"        },
    {  0.3000,        0.0220,        "Spiral Galaxy"         },
    { -0.7463,        0.1102,        "Double Spiral"         },
    { -1.7490863,     0.0000000,     "Antenna Tip"           },
};
#define NUM_POI (sizeof(poi) / sizeof(poi[0]))

/* ─── Main ───────────────────────────────────────────────────────── */

int main(void)
{
    /* Default view: centered slightly left to show the full set */
    double center_r = -0.5;
    double center_i =  0.0;
    double zoom     =  2.0;   /* Half-width in complex plane */
    int max_iter    =  100;

    /* Setup terminal */
    enable_raw_mode();
    atexit(disable_raw_mode);

    /* Handle terminal resize */
    struct sigaction sa;
    sa.sa_handler = handle_sigwinch;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGWINCH, &sa, NULL);

    /* Hide cursor and clear screen */
    term_write("\033[?25l\033[2J", 10);

    /* Print points of interest on first run */
    update_term_size();
    printf("\033[H\033[1;35m");
    printf("  ╔══════════════════════════════════════════════╗\r\n");
    printf("  ║   Mandelbrot Set Explorer                   ║\r\n");
    printf("  ║   Interesting coordinates to visit:          ║\r\n");
    for (int i = 0; i < (int)NUM_POI; i++) {
        printf("  ║   %-20s (%+.4f, %+.4f)   ║\r\n",
               poi[i].name, poi[i].r, poi[i].i);
    }
    printf("  ║                                              ║\r\n");
    printf("  ║   Press any key to start exploring...        ║\r\n");
    printf("  ╚══════════════════════════════════════════════╝\r\n");
    printf("\033[0m");

    /* Wait for a keypress */
    {
        char dummy;
        while (read(STDIN_FILENO, &dummy, 1) != 1)
            ;
    }

    /* Clear and render first frame */
    term_write("\033[2J", 4);

    int running = 1;
    int needs_render = 1;

    while (running) {
        if (needs_resize) {
            update_term_size();
            term_write("\033[2J", 4);
            needs_render = 1;
        }

        if (needs_render) {
            render_frame(center_r, center_i, zoom, max_iter);
            needs_render = 0;
        }

        key_action key = read_key();
        double pan_amount = zoom * 0.15;  /* Pan 15% of view width */

        switch (key) {
            case KEY_UP:
                center_i -= pan_amount;
                needs_render = 1;
                break;
            case KEY_DOWN:
                center_i += pan_amount;
                needs_render = 1;
                break;
            case KEY_LEFT:
                center_r -= pan_amount;
                needs_render = 1;
                break;
            case KEY_RIGHT:
                center_r += pan_amount;
                needs_render = 1;
                break;
            case KEY_ZOOM_IN:
                zoom *= 0.5;  /* Halve the view = 2× zoom */
                /* Auto-increase iterations when zooming deep */
                if (max_iter < 5000 && zoom < 0.001) {
                    max_iter = (int)(100 + 50 * log2(2.0 / zoom));
                    if (max_iter > 5000) max_iter = 5000;
                }
                needs_render = 1;
                break;
            case KEY_ZOOM_OUT:
                zoom *= 2.0;
                if (zoom > 10.0) zoom = 10.0;
                needs_render = 1;
                break;
            case KEY_ITER_UP:
                max_iter += 50;
                if (max_iter > 10000) max_iter = 10000;
                needs_render = 1;
                break;
            case KEY_ITER_DOWN:
                max_iter -= 50;
                if (max_iter < 10) max_iter = 10;
                needs_render = 1;
                break;
            case KEY_RESET:
                center_r = -0.5;
                center_i =  0.0;
                zoom     =  2.0;
                max_iter =  100;
                needs_render = 1;
                break;
            case KEY_PALETTE:
                current_palette = (current_palette + 1) % NUM_PALETTES;
                needs_render = 1;
                break;
            case KEY_QUIT:
                running = 0;
                break;
            case KEY_NONE:
                break;
        }
    }

    /* Cleanup: show cursor, reset colors, clear screen */
    term_write("\033[?25h\033[0m\033[2J\033[H", 17);
    printf("Thanks for exploring! 🌀\n");
    printf("Try zooming into Elephant Valley: (-0.7436, 0.1318)\n");

    return 0;
}
