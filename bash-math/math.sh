#!/usr/bin/env bash
# =============================================================================
# math.sh – bc float math via sourced bash functions
# =============================================================================
# Purpose
# -------
# Bash arithmetic only handles integers. The moment you have a float you're
# stuck either switching to bc directly or awkwardly piping strings. This
# library wraps bc so you never have to remember bc syntax — just source
# math.sh, type  math.<TAB>  and autocomplete shows you everything.
#
# Usage
#     source path/to/math.sh
#     math.add 1.5 2.3       # → 3.8
#     math.sqrt 2            # → 1.41421356237...
#     math.gt 3.14 2.71      # → 1
#
# How it works
# ------------
# Every function pipes an expression string into  bc -l  (the -l flag loads
# bc's math library, enabling sin/cos/log/etc and sets scale=20).
# Return values are plain strings on stdout — capture them with  $()  as you
# would any command substitution.
#
# Comparison functions return  1 (true)  or  0 (false)  as strings so they
# work naturally in  if [[ $(math.gt $a $b) == 1 ]]; then … fi  or as
# arithmetic:  (( $(math.gt $a $b) ))
#
# The core pattern to understand:
#     echo "EXPRESSION" | bc -l
# Everything here is just a thin wrapper around that single idiom.
# =============================================================================


# ── Standard arithmetic ───────────────────────────────────────────────────────

math.add(){   echo "$1+$2"   | bc -l; }   # $1 + $2
math.sub(){   echo "$1-$2"   | bc -l; }   # $1 - $2
math.mul(){   echo "$1*$2"   | bc -l; }   # $1 × $2
math.div(){   echo "$1/$2"   | bc -l; }   # $1 ÷ $2
math.mod(){   echo "$1%$2"   | bc -l; }   # $1 mod $2
math.pow(){   echo "$1^$2"   | bc -l; }   # $1 ^ $2
math.sqrt(){  echo "sqrt($1)"| bc -l; }   # √$1


# ── Derived arithmetic ────────────────────────────────────────────────────────

math.sqr(){   math.pow "$1" 2; }          # $1²
math.cub(){   math.pow "$1" 3; }          # $1³

math.abs(){                               # |$1|
    echo "x = $1; if (x < 0) -x else x" | bc -l
}

math.sum(){                               # sum of all arguments
    local total=0
    for arg in "$@"; do
        total=$(math.add "$total" "$arg")
    done
    echo "$total"
}


# ── Rounding / truncation ─────────────────────────────────────────────────────

math.floor(){                             # ⌊$1⌋ — round toward -∞
    echo "define floor(x) {
        auto r;
        if (x >= 0) { r = x/1; return r; }
        r = -x/1;
        if (x != -r) r = r + 1;
        return -r;
    }; scale=0; floor($1)" | bc -l
}

math.ceil(){                              # ⌈$1⌉  — round toward +∞
    echo "define ceil(x) {
        auto r;
        if (x <= 0) { r = -x/1; return -r; }
        r = x/1;
        if (x != r) r = r + 1;
        return r;
    }; scale=0; ceil($1)" | bc -l
}

math.round(){                             # round half-up
    echo "define round(x) {
        if (x >= 0) return (x + 0.5)/1;
        return -((-x + 0.5)/1);
    }; scale=0; round($1)" | bc -l
}

math.scale(){                             # math.scale VALUE DIGITS — trim decimal places
    echo "scale=$2; $1/1" | bc -l
}


# ── Range helpers ─────────────────────────────────────────────────────────────

math.min(){   echo "a=$1; b=$2; if (a < b) a else b"              | bc -l; }
math.max(){   echo "a=$1; b=$2; if (a > b) a else b"              | bc -l; }
math.clamp(){ echo "v=$1; lo=$2; hi=$3; if (v<lo) lo else if (v>hi) hi else v" | bc -l; }


# ── Trigonometry (radians) ────────────────────────────────────────────────────
# bc -l provides: s() sin, c() cos, a() atan  — everything else is derived.

math.sin(){   echo "s($1)"              | bc -l; }   # sin($1)
math.cos(){   echo "c($1)"              | bc -l; }   # cos($1)
math.tan(){   echo "s($1)/c($1)"        | bc -l; }   # tan($1)
math.atan(){  echo "a($1)"              | bc -l; }   # atan($1)
math.atan2(){ echo "a($1/$2)"           | bc -l; }   # atan($1/$2) — two-arg form


# ── Logarithms / exponents ────────────────────────────────────────────────────
# bc -l provides: l() natural log, e() e^x  — everything else is derived.

math.log(){   echo "l($1)"              | bc -l; }   # ln($1)
math.log2(){  echo "l($1)/l(2)"         | bc -l; }   # log₂($1)
math.log10(){ echo "l($1)/l(10)"        | bc -l; }   # log₁₀($1)
math.exp(){   echo "e($1)"              | bc -l; }   # e^$1   NOTE: replaces old "raw expression" function — use math.eval for that


# ── Constants ────────────────────────────────────────────────────────────────

math.pi(){    echo "scale=20; 4*a(1)"   | bc -l; }   # π
math.e(){     echo "scale=20; e(1)"     | bc -l; }   # e


# ── Geometry ──────────────────────────────────────────────────────────────────

math.pyth2(){ echo "sqrt(($1*$1)+($2*$2))"          | bc -l; }   # 2D hypotenuse
math.pyth3(){ echo "sqrt(($1*$1)+($2*$2)+($3*$3))"  | bc -l; }   # 3D distance


# ── Comparisons (return "1" or "0") ──────────────────────────────────────────

math.equ(){
    if (( $(echo "$1 == $2" | bc -l) )); then echo 1; else echo 0; fi
}
math.gt(){
    if (( $(echo "$1 > $2"  | bc -l) )); then echo 1; else echo 0; fi
}
math.lt(){
    if (( $(echo "$1 < $2"  | bc -l) )); then echo 1; else echo 0; fi
}
math.gte(){
    if (( $(echo "$1 >= $2" | bc -l) )); then echo 1; else echo 0; fi
}
math.lte(){
    if (( $(echo "$1 <= $2" | bc -l) )); then echo 1; else echo 0; fi
}
math.cmp(){   # returns -1, 0, or 1
    if (($(echo "$1 == $2" | bc -l))); then echo  "0"; return; fi
    if (($(echo "$1 >  $2" | bc -l))); then echo  "1"; return; fi
    echo "-1"
}


# ── Base conversions ──────────────────────────────────────────────────────────
# bc obase/ibase swap — ibase must be set AFTER obase (bc evaluates left-to-right
# and changing ibase mid-expression affects how it reads subsequent numbers).

math.toBinary(){    echo "obase=2;$1"              | bc -l; }
math.fromBinary(){  echo "obase=10;ibase=2;$1"     | bc;    }
math.toTernary(){   echo "obase=3;$1"              | bc -l; }
math.fromTernary(){ echo "obase=10;ibase=3;$1"     | bc;    }
math.toOctal(){     echo "obase=8;$1"              | bc -l; }
math.fromOctal(){   echo "obase=10;ibase=8;$1"     | bc;    }
math.toNonary(){    echo "obase=9;$1"              | bc -l; }
math.fromNonary(){  echo "obase=10;ibase=9;$1"     | bc;    }
math.toHex(){       echo "obase=16;$1"             | bc -l; }
math.fromHex(){     echo "obase=10;ibase=16;$1"    | bc;    }
math.toBase(){      echo "obase=$2;$1"             | bc -l; }   # math.toBase VALUE BASE


# ── Meta ──────────────────────────────────────────────────────────────────────

math.digits(){  echo "length($1)"  | bc -l; }   # digit count (before decimal point)
math.eval(){    echo "$1"          | bc -l; }   # raw bc expression — math.eval "3*a(1)*2"
