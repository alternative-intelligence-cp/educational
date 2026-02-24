# bash-tui-utils

Sourced bash utilities for building clean terminal UI output — spinners,
repeaters, and the building blocks you need to make scripts that look intentional
rather than like a wall of text scrolling past.

Part of a small ecosystem also including `bash-bar-utils` (horizontal rules and
boxing) and `bash-math` (float arithmetic via bc).

---

## Files

| File | Purpose |
|---|---|
| `repeat.sh` | Run a command N times, with optional iteration index injection |
| `spinner.sh` | Animated spinner that wraps a real background command |

---

## repeat.sh

A simple loop-as-a-function. The interesting extra: the token `rI` anywhere
in the command or its arguments is replaced with the current zero-based
iteration index before execution.

```bash
source repeat.sh

repeat 3 echo "hello"           # prints hello three times

repeat 5 echo "item rI"         # item 0 / item 1 / item 2 / item 3 / item 4

repeat 4 mkdir -p dir_rI        # creates dir_0 dir_1 dir_2 dir_3

repeat 3 cp "template.txt" "file_rI.txt"   # file_0.txt file_1.txt file_2.txt
```

### What it teaches

1. **`local` scope** — without `local`, every variable set inside a sourced
   function leaks into the caller's environment and persists after the function
   returns. All internal variables here use `local` to prevent that.

2. **`command` is a bash builtin** — using `command` as a variable name silently
   shadows the `command` builtin (which lets you bypass aliases / functions to
   run the real binary). Naming conventions like `_cmd_` avoid the collision.

3. **`eval` vs bare `$var` for command execution**  
   ```bash
   cmd="echo hello world"
   $cmd           # works for simple cases
   $cmd           # BREAKS if any argument contains a space
   eval "$cmd"    # correct — re-parses the string as a full command line
   ```
   The `rI` injection produces a string that has to be re-split into words
   before execution, which is exactly what `eval` does.

4. **`sed -E` token replacement** — the `rI` substitution uses extended regex
   (`-E`) piped from `echo`. Any token that's unlikely to appear naturally in
   a command name works fine; `rI` was chosen because it's short and visually
   distinct in the command string.

### Bugs fixed from original

```bash
# BUG 1, 2, 3: no `local` — all variables leaked into caller scope
reps="$1"            # global leak
command="${@:2}"     # global leak + shadows bash builtin
full="..."           # global leak

# BUG 4: bare $full doesn't handle commands whose arguments have spaces
$full                # broken for: repeat 1 echo "hello world"
eval "$_full_"       # fixed
```

---

## spinner.sh

An animated terminal spinner that actually wraps a real command. Starts the
animation in a background subshell, runs your command in the foreground, then
stops and reports.

```bash
source spinner.sh

# One-liner: wrap a command
spinner_run -- sleep 3
spinner_run --message "Building..." --style dots -- make -j8
spinner_run --message "Fetching" --style arrows --color magenta -- curl -sO URL

# Manual start/stop: when multiple steps share one spinner
spinner_start --message "Initializing..."
step_one
step_two
spinner_stop "All done!"

# See all styles
spinner_demo
```

### Options

| Option | Default | Description |
|---|---|---|
| `--message TEXT` | `Working...` | text shown beside the spinner |
| `--style NAME` | `classic` | frame set (see table below) |
| `--color NAME` | `cyan` | spinner color |
| `--tick SECS` | `0.08` | seconds between frames |
| `--no-time` | *(time shown)* | hide elapsed time counter |

### Built-in styles

| Name | Frames | Feel |
|---|---|---|
| `classic` | `\| / - \` | familiar, ASCII-safe |
| `dots` | `⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏` | braille dot rotation |
| `braille` | `⣾⣽⣻⢿⡿⣟⣯⣷` | braille fill sweep |
| `arrows` | `← ↖ ↑ ↗ → ↘ ↓ ↙` | compass sweep |
| `bar` | `▏▎▍▌▋▊▉█▉…` | fill/drain bar |
| `pulse` | `· •• ••• …` | grow and shrink |
| `moon` | `🌑🌒🌓🌔🌕🌖🌗🌘` | lunar cycle |

### What it teaches

#### `\r` vs `\n` — the core trick

```
\n  =  newline (line feed)     — cursor to start of NEXT line
\r  =  carriage return         — cursor to start of CURRENT line, no feed
```

`\r` is what makes in-place terminal animation possible. You print a frame,
then `\r` to return to column 0 without advancing the line. The next `printf`
overwrites character-by-character in-place. The previous frame disappears.

Combined with `\033[K` (ANSI "erase to end of line") you get a clean overwrite
even when the new text is shorter than the old — without it you'd see leftover
characters from longer previous frames.

```bash
# Minimal spinner — 4 lines that show the whole principle:
for frame in '|' '/' '-' '\'; do
    printf "\r%s thinking..." "$frame"
    sleep 0.1
done
printf "\r done!           \n"   # \n at the very end to advance off the line
```

#### Cursor hide/show

```bash
printf '\033[?25l'   # hide the blinking text cursor (cleaner while animating)
printf '\033[?25h'   # restore it
```

Always restore it. The `trap 'spinner_stop' EXIT INT TERM` in `spinner_start`
ensures this happens even if the script is Ctrl-C'd.

#### Background subshell pattern

The spinner loop runs in a background subshell (`&`) so the foreground shell
can execute the real command. `$!` captures the background PID so
`spinner_stop` can `kill` it precisely when the work is done.

```bash
_spinner_loop ... &
_SPINNER_PID_=$!

"${real_command[@]}"    # runs in foreground
exit_code=$?

kill "$_SPINNER_PID_"   # stop the animation
wait "$_SPINNER_PID_"   # reap the process (prevents zombie)
```

#### ANSI escape codes used

| Sequence | Effect |
|---|---|
| `\r` | carriage return (back to column 0) |
| `\033[K` | erase from cursor to end of line |
| `\033[?25l` | hide cursor |
| `\033[?25h` | show cursor |
| `\033[36m` | foreground color (cyan) |
| `\033[32m` | foreground color (green) |
| `\033[0m` | reset all attributes |

`\033` is octal for ESC (byte 0x1B). All ANSI control sequences start with
ESC followed by `[` — the pair is called the "Control Sequence Introducer" (CSI).

---

## Combining with bash-bar-utils

These tools are designed to compose:

```bash
source bar80.sh
source spinner.sh

echo "$BAR80_HASH"
BAR80_BOX "Starting build" HASH
echo ""
spinner_run --message "Compiling..." --style braille --color green -- make -j8
echo ""
BAR80_BOX "Build complete" HASH
```
