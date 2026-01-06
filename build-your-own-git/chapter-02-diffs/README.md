# Chapter 2: What Changed? (Understanding Diffs)

## The Problem

You just restored snapshot #3. But wait... what exactly was different between snapshot #3 and snapshot #4? You can't remember! You open both directories and start comparing files line by line. There has to be a better way...

## The "Aha!" Moment

**Git doesn't just save snapshots—it shows you what changed between them.**

When you run `git log -p` or `git diff`, you see exactly which lines were added, removed, or modified. This is one of git's superpowers: understanding **deltas** (changes) between versions.

## What We're Building

In this chapter, we'll add a `diff` command to our snapshot tool:

```bash
python3 snapshot.py diff 3 4
```

This will show us:
- Which files were added, removed, or modified
- Exactly which lines changed in each file
- A nice visual display (+ for additions, - for deletions)

## How Diffing Works

Let's think through this step by step:

### Step 1: Compare File Lists

First, figure out which files exist in each snapshot:

```
Snapshot 3:              Snapshot 4:
- hello.py               - hello.py
- README.md              - README.md
                         - config.py    <- NEW FILE!
- old_stuff.txt          <- DELETED!
```

### Step 2: For Changed Files, Compare Line by Line

For files that exist in both snapshots, we need to compare their contents:

```python
# Old version (snapshot 3):
def greet(name):
    print("Hello, " + name)

# New version (snapshot 4):
def greet(name):
    print(f"Hello, {name}!")
    print("Welcome!")
```

The diff should show:
```
- print("Hello, " + name)
+ print(f"Hello, {name}!")
+ print("Welcome!")
```

### Step 3: Make It Pretty

Add some visual flair:
- Green `+` for additions
- Red `-` for deletions
- Context lines (unchanged lines around changes) to show where the change happened

## The Code

Let's build this! Create `diff.py`:

```python
#!/usr/bin/env python3
"""
Add diff capability to our snapshot system.

Usage:
    python3 diff.py <snapshot1> <snapshot2>

Examples:
    python3 diff.py 1 2
    python3 diff.py 3 5
"""

import os
import sys
from pathlib import Path

SNAPSHOT_DIR = '.snapshots'


def get_snapshot_path(snapshot_num):
    """Get the path to a snapshot by number."""
    if not os.path.exists(SNAPSHOT_DIR):
        print("Error: No snapshots exist.")
        return None
    
    snapshots = sorted([d for d in os.listdir(SNAPSHOT_DIR) 
                       if os.path.isdir(os.path.join(SNAPSHOT_DIR, d))])
    
    if snapshot_num < 1 or snapshot_num > len(snapshots):
        print(f"Error: Snapshot #{snapshot_num} doesn't exist.")
        return None
    
    return os.path.join(SNAPSHOT_DIR, snapshots[snapshot_num - 1])


def get_files_in_snapshot(snapshot_path):
    """Get a set of all files in a snapshot (excluding message file)."""
    files = set()
    
    for root, dirs, filenames in os.walk(snapshot_path):
        for filename in filenames:
            if filename == 'SNAPSHOT_MESSAGE.txt':
                continue
            
            # Get path relative to snapshot directory
            full_path = os.path.join(root, filename)
            rel_path = os.path.relpath(full_path, snapshot_path)
            files.add(rel_path)
    
    return files


def read_file_lines(filepath):
    """Read a file and return its lines. Return empty list if file doesn't exist or can't be read."""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            return f.readlines()
    except:
        return []


def simple_diff(lines1, lines2):
    """
    Simple line-by-line diff algorithm.
    Returns a list of changes in format: ('context', line), ('-', line), ('+', line)
    
    This is a VERY simple algorithm. Real git uses Myers' diff algorithm which is more sophisticated.
    """
    changes = []
    
    # For simplicity, we'll use Python's difflib
    import difflib
    
    differ = difflib.unified_diff(
        lines1, 
        lines2, 
        lineterm='',
        n=3  # 3 lines of context
    )
    
    for line in differ:
        if line.startswith('---') or line.startswith('+++'):
            continue  # Skip header lines
        elif line.startswith('@@'):
            changes.append(('header', line))
        elif line.startswith('-'):
            changes.append(('-', line[1:]))
        elif line.startswith('+'):
            changes.append(('+', line[1:]))
        else:
            changes.append(('context', line[1:] if line else ''))
    
    return changes


def print_diff(filename, changes):
    """Print a diff in a nice format."""
    if not changes:
        return
    
    print(f"\n📄 {filename}")
    print("=" * 70)
    
    for change_type, line in changes:
        line = line.rstrip('\n')
        
        if change_type == 'header':
            print(f"\033[36m{line}\033[0m")  # Cyan
        elif change_type == '-':
            print(f"\033[31m- {line}\033[0m")  # Red
        elif change_type == '+':
            print(f"\033[32m+ {line}\033[0m")  # Green
        else:  # context
            print(f"  {line}")


def diff_snapshots(num1, num2):
    """Show the diff between two snapshots."""
    path1 = get_snapshot_path(num1)
    path2 = get_snapshot_path(num2)
    
    if not path1 or not path2:
        return
    
    print(f"\n🔍 Comparing snapshot #{num1} → #{num2}")
    print("=" * 70)
    
    files1 = get_files_in_snapshot(path1)
    files2 = get_files_in_snapshot(path2)
    
    # Files only in snapshot 1 (deleted)
    deleted = files1 - files2
    if deleted:
        print("\n❌ Deleted files:")
        for f in sorted(deleted):
            print(f"  - {f}")
    
    # Files only in snapshot 2 (added)
    added = files2 - files1
    if added:
        print("\n✨ New files:")
        for f in sorted(added):
            print(f"  + {f}")
    
    # Files in both (potentially modified)
    common = files1 & files2
    
    print(f"\n📝 Comparing {len(common)} common files...")
    
    changes_found = False
    for filename in sorted(common):
        file1 = os.path.join(path1, filename)
        file2 = os.path.join(path2, filename)
        
        lines1 = read_file_lines(file1)
        lines2 = read_file_lines(file2)
        
        if lines1 != lines2:
            changes_found = True
            changes = simple_diff(lines1, lines2)
            print_diff(filename, changes)
    
    if not changes_found and not added and not deleted:
        print("\n✓ No changes between these snapshots!")


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return
    
    try:
        num1 = int(sys.argv[1])
        num2 = int(sys.argv[2])
    except ValueError:
        print("Error: Snapshot numbers must be integers")
        return
    
    diff_snapshots(num1, num2)


if __name__ == '__main__':
    main()
```

## Try It Out!

1. **Make some test snapshots:**

```bash
# Create a simple Python file
echo "def hello():" > hello.py
echo "    print('Hello')" >> hello.py
python3 snapshot.py save "Initial version"

# Modify it
echo "def hello(name='World'):" > hello.py
echo "    print(f'Hello, {name}!')" >> hello.py
python3 snapshot.py save "Added name parameter"

# Add more features
echo "def hello(name='World'):" > hello.py
echo "    print(f'Hello, {name}!')" >> hello.py
echo "" >> hello.py
echo "def goodbye():" >> hello.py
echo "    print('Goodbye!')" >> hello.py
python3 snapshot.py save "Added goodbye function"
```

2. **Now run the diff:**

```bash
python3 diff.py 1 2
python3 diff.py 2 3
python3 diff.py 1 3
```

You should see exactly what changed between each version!

## The "Aha!" Moment (Part 2)

**This is why git commits are so fast!**

When you make a git commit, git doesn't copy your entire project. It:
1. Looks at what files changed
2. Stores only the changes (deltas)
3. Links to unchanged files from the previous commit

This is why you can have thousands of commits without your `.git` directory becoming gigantic. Git is clever about storage!

## Challenges

### Challenge 1: Side-by-Side Diff
Modify the diff to show old and new versions side by side:

```
Old (snapshot 1)  |  New (snapshot 2)
----------------  |  ----------------
def hello():      |  def hello(name):
    print('Hi')   |      print(f'Hi {name}')
```

### Challenge 2: Diff Summary
Add a summary at the end showing total changes:

```
Summary: 5 files changed, 23 insertions(+), 12 deletions(-)
```

### Challenge 3: Unified Snapshot Command
Combine `snapshot.py` and `diff.py` into one command:

```bash
python3 snapshot.py diff 1 2
```

## Key Concepts

1. **Diff = Delta**: A diff shows the delta (change) between two versions
2. **Line-level granularity**: Most version control works at the line level
3. **Context matters**: Showing unchanged lines around changes helps you understand where the change happened
4. **Efficiency**: Storing changes instead of full copies saves space

## Next Up: Chapter 3 - Parallel Universes (Branches)

You now have snapshots and can see what changed between them. But what if you want to try an experiment without messing up your main work? What if you want to work on two features simultaneously?

**That's where branches come in!** 🌳

In the next chapter, we'll add the ability to create "parallel universes" where you can work on different ideas independently, then merge them back together when you're ready.

---

*Remember: Git's diff algorithm (Myers) is more sophisticated than our simple version, but the core concept is the same: compare files line by line and show what changed!*
