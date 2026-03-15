#!/usr/bin/env python3
"""
ResourceMemory — Consumable, Expiring Memory Cells
====================================================
A memory cell with configurable constraints on how many times it can be
read, written, and how long it lives.  When any constraint is exhausted
the cell *expires* and all further access returns an error result rather
than raising an exception.

The caller is expected to check the result before using the value —
this mirrors the Result<T> pattern used in Aria: errors are values,
not exceptions.

Constraints (all optional, None = unlimited):
    max_reads            int|None    Cell expires after N successful reads.
    max_writes           int|None    Cell expires after N successful writes.
    ttl                  float|None  Seconds until expiry from creation
                                     (or from last write, if ttl_reset_on_write).
    ttl_reset_on_write   bool        If True, a successful write refreshes
                                     the TTL clock.  Default False.

Expiry semantics:
    Expiry is checked *before* the operation.  The last allowed read/write
    succeeds and returns an ok result.  The very next access returns an
    error result with the appropriate status.

    If multiple constraints are configured, the first one to be exhausted
    wins — the cell expires on whichever limit is hit first.

ResourceResult (namedtuple):
    ok           bool    — True = operation succeeded; False = expired or error.
    value        any     — The cell's value (None on error results).
    status       str     — 'LIVE' | 'EXPIRED_READS' | 'EXPIRED_WRITES'
                         | 'EXPIRED_TTL' | 'WRITE_ONLY' (write returned ok)
    reads_left   int|None
    writes_left  int|None
    ttl_left     float|None   — seconds remaining; None if no TTL; 0.0 if expired

ResourceMemory is intentionally *not* thread-safe in this POC.
For thread-safe use, add a lock around read/write/peek.

Author: Randy Hoggard
Date: March 2026
"""

import time
from collections import namedtuple
from typing import Any, Optional


# ── ResourceResult ────────────────────────────────────────────────────────────

ResourceResult = namedtuple(
    "ResourceResult",
    ["ok", "value", "status", "reads_left", "writes_left", "ttl_left"]
)


# ── Status constants ──────────────────────────────────────────────────────────

LIVE            = "LIVE"
WRITE_OK        = "WRITE_OK"
EXPIRED_READS   = "EXPIRED_READS"
EXPIRED_WRITES  = "EXPIRED_WRITES"
EXPIRED_TTL     = "EXPIRED_TTL"


# ── ResourceMemory ────────────────────────────────────────────────────────────

class ResourceMemory:
    """
    A memory cell with consumable constraints.

    Args:
        value              (any):         Initial value.
        max_reads          (int|None):    Max successful reads before expiry.
        max_writes         (int|None):    Max successful writes before expiry.
                                          Note: the initial value set in __init__
                                          does NOT count as a write.
        ttl                (float|None):  Time-to-live in seconds from creation
                                          (or from last write if ttl_reset_on_write).
        ttl_reset_on_write (bool):        Refresh TTL clock on each write.
        label              (str):         Optional name for display purposes.
    """

    def __init__(
        self,
        value: Any,
        max_reads: Optional[int] = None,
        max_writes: Optional[int] = None,
        ttl: Optional[float] = None,
        ttl_reset_on_write: bool = False,
        label: str = "cell",
    ):
        if max_reads is not None and (not isinstance(max_reads, int) or max_reads < 1):
            raise ValueError(f"max_reads must be a positive int or None, got {max_reads!r}")
        if max_writes is not None and (not isinstance(max_writes, int) or max_writes < 1):
            raise ValueError(f"max_writes must be a positive int or None, got {max_writes!r}")
        if ttl is not None and (not isinstance(ttl, (int, float)) or ttl <= 0):
            raise ValueError(f"ttl must be a positive number or None, got {ttl!r}")

        self._value              = value
        self._max_reads          = max_reads
        self._max_writes         = max_writes
        self._ttl                = float(ttl) if ttl is not None else None
        self._ttl_reset_on_write = ttl_reset_on_write
        self.label               = label

        self._reads              = 0
        self._writes             = 0
        self._born_at            = time.monotonic()
        self._ttl_start          = self._born_at   # reset on write if configured
        self._expired_status: Optional[str] = None  # set once, never cleared

    # ── Internal helpers ──────────────────────────────────────────────────────

    def _ttl_elapsed(self) -> float:
        return time.monotonic() - self._ttl_start

    def _ttl_remaining(self) -> Optional[float]:
        if self._ttl is None:
            return None
        remaining = self._ttl - self._ttl_elapsed()
        return max(0.0, remaining)

    def _reads_left(self) -> Optional[int]:
        if self._max_reads is None:
            return None
        return max(0, self._max_reads - self._reads)

    def _writes_left(self) -> Optional[int]:
        if self._max_writes is None:
            return None
        return max(0, self._max_writes - self._writes)

    def _check_expiry(self) -> Optional[str]:
        """
        Return the expiry status if the cell is expired, else None.

        Does NOT set self._expired_status — call _expire() to lock it in.
        Order: TTL checked first (time is always running), then reads, then writes.
        """
        if self._expired_status is not None:
            return self._expired_status
        if self._ttl is not None and self._ttl_elapsed() >= self._ttl:
            return EXPIRED_TTL
        if self._max_reads is not None and self._reads >= self._max_reads:
            return EXPIRED_READS
        if self._max_writes is not None and self._writes >= self._max_writes:
            return EXPIRED_WRITES
        return None

    def _expire(self, reason: str) -> None:
        if self._expired_status is None:
            self._expired_status = reason

    def _error_result(self, status: str) -> ResourceResult:
        return ResourceResult(
            ok          = False,
            value       = None,
            status      = status,
            reads_left  = self._reads_left(),
            writes_left = self._writes_left(),
            ttl_left    = self._ttl_remaining(),
        )

    def _ok_result(self, value: Any, status: str) -> ResourceResult:
        return ResourceResult(
            ok          = True,
            value       = value,
            status      = status,
            reads_left  = self._reads_left(),
            writes_left = self._writes_left(),
            ttl_left    = self._ttl_remaining(),
        )

    # ── Public API ────────────────────────────────────────────────────────────

    def read(self) -> ResourceResult:
        """
        Read the cell's value.  Consumes one read count if max_reads is set.

        Returns:
            ResourceResult with ok=True and the value if the cell is live.
            ResourceResult with ok=False if the cell is expired.
        """
        expired = self._check_expiry()
        if expired:
            self._expire(expired)
            return self._error_result(expired)

        self._reads += 1
        return self._ok_result(self._value, LIVE)

    def write(self, value: Any) -> ResourceResult:
        """
        Write a new value to the cell.  Consumes one write count if max_writes is set.
        Optionally resets the TTL clock.

        Returns:
            ResourceResult with ok=True if successful.
            ResourceResult with ok=False if the cell is expired.
        """
        expired = self._check_expiry()
        if expired:
            self._expire(expired)
            return self._error_result(expired)

        self._value = value
        self._writes += 1
        if self._ttl_reset_on_write and self._ttl is not None:
            self._ttl_start = time.monotonic()

        return self._ok_result(value, WRITE_OK)

    def peek(self) -> ResourceResult:
        """
        Read the current value without consuming a read count.

        Useful for status checks before committing to a read.
        Returns an error result if the cell is already expired.
        """
        expired = self._check_expiry()
        if expired:
            self._expire(expired)
            return self._error_result(expired)
        return self._ok_result(self._value, LIVE)

    def status(self) -> str:
        """
        Return the current status string without consuming any resource.

        Returns one of: 'LIVE' | 'EXPIRED_READS' | 'EXPIRED_WRITES' | 'EXPIRED_TTL'
        """
        expired = self._check_expiry()
        return expired if expired else LIVE

    def is_live(self) -> bool:
        """True if the cell is not expired."""
        return self._check_expiry() is None

    def remaining(self) -> dict:
        """
        Return remaining resource budget as a dict.

        Keys: 'reads_left', 'writes_left', 'ttl_left'
        Values: int|float or None (None = unlimited).
        """
        return {
            "reads_left":  self._reads_left(),
            "writes_left": self._writes_left(),
            "ttl_left":    self._ttl_remaining(),
        }

    def stats(self) -> dict:
        """
        Return full usage statistics.
        """
        return {
            "label":             self.label,
            "status":            self.status(),
            "reads_done":        self._reads,
            "writes_done":       self._writes,
            "reads_left":        self._reads_left(),
            "writes_left":       self._writes_left(),
            "ttl_left":          self._ttl_remaining(),
            "age_seconds":       time.monotonic() - self._born_at,
            "ttl_reset_on_write": self._ttl_reset_on_write,
        }

    def __repr__(self) -> str:
        s = self.status()
        r = self._reads_left()
        w = self._writes_left()
        t = self._ttl_remaining()
        parts = [f"status={s}"]
        if r is not None:
            parts.append(f"reads_left={r}")
        if w is not None:
            parts.append(f"writes_left={w}")
        if t is not None:
            parts.append(f"ttl_left={t:.2f}s")
        return f"ResourceMemory({self.label!r}, {', '.join(parts)})"
