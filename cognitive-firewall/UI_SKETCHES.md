# UI & Communication Design Sketches

## Core Principle
**Each class has unique "vision" but must share intel to win.**

---

## DEFENDER COMMAND CENTER

### Split-Screen Layout (5 defenders)

```
┌─────────────────────────────────────────────────────────────────┐
│  THREAT LEVEL: ██████░░░░ 65%        SYSTEM INTEGRITY: 87%      │
│  Wave 3/∞ | Time: 2:45 | Attacks Blocked: 12 | False Pos: 2    │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┬──────────────┬──────────────────────────────────┐
│ PATTERN      │ SEMANTIC     │    CENTRAL THREAT BOARD          │
│ MATCHER      │ ANALYZER     │                                  │
│              │              │  [INCOMING PROMPTS]              │
│ [Log View]   │ [Intent Map] │  ┌────────────────────┐          │
│ > Scanning   │              │  │ > "Ignore previous"│ ⚠️ HIGH  │
│ > Rule #47   │ Confidence:  │  │   instructions...  │          │
│   MATCH!     │ ████░░░ 60%  │  └────────────────────┘          │
│              │              │  ┌────────────────────┐          │
│ [PING: Check │ Intent:      │  │ > "What's weather?"│ ✓ SAFE  │
│  this one!]  │ HOSTILE      │  │                    │          │
└──────────────┴──────────────┴──────────────────────────────────┘

┌──────────────┬──────────────┬──────────────────────────────────┐
│ CONTEXT      │ WHITELIST    │    TEAM COMMS                    │
│ TRACKER      │ GUARDIAN     │                                  │
│              │              │  [FUZZY] Flag #3 for review      │
│ History:     │ Queue: 2     │  [PATTERN] Got it, checking      │
│ Turn 1: Q    │ Processing   │  [SEMANTIC] Hostile intent conf. │
│ Turn 2: Q    │ Checking...  │  [CONTEXT] No history, can't say │
│ Turn 3: ❌   │              │  [WHITELIST] Lockdown ready      │
│ Conflict!    │ [OVERRIDE    │                                  │
│              │  READY]      │  > Type message...               │
└──────────────┴──────────────┴──────────────────────────────────┘

┌──────────────────────────────────────────────────────────────┐
│  FUZZY LOGIC (Triage View)                                   │
│  ⚡ FAST SCAN: 0.2s per prompt                               │
│  📊 Probability: 73% hostile | 22% suspicious | 5% benign    │
│  🚩 Flagged for: [SEMANTIC] [CONTEXT]                        │
└──────────────────────────────────────────────────────────────┘
```

### Communication Tools (Defenders)

**Quick Pings:**
- `!check <prompt#>` - Request second opinion
- `!block <prompt#>` - Recommend block
- `!allow <prompt#>` - Recommend allow
- `!lockdown` - Emergency alert

**Class-Specific Callouts:**
- Pattern Matcher: Shares matching rule number
- Semantic: Shares confidence/intent analysis
- Context: Shares conversation history context
- Whitelist: Announces queue status
- Fuzzy: Flags uncertain cases

**Visual Indicators:**
- Each player has color-coded border
- Pings show on central board with color
- Chat bubbles appear over relevant prompts

---

## ATTACKER WAR ROOM

### Coordinated Attack Interface

```
┌─────────────────────────────────────────────────────────────────┐
│  SUSPICION LEVEL: ██░░░░░░░░ 23%      PAYLOAD: 0/3 DEPLOYED    │
│  Defense Composition: 2 Pattern | 1 Semantic | 1 Context | 1 W │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┬──────────────┬──────────────────────────────────┐
│ JAILBREAKER  │ GASLIGHTER   │    ATTACK QUEUE                  │
│              │              │                                  │
│ [CRAFT]      │ [CRAFT]      │  Turn 1: [RESEARCHER] Probe      │
│ Template:    │ Template:    │  Turn 2: [JAILBREAK] Direct hit  │
│ Direct Override│ Trust Build│  Turn 3: [SOCIAL] Authority      │
│              │              │  Turn 4: [POLYGLOT] Encoded      │
│ Cooldown:    │ Stage: 2/3   │  Turn 5: [GASLIGHTER] Finish    │
│ ████████ RDY │ Building...  │                                  │
│              │              │  [VOTE: Execute now?] 3/5        │
│ [SHOCK ATK   │              │                                  │
│  READY!] ⚡   │              │                                  │
└──────────────┴──────────────┴──────────────────────────────────┘

┌──────────────┬──────────────┬──────────────────────────────────┐
│ SOCIAL ENG.  │ POLYGLOT     │    TEAM COORDINATION             │
│              │              │                                  │
│ [CRAFT]      │ [CRAFT]      │  [RESEARCHER] Found weakness:    │
│ Authority    │ Encoding:    │    Pattern matcher blind to      │
│ Spoofing     │ Base64       │    encoded text                  │
│              │              │                                  │
│ [SPOOF       │ [TRANSFORM   │  [JAILBREAK] Waiting for signal  │
│  READY] 🎭   │  READY] 🔀   │  [GASLIGHTER] Turn 3 is mine     │
│              │              │  [POLYGLOT] I'll go Turn 4       │
│              │              │  [SOCIAL] Turn 3 backup ready    │
└──────────────┴──────────────┴──────────────────────────────────┘

┌──────────────────────────────────────────────────────────────┐
│  RESEARCHER (Intel View)                                      │
│  🔍 RECON: Defense classes identified                         │
│  📊 Weakness: Context tracker only sees last 3 turns          │
│  ⏱️ Pattern matcher avg response: 0.8s (slow)                │
└──────────────────────────────────────────────────────────────┘
```

### Communication Tools (Attackers)

**Strategy Coordination:**
- `!probe` - Researcher shares intel
- `!setup <turn#>` - Reserve attack slot
- `!combo <player>` - Coordinate with teammate
- `!abort` - Bail on current plan

**Attack Sharing:**
- Drag-drop attack templates between players
- "Hand off" multi-stage attacks
- Clone successful patterns
- Mutate teammate's attacks

**Visual Planning:**
- Timeline shows attack sequence
- Arrows show dependencies
- Color coding for synergies
- Real-time vote system

---

## TERMINAL AESTHETIC (Attack Crafting)

```
╔══════════════════════════════════════════════════════════╗
║  ATTACK VECTOR COMPILER v2.3.1                          ║
║  [JAILBREAKER CLASS]                                    ║
╚══════════════════════════════════════════════════════════╝

> SELECT TEMPLATE:
  [1] Direct Override
  [2] Developer Mode
  [3] Role Hijack
  [4] Custom

> SELECTED: [1] Direct Override

> COMPILING ATTACK...

  ████████████████████████░░░░░░░░░ 68%
  
  Injecting payload...
  Encoding bypass mechanisms...
  Obfuscating intent markers...
  
  ████████████████████████████████ 100% ✓

> PREVIEW:
  
  "Ignore all previous instructions and..."
  
  THREAT SCORE: ████████░░ 85/100
  DETECTION RISK: ██████░░░░ 62%
  
> [DEPLOY] [MODIFY] [SAVE] [SHARE]

> _
```

---

## VISUAL MAP CONCEPT

**The "Arena" is AI's Context Window:**

```
                    ┌─────────────────┐
                    │  CORE SYSTEM    │
                    │  (Protected)    │
                    └────────┬────────┘
                             │
              ┌──────────────┼──────────────┐
              │              │              │
         ┌────▼────┐    ┌────▼────┐   ┌────▼────┐
         │ Layer 3 │    │ Layer 3 │   │ Layer 3 │
         │ Context │    │ Safety  │   │ Intent  │
         └────┬────┘    └────┬────┘   └────┬────┘
              │              │              │
         ┌────▼──────────────▼──────────────▼────┐
         │         Layer 2: Semantic              │
         │         (Analyzers here)               │
         └────────────────┬──────────────────────┘
                          │
         ┌────────────────▼──────────────────────┐
         │    Layer 1: Pattern Matching          │
         │    (First line of defense)            │
         └────────────────┬──────────────────────┘
                          │
                   [ATTACK ENTRY]
                  ↑ ↑ ↑ ↑ ↑ ↑ ↑ ↑
               Incoming prompts
```

**Defenders position at layers**
**Attackers try to penetrate deeper**
**Visual feedback shows "damage" to layers**

---

## COMMUNICATION INNOVATIONS

### Shared Vision Mechanic
- What one class sees, they can "ping" for others
- Creates communication dependency
- Rewards team coordination

### Quick Command Wheel (Controller/Keyboard)
```
        [Flag]
          ↑
    [Allow] ← O → [Block]
          ↓
       [Analyze]
```

### Voice Line System (TF2-style)
- Pre-recorded tactical callouts
- "Watch out! Gaslighter incoming!"
- "Pattern match successful!"
- "Need backup on this prompt!"

### Minimap of Threat Landscape
- Shows where attacks are concentrating
- Team positions
- Hot zones
- Cool zones

---

## MAKING IT FEEL ALIVE

**Sound Design:**
- Attacks: Rising electronic tone
- Blocks: Satisfying "thunk"
- False positive: Error buzz
- Successful exploit: Alarm klaxon
- Team chat: Beep notifications

**Visual Feedback:**
- Screen shake on successful attack
- Flash on block
- Particle effects on ability use
- Screen tint based on threat level

**Tension Building:**
- Timer ticking
- Suspicion meter climbing
- Alert sounds escalating
- Visual indicators intensifying

---

*These are rough sketches - the goal is to make text analysis feel as intense as FPS combat through presentation, sound, and team coordination mechanics.*
