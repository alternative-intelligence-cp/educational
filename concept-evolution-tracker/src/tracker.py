#!/usr/bin/env python3
"""
Concept Evolution Tracker - Core Implementation

Tracks how understanding of concepts evolves over time.
Built to answer: "How did I get from not knowing X to understanding X?"
"""

import sqlite3
import json
from datetime import datetime
from pathlib import Path
from typing import Optional, List, Dict, Any
from dataclasses import dataclass, asdict


@dataclass
class Concept:
    name: str
    tags: List[str]
    id: Optional[int] = None
    created_at: Optional[str] = None


@dataclass
class UnderstandingState:
    concept_id: int
    description: str
    confidence: int  # 0-100
    model_type: str  # technical, intuitive, hybrid, confused
    id: Optional[int] = None
    timestamp: Optional[str] = None


@dataclass
class EvolutionEvent:
    concept_id: int
    to_state_id: int
    trigger_type: str
    shift_description: str
    magnitude: int  # 0-100
    from_state_id: Optional[int] = None
    trigger_content: Optional[str] = None
    id: Optional[int] = None
    timestamp: Optional[str] = None


class ConceptTracker:
    """Track evolution of concept understanding over time."""

    def __init__(self, db_path: str = None):
        if db_path is None:
            base_dir = Path(__file__).parent
            db_path = base_dir / "concepts.db"

        self.db_path = db_path
        self.conn = sqlite3.connect(db_path)
        self.conn.row_factory = sqlite3.Row
        self._init_db()

    def _init_db(self):
        """Initialize database schema if not exists."""
        # Check if already initialized
        cursor = self.conn.execute(
            "SELECT name FROM sqlite_master WHERE type='table' AND name='concepts'"
        )
        if cursor.fetchone():
            return  # Already initialized

        schema_path = Path(__file__).parent / "schema.sql"
        with open(schema_path) as f:
            self.conn.executescript(f.read())
        self.conn.commit()

    def add_concept(self, name: str, tags: List[str] = None) -> int:
        """Add a new concept to track."""
        tags = tags or []
        cursor = self.conn.execute(
            "INSERT INTO concepts (name, tags) VALUES (?, ?)", (name, json.dumps(tags))
        )
        self.conn.commit()
        return cursor.lastrowid

    def get_concept(self, name: str) -> Optional[Dict]:
        """Get concept by name."""
        row = self.conn.execute(
            "SELECT * FROM concepts WHERE name = ?", (name,)
        ).fetchone()

        if row:
            return dict(row)
        return None

    def record_understanding(
        self,
        concept_name: str,
        description: str,
        confidence: int,
        model_type: str = "hybrid",
    ) -> int:
        """Record current understanding of a concept."""
        # Ensure concept exists
        concept = self.get_concept(concept_name)
        if not concept:
            concept_id = self.add_concept(concept_name)
        else:
            concept_id = concept["id"]

        cursor = self.conn.execute(
            """INSERT INTO understanding_states 
               (concept_id, description, confidence, model_type)
               VALUES (?, ?, ?, ?)""",
            (concept_id, description, confidence, model_type),
        )
        self.conn.commit()
        return cursor.lastrowid

    def record_evolution(
        self,
        concept_name: str,
        new_understanding: str,
        new_confidence: int,
        shift_description: str,
        trigger_type: str,
        magnitude: int,
        trigger_content: str = None,
        model_type: str = "hybrid",
    ) -> int:
        """Record an evolution in understanding."""
        concept = self.get_concept(concept_name)
        if not concept:
            raise ValueError(f"Concept '{concept_name}' not found")

        concept_id = concept["id"]

        # Get previous state
        prev_state = self.conn.execute(
            """SELECT id FROM understanding_states 
               WHERE concept_id = ? 
               ORDER BY timestamp DESC LIMIT 1""",
            (concept_id,),
        ).fetchone()

        from_state_id = prev_state["id"] if prev_state else None

        # Create new understanding state
        to_state_id = self.record_understanding(
            concept_name, new_understanding, new_confidence, model_type
        )

        # Record evolution event
        cursor = self.conn.execute(
            """INSERT INTO evolution_events
               (concept_id, from_state_id, to_state_id, trigger_type,
                trigger_content, shift_description, magnitude)
               VALUES (?, ?, ?, ?, ?, ?, ?)""",
            (
                concept_id,
                from_state_id,
                to_state_id,
                trigger_type,
                trigger_content,
                shift_description,
                magnitude,
            ),
        )
        self.conn.commit()
        return cursor.lastrowid

    def get_concept_evolution(self, concept_name: str) -> List[Dict]:
        """Get full evolution history of a concept."""
        rows = self.conn.execute(
            """SELECT 
                e.timestamp,
                e.shift_description,
                e.magnitude,
                e.trigger_type,
                s_from.description as from_understanding,
                s_from.confidence as from_confidence,
                s_to.description as to_understanding,
                s_to.confidence as to_confidence
               FROM evolution_events e
               JOIN concepts c ON e.concept_id = c.id
               LEFT JOIN understanding_states s_from ON e.from_state_id = s_from.id
               JOIN understanding_states s_to ON e.to_state_id = s_to.id
               WHERE c.name = ?
               ORDER BY e.timestamp ASC""",
            (concept_name,),
        ).fetchall()

        return [dict(row) for row in rows]

    def get_breakthroughs(self, min_magnitude: int = 70) -> List[Dict]:
        """Get major breakthrough moments."""
        rows = self.conn.execute(
            """SELECT * FROM evolution_timeline 
               WHERE magnitude >= ? 
               ORDER BY timestamp DESC""",
            (min_magnitude,),
        ).fetchall()

        return [dict(row) for row in rows]

    def get_latest_understanding(self, concept_name: str = None) -> List[Dict]:
        """Get latest understanding of concept(s)."""
        if concept_name:
            rows = self.conn.execute(
                """SELECT * FROM latest_understanding 
                   WHERE concept_name = ?""",
                (concept_name,),
            ).fetchall()
        else:
            rows = self.conn.execute("SELECT * FROM latest_understanding").fetchall()

        return [dict(row) for row in rows]

    def close(self):
        """Close database connection."""
        self.conn.close()


def main():
    """Test the tracker with today's session data."""
    tracker = ConceptTracker()

    # Record initial understanding of AILP safety requirements
    print("Recording initial AILP safety understanding...")
    tracker.record_understanding(
        concept_name="AILP Safety Requirements",
        description="Safety-critical = SIL-4 compliance, formal verification, fail-hard design",
        confidence=75,
        model_type="technical",
    )

    # Record the Millennium revelation
    print("\nRecording Millennium context shift...")
    tracker.record_evolution(
        concept_name="AILP Safety Requirements",
        new_understanding="Safety isn't abstract technical compliance - it's personal mission from lived experience. 40 years in Millennium (poverty, cult, isolation, normalized violence). 'One child harmed = game over' isn't policy, it's promise to the vulnerable kids the system failed.",
        new_confidence=95,
        shift_description="Complete reframe from technical requirements to personal mission. Safety-critical design now understood as response to community abandonment.",
        trigger_type="personal_experience",
        trigger_content="Randy from Millennium, NC - 40 years Pentecostal Holiness cult town, poverty, drugs, racism, apocalyptic beliefs. Everyone knows about the abuse but maintains 'good Christian' facade. Mirror that refused to let them look away.",
        magnitude=95,
        model_type="hybrid",
    )

    # Show evolution
    print("\n=== AILP Safety Requirements Evolution ===")
    evolution = tracker.get_concept_evolution("AILP Safety Requirements")
    for i, event in enumerate(evolution, 1):
        print(f"\nShift {i} [{event['magnitude']}/100 magnitude]:")
        print(f"  When: {event['timestamp']}")
        print(f"  Trigger: {event['trigger_type']}")
        if event["from_understanding"]:
            print(f"  From: {event['from_understanding'][:80]}...")
        print(f"  To: {event['to_understanding'][:80]}...")
        print(f"  What changed: {event['shift_description'][:80]}...")

    # Show breakthroughs
    print("\n\n=== Major Breakthroughs (70+ magnitude) ===")
    breakthroughs = tracker.get_breakthroughs()
    for bt in breakthroughs:
        print(f"\n{bt['concept']} [{bt['magnitude']}/100]")
        print(f"  {bt['shift_description']}")

    tracker.close()
    print("\n✓ Tracker prototype working!")


if __name__ == "__main__":
    main()
