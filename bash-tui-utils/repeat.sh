#!/usr/bin/env bash
# =============================================================================
# repeat.sh — run a command N times, with optional iteration index injection
# =============================================================================
# Usage:
#   source repeat.sh
#   repeat N COMMAND [ARGS...]
#
# The token  rI  anywhere in COMMAND or ARGS is replaced with the current
# zero-based iteration index before execution.
#
# Examples:
#   repeat 3 echo "hello"         # prints hello three times
#   repeat 5 echo "item rI"       # prints item 0, item 1, ... item 4
#   repeat 4 mkdir -p dir_rI      # creates dir_0 dir_1 dir_2 dir_3
#
# How it works
# ------------
# The command and all its arguments are joined into a single string. sed
# replaces the  rI  token with the loop counter, then  eval  re-splits and
# executes the result. The eval is what makes quoted-argument commands work:
# without it, word-splitting happens at the wrong time.
#
# Internal variable names use  _underscores_  to avoid clobbering variables
# the caller might have set with common names like i, full, or cmd.
# =============================================================================


repeat(){
    # ── Validate ──────────────────────────────────────────────────────────────
    if [[ -z "$1" ]] || ! [[ "$1" =~ ^[0-9]+$ ]]; then
        echo "repeat: first argument must be a non-negative integer (got: '$1')" >&2
        return 1
    fi

    # ── Local variables only — never leak into caller scope ───────────────────
    local _reps_="$1"
    local _cmd_="${@:2}"   # everything after the count is the command
    local _full_
    local _i_

    # ── Loop ──────────────────────────────────────────────────────────────────
    for (( _i_=0; _i_<_reps_; _i_++ )); do
        # Replace the rI token with the current index, then eval to re-split
        # correctly (handles commands whose arguments contain spaces).
        _full_="$(echo "$_cmd_" | sed -E "s/rI/$_i_/g")"
        eval "$_full_"
    done
}
