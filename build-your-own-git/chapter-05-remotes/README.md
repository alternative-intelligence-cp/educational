# Chapter 5: Working Together (Understanding Remotes)

## The Problem

You've built an amazing feature on your laptop. Your teammate is working on their desktop. Your manager wants to see the code on their machine. Your server needs to deploy the latest version.

**Everyone has their own copy of the project, but they're all out of sync!**

How do you keep everyone's code in sync without manually copying files around?

## The "Aha!" Moment

**Git is "distributed" - every copy is complete and equal!**

There's no "central server" in git (though GitHub acts like one). Every clone of a repository is a full copy with complete history. You can push and pull changes between ANY two repositories.

```
Your laptop ←→ GitHub ←→ Teammate's laptop
                ↕
            Server (production)
```

Each arrow is a potential push/pull connection. GitHub just happens to be the most convenient meeting point.

## What We're Building

We'll add remote capabilities to our snapshot system:

```bash
# Add a remote (like GitHub)
python3 remote.py add origin /path/to/shared/folder

# Push your branch to the remote
python3 remote.py push origin main

# Pull changes from the remote
python3 remote.py pull origin main

# List remotes
python3 remote.py list
```

## How Remotes Work

### Concept 1: Remote Repositories

A "remote" is just another copy of your repository somewhere else:
- A folder on a shared network drive
- A folder on a server (accessed via SSH)
- GitHub's servers (accessed via HTTPS or SSH)

For our implementation, we'll keep it simple: remotes are just folders on your filesystem.

### Concept 2: Push

**Pushing** means:
1. Look at your local branch
2. Look at the remote branch
3. Copy over any snapshots the remote doesn't have
4. Update the remote branch pointer

```
Before push:
  Local main:  snapshot1 → snapshot2 → snapshot3
  Remote main: snapshot1 → snapshot2

After push:
  Local main:  snapshot1 → snapshot2 → snapshot3
  Remote main: snapshot1 → snapshot2 → snapshot3  ✓
```

### Concept 3: Pull

**Pulling** means:
1. Look at the remote branch
2. Copy any snapshots you don't have locally
3. Merge the remote changes into your local branch

```
Before pull:
  Local main:  snapshot1 → snapshot2
  Remote main: snapshot1 → snapshot2 → snapshot3 → snapshot4

After pull:
  Local main:  snapshot1 → snapshot2 → snapshot3 → snapshot4  ✓
  Remote main: snapshot1 → snapshot2 → snapshot3 → snapshot4
```

### Concept 4: Conflicts During Pull

What if both you AND the remote have new commits?

```
Local main:  snapshot1 → snapshot2 → snapshot3 (yours)
Remote main: snapshot1 → snapshot2 → snapshot4 (theirs)
```

Git will:
1. Pull snapshot4 from remote
2. Try to merge it with your snapshot3
3. If there are conflicts, ask you to resolve them

This is the same merge algorithm from Chapter 4!

## The Code

Let's build our remote system:

```python
#!/usr/bin/env python3
"""
Add remote repository support to our snapshot system.

Usage:
    python3 remote.py add <name> <path>
    python3 remote.py list
    python3 remote.py remove <name>
    python3 remote.py push <name> <branch>
    python3 remote.py pull <name> <branch>

Examples:
    python3 remote.py add origin /shared/project
    python3 remote.py push origin main
    python3 remote.py pull origin main
"""

import os
import sys
import shutil
import json

SNAPSHOT_DIR = '.snapshots'
BRANCHES_DIR = os.path.join(SNAPSHOT_DIR, 'branches')
REMOTES_FILE = os.path.join(SNAPSHOT_DIR, 'remotes.json')
HEAD_FILE = os.path.join(SNAPSHOT_DIR, 'HEAD')


def load_remotes():
    """Load the remotes configuration."""
    if not os.path.exists(REMOTES_FILE):
        return {}
    
    with open(REMOTES_FILE, 'r') as f:
        return json.load(f)


def save_remotes(remotes):
    """Save the remotes configuration."""
    os.makedirs(SNAPSHOT_DIR, exist_ok=True)
    with open(REMOTES_FILE, 'w') as f:
        json.dump(remotes, f, indent=2)


def add_remote(name, path):
    """Add a remote repository."""
    # Expand path
    path = os.path.abspath(os.path.expanduser(path))
    
    remotes = load_remotes()
    
    if name in remotes:
        print(f"Error: Remote '{name}' already exists.")
        print(f"Current path: {remotes[name]}")
        return
    
    remotes[name] = path
    save_remotes(remotes)
    
    print(f"✓ Added remote '{name}': {path}")


def list_remotes():
    """List all configured remotes."""
    remotes = load_remotes()
    
    if not remotes:
        print("No remotes configured.")
        print("\nAdd one with: python3 remote.py add <name> <path>")
        return
    
    print("\nConfigured remotes:")
    print("-" * 70)
    for name, path in remotes.items():
        exists = "✓" if os.path.exists(path) else "✗ (not found)"
        print(f"  {name}: {path} {exists}")


def remove_remote(name):
    """Remove a remote."""
    remotes = load_remotes()
    
    if name not in remotes:
        print(f"Error: Remote '{name}' doesn't exist.")
        return
    
    path = remotes[name]
    del remotes[name]
    save_remotes(remotes)
    
    print(f"✓ Removed remote '{name}' ({path})")


def get_branch_snapshot(branch_name, repo_path='.'):
    """Get the snapshot a branch points to."""
    branch_file = os.path.join(repo_path, SNAPSHOT_DIR, 'branches', branch_name)
    
    if not os.path.exists(branch_file):
        return None
    
    with open(branch_file, 'r') as f:
        return f.read().strip()


def set_branch_snapshot(branch_name, snapshot_name, repo_path='.'):
    """Set a branch to point to a specific snapshot."""
    branch_file = os.path.join(repo_path, SNAPSHOT_DIR, 'branches', branch_name)
    
    os.makedirs(os.path.dirname(branch_file), exist_ok=True)
    
    with open(branch_file, 'w') as f:
        f.write(snapshot_name)


def get_snapshot_number(snapshot_name):
    """Extract snapshot number from folder name."""
    if not snapshot_name:
        return 0
    parts = snapshot_name.split('_')
    if parts and parts[0] == 'snapshot':
        return int(parts[1])
    return 0


def push_to_remote(remote_name, branch_name):
    """Push a branch to a remote repository."""
    remotes = load_remotes()
    
    if remote_name not in remotes:
        print(f"Error: Remote '{remote_name}' doesn't exist.")
        print("Use 'list' to see available remotes.")
        return
    
    remote_path = remotes[remote_name]
    
    if not os.path.exists(remote_path):
        print(f"Error: Remote path doesn't exist: {remote_path}")
        return
    
    # Get local branch snapshot
    local_snapshot = get_branch_snapshot(branch_name, '.')
    
    if not local_snapshot:
        print(f"Error: Local branch '{branch_name}' doesn't exist.")
        return
    
    print(f"\n📤 Pushing '{branch_name}' to '{remote_name}'...")
    
    # Get remote branch snapshot (if it exists)
    remote_snapshot = get_branch_snapshot(branch_name, remote_path)
    
    if remote_snapshot:
        local_num = get_snapshot_number(local_snapshot)
        remote_num = get_snapshot_number(remote_snapshot)
        
        if local_num < remote_num:
            print(f"Error: Remote is ahead of local branch.")
            print(f"  Local:  {local_snapshot}")
            print(f"  Remote: {remote_snapshot}")
            print(f"\n💡 Pull first: python3 remote.py pull {remote_name} {branch_name}")
            return
        
        if local_num == remote_num:
            print(f"✓ Already up to date!")
            return
    
    # Copy snapshots to remote
    local_snapshots_dir = SNAPSHOT_DIR
    remote_snapshots_dir = os.path.join(remote_path, SNAPSHOT_DIR)
    
    os.makedirs(remote_snapshots_dir, exist_ok=True)
    
    # Get all local snapshots
    local_snapshots = sorted([d for d in os.listdir(local_snapshots_dir)
                             if os.path.isdir(os.path.join(local_snapshots_dir, d)) 
                             and d != 'branches'])
    
    # Copy missing snapshots
    copied = 0
    for snapshot in local_snapshots:
        src = os.path.join(local_snapshots_dir, snapshot)
        dst = os.path.join(remote_snapshots_dir, snapshot)
        
        if not os.path.exists(dst):
            shutil.copytree(src, dst)
            copied += 1
            print(f"   ✓ Copied {snapshot}")
    
    # Update remote branch pointer
    set_branch_snapshot(branch_name, local_snapshot, remote_path)
    
    print(f"\n✅ Push successful!")
    print(f"   Copied {copied} snapshot(s)")
    print(f"   Remote '{branch_name}' now at {local_snapshot}")


def pull_from_remote(remote_name, branch_name):
    """Pull a branch from a remote repository."""
    remotes = load_remotes()
    
    if remote_name not in remotes:
        print(f"Error: Remote '{remote_name}' doesn't exist.")
        return
    
    remote_path = remotes[remote_name]
    
    if not os.path.exists(remote_path):
        print(f"Error: Remote path doesn't exist: {remote_path}")
        return
    
    print(f"\n📥 Pulling '{branch_name}' from '{remote_name}'...")
    
    # Get remote branch snapshot
    remote_snapshot = get_branch_snapshot(branch_name, remote_path)
    
    if not remote_snapshot:
        print(f"Error: Remote branch '{branch_name}' doesn't exist.")
        return
    
    # Get local branch snapshot
    local_snapshot = get_branch_snapshot(branch_name, '.')
    
    if local_snapshot:
        remote_num = get_snapshot_number(remote_snapshot)
        local_num = get_snapshot_number(local_snapshot)
        
        if remote_num < local_num:
            print(f"✓ Local branch is ahead of remote. Nothing to pull.")
            return
        
        if remote_num == local_num:
            print(f"✓ Already up to date!")
            return
        
        if local_num > 0 and remote_num > local_num:
            print(f"⚠️  Diverged branches detected!")
            print(f"  Local:  {local_snapshot}")
            print(f"  Remote: {remote_snapshot}")
            print(f"\n💡 This would require a merge.")
            print(f"   For now, use push --force to overwrite remote")
            return
    
    # Copy snapshots from remote
    remote_snapshots_dir = os.path.join(remote_path, SNAPSHOT_DIR)
    local_snapshots_dir = SNAPSHOT_DIR
    
    os.makedirs(local_snapshots_dir, exist_ok=True)
    
    # Get all remote snapshots
    remote_snapshots = sorted([d for d in os.listdir(remote_snapshots_dir)
                              if os.path.isdir(os.path.join(remote_snapshots_dir, d))
                              and d != 'branches'])
    
    # Copy missing snapshots
    copied = 0
    for snapshot in remote_snapshots:
        src = os.path.join(remote_snapshots_dir, snapshot)
        dst = os.path.join(local_snapshots_dir, snapshot)
        
        if not os.path.exists(dst):
            shutil.copytree(src, dst)
            copied += 1
            print(f"   ✓ Copied {snapshot}")
    
    # Update local branch pointer
    set_branch_snapshot(branch_name, remote_snapshot, '.')
    
    print(f"\n✅ Pull successful!")
    print(f"   Copied {copied} snapshot(s)")
    print(f"   Local '{branch_name}' now at {remote_snapshot}")
    print(f"\n💡 Use restore to update your working files:")
    print(f"   python3 snapshot.py restore {get_snapshot_number(remote_snapshot)}")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    command = sys.argv[1]
    
    if command == 'add':
        if len(sys.argv) != 4:
            print("Usage: python3 remote.py add <name> <path>")
            return
        add_remote(sys.argv[2], sys.argv[3])
    
    elif command == 'list':
        list_remotes()
    
    elif command == 'remove':
        if len(sys.argv) != 3:
            print("Usage: python3 remote.py remove <name>")
            return
        remove_remote(sys.argv[2])
    
    elif command == 'push':
        if len(sys.argv) != 4:
            print("Usage: python3 remote.py push <remote> <branch>")
            return
        push_to_remote(sys.argv[2], sys.argv[3])
    
    elif command == 'pull':
        if len(sys.argv) != 4:
            print("Usage: python3 remote.py pull <remote> <branch>")
            return
        pull_from_remote(sys.argv[2], sys.argv[3])
    
    else:
        print(f"Unknown command: {command}")
        print(__doc__)


if __name__ == '__main__':
    main()
```

## Try It Out!

### Simulating Two Developers

Let's create a realistic scenario with two "developers" (two folders):

```bash
# 1. Create a shared remote (like GitHub)
mkdir /tmp/shared-repo
cd /tmp/shared-repo
python3 /path/to/snapshot.py save "Initial commit"

# 2. Developer 1 sets it up
cd /tmp/dev1
python3 /path/to/snapshot.py save "Dev1 initial work"
python3 /path/to/remote.py add origin /tmp/shared-repo
python3 /path/to/remote.py push origin main

# 3. Developer 2 pulls it down
cd /tmp/dev2
python3 /path/to/remote.py add origin /tmp/shared-repo
python3 /path/to/remote.py pull origin main
python3 /path/to/snapshot.py restore 1

# 4. Dev2 makes changes and pushes
echo "print('Hello from Dev2')" > dev2.py
python3 /path/to/snapshot.py save "Dev2 feature"
python3 /path/to/remote.py push origin main

# 5. Dev1 pulls Dev2's changes
cd /tmp/dev1
python3 /path/to/remote.py pull origin main
python3 /path/to/snapshot.py restore 2
ls  # You should see dev2.py!
```

Congratulations! You've simulated distributed collaboration!

## The GitHub Connection

**How does this relate to GitHub?**

GitHub is essentially:
1. A remote repository (like our `/tmp/shared-repo`)
2. Accessible over the internet (via HTTPS/SSH)
3. With a pretty web interface
4. Plus extras: pull requests, issues, actions, etc.

When you do `git push origin main`:
- `origin` is the remote (GitHub's server)
- `main` is the branch
- Git copies your commits to GitHub's repo
- Exactly like our `push` command!

## Challenges

### Challenge 1: Clone Command
Create a `clone` command that:
1. Creates a new local repository
2. Adds the remote automatically
3. Pulls the default branch

```bash
python3 remote.py clone /shared/repo my-project
```

### Challenge 2: Fetch vs Pull
Implement `fetch` separately from `pull`:
- `fetch`: Download snapshots but don't merge
- `pull`: Fetch + merge (what we implemented)

### Challenge 3: Multiple Remotes
Support multiple remotes (like `origin` and `upstream`):

```bash
python3 remote.py add origin /personal/fork
python3 remote.py add upstream /original/repo
python3 remote.py pull upstream main
python3 remote.py push origin main
```

## Key Concepts

1. **Distributed**: Every repository is complete and equal
2. **Push**: Send your changes to a remote
3. **Pull**: Get changes from a remote (fetch + merge)
4. **Remote**: Another copy of the repository
5. **Origin**: Conventional name for the main remote (like GitHub)

## Project Complete! 🎉

**You just built a working version control system!**

You now understand:
1. ✅ **Snapshots**: How git stores project history
2. ✅ **Diffs**: How git shows what changed
3. ✅ **Branches**: How git enables parallel development
4. ✅ **Merging**: How git combines divergent work
5. ✅ **Remotes**: How git enables distributed collaboration

## What's Next?

### Real Git Features We Didn't Cover

Our simplified implementation skipped some important features:
- **Staging area**: Git's "index" for preparing commits
- **SHA hashing**: Git uses SHA-1 hashes instead of sequential numbers
- **Compression**: Git compresses objects to save space
- **Pack files**: Git bundles objects for efficient storage/transfer
- **Submodules**: Nested repositories
- **Rebase**: Alternative to merge for cleaner history
- **Cherry-pick**: Apply specific commits elsewhere
- **Stash**: Temporarily save uncommitted changes

### Going Deeper

Want to learn more? Check out:
1. [Pro Git Book](https://git-scm.com/book/en/v2) - Free, comprehensive guide
2. [Git Internals](https://git-scm.com/book/en/v2/Git-Internals-Plumbing-and-Porcelain) - How git really works
3. Build more features into your snapshot system!

### Final Thoughts

Git seems magical, but it's built on simple concepts:
- Content-addressed storage (SHA hashes)
- Pointers (branches, HEAD)
- Directed acyclic graphs (commit history)
- 3-way merges (combining changes)

**You can understand it. You just did!** 🚀

---

*Thank you for building version control from scratch! You now have deep intuition for how git works under the hood. May your merges be conflict-free and your branches be meaningful!* 💚

## Bonus: Combining Everything

Want to combine all chapters into one unified tool? Create `mygit.py`:

```python
#!/usr/bin/env python3
"""
My Git - A simple version control system

Commands:
    init                  Initialize a new repository
    save <message>        Save a snapshot
    list                  List snapshots
    restore <number>      Restore a snapshot
    diff <num1> <num2>    Compare snapshots
    branch <command>      Branch operations
    merge <branch>        Merge a branch
    remote <command>      Remote operations

Examples:
    python3 mygit.py init
    python3 mygit.py save "Initial commit"
    python3 mygit.py branch create feature
    python3 mygit.py branch switch feature
    python3 mygit.py save "Added feature"
    python3 mygit.py branch switch main
    python3 mygit.py merge feature
"""

import sys

# Import all your modules
# import snapshot, diff, branches, merge, remote

def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    command = sys.argv[1]
    
    if command == 'save':
        # Call snapshot.save_snapshot()
        pass
    elif command == 'diff':
        # Call diff.diff_snapshots()
        pass
    # ... etc
    
    else:
        print(f"Unknown command: {command}")
        print(__doc__)

if __name__ == '__main__':
    main()
```

Now you have a real `mygit` command! 🎉
