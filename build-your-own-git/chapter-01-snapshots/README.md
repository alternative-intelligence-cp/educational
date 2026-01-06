# Chapter 1: Time Travel for Files

## The Problem

You're working on a project. It's going great. Then you try to "improve" something and... everything breaks. Now you wish you could go back to when it worked.

Sound familiar?

You need a **time machine** for your files. Since those don't exist (yet), we'll build the next best thing: a **snapshot system**.

## What We're Building

By the end of this chapter, you'll have two commands:

```bash
python3 snapshot.py save "Working version before I broke it"
# ... make changes, break things ...
python3 snapshot.py restore 1
# Ta-da! Back to the working version
```

Simple. Powerful. The foundation of all version control.

## The Core Idea

**A snapshot is just a copy of your entire project at a moment in time.**

That's it. No magic. When you "save" a snapshot, we copy all your files to a hidden folder. When you "restore," we copy them back.

Git does this too - it's just more clever about *how* it stores the copies (we'll get to that in later chapters).

## Let's Build It

### Step 1: Where Do We Keep Snapshots?

We need a hidden folder to store our copies. Convention: use `.snapshots/`

```python
import os
import shutil
from datetime import datetime

SNAPSHOT_DIR = '.snapshots'

def init_snapshot_system():
    """Create the .snapshots folder if it doesn't exist."""
    if not os.path.exists(SNAPSHOT_DIR):
        os.makedirs(SNAPSHOT_DIR)
        print(f"✓ Initialized snapshot system in {SNAPSHOT_DIR}/")
    else:
        print(f"✓ Snapshot system already exists")
```

**Try it yourself**: What happens if you run this twice? (Hint: the `if` statement makes it safe)

### Step 2: Saving a Snapshot

To save a snapshot:
1. Create a new folder with a unique name (we'll use timestamp + number)
2. Copy all project files into it
3. Save a message describing what this snapshot is

```python
def save_snapshot(message):
    """Save a snapshot of the current project state."""
    init_snapshot_system()
    
    # Get list of existing snapshots
    snapshots = sorted([d for d in os.listdir(SNAPSHOT_DIR) if os.path.isdir(os.path.join(SNAPSHOT_DIR, d))])
    snapshot_num = len(snapshots) + 1
    
    # Create snapshot folder with number and timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    snapshot_name = f"snapshot_{snapshot_num:03d}_{timestamp}"
    snapshot_path = os.path.join(SNAPSHOT_DIR, snapshot_name)
    
    os.makedirs(snapshot_path)
    
    # Copy all files (except .snapshots itself!)
    for item in os.listdir('.'):
        if item == SNAPSHOT_DIR:
            continue  # Don't copy snapshots into snapshots!
        
        src = item
        dst = os.path.join(snapshot_path, item)
        
        if os.path.isfile(src):
            shutil.copy2(src, dst)
        elif os.path.isdir(src):
            shutil.copytree(src, dst)
    
    # Save the message
    with open(os.path.join(snapshot_path, 'SNAPSHOT_MESSAGE.txt'), 'w') as f:
        f.write(message)
    
    print(f"✓ Saved snapshot #{snapshot_num}: {message}")
    return snapshot_num
```

**Question for you**: Why do we skip copying `.snapshots/` itself? What would happen if we didn't?

### Step 3: Listing Snapshots

Before we can restore, we need to see what snapshots exist:

```python
def list_snapshots():
    """List all saved snapshots."""
    if not os.path.exists(SNAPSHOT_DIR):
        print("No snapshots yet. Use 'save' to create one.")
        return
    
    snapshots = sorted([d for d in os.listdir(SNAPSHOT_DIR) if os.path.isdir(os.path.join(SNAPSHOT_DIR, d))])
    
    if not snapshots:
        print("No snapshots yet. Use 'save' to create one.")
        return
    
    print("\nAvailable snapshots:")
    print("-" * 60)
    
    for i, snapshot in enumerate(snapshots, 1):
        snapshot_path = os.path.join(SNAPSHOT_DIR, snapshot)
        message_file = os.path.join(snapshot_path, 'SNAPSHOT_MESSAGE.txt')
        
        if os.path.exists(message_file):
            with open(message_file, 'r') as f:
                message = f.read().strip()
        else:
            message = "(no message)"
        
        # Extract timestamp from folder name
        parts = snapshot.split('_')
        if len(parts) >= 3:
            date_str = parts[1]
            time_str = parts[2]
            formatted_time = f"{date_str[:4]}-{date_str[4:6]}-{date_str[6:8]} {time_str[:2]}:{time_str[2:4]}:{time_str[4:6]}"
        else:
            formatted_time = "unknown"
        
        print(f"  [{i}] {formatted_time}")
        print(f"      {message}")
        print()
```

### Step 4: Restoring a Snapshot

The moment of truth - can we go back in time?

```python
def restore_snapshot(snapshot_num):
    """Restore project to a previous snapshot."""
    if not os.path.exists(SNAPSHOT_DIR):
        print("Error: No snapshots exist.")
        return
    
    snapshots = sorted([d for d in os.listdir(SNAPSHOT_DIR) if os.path.isdir(os.path.join(SNAPSHOT_DIR, d))])
    
    if snapshot_num < 1 or snapshot_num > len(snapshots):
        print(f"Error: Snapshot #{snapshot_num} doesn't exist.")
        print(f"Available snapshots: 1-{len(snapshots)}")
        return
    
    snapshot_name = snapshots[snapshot_num - 1]
    snapshot_path = os.path.join(SNAPSHOT_DIR, snapshot_name)
    
    print(f"⚠️  This will replace your current files with snapshot #{snapshot_num}")
    response = input("Continue? (yes/no): ")
    
    if response.lower() not in ['yes', 'y']:
        print("Cancelled.")
        return
    
    # Remove current files (except .snapshots)
    for item in os.listdir('.'):
        if item == SNAPSHOT_DIR:
            continue
        
        if os.path.isfile(item):
            os.remove(item)
        elif os.path.isdir(item):
            shutil.rmtree(item)
    
    # Copy snapshot files back
    for item in os.listdir(snapshot_path):
        if item == 'SNAPSHOT_MESSAGE.txt':
            continue  # Don't restore the message file
        
        src = os.path.join(snapshot_path, item)
        dst = item
        
        if os.path.isfile(src):
            shutil.copy2(src, dst)
        elif os.path.isdir(src):
            shutil.copytree(src, dst)
    
    print(f"✓ Restored snapshot #{snapshot_num}")
```

**Safety question**: Why do we ask for confirmation before restoring?

### Step 5: Putting It All Together

Now we just need a command-line interface:

```python
#!/usr/bin/env python3
"""
Simple snapshot-based version control system.

Usage:
    python3 snapshot.py save "Your message"
    python3 snapshot.py list
    python3 snapshot.py restore <number>
"""

import sys

def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    command = sys.argv[1]
    
    if command == 'save':
        if len(sys.argv) < 3:
            print("Error: 'save' requires a message")
            print("Usage: python3 snapshot.py save \"Your message\"")
            return
        message = ' '.join(sys.argv[2:])
        save_snapshot(message)
    
    elif command == 'list':
        list_snapshots()
    
    elif command == 'restore':
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

if __name__ == '__main__':
    main()
```

## Try It Out!

Create a test project and experiment:

```bash
# Create a test project
mkdir test_project
cd test_project
echo "Hello, world!" > hello.txt

# Save your first snapshot
python3 ../snapshot.py save "Initial version"

# Make some changes
echo "This is version 2" > hello.txt
python3 ../snapshot.py save "Updated greeting"

# Break something
rm hello.txt
python3 ../snapshot.py save "Oops, deleted the file"

# List snapshots
python3 ../snapshot.py list

# Restore to version 1
python3 ../snapshot.py restore 1

# Check - hello.txt is back!
cat hello.txt
```

## What You Just Built

Congratulations! You've built the core concept behind **every version control system**:

- **Snapshots**: Save the state of your project
- **History**: Track multiple versions over time
- **Restore**: Go back to any previous version

This is literally how git started - as a simple snapshot system. The rest is optimization and clever features built on top of this foundation.

## Aha Moments

**"Wait, version control is just copying files?"**

Basically, yes! The magic is in:
- Organizing the copies intelligently
- Making it fast (we'll optimize in later chapters)
- Adding tools to compare and merge (coming up!)

**"Why not just manually copy files to backup folders?"**

You could! But:
- You'd forget which copy was which
- You'd have no record of what changed
- Comparing versions would be manual and painful
- Collaborating would be chaos

Version control automates and organizes what you'd do manually.

## Challenges

Ready to extend your snapshot system? Try these:

### Challenge 1: Add a `diff` Command
Show what files changed between two snapshots.

<details>
<summary>Hint</summary>
Compare the list of files in two snapshot folders. For files that exist in both, compare their contents.
</details>

### Challenge 2: Make It Faster
Copying everything every time is slow. Can you skip copying files that haven't changed?

<details>
<summary>Hint</summary>
Compare file modification times or content hashes before copying.
</details>

### Challenge 3: Add Descriptions
Let users add longer descriptions to snapshots, not just one-line messages.

<details>
<summary>Hint</summary>
Prompt for multi-line input, or read from a file.
</details>

## What's Next?

You can save and restore snapshots. But what *changed* between them? How do you see what's different?

That's what we'll build in **[Chapter 2: What Changed?](../chapter-02-diffs/README.md)**

We'll implement a diff algorithm that shows exactly what changed between any two snapshots. This is what makes version control useful - not just saving history, but understanding it.

---

**Key Takeaway**: Version control isn't magic. It's organized file copying plus tools to understand and manage those copies. Everything else builds on this foundation.

You've taken the first step. Keep going! 🚀
