# Chapter 3: Parallel Universes (Understanding Branches)

## The Problem

You're working on two features:
1. A new user authentication system
2. A redesigned homepage

You don't want to mix them up. You want to work on auth, test it, then work on homepage, test it, then combine them. With snapshots alone, you'd have to carefully manage which files belong to which feature. Nightmare!

## The "Aha!" Moment

**Branches are just pointers to snapshots!**

That's it. That's the secret. A branch is nothing more than a lightweight pointer that says "I'm currently pointing at snapshot #7". When you make a new commit, the pointer moves forward to the new snapshot.

```
main branch:    snapshot1 → snapshot2 → snapshot3 → snapshot4
                                              ↑
                                          (main points here)

auth-feature:   snapshot1 → snapshot2 → snapshot5
                                          ↑
                                     (auth-feature points here)
```

**Git branches are "cheap"** because they're just tiny files containing a snapshot ID. Creating a branch doesn't copy your project—it just creates a new pointer!

## What We're Building

We'll add branch support to our snapshot system:

```bash
python3 snapshot.py branch create auth-feature
python3 snapshot.py branch switch auth-feature
python3 snapshot.py save "Added login function"
python3 snapshot.py branch list
python3 snapshot.py branch switch main
```

## How Branches Work

### Concept 1: HEAD

`HEAD` is a special pointer that points to the current branch. It tells you "where you are right now".

```
HEAD → main → snapshot4
```

When you switch branches, HEAD moves:

```
HEAD → auth-feature → snapshot5
```

### Concept 2: Branch Creation

Creating a branch:
1. Take the current snapshot ID
2. Create a new branch file pointing to that ID
3. Now you have two pointers to the same snapshot

```
Before:
HEAD → main → snapshot4

After creating "experiment" branch:
HEAD → main → snapshot4
       experiment → snapshot4   (same snapshot!)
```

### Concept 3: Diverging Branches

When you make a commit on a branch:
1. Save the new snapshot
2. Update the branch pointer to the new snapshot
3. Other branches stay where they were

```
After commit on experiment:
HEAD → experiment → snapshot5
       main → snapshot4
```

Now the branches have "diverged"!

## The Code

Let's build it! Create `branches.py`:

```python
#!/usr/bin/env python3
"""
Add branching capability to our snapshot system.

Usage:
    python3 branches.py create <branch-name>
    python3 branches.py list
    python3 branches.py switch <branch-name>
    python3 branches.py current
    python3 branches.py delete <branch-name>

Examples:
    python3 branches.py create feature-auth
    python3 branches.py switch feature-auth
    python3 branches.py list
"""

import os
import sys
import json

SNAPSHOT_DIR = '.snapshots'
BRANCHES_DIR = os.path.join(SNAPSHOT_DIR, 'branches')
HEAD_FILE = os.path.join(SNAPSHOT_DIR, 'HEAD')


def init_branch_system():
    """Initialize the branch system if it doesn't exist."""
    if not os.path.exists(SNAPSHOT_DIR):
        os.makedirs(SNAPSHOT_DIR)
    
    if not os.path.exists(BRANCHES_DIR):
        os.makedirs(BRANCHES_DIR)
    
    # Create main branch if it doesn't exist
    main_branch_file = os.path.join(BRANCHES_DIR, 'main')
    if not os.path.exists(main_branch_file):
        # Find the latest snapshot
        snapshots = sorted([d for d in os.listdir(SNAPSHOT_DIR) 
                           if os.path.isdir(os.path.join(SNAPSHOT_DIR, d)) and d != 'branches'])
        
        if snapshots:
            latest_snapshot = snapshots[-1]
        else:
            latest_snapshot = None
        
        with open(main_branch_file, 'w') as f:
            f.write(latest_snapshot or '')
    
    # Create HEAD file if it doesn't exist
    if not os.path.exists(HEAD_FILE):
        with open(HEAD_FILE, 'w') as f:
            f.write('main')  # HEAD points to main by default


def get_current_branch():
    """Get the name of the current branch."""
    init_branch_system()
    
    with open(HEAD_FILE, 'r') as f:
        return f.read().strip()


def get_branch_snapshot(branch_name):
    """Get the snapshot that a branch points to."""
    branch_file = os.path.join(BRANCHES_DIR, branch_name)
    
    if not os.path.exists(branch_file):
        return None
    
    with open(branch_file, 'r') as f:
        return f.read().strip()


def set_branch_snapshot(branch_name, snapshot_name):
    """Update a branch to point to a specific snapshot."""
    branch_file = os.path.join(BRANCHES_DIR, branch_name)
    
    with open(branch_file, 'w') as f:
        f.write(snapshot_name)


def list_branches():
    """List all branches and show which is current."""
    init_branch_system()
    
    current = get_current_branch()
    branches = sorted([b for b in os.listdir(BRANCHES_DIR) if os.path.isfile(os.path.join(BRANCHES_DIR, b))])
    
    print("\nBranches:")
    print("-" * 70)
    
    for branch in branches:
        snapshot = get_branch_snapshot(branch)
        marker = "* " if branch == current else "  "
        
        if snapshot:
            # Extract snapshot number
            parts = snapshot.split('_')
            if parts and parts[0] == 'snapshot':
                snap_num = parts[1]
            else:
                snap_num = '???'
        else:
            snap_num = 'none'
        
        print(f"{marker}{branch}")
        print(f"  └─ points to snapshot #{snap_num}")


def create_branch(branch_name):
    """Create a new branch pointing to the current snapshot."""
    init_branch_system()
    
    # Check if branch already exists
    branch_file = os.path.join(BRANCHES_DIR, branch_name)
    if os.path.exists(branch_file):
        print(f"Error: Branch '{branch_name}' already exists.")
        return
    
    # Get current branch's snapshot
    current_branch = get_current_branch()
    current_snapshot = get_branch_snapshot(current_branch)
    
    if not current_snapshot:
        print("Error: No snapshots exist yet. Make a commit first.")
        return
    
    # Create new branch pointing to the same snapshot
    set_branch_snapshot(branch_name, current_snapshot)
    
    print(f"✓ Created branch '{branch_name}' pointing to {current_snapshot}")


def switch_branch(branch_name):
    """Switch to a different branch."""
    init_branch_system()
    
    # Check if branch exists
    branch_file = os.path.join(BRANCHES_DIR, branch_name)
    if not os.path.exists(branch_file):
        print(f"Error: Branch '{branch_name}' doesn't exist.")
        print(f"Use 'create {branch_name}' to create it.")
        return
    
    current_branch = get_current_branch()
    if branch_name == current_branch:
        print(f"Already on branch '{branch_name}'")
        return
    
    # Update HEAD to point to new branch
    with open(HEAD_FILE, 'w') as f:
        f.write(branch_name)
    
    # Get the snapshot this branch points to
    snapshot = get_branch_snapshot(branch_name)
    
    if snapshot:
        print(f"✓ Switched to branch '{branch_name}'")
        print(f"  (currently at {snapshot})")
        print(f"\n💡 Tip: Your working directory hasn't changed yet!")
        print(f"    Use 'restore' to update files to this branch's state.")
    else:
        print(f"✓ Switched to branch '{branch_name}' (empty branch)")


def delete_branch(branch_name):
    """Delete a branch (except main and current)."""
    init_branch_system()
    
    if branch_name == 'main':
        print("Error: Cannot delete 'main' branch.")
        return
    
    current = get_current_branch()
    if branch_name == current:
        print(f"Error: Cannot delete current branch '{branch_name}'.")
        print(f"Switch to another branch first.")
        return
    
    branch_file = os.path.join(BRANCHES_DIR, branch_name)
    if not os.path.exists(branch_file):
        print(f"Error: Branch '{branch_name}' doesn't exist.")
        return
    
    os.remove(branch_file)
    print(f"✓ Deleted branch '{branch_name}'")


def show_current():
    """Show the current branch."""
    init_branch_system()
    current = get_current_branch()
    snapshot = get_branch_snapshot(current)
    
    print(f"\nCurrent branch: {current}")
    if snapshot:
        print(f"Points to: {snapshot}")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    command = sys.argv[1]
    
    if command == 'list':
        list_branches()
    
    elif command == 'current':
        show_current()
    
    elif command == 'create':
        if len(sys.argv) < 3:
            print("Error: 'create' requires a branch name")
            return
        create_branch(sys.argv[2])
    
    elif command == 'switch':
        if len(sys.argv) < 3:
            print("Error: 'switch' requires a branch name")
            return
        switch_branch(sys.argv[2])
    
    elif command == 'delete':
        if len(sys.argv) < 3:
            print("Error: 'delete' requires a branch name")
            return
        delete_branch(sys.argv[2])
    
    else:
        print(f"Unknown command: {command}")
        print(__doc__)


if __name__ == '__main__':
    main()
```

## Integration with snapshot.py

We need to update `snapshot.py` to work with branches. When saving a snapshot, we should:
1. Create the snapshot
2. Update the current branch to point to the new snapshot

Add this function to `snapshot.py`:

```python
def update_current_branch(snapshot_name):
    """Update the current branch to point to the new snapshot."""
    HEAD_FILE = os.path.join(SNAPSHOT_DIR, 'HEAD')
    BRANCHES_DIR = os.path.join(SNAPSHOT_DIR, 'branches')
    
    if not os.path.exists(HEAD_FILE):
        return  # No branch system yet, skip
    
    # Get current branch
    with open(HEAD_FILE, 'r') as f:
        current_branch = f.read().strip()
    
    # Update branch pointer
    branch_file = os.path.join(BRANCHES_DIR, current_branch)
    with open(branch_file, 'w') as f:
        f.write(snapshot_name)
```

Then modify the `save_snapshot()` function to call it at the end:

```python
def save_snapshot(message):
    # ... existing code ...
    
    print(f"✓ Saved snapshot #{snapshot_num}: {message}")
    print(f"  ({file_count} items backed up)")
    
    # Update the current branch to point to this snapshot
    update_current_branch(snapshot_name)
    
    return snapshot_num
```

## Try It Out!

```bash
# Start with main branch
python3 snapshot.py save "Initial commit on main"

# Create a feature branch
python3 branches.py create feature-auth
python3 branches.py switch feature-auth

# Make changes on feature branch
echo "def login(): pass" > auth.py
python3 snapshot.py save "Added login function"

echo "def logout(): pass" >> auth.py
python3 snapshot.py save "Added logout function"

# Switch back to main
python3 branches.py switch main
# Notice: auth.py still exists! The files haven't changed.

# Create another branch
python3 branches.py create feature-homepage
python3 branches.py switch feature-homepage

# Make changes
echo "<h1>Welcome!</h1>" > index.html
python3 snapshot.py save "New homepage"

# List all branches
python3 branches.py list
```

You should see:
```
Branches:
----------------------------------------------------------------------
  feature-auth
  └─ points to snapshot #003
  feature-homepage
  └─ points to snapshot #004
* main
  └─ points to snapshot #001
```

## The "Aha!" Moment (Part 2)

**Branches let you maintain multiple "timelines" of your project!**

- `main` is at snapshot #1
- `feature-auth` is at snapshot #3
- `feature-homepage` is at snapshot #4

They all started at the same place (snapshot #1) but then diverged. This is how teams work on features independently!

## Challenges

### Challenge 1: Branch From Specific Snapshot
Allow creating a branch from any snapshot, not just the current one:

```bash
python3 branches.py create bugfix --from 5
```

### Challenge 2: Branch History
Show the history of commits on a branch:

```bash
python3 branches.py history feature-auth
```

### Challenge 3: Visual Branch Graph
Create a visual representation of branches:

```
* main (snapshot #1)
  ├─ feature-auth (snapshot #3)
  └─ feature-homepage (snapshot #4)
```

## Key Concepts

1. **Branches are pointers**: Just files containing snapshot IDs
2. **HEAD is a pointer to a pointer**: It points to the current branch
3. **Divergence is natural**: Branches start together, then grow apart
4. **Cheap and fast**: Creating/switching branches is instant

## Next Up: Chapter 4 - Merging Realities

You have parallel universes (branches), but how do you combine them back together? What if both branches changed the same file? What if they changed the same LINE?

**That's where merging comes in!** 🔀

In the next chapter, we'll implement git's merge algorithm and learn how to combine divergent branches (and handle the inevitable conflicts).

---

*Remember: In real git, a branch is literally just a 41-byte file containing a commit SHA. That's why git can have thousands of branches with zero overhead!*
