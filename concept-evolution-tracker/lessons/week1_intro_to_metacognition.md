# Lesson Plan: Introduction to Metacognition

**Grade Level**: 7-12 (adaptable)
**Duration**: 45-50 minutes
**Subject**: Any (this is a meta-skill)

## Learning Objectives

By end of lesson, students will:
1. Define metacognition in their own words
2. Identify examples of metacognitive thinking
3. Record their first concept in the tracker
4. Understand why tracking learning evolution helps them

## Materials

- Concept Evolution Tracker installed
- Projector for demonstration
- Student worksheet (included below)
- Example learning journey (from examples folder)

## Lesson Flow

### Opening (5 min)

**Hook Question**: "Think of something you're really good at now that you used to be terrible at. How did you get better?"

Have 2-3 students share briefly.

**Transition**: "Today we're going to learn how to become scientists of our own minds."

### Direct Instruction (10 min)

**Define Metacognition**
- "Meta" = about
- "Cognition" = thinking
- Metacognition = thinking about thinking

**Examples**:
- BAD: "I don't get math" (vague, helpless)
- GOOD: "I struggle with word problems but I'm good at computation. I need to practice translating English to equations." (specific, actionable)

**Why It Matters**:
- Athletes watch game film to improve
- Musicians record themselves to hear mistakes  
- Programmers debug by understanding errors
- Learners need to observe their own learning process

**Introduce Concept Tracker**:
- Tool to track HOW your understanding changes
- Not just WHAT you know, but HOW you came to know it
- Shows patterns in how YOU specifically learn best

### Demonstration (10 min)

**Live Demo**:

Show on projector:

```bash
# Record initial understanding
python3 cli.py init "Photosynthesis" \
  "Plants turn sunlight into food" \
  --confidence 40

# Show current state
python3 cli.py state

# (Pretend you learned more)

# Record evolution
python3 cli.py evolve "Photosynthesis" \
  "Light reactions split water creating ATP/NADPH. Calvin cycle uses that to fix CO2 into glucose." \
  "Understood the two-stage process from video animation" \
  --confidence 75 \
  --magnitude 60 \
  --trigger documentation

# Show timeline
python3 cli.py show "Photosynthesis"
```

**Point out**:
- Confidence increased (40 → 75)
- We recorded WHAT triggered the change (video)
- Magnitude shows how BIG the shift was
- We can see our learning journey

### Guided Practice (15 min)

**Activity**: Record Your First Concept

Students think of something they're currently learning (in any subject).

**Worksheet prompts**:
1. What concept are you learning?
2. What do you understand about it RIGHT NOW?
3. How confident are you (0-100)?
4. Is your understanding technical, intuitive, or both?

**Then**: Guide them through recording it

```bash
python3 cli.py init "YOUR CONCEPT" \
  "YOUR CURRENT UNDERSTANDING" \
  --confidence YOUR_NUMBER \
  --model YOUR_TYPE
```

**Walk around**: Help students who struggle with command line.

### Discussion (8 min)

**Reflection Questions**:
- "How did it feel to rate your own confidence?"
- "Why is it important to be honest about what you don't know?"
- "What do you think will happen as you track over time?"

**Preview Next Steps**:
- This week: Pay attention to when something "clicks"
- Next class: We'll record our first breakthrough
- Goal: Identify patterns in how YOU learn best

### Closing (2 min)

**Homework**: 
1. Keep thinking about the concept you recorded
2. Study/learn about it this week
3. Notice if/when your understanding changes
4. Write down what triggered any change (we'll record it next class)

**Exit Ticket**: 
"In one sentence, explain why tracking how you learn is useful."

## Worksheet Template

```
METACOGNITION TRACKER - FIRST ENTRY

Name: ________________    Date: ________________

1. What concept are you currently learning? (Any subject)
   
   Concept: _____________________________________________

2. What do you understand about it RIGHT NOW? (Be specific)
   
   Current understanding: ________________________________
   ________________________________________________________
   ________________________________________________________

3. How confident are you in this understanding? (Circle one)
   
   0    10    20    30    40    50    60    70    80    90    100
   |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
   Totally                  Sort of                    Completely
   lost                     get it                     confident

4. What KIND of understanding do you have?
   
   □ Technical (precise, can do calculations/procedures)
   □ Intuitive (general feel, analogies, not precise)
   □ Hybrid (both technical and intuitive)
   □ Confused (multiple conflicting ideas)

5. REFLECTION: Why do you think tracking how your understanding 
   changes over time could help you learn better?
   
   ________________________________________________________
   ________________________________________________________
   ________________________________________________________
```

## Assessment

**Formative**:
- Participation in discussion
- Completion of first tracker entry
- Quality of reflection (does student understand why this matters?)

**Not Assessed**:
- Confidence level (honesty is more important than high confidence)
- Understanding level (this is baseline, not outcome)

## Differentiation

**For struggling students**:
- Teacher assistance with command line
- Partner work for first entry
- Simpler concepts to track

**For advanced students**:
- Track multiple concepts
- Run statistics on existing data
- Create visualizations

**For visual learners**:
- Show timeline graphs
- Use breakthrough visualizations
- Draw concept maps

**For kinesthetic learners**:
- Physical timeline (students place cards on wall)
- Act out learning journey (stages of understanding)

## Extension Activities

**If time permits**:
- Show example student journey (sample_algebra_journey.md)
- Discuss: What patterns do you notice?
- Predict: What will YOUR journey look like?

**For homework**:
- Research article: "The Science of Metacognition"
- Interview: Ask parent/teacher about a time they learned something difficult
- Reflection: Write about your best learning experience and what made it work

## Common Challenges

**"This seems like extra work"**
Response: "2 minutes to record a breakthrough saves hours of ineffective studying. You'll see."

**"I don't know my confidence level"**
Response: "That's OK! Start with a guess. You'll get better at self-assessment with practice."

**"Command line is confusing"**
Response: "I'll help. It gets easier fast. Soon you'll be faster than clicking through menus."

**"What if I never have breakthroughs?"**
Response: "You do, you just don't notice them. This tool helps you SEE the small shifts that add up."

## Teacher Notes

### Before Class
- Install tracker on all student machines (or set up shared computer)
- Test all commands to ensure working
- Prepare your own example to share (model vulnerability)

### During Class
- Emphasize HONESTY over HIGH NUMBERS
- Celebrate low confidence (means good self-awareness)
- Share your own learning struggles (modeling)

### After Class
- Check that all students have at least one entry
- Follow up with students who struggled
- Prepare for next week's breakthrough recording lesson

## Connection to Standards

**Common Core**:
- CCSS.ELA-LITERACY.CCRA.SL.1: Prepare for and participate effectively in collaborative discussions

**21st Century Skills**:
- Self-directed learning
- Critical thinking
- Information literacy

**Social-Emotional Learning**:
- Self-awareness
- Self-management
- Growth mindset

## Success Criteria

This lesson is successful if:
- ✓ Students can explain metacognition in their own words
- ✓ Every student has recorded at least one concept
- ✓ Students understand they'll track changes over time
- ✓ Class discussion shows engagement with the idea
- ✓ Students seem curious (not burdened) by the tracking

## Next Lesson Preview

**Week 2: Recognizing Breakthroughs**
- What does it feel like when something "clicks"?
- Recording evolution events
- Identifying trigger types
- Sharing breakthrough moments
