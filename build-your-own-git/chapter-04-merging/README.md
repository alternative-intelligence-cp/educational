# Chapter 4: Merging Realities (Understanding Merge)

## The Problem

You've been working on `feature-auth` while your teammate worked on `feature-homepage`. Both branches have great code. Now you want to combine them into `main`. 

But wait... both of you modified `config.py`. Your branch added authentication settings. Their branch added homepage settings. How do you combine them without losing anyone's work?

## The "Aha!" Moment

**Merging uses a "common ancestor" to figure out what changed!**

Think about it like this:

```
Common ancestor (snapshot #1):
    config.py contains: DEBUG = True

Your branch (snapshot #3):
    config.py contains: DEBUG = True
                        AUTH_TIMEOUT = 3600

Their branch (snapshot #4):
    config.py contains: DEBUG = True
                        HOMEPAGE_CACHE = True
```

Git looks at all three versions:
1. **Base**: What did it look like at the common ancestor?
2. **Yours**: What did YOU change since then?
3. **Theirs**: What did THEY change since then?

If your changes and their changes don't overlap, git can automatically merge them:

```
Merged result:
    DEBUG = True
    AUTH_TIMEOUT = 3600    ← from your branch
    HOMEPAGE_CACHE = True  ← from their branch
```

**This is called a "3-way merge"** (base, yours, theirs).

## What We're Building

We'll add merge capability to our snapshot system:

```bash
# Switch to main
python3 branches.py switch main

# Merge feature-auth into main
python3 merge.py feature-auth
```

This will:
- Find the common ancestor
- Compare changes from both branches
- Automatically merge non-conflicting changes
- Alert you to conflicts that need manual resolution

## How 3-Way Merge Works

### Scenario 1: No Conflicts (Easy!)

```
Base:           Yours:          Theirs:
line 1          line 1          line 1
line 2          line 2          NEW LINE!
line 3          MODIFIED!       line 3

Result:
line 1
NEW LINE!       ← from theirs
MODIFIED!       ← from yours
```

Both changes are accepted because they modified different parts.

### Scenario 2: Conflict! (Uh oh...)

```
Base:           Yours:          Theirs:
line 1          line 1          line 1
line 2          YOURS!          THEIRS!
line 3          line 3          line 3
```

Both branches modified line 2. Git can't decide which to keep. This is a **merge conflict**.

Git will mark it like this:

```
line 1
<<<<<<< HEAD (yours)
YOURS!
=======
THEIRS!
>>>>>>> feature-branch
line 3
```

**You** have to manually decide what to keep.

### Finding the Common Ancestor

How do we find where the branches diverged? We need to trace back the branch history:

```
snapshot #1 → snapshot #2 → snapshot #3 (your branch)
             ↘ snapshot #4 (their branch)

Common ancestor: snapshot #2
```

## The Code

This is our most complex chapter yet! Let's build it step by step.

### Step 1: Find Common Ancestor

```python
#!/usr/bin/env python3
"""
Add merge capability to our snapshot system.

Usage:
    python3 merge.py <branch-name>

Examples:
    python3 merge.py feature-auth
"""

import os
import sys

SNAPSHOT_DIR = '.snapshots'
BRANCHES_DIR = os.path.join(SNAPSHOT_DIR, 'branches')
HEAD_FILE = os.path.join(SNAPSHOT_DIR, 'HEAD')


def get_current_branch():
    """Get the name of the current branch."""
    with open(HEAD_FILE, 'r') as f:
        return f.read().strip()


def get_branch_snapshot(branch_name):
    """Get the snapshot a branch points to."""
    branch_file = os.path.join(BRANCHES_DIR, branch_name)
    with open(branch_file, 'r') as f:
        return f.read().strip()


def get_snapshot_number(snapshot_name):
    """Extract snapshot number from snapshot folder name."""
    if not snapshot_name:
        return 0
    parts = snapshot_name.split('_')
    if parts and parts[0] == 'snapshot':
        return int(parts[1])
    return 0


def find_common_ancestor(snapshot1, snapshot2):
    """
    Find common ancestor of two snapshots.
    
    For simplicity, we'll use a naive approach:
    The common ancestor is the snapshot with the smaller number.
    
    In real git, this uses a graph traversal algorithm to find
    the "merge base" - the most recent common ancestor.
    """
    num1 = get_snapshot_number(snapshot1)
    num2 = get_snapshot_number(snapshot2)
    
    # Naive approach: return the earlier snapshot
    common_num = min(num1, num2)
    
    # Find snapshot folder name for this number
    snapshots = sorted([d for d in os.listdir(SNAPSHOT_DIR) 
                       if os.path.isdir(os.path.join(SNAPSHOT_DIR, d)) and d != 'branches'])
    
    for snapshot in snapshots:
        if get_snapshot_number(snapshot) == common_num:
            return snapshot
    
    return snapshots[0] if snapshots else None


def get_files_in_snapshot(snapshot_path):
    """Get a dictionary of files in a snapshot: {relative_path: full_path}"""
    files = {}
    
    for root, dirs, filenames in os.walk(snapshot_path):
        for filename in filenames:
            if filename == 'SNAPSHOT_MESSAGE.txt':
                continue
            
            full_path = os.path.join(root, filename)
            rel_path = os.path.relpath(full_path, snapshot_path)
            files[rel_path] = full_path
    
    return files


def read_file_lines(filepath):
    """Read a file and return its lines."""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            return f.readlines()
    except:
        return []


def merge_files(base_lines, ours_lines, theirs_lines):
    """
    3-way merge of file contents.
    
    Returns: (merged_lines, has_conflicts)
    """
    # For simplicity, we'll do a very basic line-by-line merge
    # Real git uses sophisticated algorithms
    
    result = []
    conflicts = []
    
    # If files are identical, no merge needed
    if ours_lines == theirs_lines:
        return ours_lines, False
    
    # Simple strategy: if either side changed from base, use that change
    # If both changed differently, mark as conflict
    
    max_len = max(len(base_lines), len(ours_lines), len(theirs_lines))
    
    i = 0
    while i < max_len:
        base_line = base_lines[i] if i < len(base_lines) else None
        our_line = ours_lines[i] if i < len(ours_lines) else None
        their_line = theirs_lines[i] if i < len(theirs_lines) else None
        
        if our_line == their_line:
            # Both sides agree (or both added same line)
            if our_line is not None:
                result.append(our_line)
        elif our_line == base_line:
            # We didn't change it, they did - use theirs
            if their_line is not None:
                result.append(their_line)
        elif their_line == base_line:
            # They didn't change it, we did - use ours
            if our_line is not None:
                result.append(our_line)
        else:
            # CONFLICT! Both changed the same line differently
            result.append(f"<<<<<<< HEAD\n")
            if our_line:
                result.append(our_line)
            result.append(f"=======\n")
            if their_line:
                result.append(their_line)
            result.append(f">>>>>>> merge\n")
            conflicts.append(i + 1)  # Line number (1-indexed)
        
        i += 1
    
    has_conflicts = len(conflicts) > 0
    return result, has_conflicts


def merge_branch(branch_name):
    """Merge a branch into the current branch."""
    current_branch = get_current_branch()
    
    if branch_name == current_branch:
        print(f"Error: Cannot merge a branch into itself.")
        return
    
    # Get snapshots
    current_snapshot = get_branch_snapshot(current_branch)
    merge_snapshot = get_branch_snapshot(branch_name)
    
    if not merge_snapshot:
        print(f"Error: Branch '{branch_name}' doesn't exist.")
        return
    
    print(f"\n🔀 Merging '{branch_name}' into '{current_branch}'...")
    
    # Find common ancestor
    base_snapshot = find_common_ancestor(current_snapshot, merge_snapshot)
    
    print(f"   Base: {base_snapshot}")
    print(f"   Ours: {current_snapshot} ({current_branch})")
    print(f"   Theirs: {merge_snapshot} ({branch_name})")
    
    # Get file lists
    base_path = os.path.join(SNAPSHOT_DIR, base_snapshot)
    ours_path = os.path.join(SNAPSHOT_DIR, current_snapshot)
    theirs_path = os.path.join(SNAPSHOT_DIR, merge_snapshot)
    
    base_files = get_files_in_snapshot(base_path)
    our_files = get_files_in_snapshot(ours_path)
    their_files = get_files_in_snapshot(theirs_path)
    
    # Collect all file paths
    all_files = set(base_files.keys()) | set(our_files.keys()) | set(their_files.keys())
    
    conflicts = []
    merged_files = {}
    
    print(f"\n📝 Merging {len(all_files)} files...")
    
    for filepath in sorted(all_files):
        # Get file contents from each version
        base_lines = read_file_lines(base_files[filepath]) if filepath in base_files else []
        our_lines = read_file_lines(our_files[filepath]) if filepath in our_files else []
        their_lines = read_file_lines(their_files[filepath]) if filepath in their_files else []
        
        # Merge
        merged, has_conflict = merge_files(base_lines, our_lines, their_lines)
        merged_files[filepath] = merged
        
        if has_conflict:
            conflicts.append(filepath)
            print(f"   ⚠️  CONFLICT: {filepath}")
        elif our_lines != their_lines:
            print(f"   ✓ Merged: {filepath}")
    
    # Write merged files
    print(f"\n📁 Writing merged files...")
    for filepath, lines in merged_files.items():
        # Write to current working directory
        os.makedirs(os.path.dirname(filepath) if os.path.dirname(filepath) else '.', exist_ok=True)
        with open(filepath, 'w', encoding='utf-8') as f:
            f.writelines(lines)
    
    if conflicts:
        print(f"\n⚠️  MERGE CONFLICTS in {len(conflicts)} file(s):")
        for filepath in conflicts:
            print(f"   - {filepath}")
        print(f"\n💡 Resolve conflicts manually, then commit the result.")
        print(f"   Look for <<<<<<< and >>>>>>> markers in the files.")
    else:
        print(f"\n✅ Merge successful! No conflicts.")
        print(f"💡 Review the changes and commit the merge:")
        print(f"   python3 snapshot.py save \"Merged {branch_name} into {current_branch}\"")


def main():
    if len(sys.argv) != 2:
        print(__doc__)
        return
    
    branch_name = sys.argv[1]
    merge_branch(branch_name)


if __name__ == '__main__':
    main()
```

## Try It Out!

Let's create a realistic merge scenario:

```bash
# 1. Start with a base file
echo "# Config" > config.py
echo "DEBUG = True" >> config.py
python3 snapshot.py save "Initial config"

# 2. Create feature-auth branch and make changes
python3 branches.py create feature-auth
python3 branches.py switch feature-auth
echo "AUTH_TIMEOUT = 3600" >> config.py
python3 snapshot.py save "Added auth settings"

# 3. Switch back to main and create feature-homepage
python3 branches.py switch main
python3 branches.py create feature-homepage
python3 branches.py switch feature-homepage
echo "HOMEPAGE_CACHE = True" >> config.py
python3 snapshot.py save "Added homepage settings"

# 4. Merge feature-homepage into main
python3 branches.py switch main
python3 merge.py feature-homepage

# 5. Now merge feature-auth (should work cleanly!)
python3 merge.py feature-auth
```

You should see both settings merged together!

## Creating a Conflict

Now let's intentionally create a conflict:

```bash
# 1. Start fresh
echo "version = 1" > version.py
python3 snapshot.py save "Initial version"

# 2. Branch A changes version
python3 branches.py create branch-a
python3 branches.py switch branch-a
echo "version = 2" > version.py
python3 snapshot.py save "Bumped to version 2"

# 3. Branch B changes version differently
python3 branches.py switch main
python3 branches.py create branch-b
python3 branches.py switch branch-b
echo "version = 3" > version.py
python3 snapshot.py save "Bumped to version 3"

# 4. Try to merge - CONFLICT!
python3 branches.py switch main
python3 merge.py branch-a  # OK
python3 merge.py branch-b  # CONFLICT!
```

You'll see:

```
<<<<<<< HEAD
version = 2
=======
version = 3
>>>>>>> merge
```

You decide: keep `version = 2`, `version = 3`, or maybe `version = 4`?

## Challenges

### Challenge 1: Smart Merge
Improve the merge algorithm to handle added/removed lines better. Currently, it's very basic.

### Challenge 2: Merge Visualization
Show a visual diff before and after merge:

```
Before merge:
  main:        config.py (10 lines)
  feature-auth: config.py (12 lines)

After merge:
  config.py (13 lines)
  - 2 lines from main
  - 3 lines from feature-auth
```

### Challenge 3: Abort Merge
Add ability to abort a conflicted merge:

```bash
python3 merge.py --abort
```

This should restore files to pre-merge state.

## Key Concepts

1. **3-way merge**: Uses base, ours, and theirs to determine changes
2. **Common ancestor**: The snapshot where branches diverged
3. **Automatic merge**: Non-overlapping changes merge cleanly
4. **Conflicts**: Overlapping changes require manual resolution
5. **Conflict markers**: `<<<<<<<`, `=======`, `>>>>>>>` show both versions

## Next Up: Chapter 5 - Working Together (Remotes)

You can now branch and merge like a pro! But all your work is on your local machine. What if you want to:
- Share your code with teammates?
- Back up your work to the cloud?
- Collaborate on the same project?

**That's where remotes come in!** 🌐

In the final chapter, we'll add the ability to push and pull snapshots to/from remote locations, completing our understanding of how git enables distributed collaboration.

---

*Remember: Real git's merge algorithm (called "recursive merge") is much more sophisticated, but the core concept is the same: compare base, ours, and theirs to automatically merge changes!*
