#!/usr/bin/env python3
"""
Concept Tracker Visualizer - Generate visual representations of concept evolution
"""

import sys
from pathlib import Path
from datetime import datetime
from typing import List, Dict

sys.path.insert(0, str(Path(__file__).parent))

from tracker import ConceptTracker


class ConceptVisualizer:
    """Generate ASCII visualizations of concept evolution."""

    def __init__(self, tracker: ConceptTracker):
        self.tracker = tracker

    def timeline(self, concept: str, width: int = 80):
        """Generate ASCII timeline of concept evolution."""
        evolution = self.tracker.get_concept_evolution(concept)

        if not evolution:
            return f"No data for '{concept}'"

        lines = []
        lines.append(f"\n{'='*width}")
        lines.append(f"Concept Evolution Timeline: {concept}")
        lines.append(f"{'='*width}\n")

        # Extract timeline data
        events = []
        for event in evolution:
            timestamp = datetime.fromisoformat(event["timestamp"])
            events.append(
                {
                    "time": timestamp,
                    "magnitude": event["magnitude"],
                    "confidence": event["to_confidence"],
                    "trigger": event["trigger_type"],
                    "description": event["shift_description"],
                }
            )

        # Confidence graph over time
        lines.append("Confidence over time:")
        lines.append("")

        max_conf = 100
        graph_height = 10
        graph_width = min(60, len(events) * 3)

        # Build graph
        for level in range(graph_height, -1, -1):
            threshold = (level / graph_height) * max_conf
            line = f"{int(threshold):3d} |"

            for event in events:
                conf = event["confidence"]
                if conf >= threshold:
                    # Different symbols for magnitude
                    if event["magnitude"] >= 80:
                        line += " ●"
                    elif event["magnitude"] >= 50:
                        line += " ○"
                    else:
                        line += " ·"
                else:
                    line += "  "

            lines.append(line)

        # X-axis
        lines.append("    " + "─" * (len(events) * 2 + 1))

        # Time labels
        time_line = "      "
        for i, event in enumerate(events):
            if i % 3 == 0:
                time_line += f"{i+1:<3}"
            else:
                time_line += "   "
        lines.append(time_line)

        lines.append("\n    ● Major shift (80+)  ○ Significant (50+)  · Minor")
        lines.append("")

        # Event list
        lines.append("\nEvents:")
        for i, event in enumerate(events, 1):
            mag_bar = "█" * (event["magnitude"] // 10)
            lines.append(
                f"{i:2d}. [{event['magnitude']:3d}] {mag_bar:<10} {event['trigger']}"
            )
            lines.append(f"    {self._truncate(event['description'], width-4)}")

        return "\n".join(lines)

    def concept_map(self, min_confidence: int = 0):
        """Generate concept relationship map."""
        concepts = self.tracker.get_latest_understanding()

        if not concepts:
            return "No concepts tracked"

        # Filter by confidence
        concepts = [c for c in concepts if c["confidence"] >= min_confidence]

        lines = []
        lines.append(f"\n{'='*80}")
        lines.append(f"Concept Map (confidence >= {min_confidence})")
        lines.append(f"{'='*80}\n")

        # Sort by confidence
        concepts.sort(key=lambda x: x["confidence"], reverse=True)

        for concept in concepts:
            conf = concept["confidence"]
            bar = "█" * (conf // 10)

            lines.append(f"{concept['concept_name']}")
            lines.append(f"  [{conf:3d}] {bar}")
            lines.append(f"  {concept['model_type']}")

            # Show evolution count
            evolution = self.tracker.get_concept_evolution(concept["concept_name"])
            if evolution:
                lines.append(f"  {len(evolution)} evolution event(s)")

                # Show major breakthroughs
                major = [e for e in evolution if e["magnitude"] >= 70]
                if major:
                    lines.append(f"  🔥 {len(major)} breakthrough(s)")

            lines.append("")

        return "\n".join(lines)

    def breakthrough_timeline(self, min_magnitude: int = 70, width: int = 80):
        """Timeline of breakthrough moments."""
        breakthroughs = self.tracker.get_breakthroughs(min_magnitude)

        if not breakthroughs:
            return f"No breakthroughs (magnitude >= {min_magnitude})"

        lines = []
        lines.append(f"\n{'='*width}")
        lines.append(f"Breakthrough Timeline (magnitude >= {min_magnitude})")
        lines.append(f"{'='*width}\n")

        for bt in breakthroughs:
            timestamp = datetime.fromisoformat(bt["timestamp"])

            # Magnitude visualization
            mag_bar = "█" * (bt["magnitude"] // 10)

            lines.append(f"🔥 {timestamp.strftime('%Y-%m-%d %H:%M')}")
            lines.append(f"   {bt['concept']}")
            lines.append(f"   [{bt['magnitude']:3d}] {mag_bar}")
            lines.append(f"   {self._truncate(bt['shift_description'], width-3)}")
            lines.append(f"   Trigger: {bt['trigger_type']}")
            lines.append("")

        return "\n".join(lines)

    def learning_velocity(self, days: int = 7):
        """Show learning velocity over time period."""
        all_events = self.tracker.get_breakthroughs(0)  # All events

        if not all_events:
            return "No learning events tracked"

        # Group by day
        from datetime import timedelta

        now = datetime.now()
        cutoff = now - timedelta(days=days)

        daily_events = {}
        for event in all_events:
            timestamp = datetime.fromisoformat(event["timestamp"])
            if timestamp >= cutoff:
                day = timestamp.date()
                if day not in daily_events:
                    daily_events[day] = []
                daily_events[day].append(event)

        if not daily_events:
            return f"No learning events in past {days} days"

        lines = []
        lines.append(f"\n{'='*80}")
        lines.append(f"Learning Velocity (past {days} days)")
        lines.append(f"{'='*80}\n")

        # Sort by date
        sorted_days = sorted(daily_events.keys())

        max_events = max(len(events) for events in daily_events.values())

        for day in sorted_days:
            events = daily_events[day]
            count = len(events)
            avg_magnitude = sum(e["magnitude"] for e in events) / count

            bar = "█" * min(50, count * 5)

            lines.append(f"{day.strftime('%Y-%m-%d')} [{count:2d} events] {bar}")
            lines.append(f"  Avg magnitude: {avg_magnitude:.1f}")

            # Show concepts
            concepts = set(e["concept"] for e in events)
            lines.append(f"  Concepts: {', '.join(list(concepts)[:3])}")
            if len(concepts) > 3:
                lines.append(f"            + {len(concepts) - 3} more")
            lines.append("")

        return "\n".join(lines)

    def _truncate(self, text: str, length: int) -> str:
        """Truncate text with ellipsis."""
        if len(text) <= length:
            return text
        return text[: length - 3] + "..."


def main():
    """Demo visualizations."""
    import argparse

    parser = argparse.ArgumentParser(description="Visualize concept evolution")
    parser.add_argument(
        "mode",
        choices=["timeline", "map", "breakthroughs", "velocity"],
        help="Visualization mode",
    )
    parser.add_argument("--concept", help="Concept name (for timeline)")
    parser.add_argument(
        "--min-confidence", type=int, default=0, help="Minimum confidence (for map)"
    )
    parser.add_argument(
        "--min-magnitude",
        type=int,
        default=70,
        help="Minimum magnitude (for breakthroughs)",
    )
    parser.add_argument(
        "--days", type=int, default=7, help="Days to show (for velocity)"
    )

    args = parser.parse_args()

    tracker = ConceptTracker()
    viz = ConceptVisualizer(tracker)

    if args.mode == "timeline":
        if not args.concept:
            print("Error: --concept required for timeline mode")
            return
        print(viz.timeline(args.concept))

    elif args.mode == "map":
        print(viz.concept_map(args.min_confidence))

    elif args.mode == "breakthroughs":
        print(viz.breakthrough_timeline(args.min_magnitude))

    elif args.mode == "velocity":
        print(viz.learning_velocity(args.days))

    tracker.close()


if __name__ == "__main__":
    main()
