#!/usr/bin/env python3
"""
ResourceMemory Demonstrations
==============================
Four scenarios illustrating consumable memory constraints.

  Demo 1: Read-once secret
          A one-time-password token that self-destructs after a single read.
          Any further read attempt returns an expired error result.
          Models: OTP tokens, self-destructing messages, medical record single-access.

  Demo 2: Write-limited configuration
          A config cell that can only be written 3 times (initial setup + 2 updates).
          After the limit, writes are rejected.  The full write history is visible
          through the read/stats APIs.
          Models: immutable-after-N config, firmware write counters, audit-locked records.

  Demo 3: TTL expiry
          A session token that expires after a short TTL.
          Reads during the window succeed; reads after expiry fail.
          Also shows TTL-reset-on-write: a renewable lease pattern.
          Models: session tokens, cache entries, leases.

  Demo 4: Compound constraints — first constraint wins
          A cell with max_reads=5, max_writes=2, ttl=30.
          We exhaust reads before writes before TTL.
          Shows which status is set when different limits are hit.
          Models: rate-limited API responses, one-use vouchers with expiry.

Author: Randy Hoggard
Date: March 2026
"""

import sys
import os
import time

sys.path.insert(0, os.path.dirname(__file__))
from resource_memory import ResourceMemory, LIVE, WRITE_OK, EXPIRED_READS, EXPIRED_WRITES, EXPIRED_TTL


# ── Formatting helpers ────────────────────────────────────────────────────────

W = 68

def banner(title: str) -> None:
    print()
    print("=" * W)
    print(f"  {title}")
    print("=" * W)

def section(title: str) -> None:
    print()
    print(title)
    print("-" * W)

def print_result(label: str, result) -> None:
    """Print a ResourceResult in a readable one-line format."""
    ok_str   = "✓ OK   " if result.ok else "✗ ERROR"
    val_str  = repr(result.value) if result.ok else "(none)"
    rl       = f"reads_left={result.reads_left}" if result.reads_left is not None else ""
    wl       = f"writes_left={result.writes_left}" if result.writes_left is not None else ""
    tl       = f"ttl={result.ttl_left:.2f}s" if result.ttl_left is not None else ""
    extras   = "  ".join(x for x in [rl, wl, tl] if x)
    extras_s = f"  [{extras}]" if extras else ""
    print(f"  {ok_str}  {label:<28}  value={val_str:<20}  {result.status}{extras_s}")


# ── Demo 1: Read-once secret ──────────────────────────────────────────────────

def demo1():
    banner("DEMO 1: Read-Once Secret (max_reads=1)")
    print()
    print("An OTP token is created with max_reads=1.")
    print("The first read succeeds.  All further reads return an expired result.")
    print()
    print("The caller checks result.ok before using result.value.")
    print("There is no exception to catch — the contract is always satisfied.")

    otp = ResourceMemory(
        value     = "482-917",
        max_reads = 1,
        label     = "otp_token",
    )

    section("Attempt 1: legitimate consumer reads the token")
    r = otp.read()
    print_result("otp.read() — attempt 1", r)
    if r.ok:
        print(f"\n  Consumer received token: {r.value}")
        print(f"  Consumer processes it... token consumed.")

    section("Attempt 2: replay attack or duplicate read")
    r2 = otp.read()
    print_result("otp.read() — attempt 2", r2)
    if not r2.ok:
        print(f"\n  Read rejected: status={r2.status}")
        print(f"  The token is gone. No value was leaked.")

    section("peek() after expiry")
    r3 = otp.peek()
    print_result("otp.peek()", r3)
    print(f"\n  Even peek() returns an error result — the cell is fully expired.")

    section("Stats")
    s = otp.stats()
    print(f"  reads_done={s['reads_done']}  status={s['status']}")

    section("Minimal caller pattern")
    demo_otp = ResourceMemory("TOP-SECRET", max_reads=1)
    result = demo_otp.read()
    if result.ok:
        print(f"  Secret: {result.value}")
    else:
        print(f"  Access denied: {result.status}")

    print()
    print("  Note: re-checking after access automatically triggers expiry check.")
    print("  The cell does not need to be 'closed' or 'destroyed' explicitly.")


# ── Demo 2: Write-limited configuration ──────────────────────────────────────

def demo2():
    banner("DEMO 2: Write-Limited Configuration (max_writes=3)")
    print()
    print("A config record starts with an initial value (writes do not count")
    print("the constructor assignment — only explicit write() calls count).")
    print("3 writes are allowed: initial setup + 2 updates.")
    print("After that, the config is locked.")

    config = ResourceMemory(
        value      = {"env": "dev", "debug": True, "version": "1.0.0"},
        max_writes = 3,
        label      = "server_config",
    )

    section("Initial read (reads are unlimited)")
    r = config.read()
    print_result("config.read()", r)

    section("Write 1: promote to staging")
    r = config.write({"env": "staging", "debug": True, "version": "1.0.0"})
    print_result("config.write() — staging", r)
    print(f"  writes_left after: {config.remaining()['writes_left']}")

    section("Write 2: promote to production")
    r = config.write({"env": "prod", "debug": False, "version": "1.0.0"})
    print_result("config.write() — prod", r)
    print(f"  writes_left after: {config.remaining()['writes_left']}")

    section("Write 3: hotfix version bump")
    r = config.write({"env": "prod", "debug": False, "version": "1.0.1"})
    print_result("config.write() — hotfix", r)
    print(f"  writes_left after: {config.remaining()['writes_left']}")

    section("Write 4: attempted emergency patch — REJECTED")
    r = config.write({"env": "prod", "debug": True, "version": "1.0.2"})
    print_result("config.write() — emergency (attempt 4)", r)
    if not r.ok:
        print(f"\n  Write locked out: status={r.status}")
        print(f"  Config is now immutable.")

    section("Read attempt after write-expiry — also blocked")
    r = config.read()
    print_result("config.read() after write lock", r)
    if not r.ok:
        print(f"\n  Read also rejected: status={r.status}")
        print(f"  Once a cell is expired for any reason, ALL operations fail.")

    section("Stats")
    s = config.stats()
    print(f"  reads_done={s['reads_done']}  writes_done={s['writes_done']}  status={s['status']}")
    print()
    print("  Note: expiry is cell-level, not per-axis.")
    print("  When max_writes fires, the cell is locked entirely — reads too.")
    print("  To allow unlimited reads on a write-locked value, copy the value")
    print("  out on the last successful write and store it in a read-only cell")
    print("  (max_writes=0 is invalid — just use max_writes=N and read the last result).")


# ── Demo 3: TTL expiry ────────────────────────────────────────────────────────

def demo3():
    banner("DEMO 3: TTL Expiry + Renewable Lease Pattern")
    print()
    print("Part A: A session token with a 0.4s TTL.")
    print("Reads succeed during the window; fail after expiry.")
    print()
    print("Part B: A renewable lease (ttl_reset_on_write=True).")
    print("Writing before the TTL expires resets the clock.")

    section("Part A: fixed TTL session token (0.4s)")
    token = ResourceMemory(
        value  = "sess_abc123",
        ttl    = 0.4,
        label  = "session_token",
    )

    print()
    for i in range(1, 5):
        r = token.read()
        ttl_s = f"{r.ttl_left:.3f}s" if r.ttl_left is not None else "n/a"
        print_result(f"read attempt {i}  (ttl={ttl_s})", r)
        if i == 2:
            print(f"\n  Waiting 0.5s for TTL to expire...\n")
            time.sleep(0.5)

    section("Part B: renewable lease (0.3s TTL, resets on write)")
    lease = ResourceMemory(
        value              = "lease_holder_A",
        ttl                = 0.3,
        ttl_reset_on_write = True,
        label              = "distributed_lease",
    )

    print()
    # Read 1 — healthy
    r = lease.read()
    print_result(f"read 1  (ttl={r.ttl_left:.3f}s)", r)

    # Sleep almost to expiry, then write to renew
    time.sleep(0.25)
    r = lease.write("lease_holder_A")   # renew
    ttl_s = f"{r.ttl_left:.3f}s" if r.ttl_left is not None else "n/a"
    print_result(f"write (renew)  (ttl={ttl_s})", r)
    print(f"\n  Lease renewed.  Clock reset.  Sleeping 0.25s more...\n")

    # Sleep again — this time don't renew
    time.sleep(0.25)
    r = lease.read()
    ttl_s = f"{r.ttl_left:.3f}s" if r.ttl_left is not None else "n/a"
    print_result(f"read 2 post-renewal  (ttl={ttl_s})", r)

    # Now let it expire
    time.sleep(0.15)
    r = lease.read()
    ttl_s = f"{r.ttl_left:.3f}s" if r.ttl_left is not None else "n/a"
    print_result(f"read 3 post-expiry   (ttl={ttl_s})", r)
    if not r.ok:
        print(f"\n  Lease expired: {r.status}")
        print(f"  New lease would need to be created by a new ResourceMemory().")


# ── Demo 4: Compound constraints ──────────────────────────────────────────────

def demo4():
    banner("DEMO 4: Compound Constraints — First Limit Wins")
    print()
    print("A cell with three simultaneous constraints:")
    print("  max_reads=5    — exhausted first in this scenario")
    print("  max_writes=2   — exhausted second")
    print("  ttl=30.0       — never reached (scenario runs in ms)")
    print()
    print("Shows exactly which constraint fires, and that the others are irrelevant.")

    cell = ResourceMemory(
        value      = "initial_value",
        max_reads  = 5,
        max_writes = 2,
        ttl        = 30.0,
        label      = "compound_cell",
    )

    section("Write 1 (max_writes=2: 1 remaining after)")
    r = cell.write("value_v1")
    print_result("write v1", r)

    section("Consume all 5 allowed reads")
    for i in range(1, 7):
        r = cell.read()
        print_result(f"read {i}", r)
        if not r.ok:
            print(f"\n  Read {i} rejected: status={r.status}  ← first constraint hit")
            break

    section("Try to write after read-expiry (writes_left was still 1)")
    r = cell.write("value_v2_never_committed")
    print_result("write after expiry", r)
    if not r.ok:
        print(f"\n  Write also rejected: status={r.status}")
        print(f"  Once a cell is expired (for any reason), ALL operations fail.")
        print(f"  Writes_left was {cell._writes_left()}, but it doesn't matter —")
        print(f"  the read limit fired first and locked the cell.")

    section("Full stats at end")
    s = cell.stats()
    for k, v in s.items():
        if v is not None:
            print(f"  {k:<24} = {v}")

    section("Decision rule summary")
    print()
    print("  Constraint priority at expiry check:")
    print("    1. TTL  (runs whether you access the cell or not)")
    print("    2. reads (checked on read())")
    print("    3. writes (checked on write())")
    print()
    print("  The expired_status is set on the *first* operation that detects expiry.")
    print("  Subsequent operations see the same status without rechecking constraints.")
    print()
    print("  Caller pattern:")
    print("    result = cell.read()")
    print("    if result.ok:")
    print("        use(result.value)")
    print("    elif result.status == EXPIRED_READS:")
    print("        handle_read_exhaustion()")
    print("    elif result.status == EXPIRED_TTL:")
    print("        handle_timeout()")


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    print("ResourceMemory — Consumable, Expiring Memory Cell Demonstrations")
    print("=" * W)
    print("Cells expire when constraints are hit.  Access returns a result,")
    print("never raises.  The caller checks result.ok before using result.value.")
    print("Demos: read-once | write-limited | TTL expiry | compound constraints")

    demo1()
    demo2()
    demo3()
    demo4()

    print()
    print("=" * W)
    print("  All demos complete.")
    print("=" * W)
