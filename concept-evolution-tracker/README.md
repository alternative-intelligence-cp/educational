# Concept Evolution Tracker

**Track how your understanding evolves over time**

## What Students Learn

- **Metacognition**: Think about how you think
- **Data literacy**: Track and analyze personal learning data
- **Pattern recognition**: Identify what helps you learn best
- **Self-reflection**: Become an active participant in your own learning
- **Progress visibility**: See yourself getting smarter over time

## What Makes This Different

Most learning tools track **what you know**.

This tracks **how you came to know it**.

Big difference.

## Quick Example

**Before using the tracker:**
"I understand photosynthesis now."

**After using the tracker:**
"I went from thinking photosynthesis was just 'plants eat sunlight' (confidence: 40%) to understanding the electron transport chain and ATP synthesis (confidence: 85%). The breakthrough happened when I watched a video showing the molecular dance - that visualization made it click. Magnitude: 70/100."

You don't just know photosynthesis. You know:
- How your understanding evolved
- What triggered the breakthrough  
- How confident you are
- What learning method worked

## For Students

### Track Any Subject
- Math (when did algebra click?)
- Science (how did my model of evolution change?)
- History (what shifted my understanding of the Civil War?)
- Programming (track your journey from variables to algorithms)
- Art (how did my technique evolve?)

### See Your Progress
- Confidence graphs showing improvement
- Breakthrough moments highlighted
- Learning patterns revealed
- Study method effectiveness measured

### Learn How YOU Learn
- Do you learn best from videos, code, or reading?
- What contexts trigger breakthroughs?
- Which subjects come naturally vs need more work?
- How long does it take you to master concepts?

## For Teachers

### Understand Student Learning
- See where students are struggling
- Identify what teaching methods actually work
- Track class-wide learning patterns
- Celebrate breakthrough moments

### Customize Instruction
- Student learns from examples? Provide more
- Student needs intuitive models? Change framing
- Student having breakthroughs from discussion? More group work

### Evidence-Based Teaching
- See which lessons trigger understanding
- Track long-term retention vs temporary memorization
- Measure teaching effectiveness with real data

## Installation

```bash
cd concept-evolution-tracker/src
python3 tracker.py  # Run demo
```

No dependencies except Python 3.6+. Uses built-in SQLite.

## Quick Start

```bash
# Record initial understanding
python3 cli.py init "Photosynthesis" \
  "Plants convert sunlight to energy" \
  --confidence 40

# Study, learn, breakthrough happens...

# Record evolution
python3 cli.py evolve "Photosynthesis" \
  "Light-dependent reactions split water, create ATP/NADPH. Calvin cycle fixes CO2 using that energy" \
  "Understood molecular mechanism from animated video" \
  --confidence 85 \
  --magnitude 70 \
  --trigger documentation

# See your progress
python3 cli.py show "Photosynthesis"
python3 visualizer.py timeline --concept "Photosynthesis"
```

## What Makes Learning "Click"?

The tracker helps you figure this out FOR YOURSELF.

Everyone learns differently. Instead of following generic advice, use data from YOUR learning to optimize YOUR process.

## Philosophy

**Learning isn't linear accumulation.**

You don't slowly add facts until you "know" something.

You build mental models, encounter context that breaks them, experience breakthroughs, and rebuild better models.

**Track the journey, not just the destination.**

The path from confusion to clarity is where real learning happens.

**Build what you need, then realize it's educational.**

This started as a personal project ("I want to track my own learning"). Turned out everyone needs this. That's the superposition of fun and learning - they're the same thing when done right.

## Directory Structure

- `src/` - Core tracker code
- `examples/` - Sample learning journeys
- `docs/` - Full documentation
- `lessons/` - Lesson plans for teachers

## License

Open source. Build on it. Modify it. Use it.

If it helps you learn, that's the only license requirement.
