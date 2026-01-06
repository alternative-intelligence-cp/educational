#!/usr/bin/env python3
"""
Concept Tracker CLI - Interactive interface for tracking understanding evolution
"""

import sys
import argparse
from pathlib import Path
from datetime import datetime, timedelta
from typing import Optional

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from tracker import ConceptTracker


class TrackerCLI:
    """Interactive CLI for concept tracking."""

    def __init__(self, db_path: str = None):
        self.tracker = ConceptTracker(db_path)

    def add_concept(self, name: str, tags: list = None):
        """Add a new concept to track."""
        try:
            concept_id = self.tracker.add_concept(name, tags or [])
            print(f"✓ Added concept: {name} (ID: {concept_id})")
            if tags:
                print(f"  Tags: {', '.join(tags)}")
        except Exception as e:
            print(f"✗ Error adding concept: {e}")

    def record_initial(
        self,
        concept: str,
        description: str,
        confidence: int,
        model_type: str = "hybrid",
    ):
        """Record initial understanding of a concept."""
        try:
            state_id = self.tracker.record_understanding(
                concept, description, confidence, model_type
            )
            print(f"✓ Recorded initial understanding of '{concept}'")
            print(f"  Confidence: {confidence}/100")
            print(f"  Model: {model_type}")
        except Exception as e:
            print(f"✗ Error recording understanding: {e}")

    def record_shift(
        self,
        concept: str,
        new_understanding: str,
        new_confidence: int,
        shift_desc: str,
        trigger_type: str,
        magnitude: int,
        trigger_content: str = None,
        model_type: str = "hybrid",
    ):
        """Record an evolution in understanding."""
        try:
            event_id = self.tracker.record_evolution(
                concept,
                new_understanding,
                new_confidence,
                shift_desc,
                trigger_type,
                magnitude,
                trigger_content,
                model_type,
            )
            print(f"✓ Recorded evolution for '{concept}' [magnitude: {magnitude}/100]")
            print(f"  New confidence: {new_confidence}/100")
            print(f"  Trigger: {trigger_type}")
        except Exception as e:
            print(f"✗ Error recording evolution: {e}")

    def show_evolution(self, concept: str):
        """Display evolution timeline for a concept."""
        evolution = self.tracker.get_concept_evolution(concept)

        if not evolution:
            print(f"No evolution data for '{concept}'")
            return

        print(f"\n{'='*80}")
        print(f"Evolution Timeline: {concept}")
        print(f"{'='*80}\n")

        for i, event in enumerate(evolution, 1):
            timestamp = datetime.fromisoformat(event["timestamp"])
            print(
                f"Shift {i} • {timestamp.strftime('%Y-%m-%d %H:%M')} • Magnitude: {event['magnitude']}/100"
            )
            print(f"├─ Trigger: {event['trigger_type']}")

            if event["from_understanding"]:
                print(
                    f"├─ From [{event['from_confidence']}/100]: {self._truncate(event['from_understanding'], 70)}"
                )
            else:
                print(f"├─ From: [Initial understanding]")

            print(
                f"├─ To [{event['to_confidence']}/100]: {self._truncate(event['to_understanding'], 70)}"
            )
            print(f"└─ Change: {self._truncate(event['shift_description'], 70)}")

            # Visual confidence/magnitude indicators
            conf_change = event["to_confidence"] - (event["from_confidence"] or 0)
            if conf_change > 0:
                print(
                    f"   Confidence: {'↑' * min(5, conf_change // 10)} +{conf_change}"
                )

            if event["magnitude"] >= 80:
                print(f"   🔥 MAJOR BREAKTHROUGH")
            elif event["magnitude"] >= 50:
                print(f"   ⚡ Significant shift")

            print()

    def show_breakthroughs(self, min_magnitude: int = 70):
        """Show major breakthrough moments."""
        breakthroughs = self.tracker.get_breakthroughs(min_magnitude)

        if not breakthroughs:
            print(f"No breakthroughs found (magnitude >= {min_magnitude})")
            return

        print(f"\n{'='*80}")
        print(f"Major Breakthroughs (magnitude >= {min_magnitude})")
        print(f"{'='*80}\n")

        for bt in breakthroughs:
            timestamp = datetime.fromisoformat(bt["timestamp"])
            print(f"🔥 {bt['concept']} [{bt['magnitude']}/100]")
            print(f"   {timestamp.strftime('%Y-%m-%d %H:%M')}")
            print(f"   {self._truncate(bt['shift_description'], 70)}")
            print(f"   Trigger: {bt['trigger_type']}\n")

    def show_current_state(self, concept: str = None):
        """Show current understanding of concept(s)."""
        states = self.tracker.get_latest_understanding(concept)

        if not states:
            print(
                "No concepts tracked yet" if not concept else f"No data for '{concept}'"
            )
            return

        print(f"\n{'='*80}")
        print(
            "Current Understanding"
            if not concept
            else f"Current Understanding: {concept}"
        )
        print(f"{'='*80}\n")

        for state in states:
            print(f"📌 {state['concept_name']}")
            print(
                f"   Confidence: {state['confidence']}/100  {'█' * (state['confidence'] // 10)}"
            )
            print(f"   Model: {state['model_type']}")
            print(f"   Understanding: {self._truncate(state['description'], 65)}")
            timestamp = datetime.fromisoformat(state["timestamp"])
            print(f"   Last updated: {timestamp.strftime('%Y-%m-%d %H:%M')}\n")

    def show_stats(self):
        """Show tracker statistics and insights."""
        # Get all concepts
        all_concepts = self.tracker.get_latest_understanding()

        if not all_concepts:
            print("No data tracked yet")
            return

        print(f"\n{'='*80}")
        print("Tracker Statistics")
        print(f"{'='*80}\n")

        print(f"Total concepts tracked: {len(all_concepts)}")

        # Average confidence
        avg_conf = sum(c["confidence"] for c in all_concepts) / len(all_concepts)
        print(f"Average confidence: {avg_conf:.1f}/100")

        # Model type distribution
        model_types = {}
        for c in all_concepts:
            mt = c["model_type"]
            model_types[mt] = model_types.get(mt, 0) + 1

        print(f"\nModel types:")
        for mt, count in model_types.items():
            print(f"  {mt}: {count}")

        # Recent activity
        breakthroughs = self.tracker.get_breakthroughs(70)
        print(f"\nMajor breakthroughs: {len(breakthroughs)}")

        # Trigger analysis
        trigger_counts = {}
        for bt in self.tracker.get_breakthroughs(0):  # All events
            tt = bt["trigger_type"]
            trigger_counts[tt] = trigger_counts.get(tt, 0) + 1

        if trigger_counts:
            print(f"\nMost effective trigger types:")
            sorted_triggers = sorted(
                trigger_counts.items(), key=lambda x: x[1], reverse=True
            )
            for tt, count in sorted_triggers[:5]:
                print(f"  {tt}: {count}")

    def _truncate(self, text: str, length: int) -> str:
        """Truncate text with ellipsis."""
        if len(text) <= length:
            return text
        return text[: length - 3] + "..."

    def close(self):
        """Close tracker connection."""
        self.tracker.close()


def main():
    parser = argparse.ArgumentParser(
        description="Concept Evolution Tracker - Track how understanding evolves over time"
    )

    subparsers = parser.add_subparsers(dest="command", help="Commands")

    # Add concept
    add_parser = subparsers.add_parser("add", help="Add a new concept")
    add_parser.add_argument("name", help="Concept name")
    add_parser.add_argument("--tags", nargs="*", help="Tags for the concept")

    # Record initial understanding
    init_parser = subparsers.add_parser("init", help="Record initial understanding")
    init_parser.add_argument("concept", help="Concept name")
    init_parser.add_argument("description", help="Current understanding")
    init_parser.add_argument(
        "--confidence", type=int, default=50, help="Confidence (0-100)"
    )
    init_parser.add_argument(
        "--model",
        choices=["technical", "intuitive", "hybrid", "confused"],
        default="hybrid",
        help="Understanding model type",
    )

    # Record evolution
    evolve_parser = subparsers.add_parser(
        "evolve", help="Record understanding evolution"
    )
    evolve_parser.add_argument("concept", help="Concept name")
    evolve_parser.add_argument("new_understanding", help="New understanding")
    evolve_parser.add_argument("shift_description", help="What changed")
    evolve_parser.add_argument(
        "--confidence", type=int, required=True, help="New confidence (0-100)"
    )
    evolve_parser.add_argument(
        "--magnitude", type=int, required=True, help="Shift magnitude (0-100)"
    )
    evolve_parser.add_argument(
        "--trigger",
        required=True,
        choices=[
            "context",
            "conversation",
            "code",
            "error",
            "personal_experience",
            "documentation",
        ],
        help="What triggered the shift",
    )
    evolve_parser.add_argument("--trigger-content", help="Details about the trigger")
    evolve_parser.add_argument(
        "--model",
        choices=["technical", "intuitive", "hybrid", "confused"],
        default="hybrid",
        help="New understanding model type",
    )

    # Show evolution
    show_parser = subparsers.add_parser("show", help="Show concept evolution")
    show_parser.add_argument("concept", help="Concept name")

    # Show breakthroughs
    bt_parser = subparsers.add_parser("breakthroughs", help="Show major breakthroughs")
    bt_parser.add_argument(
        "--min-magnitude", type=int, default=70, help="Minimum magnitude"
    )

    # Show current state
    state_parser = subparsers.add_parser("state", help="Show current understanding")
    state_parser.add_argument("--concept", help="Specific concept (or all if omitted)")

    # Show stats
    subparsers.add_parser("stats", help="Show tracker statistics")

    args = parser.parse_args()

    if not args.command:
        parser.print_help()
        return

    cli = TrackerCLI()

    try:
        if args.command == "add":
            cli.add_concept(args.name, args.tags)

        elif args.command == "init":
            cli.record_initial(
                args.concept, args.description, args.confidence, args.model
            )

        elif args.command == "evolve":
            cli.record_shift(
                args.concept,
                args.new_understanding,
                args.confidence,
                args.shift_description,
                args.trigger,
                args.magnitude,
                args.trigger_content,
                args.model,
            )

        elif args.command == "show":
            cli.show_evolution(args.concept)

        elif args.command == "breakthroughs":
            cli.show_breakthroughs(args.min_magnitude)

        elif args.command == "state":
            cli.show_current_state(args.concept)

        elif args.command == "stats":
            cli.show_stats()

    finally:
        cli.close()


if __name__ == "__main__":
    main()
