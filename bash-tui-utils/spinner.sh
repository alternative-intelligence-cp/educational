#!/usr/bin/env bash
# =============================================================================
# spinner.sh — animated TUI spinner that wraps a real background command
# =============================================================================
# Key concept: \r vs \n
# ---------------------
#   \n  — newline: moves cursor to start of NEXT line (line feed)
#   \r  — carriage return: moves cursor to start of CURRENT line (no feed)
#
# \r is what makes terminal animation possible. You print a frame, then \r
# to return to column 0 without advancing, then overwrite with the next frame.
# The previous frame's content is replaced character-by-character in-place.
#
# Combined with  \033[K  (erase to end of line) you get a clean overwrite even
# if the new text is shorter than the old text.
#
# Usage:
#   source spinner.sh
#
#   # Wrap a command — spinner runs while it executes, then prints done message
#   spinner_run -- sleep 3
#   spinner_run --message "Building..." --style dots -- make -j8
#   spinner_run --message "Fetching" --style arrows --color cyan -- curl -sO URL
#
#   # Manual start/stop (when you need to run multiple things under one spinner)
#   spinner_start --message "Loading data..."
#   long_task_a
#   long_task_b
#   spinner_stop "All done!"
#
# Options (for both spinner_run and spinner_start):
#   --message TEXT    text shown next to the spinner  (default: "Working...")
#   --style   NAME    frame set: classic dots braille arrows bar pulse moon
#                     (default: classic)
#   --color   NAME    black red green yellow blue magenta cyan white  (default: cyan)
#   --tick    SECS    frame interval in seconds  (default: 0.08)
#   --no-time         hide elapsed time display
# =============================================================================


# ── Frame sets ────────────────────────────────────────────────────────────────
# Each is an array of single characters (or short strings) cycled in order.

declare -A _SPINNER_STYLES_
_SPINNER_STYLES_[classic]='| / - \'
_SPINNER_STYLES_[dots]='⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏'
_SPINNER_STYLES_[braille]='⣾ ⣽ ⣻ ⢿ ⡿ ⣟ ⣯ ⣷'
_SPINNER_STYLES_[arrows]='← ↖ ↑ ↗ → ↘ ↓ ↙'
_SPINNER_STYLES_[bar]='▏ ▎ ▍ ▌ ▋ ▊ ▉ █ ▉ ▊ ▋ ▌ ▍ ▎'
_SPINNER_STYLES_[pulse]='· •• ••• •••• ••• •• ·'
_SPINNER_STYLES_[moon]='🌑 🌒 🌓 🌔 🌕 🌖 🌗 🌘'

# ── ANSI color codes ──────────────────────────────────────────────────────────
declare -A _SPINNER_COLORS_
_SPINNER_COLORS_[black]='\033[30m'
_SPINNER_COLORS_[red]='\033[31m'
_SPINNER_COLORS_[green]='\033[32m'
_SPINNER_COLORS_[yellow]='\033[33m'
_SPINNER_COLORS_[blue]='\033[34m'
_SPINNER_COLORS_[magenta]='\033[35m'
_SPINNER_COLORS_[cyan]='\033[36m'
_SPINNER_COLORS_[white]='\033[37m'
_SPINNER_RESET_='\033[0m'

# ── Cursor control sequences ─────────────────────────────────────────────────
_CURSOR_HIDE_='\033[?25l'   # hide the blinking text cursor
_CURSOR_SHOW_='\033[?25h'   # restore it
_ERASE_LINE_='\033[K'       # erase from cursor to end of line

# ── Internal state ────────────────────────────────────────────────────────────
_SPINNER_PID_=0             # PID of the background spinner loop
_SPINNER_START_TIME_=0      # epoch second when spinner_start was called


# =============================================================================
# _spinner_parse_opts VARNAME_message VARNAME_style VARNAME_color
#                     VARNAME_tick VARNAME_show_time  "$@"
# Parses --key value flags out of "$@", sets named variables, returns remaining
# args (after --) in the global _SPINNER_REST_ array.
# =============================================================================
_spinner_parse_opts(){
    local -n _ref_msg_=$1;  shift
    local -n _ref_sty_=$1;  shift   # BUG NOTE: -n nameref requires bash 4.3+
    local -n _ref_col_=$1;  shift
    local -n _ref_tik_=$1;  shift
    local -n _ref_tim_=$1;  shift

    _SPINNER_REST_=()
    local past_sep=0
    while [[ $# -gt 0 ]]; do
        if [[ "$1" == "--" ]]; then
            past_sep=1; shift; continue
        fi
        if [[ $past_sep -eq 1 ]]; then
            _SPINNER_REST_+=("$1"); shift; continue
        fi
        case "$1" in
            --message) _ref_msg_="$2";  shift 2 ;;
            --style)   _ref_sty_="$2";  shift 2 ;;
            --color)   _ref_col_="$2";  shift 2 ;;
            --tick)    _ref_tik_="$2";  shift 2 ;;
            --no-time) _ref_tim_=0;     shift   ;;
            *)         shift ;;
        esac
    done
}


# =============================================================================
# _spinner_loop MESSAGE STYLE COLOR TICK SHOW_TIME
# Background process: draws frames until killed. Never call directly.
# =============================================================================
_spinner_loop(){
    local message="$1"
    local style="$2"
    local color="$3"
    local tick="$4"
    local show_time="$5"
    local start_time="$6"

    # Build frames array from the selected style string
    local frame_str="${_SPINNER_STYLES_[$style]:-${_SPINNER_STYLES_[classic]}}"
    local -a frames=($frame_str)   # word-split on spaces
    local nframes=${#frames[@]}
    local color_code="${_SPINNER_COLORS_[$color]:-${_SPINNER_COLORS_[cyan]}}"

    local idx=0
    local frame elapsed

    while true; do
        frame="${frames[$((idx % nframes))]}"
        (( idx++ ))

        if [[ $show_time -eq 1 ]]; then
            elapsed=$(( $(date +%s) - start_time ))
            printf "\r${color_code}%s${_SPINNER_RESET_} %s  [%ds]${_ERASE_LINE_}" \
                "$frame" "$message" "$elapsed"
        else
            printf "\r${color_code}%s${_SPINNER_RESET_} %s${_ERASE_LINE_}" \
                "$frame" "$message"
        fi

        sleep "$tick"
    done
}


# =============================================================================
# spinner_start [options]
# Starts the spinner in a background subshell. Call spinner_stop when done.
# =============================================================================
spinner_start(){
    local message="Working..."
    local style="classic"
    local color="cyan"
    local tick="0.08"
    local show_time=1

    # Parse flags (no -- separator needed for start)
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --message) message="$2"; shift 2 ;;
            --style)   style="$2";   shift 2 ;;
            --color)   color="$2";   shift 2 ;;
            --tick)    tick="$2";    shift 2 ;;
            --no-time) show_time=0;  shift   ;;
            *)         shift ;;
        esac
    done

    _SPINNER_START_TIME_=$(date +%s)

    # Hide cursor, run the loop in background, record its PID
    printf "${_CURSOR_HIDE_}"
    _spinner_loop "$message" "$style" "$color" "$tick" "$show_time" \
                  "$_SPINNER_START_TIME_" &
    _SPINNER_PID_=$!

    # Make sure the cursor is always restored even if the script dies
    trap 'spinner_stop' EXIT INT TERM
}


# =============================================================================
# spinner_stop [DONE_MESSAGE]
# Kills the background spinner, clears the line, prints done message.
# =============================================================================
spinner_stop(){
    local done_message="${1:-Done!}"

    if [[ $_SPINNER_PID_ -ne 0 ]]; then
        kill "$_SPINNER_PID_" 2>/dev/null
        wait "$_SPINNER_PID_" 2>/dev/null
        _SPINNER_PID_=0
    fi

    local elapsed=$(( $(date +%s) - _SPINNER_START_TIME_ ))

    # \r to column 0, erase the spinner line, print done message in green
    printf "\r${_ERASE_LINE_}\033[32m✓\033[0m %s  [%ds]\n" \
        "$done_message" "$elapsed"

    # Restore cursor
    printf "${_CURSOR_SHOW_}"

    # Remove the trap we installed
    trap - EXIT INT TERM
}


# =============================================================================
# spinner_run [options] -- COMMAND [ARGS...]
# All-in-one: starts spinner, runs command, stops spinner.
# Exit code of the wrapped command is preserved.
# =============================================================================
spinner_run(){
    local message="Working..."
    local style="classic"
    local color="cyan"
    local tick="0.08"
    local show_time=1
    local -a rest=()
    local past_sep=0

    while [[ $# -gt 0 ]]; do
        if [[ "$1" == "--" ]]; then
            past_sep=1; shift; continue
        fi
        if [[ $past_sep -eq 1 ]]; then
            rest+=("$1"); shift; continue
        fi
        case "$1" in
            --message) message="$2"; shift 2 ;;
            --style)   style="$2";   shift 2 ;;
            --color)   color="$2";   shift 2 ;;
            --tick)    tick="$2";    shift 2 ;;
            --no-time) show_time=0;  shift   ;;
            *)         shift ;;
        esac
    done

    if [[ ${#rest[@]} -eq 0 ]]; then
        echo "spinner_run: no command specified after --" >&2
        return 1
    fi

    spinner_start --message "$message" --style "$style" \
                  --color "$color" --tick "$tick" \
                  $( [[ $show_time -eq 0 ]] && echo "--no-time" )

    # Run the actual command — stdout/stderr pass through normally
    "${rest[@]}"
    local exit_code=$?

    if [[ $exit_code -eq 0 ]]; then
        spinner_stop "Done!"
    else
        spinner_stop "Failed (exit $exit_code)"
    fi

    return $exit_code
}


# =============================================================================
# spinner_demo
# Cycles through all built-in styles so you can see them.
# =============================================================================
spinner_demo(){
    local styles=( classic dots braille arrows bar pulse moon )
    local colors=( cyan green yellow magenta blue )
    local i=0

    echo "Spinner style demo (each runs for 2 seconds):"
    echo ""

    for style in "${styles[@]}"; do
        local color="${colors[$((i % ${#colors[@]}))]}"
        spinner_run --message "style: $style" --style "$style" \
                    --color "$color" --tick 0.1 \
                    -- sleep 2
        (( i++ ))
    done
}
