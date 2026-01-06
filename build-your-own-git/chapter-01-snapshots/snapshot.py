#!/usr/bin/env python3
"""
Simple snapshot-based version control system.

Usage:
    python3 snapshot.py save "Your message"
    python3 snapshot.py list
    python3 snapshot.py restore <number>

Examples:
    python3 snapshot.py save "Working version before refactor"
    python3 snapshot.py list
    python3 snapshot.py restore 1
"""

import os
import shutil
from datetime import datetime
import sys

SNAPSHOT_DIR = ".snapshots"


def init_snapshot_system():
    """Create the .snapshots folder if it doesn't exist."""
    if not os.path.exists(SNAPSHOT_DIR):
        os.makedirs(SNAPSHOT_DIR)
        print(f"✓ Initialized snapshot system in {SNAPSHOT_DIR}/")
    else:
        print(f"✓ Snapshot system already exists")


def save_snapshot(message):
    """Save a snapshot of the current project state."""
    init_snapshot_system()

    # Get list of existing snapshots
    snapshots = sorted(
        [
            d
            for d in os.listdir(SNAPSHOT_DIR)
            if os.path.isdir(os.path.join(SNAPSHOT_DIR, d))
        ]
    )
    snapshot_num = len(snapshots) + 1

    # Create snapshot folder with number and timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    snapshot_name = f"snapshot_{snapshot_num:03d}_{timestamp}"
    snapshot_path = os.path.join(SNAPSHOT_DIR, snapshot_name)

    os.makedirs(snapshot_path)

    # Copy all files (except .snapshots itself!)
    file_count = 0
    for item in os.listdir("."):
        if item == SNAPSHOT_DIR:
            continue  # Don't copy snapshots into snapshots!

        src = item
        dst = os.path.join(snapshot_path, item)

        try:
            if os.path.isfile(src):
                shutil.copy2(src, dst)
                file_count += 1
            elif os.path.isdir(src):
                shutil.copytree(src, dst)
                file_count += 1
        except Exception as e:
            print(f"⚠️  Warning: Could not copy {item}: {e}")

    # Save the message
    with open(os.path.join(snapshot_path, "SNAPSHOT_MESSAGE.txt"), "w") as f:
        f.write(message)

    print(f"✓ Saved snapshot #{snapshot_num}: {message}")
    print(f"  ({file_count} items backed up)")
    return snapshot_num


def list_snapshots():
    """List all saved snapshots."""
    if not os.path.exists(SNAPSHOT_DIR):
        print("No snapshots yet. Use 'save' to create one.")
        return

    snapshots = sorted(
        [
            d
            for d in os.listdir(SNAPSHOT_DIR)
            if os.path.isdir(os.path.join(SNAPSHOT_DIR, d))
        ]
    )

    if not snapshots:
        print("No snapshots yet. Use 'save' to create one.")
        return

    print("\nAvailable snapshots:")
    print("-" * 70)

    for i, snapshot in enumerate(snapshots, 1):
        snapshot_path = os.path.join(SNAPSHOT_DIR, snapshot)
        message_file = os.path.join(snapshot_path, "SNAPSHOT_MESSAGE.txt")

        if os.path.exists(message_file):
            with open(message_file, "r") as f:
                message = f.read().strip()
        else:
            message = "(no message)"

        # Extract timestamp from folder name
        parts = snapshot.split("_")
        if len(parts) >= 3:
            date_str = parts[1]
            time_str = parts[2]
            formatted_time = f"{date_str[:4]}-{date_str[4:6]}-{date_str[6:8]} {time_str[:2]}:{time_str[2:4]}:{time_str[4:6]}"
        else:
            formatted_time = "unknown"

        print(f"  [{i}] {formatted_time}")
        print(f"      {message}")
        print()


def restore_snapshot(snapshot_num):
    """Restore project to a previous snapshot."""
    if not os.path.exists(SNAPSHOT_DIR):
        print("Error: No snapshots exist.")
        return

    snapshots = sorted(
        [
            d
            for d in os.listdir(SNAPSHOT_DIR)
            if os.path.isdir(os.path.join(SNAPSHOT_DIR, d))
        ]
    )

    if snapshot_num < 1 or snapshot_num > len(snapshots):
        print(f"Error: Snapshot #{snapshot_num} doesn't exist.")
        print(f"Available snapshots: 1-{len(snapshots)}")
        return

    snapshot_name = snapshots[snapshot_num - 1]
    snapshot_path = os.path.join(SNAPSHOT_DIR, snapshot_name)

    # Show what snapshot we're restoring
    message_file = os.path.join(snapshot_path, "SNAPSHOT_MESSAGE.txt")
    if os.path.exists(message_file):
        with open(message_file, "r") as f:
            message = f.read().strip()
        print(f"\nRestoring snapshot #{snapshot_num}: {message}")

    print(f"\n⚠️  WARNING: This will replace your current files!")
    response = input("Continue? (yes/no): ")

    if response.lower() not in ["yes", "y"]:
        print("Cancelled.")
        return

    # Remove current files (except .snapshots)
    removed_count = 0
    for item in os.listdir("."):
        if item == SNAPSHOT_DIR:
            continue

        try:
            if os.path.isfile(item):
                os.remove(item)
                removed_count += 1
            elif os.path.isdir(item):
                shutil.rmtree(item)
                removed_count += 1
        except Exception as e:
            print(f"⚠️  Warning: Could not remove {item}: {e}")

    # Copy snapshot files back
    restored_count = 0
    for item in os.listdir(snapshot_path):
        if item == "SNAPSHOT_MESSAGE.txt":
            continue  # Don't restore the message file

        src = os.path.join(snapshot_path, item)
        dst = item

        try:
            if os.path.isfile(src):
                shutil.copy2(src, dst)
                restored_count += 1
            elif os.path.isdir(src):
                shutil.copytree(src, dst)
                restored_count += 1
        except Exception as e:
            print(f"⚠️  Warning: Could not restore {item}: {e}")

    print(f"\n✓ Restored snapshot #{snapshot_num}")
    print(f"  ({restored_count} items restored)")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return

    command = sys.argv[1]

    if command == "save":
        if len(sys.argv) < 3:
            print("Error: 'save' requires a message")
            print('Usage: python3 snapshot.py save "Your message"')
            return
        message = " ".join(sys.argv[2:])
        save_snapshot(message)

    elif command == "list":
        list_snapshots()

    elif command == "restore":
        if len(sys.argv) < 3:
            print("Error: 'restore' requires a snapshot number")
            print("Usage: python3 snapshot.py restore <number>")
            return
        try:
            snapshot_num = int(sys.argv[2])
            restore_snapshot(snapshot_num)
        except ValueError:
            print("Error: Snapshot number must be an integer")

    else:
        print(f"Unknown command: {command}")
        print(__doc__)


if __name__ == "__main__":
    main()
