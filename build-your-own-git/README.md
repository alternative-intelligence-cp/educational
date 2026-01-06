# 🚀 Build Your Own Git - Understanding by Building

## What You'll Learn

Ever feel like git is magic? Commands work, but you're not quite sure *why*? 

This project changes that. By building a simplified version control system from scratch, you'll understand git at a level that makes everything click.

**No prior git expertise needed.** Just curiosity and willingness to experiment.

## The Journey

We'll build a version control system in stages, each chapter adding one major concept:

### 📸 Chapter 1: Time Travel for Files
Build a simple snapshot system. Save your project state, restore it later.
- **Build**: `snapshot save` and `snapshot restore` commands
- **Learn**: Why version control exists, how snapshots work
- **Aha Moment**: "Oh! Git is just organized file copying!"

### 🔍 Chapter 2: What Changed?
Add the ability to see differences between versions.
- **Build**: A diff algorithm that shows what changed
- **Learn**: How git tracks changes efficiently
- **Aha Moment**: "That's why commits are so fast!"

### 🌿 Chapter 3: Parallel Universes
Implement branches - multiple versions of your project existing simultaneously.
- **Build**: Branch creation and switching
- **Learn**: Why git branches are "cheap"
- **Aha Moment**: "Branches are just pointers to commits!"

### 🔀 Chapter 4: Merging Realities
Combine changes from different branches.
- **Build**: 3-way merge algorithm
- **Learn**: Why merge conflicts happen (and how to resolve them)
- **Aha Moment**: "Now I understand what git is actually doing!"

### 🌐 Chapter 5: Working Together
Add collaboration features - push, pull, and remote repositories.
- **Build**: Remote synchronization
- **Learn**: Why distributed version control is powerful
- **Aha Moment**: "GitHub is just git with a server!"

## What Makes This Different?

**You Build It**: Not just reading about git - you write the code that makes version control work.

**Intuition First**: Each chapter starts with the problem, not the solution. You'll understand *why* before diving into *how*.

**Working Code**: Every chapter produces something you can actually use. No theory-only exercises.

**Your Pace**: Chapters are independent enough that you can take breaks, or binge them all in a weekend.

**Celebrates Learning**: Making mistakes is part of the process. We'll debug together.

## Prerequisites

**What you need to know:**
- Basic programming (any language is fine, examples in Python)
- How to use a terminal
- Basic file operations (create, read, write files)

**What you DON'T need to know:**
- How git works internally (that's what we're building!)
- Advanced algorithms or data structures
- Anything about hashing, trees, or graphs (we'll explain as we go)

## Getting Started

```bash
# Clone the educational repository
cd /path/to/educational

# Navigate to this project
cd build-your-own-git

# Start with Chapter 1
cd chapter-01-snapshots
python3 snapshot.py
```

Each chapter has:
- `README.md` - Concept explanation and goals
- Working code examples
- Exercises to try on your own
- Solutions (don't peek too early!)

## Philosophy

> "I cannot teach anybody anything. I can only make them think." - Socrates

This project is designed to make you think. Each chapter poses a problem, gives you tools to solve it, and lets you discover the solution.

Git's commands will make sense because you'll have built the engine underneath them.

## Time Commitment

- **Quick path**: ~4-6 hours (read, run examples, move on)
- **Deep path**: ~10-15 hours (read, understand, do exercises, experiment)
- **Mastery path**: ~20+ hours (build your own variations, extend the system)

Pick your own adventure. There's no wrong pace.

## Project Structure

```
build-your-own-git/
├── README.md (you are here)
├── chapter-01-snapshots/
│   ├── README.md
│   ├── snapshot.py (working example)
│   ├── exercises/
│   └── solutions/
├── chapter-02-diffs/
│   ├── README.md
│   ├── differ.py
│   ├── exercises/
│   └── solutions/
├── chapter-03-branches/
├── chapter-04-merging/
├── chapter-05-remotes/
└── final-project/
    └── mygit.py (complete system)
```

## Community & Support

**Found a bug?** Great! That's part of learning. Try to debug it yourself first, then check the solutions folder.

**Want to share your variation?** Please do! The best way to solidify learning is to teach others.

**Stuck?** That's normal. Re-read the chapter, try the simpler exercises first, or take a break and come back.

## What You'll Have at the End

A working, minimal version control system that you built yourself. You'll:

- Understand every git command you use daily
- Never be confused by merge conflicts again
- Know when to use rebase vs merge (and why)
- Be able to debug git problems by understanding what's happening under the hood
- Have the confidence to explore git's advanced features

## Next Steps

Ready to start? Head to **[Chapter 1: Time Travel for Files](./chapter-01-snapshots/README.md)**

Or browse the chapters:
- [Chapter 1: Snapshots](./chapter-01-snapshots/README.md)
- [Chapter 2: Diffs](./chapter-02-diffs/README.md)
- [Chapter 3: Branches](./chapter-03-branches/README.md)
- [Chapter 4: Merging](./chapter-04-merging/README.md)
- [Chapter 5: Remotes](./chapter-05-remotes/README.md)

---

**Remember**: Every expert was once a beginner. The only difference is they kept learning.

Let's build something cool together. 🚀
