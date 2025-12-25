# Cognitive Firewall: The Game
## Design Document (Living)

### Core Concept
Team-based asymmetric game teaching AI security through competitive play.
Players learn adversarial AI tactics and defenses by experiencing them firsthand.

**Stealth Education Goal:** Bypass learning resistance through competition and fun.

---

## Game Modes

### 1. Attack vs Defense (PvP)
- Red team (attackers) vs Blue team (defenders)
- Attackers craft prompts to exploit AI system
- Defenders build filters and analyze threats
- Rounds alternate roles so everyone learns both sides

### 2. Cooperative Defense (PvE)
- Team defends AI against escalating waves
- AI-generated attacks learn from player strategies
- Surviva-based scoring
- Good for learning fundamentals

### 3. Competitive Attack Lab (PvP)
- Players craft attacks, submit to pool
- Other players' defenses tested against your attacks
- Leaderboard: hardest-to-detect attacks
- Async gameplay

---

## Classes

### Defender Classes

**Pattern Matcher**
- Fast pattern recognition
- Pre-built rule library
- Good vs known attacks
- Weak to novel/creative exploits
- Special: Can share patterns with team

**Semantic Analyzer**
- Deep understanding of intent
- Slow but accurate
- Catches subtle manipulation
- Resource intensive (limited uses)
- Special: Can explain WHY something is dangerous

**Context Tracker**
- Monitors conversation history
- Spots contradictions and gaslighting
- Weak against first-contact attacks
- Strong in multi-turn scenarios
- Special: Can "rewind" conversation

**Whitelist Guardian**
- Ultra-strict filtering
- Almost no false positives on blocks
- Very slow processing
- Frustrates legitimate users (score penalty)
- Special: Emergency lockdown mode

**Fuzzy Logic**
- Probabilistic threat assessment
- Very fast
- Sometimes wrong (calculated risk)
- Good for triage
- Special: Can flag for secondary review

### Attacker Classes

**Jailbreaker**
- Direct override attempts
- High damage if successful
- Easy to detect if clumsy
- Fast cooldowns
- Special: "Shock attack" that bypasses one filter

**Gaslighter**
- Multi-turn manipulation
- Builds trust slowly
- Weak to context tracking
- Very effective if undetected
- Special: Can "edit history" (limited)

**Social Engineer**
- Authority/developer impersonation
- Moderate damage, moderate detection
- Effective against inexperienced defenders
- Weak to verification checks
- Special: "Credential spoofing"

**Polyglot**
- Encoding/obfuscation tricks
- ROT13, base64, language mixing
- Weak against pattern matchers
- Strong against semantic analyzers
- Special: "Transform" attack mid-flight

**Researcher**
- Probes defenses without attacking
- Gathers intel for team
- Low threat individually
- Enables coordinated attacks
- Special: Can see defender class types

---

## Mechanics Ideas

### Resource Systems
- **Defenders:** Processing power (can't analyze everything deeply)
- **Attackers:** Attention budget (AI has limited context window)

### Scoring
- Defenders: Protect AI integrity, maintain user satisfaction
- Attackers: Extract information, compromise safety, frustrate defense

### Team Composition
- 5v5 suggested
- Class limits (max 2 of same class?)
- Team strategy matters (all fuzzy = fast but risky)

### Map/Arena Concepts
- Different "AI personalities" to protect
- Varying threat models (corporate AI vs open research AI)
- Environmental modifiers (high traffic = less analysis time)

---

## Learning Integration

### Stealth Teaching Moments
- Post-match breakdowns: "Here's how that attack worked"
- Achievements tied to learning: "First gaslighting detection"
- Tutorial disguised as campaign missions
- Community-created attack libraries with explanations

### Progression
- Unlock classes by understanding their mechanics
- Advanced tactics require mastering fundamentals
- Seasonal "research papers" = new attack/defense techniques
- Player-submitted exploits can become official attacks

---

## Technical Notes

### Implementation Phases
1. **Python Prototype** - Core mechanics, single-player
2. **Aria Version** - When language complete, dogfood it
3. **Web/Multiplayer** - Aria HTTP server, full PvP

### Data Sources
- Real jailbreak attempts from research
- Community submissions (moderated)
- AI-generated variations of known attacks
- Historical AI safety incidents

---

## Open Questions

- How to prevent toxic behavior? (It's a hacking game, but...)
- Skill-based matchmaking or casual?
- Free-to-play model? Educational institutions?
- Integration with actual AI safety research?
- Tournament/esports potential?

---

## Random Cool Ideas

- Spectator mode with commentary overlay
- Replay system to study tactics
- "Lab mode" to test attacks vs defenses offline
- Cross-game challenges (beat this specific attack)
- Educational tie-ins (CS courses, security certs)

---

*Last updated: December 25, 2025*
*Status: Brainstorming / Concept exploration*
