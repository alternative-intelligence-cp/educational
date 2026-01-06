# Sample Learning Journey: Algebra Student

This is a fictional but realistic example of how a student might track their understanding of quadratic equations over 6 weeks.

## Week 1: Introduction

**Initial Understanding**
```bash
python3 cli.py init "Quadratic Equations" \
  "Equations with x-squared in them. They make parabolas." \
  --confidence 25 \
  --model intuitive
```

**Student's thoughts**: "I know the word 'quadratic' and I've seen parabola graphs. That's about it."

---

## Week 2: Learning the Formula

**First Evolution**
```bash
python3 cli.py evolve "Quadratic Equations" \
  "Use quadratic formula: x = (-b ± √(b²-4ac)) / 2a to solve ax²+bx+c=0" \
  "Memorized the formula from lecture and homework" \
  --confidence 45 \
  --magnitude 30 \
  --trigger documentation \
  --model technical
```

**Student's thoughts**: "I can plug numbers into the formula now. Don't really get WHY it works though."

**Confidence increased**: 25 → 45 (+20)
**Magnitude**: 30 (moderate shift - learned a technique)

---

## Week 3: The Breakthrough

**Major Evolution**
```bash
python3 cli.py evolve "Quadratic Equations" \
  "Quadratic formula finds where parabola crosses x-axis. The ± means two solutions (or one if discriminant=0, or imaginary if negative). Completing the square shows WHY the formula works." \
  "Teacher showed step-by-step derivation. Suddenly everything connected - the formula isn't magic, it's completing the square generalized!" \
  --confidence 78 \
  --magnitude 85 \
  --trigger conversation \
  --trigger-content "Ms. Johnson derived formula on board, showed geometric meaning" \
  --model hybrid
```

**Student's thoughts**: "OH! It's not just memorizing a formula. It MEANS something geometrically. I get it now!"

**Confidence jump**: 45 → 78 (+33)
**Magnitude**: 85 (MAJOR breakthrough - paradigm shift)
**Model shift**: technical → hybrid (now has both procedural and conceptual understanding)

---

## Week 4: Application Practice

**Refinement**
```bash
python3 cli.py evolve "Quadratic Equations" \
  "Can solve word problems by setting up quadratic equations. Understand when to use quadratics (area problems, projectile motion, optimization). Discriminant tells me about solutions before solving." \
  "Did 20 practice problems. Patterns emerged - certain problem types always give quadratics." \
  --confidence 82 \
  --magnitude 25 \
  --trigger code \
  --trigger-content "Worked through problem set, tried different problem types"
```

**Student's thoughts**: "Practice is helping. Seeing when to USE quadratics, not just how to solve them."

**Confidence increase**: 78 → 82 (+4)
**Magnitude**: 25 (minor refinement - deepening existing understanding)

---

## Week 5: Making Connections

**Deep Integration**
```bash
python3 cli.py evolve "Quadratic Equations" \
  "Quadratics are everywhere! Physics (projectiles), economics (profit optimization), engineering (bridge cables). All parabolas. Can transform between vertex form, standard form, factored form - they're different views of same thing." \
  "Physics teacher used quadratics for projectile motion. Realized algebra and physics are connected!" \
  --confidence 88 \
  --magnitude 60 \
  --trigger personal_experience \
  --trigger-content "Launched water balloon in physics, calculated trajectory with quadratics"
```

**Student's thoughts**: "This isn't just abstract math. It's USEFUL. Real things follow parabolic paths!"

**Confidence increase**: 82 → 88 (+6)
**Magnitude**: 60 (significant - connecting across domains)

---

## Week 6: Teaching Others

**Mastery Demonstration**
```bash
python3 cli.py evolve "Quadratic Equations" \
  "Can explain quadratics multiple ways: geometric (parabolas), algebraic (formula derivation), applied (real problems). Can teach someone else. Understand limitations (only works for degree-2 polynomials)." \
  "Helped classmate who was struggling. Explaining it solidified my own understanding. Realized I actually GET this now." \
  --confidence 92 \
  --magnitude 40 \
  --trigger conversation \
  --trigger-content "Tutored friend, had to explain from scratch"
```

**Student's thoughts**: "If I can teach it, I really understand it. I'm actually good at quadratics now!"

**Confidence increase**: 88 → 92 (+4)
**Magnitude**: 40 (moderate - teaching solidifies understanding)

---

## Final Analysis

Running `python3 cli.py show "Quadratic Equations"` shows:

```
Evolution Timeline: Quadratic Equations

Shift 1 • Week 2 • Magnitude: 30/100
├─ From [25/100]: Equations with x-squared in them. They make parabolas.
├─ To [45/100]: Use quadratic formula...
└─ Change: Memorized the formula from lecture and homework
   Confidence: ↑↑ +20

Shift 2 • Week 3 • Magnitude: 85/100
├─ From [45/100]: Use quadratic formula...  
├─ To [78/100]: Quadratic formula finds where parabola crosses x-axis...
└─ Change: Teacher showed derivation, everything connected!
   Confidence: ↑↑↑ +33
   🔥 MAJOR BREAKTHROUGH

Shift 3 • Week 4 • Magnitude: 25/100
├─ From [78/100]: Quadratic formula finds where parabola crosses x-axis...
├─ To [82/100]: Can solve word problems by setting up quadratic equations...
└─ Change: Did practice problems, patterns emerged
   Confidence: ↑ +4

Shift 4 • Week 5 • Magnitude: 60/100
├─ From [82/100]: Can solve word problems...
├─ To [88/100]: Quadratics are everywhere! Physics, economics, engineering...
└─ Change: Physics class used quadratics for projectiles
   Confidence: ↑ +6
   ⚡ Significant shift

Shift 5 • Week 6 • Magnitude: 40/100
├─ From [88/100]: Quadratics are everywhere...
├─ To [92/100]: Can explain multiple ways, can teach others...
└─ Change: Teaching classmate solidified understanding
   Confidence: ↑ +4
```

## Student Insights

Running `python3 cli.py stats`:

```
Total concepts tracked: 1
Average confidence: 92/100

Most effective trigger types:
  conversation: 2 (derivation lecture, teaching friend)
  code: 1 (practice problems)
  personal_experience: 1 (physics connection)
  documentation: 1 (initial formula)
```

**Pattern identified**: This student learns best through **conversation** and **application**. Lectures with derivations work better than just formulas. Connecting to real uses (physics) creates big shifts.

**Learning strategy recommendation**: 
- Seek out derivations, not just formulas
- Do lots of practice problems
- Connect math to other subjects
- Teach others to solidify understanding

## Visualizing Progress

Running `python3 visualizer.py timeline --concept "Quadratic Equations"` shows:

- Confidence graph: Clear upward trajectory
- Week 3 marked with ● (major breakthrough)
- Week 5 marked with ○ (significant shift)
- Steady climb from 25 → 92 over 6 weeks

## Teacher Perspective

**What the teacher learns from this data:**

1. **Derivation worked**: The big breakthrough (mag 85) came from showing WHERE the formula comes from, not just WHAT it is

2. **Application matters**: Cross-subject connections (physics) triggered significant shift

3. **Practice important**: Doing problems helped, but wasn't the biggest driver (mag 25)

4. **Teaching solidifies**: Student jumped when they taught someone else

**Instructional changes based on this pattern:**
- Always show derivations for major formulas
- Create cross-curricular projects
- Build in peer teaching opportunities
- Don't rely solely on problem sets

## The Journey in Numbers

- **Start**: 25% confidence, intuitive model only
- **End**: 92% confidence, hybrid model (technical + intuitive)
- **Total increase**: +67 confidence points
- **Biggest jump**: Week 3 derivation lecture (+33 in one event)
- **Time to mastery**: 6 weeks
- **Learning events**: 5 recorded shifts
- **Breakthroughs**: 2 (week 3, week 5)

## Key Takeaway

This student went from "parabolas exist" to "I can teach quadratics" in 6 weeks.

But more importantly: **They now know HOW they learn math best.**

That's a transferable skill applicable to every future math topic.
