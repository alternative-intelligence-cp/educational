# The "Video Game Save" Guide to Git & GitHub
**A Complete Beginner's Guide**

---

## Introduction

Think of **Git** like a save system in a video game (like RPG save slots), and **GitHub** as the "cloud save" where you can share files and collaborate on projects with others.

This guide will walk you through everything you need to know, step by step. Don't worry if you've never done this before - we'll start from the very beginning.

---

## Key Terminology (The Basics)

Before we start, here are the main terms you'll see:

| Term | What It Means | Video Game Analogy |
|------|--------------|-------------------|
| **Repository (Repo)** | The project folder | Your game save folder |
| **Commit** | A "Save Point" | Creating a save slot |
| **Push** | Upload your saves to the cloud (GitHub) | Uploading to cloud save |
| **Pull** | Download saves from the cloud to your computer | Downloading from cloud save |
| **Branch** | An alternate timeline/workspace | Creating a separate save file to experiment |
| **Merge** | Combining two timelines | Combining two save files |
| **Clone** | Copy a project from GitHub to your computer | Downloading someone else's save |

---

## Part 1: First Time Setup
**You only need to do this once per computer.**

### Step 1: Install Git

1. Go to [git-scm.com](https://git-scm.com/)
2. Download the installer for your operating system (Windows/Mac/Linux)
3. Run the installer
   - On **Linux**: You can also install via terminal:
     ```bash
     sudo apt install git
     ```
   - Accept all the default options during installation

### Step 2: Configure Git

Git needs to know who you are so it can track who made which changes.

1. Open your terminal:
   - **Linux**: Press `Ctrl + Alt + T` or search for "Terminal"
   - **Windows**: Search for "Git Bash" (installed with Git)
   - **Mac**: Search for "Terminal"

2. Type these commands (replace with your actual name and email):
   ```bash
   git config --global user.name "Your Name"
   git config --global user.email "your.email@example.com"
   ```

3. Verify it worked:
   ```bash
   git config --global --list
   ```
   You should see your name and email displayed.

### Step 3: Set Up GitHub Account

1. Go to [github.com](https://github.com)
2. Click "Sign Up" and create a free account
3. Verify your email address
4. You're ready to go!

---

## Part 2: Starting a Project

### Scenario A: Copying an Existing Project (Clone)
**Use this when someone shares a project with you to work on.**

1. Go to the GitHub page for the project
2. Click the green **Code** button
3. Copy the URL (it looks like `https://github.com/username/project-name.git`)
4. Open your terminal
5. Navigate to where you want the project folder (e.g., `cd ~/Desktop`)
6. Clone the repository:
   ```bash
   git clone https://github.com/username/project-name.git
   ```
7. Enter the project folder:
   ```bash
   cd project-name
   ```
8. You're ready to work!

**Example:**
```bash
cd ~/Desktop/Projects
git clone https://github.com/username/example-project.git
cd example-project
ls  # Shows all the files in the project
```

### Scenario B: Creating a New Project from Scratch
**Use this when starting something completely new.**

1. Create a new folder:
   ```bash
   mkdir my-new-project
   cd my-new-project
   ```

2. Initialize Git (tells Git to start tracking this folder):
   ```bash
   git init
   ```

3. Rename the default branch to "main" (modern standard):
   ```bash
   git branch -M main
   ```

4. Create a README file:
   ```bash
   echo "# My New Project" > README.md
   ```

5. Make your first commit:
   ```bash
   git add .
   git commit -m "Initial commit"
   ```

6. **Create the repository on GitHub:**
   - Go to [github.com](https://github.com)
   - Click the **+** in the top right
   - Click **New repository**
   - Name it (same as your folder: `my-new-project`)
   - **DON'T** check "Initialize with README" (you already have one)
   - Click **Create repository**

7. **Connect your local folder to GitHub:**
   GitHub will show you commands to run. They'll look like this:
   ```bash
   git remote add origin https://github.com/yourusername/my-new-project.git
   git push -u origin main
   ```

8. Refresh GitHub - your code is now online!

---

## Part 3: The Daily Workflow (The "Save" Cycle)

**Every time you finish a task** (even a small one), follow these steps:

### Step 1: Check What Changed (Optional but Helpful)
```bash
git status
```
This shows you which files you've modified. Red = not staged, Green = staged.

### Step 2: Stage Your Changes (`add`)
This is like putting items in your shopping cart before checkout. You're telling Git *which* files to include in your save.

```bash
git add .
```
The `.` means "add everything in this folder that changed."

**To add specific files only:**
```bash
git add file1.py file2.py
```

### Step 3: Commit (Create the Save Point)
This actually creates the save point. You **must** include a message explaining what you did.

```bash
git commit -m "Fixed the login button color"
```

**Good commit messages:**
- "Fixed bug in order book data collection"
- "Added new feature: backtesting framework"
- "Updated README with installation instructions"

**Bad commit messages:**
- "stuff"
- "changes"
- "asdf"

### Step 4: Push (Upload to GitHub)
This sends your save to GitHub so your collaborators can see it.

```bash
git push origin main
```

**If you get an error** about the branch, Git will tell you the exact command to run. Just copy and paste it.

### Complete Example Workflow
```bash
# 1. Check what changed
git status

# 2. Stage all changes
git add .

# 3. Create save point
git commit -m "Fixed Kraken WebSocket connection bug"

# 4. Upload to GitHub
git push origin main
```

---

## Part 4: Branches (Working Without Breaking Things)

**Golden Rule:** Never work directly on the `main` branch. `main` is for the "finished" product.

When you want to work on a new feature or fix a bug, you create a **Branch** (think: an alternate timeline or a separate save file to experiment in).

### Why Use Branches?
- You can experiment without breaking the working code
- Multiple people can work on different features at the same time
- If your experiment fails, just delete the branch - `main` is safe!

### Creating and Using a Branch

**1. Make sure you're on `main` and it's up to date:**
```bash
git checkout main
git pull
```

**2. Create a new branch and switch to it:**
```bash
git checkout -b fix-orderbook-bug
```
The `-b` means "create a new branch called..."

**3. Work on your changes:**
- Edit files
- Test your code
- Make sure it works

**4. Save your work on the branch:**
```bash
git add .
git commit -m "Fixed order book WebSocket connection"
```

**5. Upload your branch to GitHub:**
```bash
git push origin fix-orderbook-bug
```

**6. See all branches:**
```bash
git branch
```
The one with a `*` is your current branch.

**7. Switch between branches:**
```bash
git checkout main              # Switch to main
git checkout fix-orderbook-bug # Switch back to your feature
```

### Branch Naming Conventions
Good branch names are descriptive:
- `fix-orderbook-bug`
- `add-backtesting-framework`
- `update-documentation`
- `feature-multiday-training`

Avoid:
- `test`
- `my-branch`
- `asdf`

---

## Part 5: Pull Requests (Merging Your Work)

Once you've pushed your branch to GitHub, you need to merge your "alternate timeline" back into the main project. This is called a **Pull Request** (PR).

### Creating a Pull Request

**1. Go to the GitHub repository page in your browser**

**2. You'll see a yellow/green bar:**
   > "fix-orderbook-bug had recent pushes 2 minutes ago"  
   > [Compare & Pull Request]

   Click **Compare & Pull Request**

**3. Fill out the Pull Request form:**
   - **Title:** Short summary (e.g., "Fix order book WebSocket connection")
   - **Description:** Detailed explanation:
     ```
     ## What This Fixes
     - Order book data was empty due to incorrect channel subscription
     - Added error logging to debug WebSocket messages
     
     ## How to Test
     1. Run the data collection script
     2. Check logs for "subscription confirmed" message
     3. Verify order book data is being saved
     
     ## Related Issue
     Fixes the critical bug mentioned in Section 9.1 of TECHNICAL_PAPER.md
     ```

**4. Click "Create Pull Request"**

**5. Wait for review:**
   - Your team members will review the code
   - They might leave comments or request changes
   - Once approved, click **Merge Pull Request**
   - Click **Confirm Merge**
   - Optionally, delete the branch (GitHub will offer this)

**6. Update your local `main`:**
```bash
git checkout main
git pull
```

---

## Part 6: Staying Up to Date (`pull`)

**Before you start working each day**, always get the latest updates that your team members might have made.

### Daily Startup Routine

```bash
# 1. Switch to main branch
git checkout main

# 2. Download latest changes
git pull

# 3. Create/switch to your working branch
git checkout -b my-feature
# OR if branch exists:
git checkout my-feature

# 4. Optional: Merge latest main into your branch
git merge main
```

### What `pull` Actually Does

`git pull` is actually two commands combined:
1. `git fetch` - Downloads the changes
2. `git merge` - Merges them into your current branch

### If Pull Says "Already up to date"
That's good! It means you already have the latest version.

### Common Pull Error: Uncommitted Changes
If you get an error like:
```
error: Your local changes would be overwritten by merge.
Please commit your changes or stash them before you merge.
```

**Solution:**
```bash
# Option 1: Save your changes first
git add .
git commit -m "Work in progress"
git pull

# Option 2: Temporarily hide your changes
git stash       # Hide changes
git pull        # Get updates
git stash pop   # Restore your changes
```

---

## Part 7: Merge Conflicts (When Things Collide)

Sometimes, you and a teammate both edit the **exact same line** of code. Git won't know whose version to keep, so it will pause and show: `CONFLICT`.

**Don't panic! This is normal and fixable.**

### How to Fix a Merge Conflict

**1. Git will tell you which files have conflicts:**
```bash
git status
```
Look for files marked `both modified:`

**2. Open the conflicted file** in your text editor.

**3. Look for the conflict markers:**
```python
<<<<<<< HEAD
# Your version of the code
connection_timeout = 30
=======
# Your teammate's version of the code
connection_timeout = 60
>>>>>>> main
```

**Explanation:**
- `<<<<<<< HEAD` - Start of your changes
- `=======` - Divider
- `>>>>>>> main` - End of the other person's changes

**4. Decide which version to keep** (or combine both):

Option A - Keep yours:
```python
connection_timeout = 30
```

Option B - Keep theirs:
```python
connection_timeout = 60
```

Option C - Combine:
```python
connection_timeout = 60  # Increased from 30 for stability
```

**5. Delete ALL the conflict markers** (`<<<<<<<`, `=======`, `>>>>>>>`).

**6. Save the file.**

**7. Mark the conflict as resolved:**
```bash
git add .
git commit -m "Resolved merge conflict in config file"
git push
```

### Avoiding Merge Conflicts

- **Pull often** - Start each day with `git pull`
- **Communicate** - Tell your team what you're working on
- **Use branches** - Work in separate branches for different features
- **Commit frequently** - Small commits are easier to merge

---

## Part 8: Useful Commands Reference

### Essential Commands

| Command | What It Does | When to Use It |
|---------|-------------|----------------|
| `git status` | Shows what files have changed | Before committing, to see what you've done |
| `git add .` | Stages all changes | Before every commit |
| `git add <file>` | Stages specific file | When you only want to commit certain files |
| `git commit -m "msg"` | Creates a save point | After staging changes |
| `git push` | Uploads to GitHub | After committing |
| `git pull` | Downloads latest changes | Start of each work session |
| `git clone <url>` | Copies a project | First time getting a project |
| `git log` | Shows commit history | To see what's been done |
| `git log --oneline` | Shows short history | Easier to read version |

### Branch Commands

| Command | What It Does |
|---------|-------------|
| `git branch` | Lists all branches |
| `git checkout main` | Switches to main branch |
| `git checkout -b feature` | Creates and switches to new branch |
| `git branch -d feature` | Deletes a branch (after merged) |
| `git merge feature` | Merges feature branch into current branch |

### Advanced Commands (Use Carefully!)

| Command | What It Does | Warning |
|---------|-------------|---------|
| `git reset --hard` | Throws away ALL uncommitted changes | **DESTRUCTIVE** - can't undo! |
| `git reset HEAD~1` | Undoes last commit (keeps changes) | Use if you made a mistake |
| `git stash` | Temporarily hides changes | Useful before switching branches |
| `git stash pop` | Restores stashed changes | Brings back hidden changes |
| `git revert <commit>` | Undoes a specific commit | Creates a new commit that undoes changes |

---

## Part 9: Common Workflows

### Daily Work Routine
```bash
# Morning: Get latest code
git checkout main
git pull
git checkout -b my-feature

# Throughout day: Save progress
git add .
git commit -m "Implemented X feature"

# End of day: Upload work
git push origin my-feature
```

### Bug Fix Workflow
```bash
# 1. Start from clean main
git checkout main
git pull

# 2. Create bugfix branch
git checkout -b fix-websocket-bug

# 3. Make changes and test
# (edit files, test, verify fix works)

# 4. Commit and push
git add .
git commit -m "Fixed WebSocket connection timeout"
git push origin fix-websocket-bug

# 5. Create Pull Request on GitHub
# 6. After merge, clean up
git checkout main
git pull
git branch -d fix-websocket-bug
```

### Emergency: "I Broke Everything!"
```bash
# Option 1: Undo all uncommitted changes
git reset --hard
git clean -fd  # Also removes untracked files

# Option 2: Go back to last commit
git reset --hard HEAD

# Option 3: Go back to specific commit
git log --oneline  # Find commit hash
git reset --hard abc1234  # Use actual hash

# Option 4: Start over completely
cd ..
rm -rf project-folder
git clone https://github.com/username/project.git
cd project
```

---

## Part 10: Troubleshooting

### "Permission denied (publickey)"
You need to set up SSH keys for GitHub.

**Quick fix:** Use HTTPS instead of SSH
```bash
# When cloning, use:
git clone https://github.com/username/project.git
# Instead of:
git clone git@github.com:username/project.git
```

**Better solution:** Set up SSH keys (one-time setup)
1. Generate key: `ssh-keygen -t ed25519 -C "your_email@example.com"`
2. Press Enter (accept default location)
3. Copy public key: `cat ~/.ssh/id_ed25519.pub`
4. Go to GitHub → Settings → SSH Keys → New SSH Key
5. Paste the key

### "Your branch is ahead of 'origin/main' by 3 commits"
You have local commits that aren't on GitHub yet.
```bash
git push
```

### "Your branch is behind 'origin/main' by 2 commits"
GitHub has changes you don't have locally.
```bash
git pull
```

### "fatal: not a git repository"
You're not in a Git folder.
```bash
# Navigate to the right folder
cd ~/path/to/project

# Or initialize Git in current folder
git init
```

### "Changes not staged for commit"
You have changes but haven't staged them.
```bash
git add .
```

---

## Part 11: Best Practices

### ✅ DO These Things
- **Commit often** - Small, focused commits are better than huge ones
- **Write good messages** - "Fixed bug #123" not "stuff"
- **Pull before you push** - Avoid conflicts
- **Test before committing** - Make sure your code works
- **Use branches** - Keep main clean
- **Delete merged branches** - Keep repo tidy

### ❌ DON'T Do These Things
- **Don't commit broken code** to main
- **Don't force push** (`git push -f`) unless you really know what you're doing
- **Don't commit passwords or API keys** - Use environment variables
- **Don't work directly on main** - Use branches
- **Don't ignore conflicts** - Fix them properly
- **Don't commit huge files** (>50MB) - Git isn't designed for large binaries

---

## Part 12: Getting Help

### Built-in Help
```bash
git help <command>
# Examples:
git help commit
git help branch
git help merge
```

### Quick Reference
```bash
git <command> --help
```

### Online Resources
- [Git Documentation](https://git-scm.com/doc)
- [GitHub Guides](https://guides.github.com/)
- [Visualizing Git](https://git-school.github.io/visualizing-git/)

### Ask Your Team!
Don't be afraid to ask questions. Everyone struggles with Git at first. It gets easier with practice!

---

## Summary Cheat Sheet

**Start each day:**
```bash
git checkout main
git pull
```

**Make changes:**
```bash
git checkout -b my-feature
# (edit files)
git add .
git commit -m "Description of changes"
git push origin my-feature
```

**Merge changes:**
- Create Pull Request on GitHub
- Review and merge
- Update local: `git checkout main && git pull`

**Remember:**
- Commit early, commit often
- Use descriptive commit messages
- Work in branches
- Pull before you push
- Ask for help when stuck!

---

**You've got this! Git seems complicated at first, but after a few days of practice, it'll become second nature. 🚀**
