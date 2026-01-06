# Quick Start Guide

Get started with the Concept Evolution Tracker in 5 minutes.

## What You Need

- Python 3.6 or higher (that's it!)
- A terminal/command prompt

## Installation

```bash
# Navigate to the source directory
cd concept-evolution-tracker/src
```

No dependencies to install. Everything uses built-in Python libraries.

## First Concept in 3 Steps

### Step 1: Record What You Know Now

Think of something you're currently learning. Anything.

```bash
python3 cli.py init "Your Concept" \
  "What you understand about it right now" \
  --confidence 50
```

**Real example**:
```bash
python3 cli.py init "Git Version Control" \
  "Saves versions of code. Can go back in time." \
  --confidence 35
```

### Step 2: Learn Something

Go study. Watch a video, read a book, do practice problems, whatever.

When something clicks, note what triggered it.

### Step 3: Record the Change

```bash
python3 cli.py evolve "Your Concept" \
  "Your NEW understanding" \
  "What made it click" \
  --confidence 70 \
  --magnitude 60 \
  --trigger conversation
```

**Real example**:
```bash
python3 cli.py evolve "Git Version Control" \
  "Git tracks changes as commits. Branches let you try ideas safely. Merge brings branches together. Remote repos enable collaboration." \
  "Drew diagram of branch/merge workflow - visual made it clear!" \
  --confidence 75 \
  --magnitude 65 \
  --trigger documentation \
  --trigger-content "Read Pro Git chapter 3, drew my own diagrams"
```

## See Your Progress

```bash
# Show your learning journey
python3 cli.py show "Your Concept"

# See all your breakthroughs
python3 cli.py breakthroughs

# Current state of all concepts
python3 cli.py state

# Visualize progress
python3 visualizer.py timeline --concept "Your Concept"
```

## That's It!

You're now tracking your learning evolution.

Keep recording shifts. Patterns will emerge.

## What to Track

### Good Concepts to Start With

**Currently Learning**:
- Math topic you're studying
- Programming concept
- Historical event
- Scientific process
- Language grammar rule

**Recently Understood**:
- Something that recently "clicked"
- New skill you just picked up
- Concept you struggled with then mastered

### How Often to Record

**Initial understanding**: Once per concept (when you start)

**Evolution events**: Whenever something shifts
- Major breakthrough? Record immediately
- Minor refinement? Record when convenient
- No change? Don't record (no forced updates)

Typically 1-3 events per week per concept.

## Understanding the Numbers

### Confidence (0-100)

How sure are you that you understand correctly?

- **20**: "I've heard the term but don't really get it"
- **40**: "Vague idea, couldn't explain it well"
- **60**: "Understand the basics, some confusion on details"
- **80**: "Solid understanding, could explain to someone else"
- **95**: "Deep mastery, could teach a class on this"

Be honest. Low confidence at start is GOOD.

### Magnitude (0-100)

How BIG was the shift in understanding?

- **20**: Learned a small detail
- **40**: Connected two related ideas
- **60**: Significant "aha!" moment
- **80**: Complete paradigm shift - everything makes sense now
- **95**: Total reframe of how you think about the topic

Most shifts are 20-50. Big breakthroughs (70+) are rare and special.

## Triggers

What made your understanding change?

- **code**: Wrote/read code and it clicked
- **conversation**: Discussion, lecture, teaching someone
- **documentation**: Read book, article, docs
- **error**: Made mistake and learned from it
- **personal_experience**: Real-life connection
- **context**: New way of framing/thinking about it

Track honestly. Patterns will show what actually works for YOU.

## Common Questions

### "How do I know when to record?"

When your understanding CHANGES. You'll feel it:
- "Oh, THAT'S what they meant!"
- "Wait, it's not X, it's actually Y"
- "Everything just connected"

That's an evolution event. Record it.

### "What if I make mistakes?"

This is for YOU. Mistakes don't matter. Honesty matters.

Low confidence that later increases? Perfect.
Wrong understanding that you later correct? That's growth, record both.

### "Can I delete/edit entries?"

Yes (it's just SQLite database). But better to record corrections as new evolution events. Shows your learning journey more accurately.

### "How many concepts should I track?"

Start with 2-3. See how it goes.

Too many = tracking becomes burden
Too few = not enough data for patterns

Sweet spot: 3-7 active concepts

### "What if nothing ever 'clicks'?"

You're having shifts, you just don't notice them. Start tracking anyway. You'll see patterns emerge.

Learning isn't always dramatic breakthroughs. Lots of small shifts (magnitude 20-30) add up.

## Example Session

```bash
# Monday: Starting to learn recursion
python3 cli.py init "Recursion" \
  "Function calling itself. Seems confusing." \
  --confidence 25

# Wednesday: Watched tutorial, something clicked
python3 cli.py evolve "Recursion" \
  "Base case stops infinite recursion. Recursive case breaks problem into smaller version. Call stack manages state." \
  "Tutorial showed factorial step-by-step, visualized call stack" \
  --confidence 65 \
  --magnitude 70 \
  --trigger documentation \
  --trigger-content "YouTube: Recursion Explained Visually"

# Friday: Wrote code, got even clearer
python3 cli.py evolve "Recursion" \
  "Recursion natural for tree/graph problems. Each call gets own stack frame. Mental model: trust the recursive call to solve smaller problem." \
  "Implemented binary tree traversal - pattern finally makes sense" \
  --confidence 80 \
  --magnitude 40 \
  --trigger code

# Check progress
python3 cli.py show "Recursion"
```

**Result**: In one week, went from confused (25) to solid understanding (80). Identified that visualizations + code practice work best for you.

## Next Steps

After tracking for a week:

1. **Run stats**: `python3 cli.py stats`
   - What triggers work best?
   - Average confidence?
   - How many breakthroughs?

2. **Review patterns**:
   - Do all your breakthroughs come from videos? Do more videos.
   - Learn best from coding? Do more practice problems.
   - Breakthroughs from teaching? Explain concepts to others.

3. **Optimize learning**:
   - Double down on what works
   - Stop wasting time on what doesn't
   - Experiment with new trigger types

## Full Documentation

- **Student Guide**: `docs/STUDENT_GUIDE.md` - Detailed walkthrough
- **Teacher Guide**: `docs/TEACHER_GUIDE.md` - Classroom integration
- **CLI Reference**: `docs/CLI_REFERENCE.md` - All commands
- **Example Journey**: `examples/sample_algebra_journey.md` - Realistic example

## Philosophy

You're not just learning concepts.

You're learning **how you learn**.

That's the skill that matters most.

---

**Ready?**

Pick a concept. Record your current understanding. Start tracking.

Your learning journey begins now.
