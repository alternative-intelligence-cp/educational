# Command Line Reference

Complete reference for all Concept Evolution Tracker commands.

## Installation

```bash
cd concept-evolution-tracker/src
```

No dependencies required except Python 3.6+.

## Core Commands

### Add Concept

Create a new concept to track (optional - concepts auto-created on first use).

```bash
python3 cli.py add "Concept Name" --tags tag1 tag2 tag3
```

**Arguments**:
- `name`: Concept name (required)
- `--tags`: Space-separated tags (optional)

**Example**:
```bash
python3 cli.py add "Calculus Derivatives" --tags math calculus
```

---

### Record Initial Understanding

Set baseline for a concept.

```bash
python3 cli.py init "Concept" "Description" \
  --confidence NUMBER \
  --model TYPE
```

**Arguments**:
- `concept`: Concept name (required)
- `description`: Your current understanding (required)
- `--confidence`: 0-100 (default: 50)
- `--model`: technical | intuitive | hybrid | confused (default: hybrid)

**Example**:
```bash
python3 cli.py init "Recursion" \
  "Function that calls itself. Used for tree traversal." \
  --confidence 45 \
  --model technical
```

---

### Record Evolution

Track how understanding changed.

```bash
python3 cli.py evolve "Concept" \
  "New understanding" \
  "What changed" \
  --confidence NUMBER \
  --magnitude NUMBER \
  --trigger TYPE \
  --trigger-content "Details" \
  --model TYPE
```

**Arguments**:
- `concept`: Concept name (required)
- `new_understanding`: Updated understanding (required)
- `shift_description`: What changed / what caused shift (required)
- `--confidence`: New confidence 0-100 (required)
- `--magnitude`: Shift size 0-100 (required)
- `--trigger`: What caused shift (required, see types below)
- `--trigger-content`: Details about trigger (optional)
- `--model`: New model type (default: hybrid)

**Trigger Types**:
- `code`: Reading/writing code
- `conversation`: Discussion, lecture, teaching
- `documentation`: Reading docs, textbooks, articles
- `error`: Making mistakes, debugging
- `personal_experience`: Life experience connecting to concept
- `context`: New framing or perspective

**Example**:
```bash
python3 cli.py evolve "Recursion" \
  "Recursion breaks problems into smaller self-similar subproblems. Base case prevents infinite recursion. Call stack manages state." \
  "Debugged stack overflow, finally understood base case importance" \
  --confidence 75 \
  --magnitude 65 \
  --trigger error \
  --trigger-content "Wrote factorial function without base case, got stack overflow" \
  --model hybrid
```

---

### Show Evolution Timeline

Display complete learning journey for a concept.

```bash
python3 cli.py show "Concept"
```

**Output**:
- All recorded shifts
- Confidence changes
- Magnitude indicators
- Trigger types
- Breakthrough markers

**Example**:
```bash
python3 cli.py show "Recursion"
```

---

### Show Breakthroughs

List major learning moments.

```bash
python3 cli.py breakthroughs --min-magnitude NUMBER
```

**Arguments**:
- `--min-magnitude`: Minimum magnitude to show (default: 70)

**Example**:
```bash
python3 cli.py breakthroughs --min-magnitude 80
```

---

### Show Current State

Display latest understanding.

```bash
python3 cli.py state
python3 cli.py state --concept "Specific Concept"
```

**Arguments**:
- `--concept`: Show specific concept only (optional)

**Examples**:
```bash
# All concepts
python3 cli.py state

# Single concept
python3 cli.py state --concept "Recursion"
```

---

### Show Statistics

Analyze learning patterns.

```bash
python3 cli.py stats
```

**Output**:
- Total concepts tracked
- Average confidence
- Model type distribution
- Breakthrough count
- Most effective trigger types

---

## Visualization Commands

### Timeline Graph

ASCII graph of confidence evolution over time.

```bash
python3 visualizer.py timeline --concept "Concept"
```

**Output**:
- Confidence graph (0-100 scale)
- Major breakthroughs marked with ●
- Significant shifts marked with ○
- Minor shifts marked with ·
- Event list with magnitudes

**Example**:
```bash
python3 visualizer.py timeline --concept "Recursion"
```

---

### Concept Map

All concepts sorted by confidence.

```bash
python3 visualizer.py map --min-confidence NUMBER
```

**Arguments**:
- `--min-confidence`: Filter by confidence (default: 0)

**Output**:
- Concepts sorted by confidence
- Confidence bars (visual)
- Model types
- Evolution event counts
- Breakthrough indicators

**Example**:
```bash
python3 visualizer.py map --min-confidence 50
```

---

### Breakthrough Timeline

Chronological list of major shifts.

```bash
python3 visualizer.py breakthroughs --min-magnitude NUMBER
```

**Arguments**:
- `--min-magnitude`: Minimum magnitude (default: 70)

**Output**:
- Timestamp
- Concept name
- Magnitude with visual bar
- Shift description
- Trigger type

**Example**:
```bash
python3 visualizer.py breakthroughs --min-magnitude 85
```

---

### Learning Velocity

Learning events per day over time period.

```bash
python3 visualizer.py velocity --days NUMBER
```

**Arguments**:
- `--days`: Days to analyze (default: 7)

**Output**:
- Events per day (bar chart)
- Average magnitude per day
- Concepts involved

**Example**:
```bash
python3 visualizer.py velocity --days 30
```

---

## Example Workflows

### Starting a New Subject

```bash
# Record baseline for all major topics
python3 cli.py init "Variables" "Store data" --confidence 70
python3 cli.py init "Functions" "Reusable code blocks" --confidence 50
python3 cli.py init "Loops" "Repeat code" --confidence 40
python3 cli.py init "Recursion" "???" --confidence 10

# See what you're starting with
python3 cli.py state
```

### Recording a Breakthrough

```bash
# Right after the "aha!" moment
python3 cli.py evolve "Recursion" \
  "Functions calling themselves to solve smaller subproblems" \
  "Watched visualization of factorial recursion - call stack made sense!" \
  --confidence 70 \
  --magnitude 80 \
  --trigger documentation \
  --trigger-content "YouTube video showing call stack animation"

# See your progress
python3 cli.py show "Recursion"
```

### Weekly Review

```bash
# What did I learn this week?
python3 cli.py breakthroughs

# What's my current state?
python3 cli.py state

# What patterns am I seeing?
python3 cli.py stats

# Visualize progress
python3 visualizer.py velocity --days 7
```

### End of Semester

```bash
# See all major breakthroughs
python3 cli.py breakthroughs --min-magnitude 70

# Generate concept map
python3 visualizer.py map

# Analyze learning patterns
python3 cli.py stats

# Export for reflection paper
# (output can be copy-pasted)
```

## Tips

### Naming Concepts
- Be specific: "Quadratic Equations" not "Math"
- Stay consistent: "Photosynthesis" not sometimes "How Plants Work"
- Use proper capitalization for readability

### Writing Descriptions
- Be specific about WHAT you understand
- Use your own words (not copy-paste from textbook)
- Include examples if helpful
- Keep under 200 characters for readability

### Choosing Confidence
- Be honest (this is for YOU)
- 0-25: Confused, don't get it
- 25-50: Vague idea, not confident
- 50-75: Understand basics, some gaps
- 75-90: Solid understanding
- 90-100: Deep mastery

### Choosing Magnitude
- 0-30: Minor refinement (learned a detail)
- 30-60: Significant shift (connected concepts)
- 60-80: Major breakthrough (paradigm change)
- 80-100: Complete reframe (everything makes sense)

### Choosing Triggers
- What ACTUALLY caused the shift?
- Not what you wish worked (be honest)
- Multiple triggers? Pick primary one
- Unsure? Use 'context'

## Database Location

Default: `src/concepts.db`

To use a different database:
```python
# In Python code
tracker = ConceptTracker("/path/to/custom.db")
```

## Troubleshooting

### "table concepts already exists"
Fixed in current version. Delete `concepts.db` if you want to start fresh.

### "Concept not found"
Either you haven't added it yet, or name doesn't match exactly (case-sensitive).

### "Command not found"
Make sure you're in the `src/` directory.

### Numbers not showing properly
Use integers (50 not 50.0) for confidence and magnitude.

## Advanced Usage

### Scripting
All commands are scriptable. Example:

```bash
# Batch record multiple concepts
for concept in "Variables" "Functions" "Loops"; do
  python3 cli.py init "$concept" "Learning this" --confidence 30
done
```

### Export Data
Database is SQLite. Can query directly:

```bash
sqlite3 concepts.db "SELECT * FROM evolution_timeline"
```

### Integration
CLI outputs are parseable. Can pipe to other tools:

```bash
python3 cli.py stats | grep "Average confidence"
```
