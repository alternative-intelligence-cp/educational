# bash-bar-utils

Sourced bash libraries of pre-built horizontal rule strings and text-centering
utilities for use in scripts, terminal output, and shell-based UIs.

Two widths: **80 columns** (traditional terminal standard) and **120 columns**
(modern widescreen terminal default). Source whichever matches your terminal
width, or source both and use them contextually.

---

## Files

| File | Width | Prefix |
|---|---|---|
| `bar80.sh` | 80 chars | `BAR80_` |
| `bar120.sh` | 120 chars | `BAR120_` |

---

## Usage

```bash
source bar80.sh

echo "$BAR80_HASH"
# ################################################################################

echo "$BAR80_DASH"
# --------------------------------------------------------------------------------

BAR80_BOX "Hello World"
# +------------------------------------------------------------------------------+
# |                                Hello World                                   |
# +------------------------------------------------------------------------------+

BAR80_BOX "Hello World" HASH
# ################################################################################
# #                               Hello World                                    #
# ################################################################################

BAR80_BOX "Hello World" ASTR
# ********************************************************************************
# *                               Hello World                                    *
# ********************************************************************************

BAR80_CENTER "Section Title" "="
# =                             Section Title                             =

BAR80_REPEAT "~" 40
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```

---

## Bar Variable Reference

Every bar string is a shell variable — just echo or embed in strings.

### Solid bars
| Variable | Character |
|---|---|
| `BAR80_HASH` | `#` |
| `BAR80_EQUL` | `=` |
| `BAR80_DASH` | `-` |
| `BAR80_ASTR` | `*` |
| `BAR80_EXCL` | `!` |
| `BAR80_UNDR` | `_` |
| `BAR80_AMPR` | `&` |
| `BAR80_PERC` | `%` |
| `BAR80_ADDR` | `@` |
| `BAR80_DOLR` | `$` |
| `BAR80_QUST` | `?` |
| `BAR80_HATT` | `^` |
| `BAR80_LVEE` | `v` |
| `BAR80_UVEE` | `V` |
| `BAR80_PLUS` | `+` |
| `BAR80_COLO` | `:` |
| `BAR80_SEMI` | `;` |
| `BAR80_PIPE` | `\|` |
| `BAR80_FSLH` | `/` |
| `BAR80_BSLH` | `\` |

### Alternating / directional (slash variants)
| Variable | Pattern | Visual feel |
|---|---|---|
| `BAR80_SLHI` | `////\\\\` | inward arrows |
| `BAR80_SLHO` | `\\\\////` | outward arrows |
| `BAR80_SLHA` | `/\/\/\` | zigzag A |
| `BAR80_SLHB` | `\/\/\/` | zigzag B |

### Bracket variants (for each bracket type: L, R, I=inward, O=outward, A=alternating, B=alt-offset)
Each bracket type (`PAR`, `BRK`, `SBK`, `THN`) follows the same pattern:
- `BAR80_LPAR` — all `(`
- `BAR80_RPAR` — all `)`
- `BAR80_PARI` — `(((…)))` (closing inward)
- `BAR80_PARO` — `)))…(((` (closing outward)
- `BAR80_PARA` — `()()()`
- `BAR80_PARB` — `)()()`

Same pattern for `BRK` `{}`, `SBK` `[]`, `THN` `<>`.

### Box / centering aids
| Variable | Content |
|---|---|
| `BAR80_ENDS` | `\|` + spaces + `\|` — a pre-spaced boxed line |
| `BAR80_TPBT` | `+----…----+` — box top/bottom |
| `BAR80_ENDB` | `\|` — single pipe end-cap |
| `BAR80_SPCE` | non-breaking space (U+00A0) — avoids word-wrap trimming |

---

## Function Reference

### `BAR80_BOX TEXT [STYLE]`
Prints a 3-line centered box (top bar, centered text, bottom bar).

```bash
BAR80_BOX "My Header"          # default: +---...---+ style
BAR80_BOX "My Header" HASH     # #########...######### style
BAR80_BOX "My Header" ASTR     # *********...*******  style
```

### `BAR80_CENTER TEXT [CHAR]`
Returns a single width-padded string — TEXT centered in an 80-char field with
CHAR as the border character (defaults to non-breaking space).

```bash
echo "$(BAR80_CENTER "Hello" "=")"
# =                              Hello                               =
```

### `BAR80_REPEAT CHAR [N]`
Repeats CHAR exactly N times (default 80). Useful for generating custom-width rules.

```bash
BAR80_REPEAT "~" 40
```

---

## The Width Difference: 80 vs 120

| Width | Origin | Still relevant? |
|---|---|---|
| **80** | IBM punch card (80 columns), then VT100 terminal default | Always — many tools, pagers, less, man pages default to 80 |
| **120** | Modern widescreen monitors, common 1080p+ terminal default | Yes — most developers run wider terminals today |

Choosing the wrong width causes visual misalignment when the script's output
is viewed in a terminal of a different width. The pattern here — naming the
width in the prefix (`BAR80_`, `BAR120_`) — makes it obvious at a glance which
you're using and easy to swap.

A unified parameterized version would be a natural next step, but the static
string approach has a real advantage: **zero computation at use time**. The
strings are already in memory as shell variables — `echo "$BAR80_HASH"` is
faster than any loop-based generator.

---

## What This Teaches

1. **Sourced library pattern** — `source file.sh` pulls functions and variables into the current shell scope; they persist for the session
2. **Shell variables as constants** — pre-computed strings stored in variables beat runtime generation for frequently repeated items
3. **Non-breaking space trick** — `printf "\xC2\xA0"` to get U+00A0; terminals don't trim it like regular spaces — using a regular `' '` for padding looks identical in source but several terminals silently drop trailing spaces, misaligning any box that depends on a fixed right edge
4. **`printf` UTF-8 escapes** — the `\xC2\xA0` sequence embeds the two-byte UTF-8 encoding of U+00A0 directly
5. **Terminal width conventions** — why 80 and 120 are the two dominant column counts
6. **Font ligature gotcha** — some terminal fonts (especially those with programming ligatures enabled) collapse runs of identical characters into a single wide glyph with no internal gaps. This makes bars appear broken or truncated even though all the bytes are present. The padding NBSP survives because it isn't part of the ligature sequence and renders independently, so the right edge stays in the correct column. Fix: disable ligatures for the font in question (confirmed working: GNOME Terminal, Tabby, Kitty — Tabby specifically exhibited this until ligatures were turned off).
