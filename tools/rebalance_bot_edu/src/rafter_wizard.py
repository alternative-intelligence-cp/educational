#!/usr/bin/env python3

"""
🏗️ RAFTER WIZARD - Educational Recreation
Applied Construction Mathematics Platform

This is an educational recreation of Randy's RafterWizard Android app,
demonstrating real-world application of geometry, trigonometry, and
practical problem-solving in software development.

Original: Android 2.2 app for construction professionals
Educational Focus: Applied mathematics meets practical programming

MATHEMATICAL CONCEPTS DEMONSTRATED:
- Pythagorean theorem in practice
- Trigonometry for roof construction
- Geometric calculations for cuts and angles
- Unit conversions and precision handling
- Real-world constraint validation
- Material quantity estimations

PROGRAMMING CONCEPTS:
- User input validation for measurements
- Mathematical precision and rounding
- Interactive calculation interfaces
- Visual representation of results
- Error handling for construction scenarios
- Cross-platform mathematical libraries

Randy's Philosophy: "Take theoretical math and make it solve real problems"
"""

import math
import sys
from typing import Tuple, Dict, Optional, List
from dataclasses import dataclass
from enum import Enum

# For enhanced interface (optional dependencies)
try:
    import matplotlib.pyplot as plt
    import numpy as np
    HAS_PLOTTING = True
except ImportError:
    HAS_PLOTTING = False
    print("📦 Optional: Install matplotlib and numpy for visual diagrams")
    print("    pip install matplotlib numpy")

class MeasurementUnit(Enum):
    """Standard construction measurement units"""
    FEET_INCHES = "feet_inches"
    DECIMAL_FEET = "decimal_feet"
    INCHES = "inches"
    CENTIMETERS = "centimeters"
    METERS = "meters"

@dataclass
class RoofSpan:
    """Represents a roof span measurement"""
    feet: int
    inches: float
    
    def to_decimal_feet(self) -> float:
        return self.feet + (self.inches / 12.0)
    
    def to_inches(self) -> float:
        return (self.feet * 12) + self.inches
    
    def __str__(self) -> str:
        return f"{self.feet}' {self.inches:.2f}\""

@dataclass
class RoofPitch:
    """Represents roof pitch as rise over run"""
    rise: float  # inches
    run: float   # inches (typically 12)
    
    def to_angle_degrees(self) -> float:
        """Convert pitch to angle in degrees"""
        return math.degrees(math.atan(self.rise / self.run))
    
    def to_decimal(self) -> float:
        """Convert to decimal ratio"""
        return self.rise / self.run
    
    def __str__(self) -> str:
        return f"{self.rise:.1f}/{self.run:.1f}"

@dataclass
class RafterCalculation:
    """Complete rafter calculation results"""
    span: RoofSpan
    pitch: RoofPitch
    rafter_length: float
    ridge_cut_angle: float
    bird_mouth_cut: Dict[str, float]
    tail_cut_angle: float
    material_waste_factor: float = 1.1
    
    def total_material_length(self) -> float:
        """Calculate total material length including waste"""
        return self.rafter_length * self.material_waste_factor

class RafterWizard:
    """
    🏗️ Educational Recreation of Randy's RafterWizard
    
    Demonstrates applied construction mathematics through practical
    rafter calculations used by builders and carpenters.
    """
    
    def __init__(self):
        self.unit_system = MeasurementUnit.FEET_INCHES
        self.precision_digits = 2
        print("🏗️ RafterWizard Educational Platform Initialized")
        print("📐 Ready for construction mathematics!")
    
    def calculate_rafter_length(self, span: RoofSpan, pitch: RoofPitch) -> float:
        """
        🧮 Calculate rafter length using Pythagorean theorem
        
        Educational Note: This is where theoretical math meets practical construction.
        The rafter forms the hypotenuse of a right triangle where:
        - One leg is half the building span (run)
        - Other leg is the rise calculated from pitch
        - Hypotenuse is the rafter length we need to cut
        """
        print(f"\n🎓 Educational Note: Rafter Length Calculation")
        print(f"📏 Building span: {span}")
        print(f"📐 Roof pitch: {pitch} ({pitch.to_angle_degrees():.1f}°)")
        
        # Half span (run from ridge to wall)
        run_feet = span.to_decimal_feet() / 2.0
        run_inches = run_feet * 12.0
        
        # Calculate rise based on pitch
        rise_inches = (run_inches / pitch.run) * pitch.rise
        
        # Pythagorean theorem: c² = a² + b²
        rafter_length_inches = math.sqrt(run_inches**2 + rise_inches**2)
        rafter_length_feet = rafter_length_inches / 12.0
        
        print(f"🔍 Calculations:")
        print(f"  Run (half span): {run_feet:.2f}' ({run_inches:.2f}\")")
        print(f"  Rise: {rise_inches:.2f}\"")
        print(f"  Rafter length: {rafter_length_feet:.2f}' ({rafter_length_inches:.2f}\")")
        print(f"💡 Using Pythagorean theorem: √({run_inches:.1f}² + {rise_inches:.1f}²)")
        
        return rafter_length_inches
    
    def calculate_ridge_cut_angle(self, pitch: RoofPitch) -> float:
        """
        📐 Calculate ridge cut angle (plumb cut)
        
        Educational Note: The ridge cut is a plumb (vertical) cut that allows
        the rafter to meet the ridge board. This angle is complementary to
        the roof angle.
        """
        roof_angle = pitch.to_angle_degrees()
        plumb_cut_angle = 90.0 - roof_angle
        
        print(f"\n🎓 Educational Note: Ridge Cut Calculation")
        print(f"📐 Roof angle: {roof_angle:.1f}°")
        print(f"📐 Plumb cut angle: {plumb_cut_angle:.1f}°")
        print(f"💡 Plumb cut = 90° - roof angle")
        
        return plumb_cut_angle
    
    def calculate_bird_mouth_cut(self, pitch: RoofPitch, wall_thickness: float = 3.5) -> Dict[str, float]:
        """
        🪚 Calculate bird's mouth cut (seat cut + plumb cut)
        
        Educational Note: The bird's mouth is a notch cut in the rafter that
        sits on the wall plate. It consists of two cuts:
        1. Seat cut (horizontal) - sits on the wall plate
        2. Plumb cut (vertical) - against the wall
        """
        roof_angle = pitch.to_angle_degrees()
        
        # Standard seat cut depth (typically 1.5" for 2x lumber)
        seat_cut_depth = 1.5
        
        # Plumb cut length based on wall thickness and angle
        plumb_cut_length = wall_thickness / math.cos(math.radians(roof_angle))
        
        bird_mouth = {
            "seat_cut_depth": seat_cut_depth,
            "seat_cut_angle": 0.0,  # Horizontal cut
            "plumb_cut_length": plumb_cut_length,
            "plumb_cut_angle": 90.0 - roof_angle
        }
        
        print(f"\n🎓 Educational Note: Bird's Mouth Cut")
        print(f"🪚 Seat cut depth: {seat_cut_depth}\"")
        print(f"🪚 Plumb cut length: {plumb_cut_length:.2f}\"")
        print(f"📐 Plumb cut angle: {bird_mouth['plumb_cut_angle']:.1f}°")
        
        return bird_mouth
    
    def calculate_tail_cut(self, pitch: RoofPitch, overhang: float = 18.0) -> Tuple[float, float]:
        """
        ✂️ Calculate tail cut for overhang
        
        Educational Note: The tail cut creates the overhang (eave).
        Can be plumb cut, level cut, or decorative cut.
        """
        roof_angle = pitch.to_angle_degrees()
        
        # For plumb tail cut
        plumb_tail_angle = 90.0 - roof_angle
        
        # Overhang length along rafter
        overhang_rafter_length = overhang / math.cos(math.radians(roof_angle))
        
        print(f"\n🎓 Educational Note: Tail Cut Calculation")
        print(f"📏 Overhang: {overhang}\"")
        print(f"📏 Overhang along rafter: {overhang_rafter_length:.2f}\"")
        print(f"📐 Plumb tail cut angle: {plumb_tail_angle:.1f}°")
        
        return plumb_tail_angle, overhang_rafter_length
    
    def complete_rafter_calculation(self, span_feet: int, span_inches: float, 
                                  pitch_rise: float, pitch_run: float = 12.0,
                                  overhang: float = 18.0) -> RafterCalculation:
        """
        🏗️ Complete rafter calculation with all cuts and measurements
        
        This is the main function that replicates what Randy's original
        RafterWizard app would have done for construction professionals.
        """
        print("=" * 60)
        print("🏗️ COMPLETE RAFTER CALCULATION")
        print("=" * 60)
        
        # Create data structures
        span = RoofSpan(span_feet, span_inches)
        pitch = RoofPitch(pitch_rise, pitch_run)
        
        # Perform all calculations
        rafter_length = self.calculate_rafter_length(span, pitch)
        ridge_cut_angle = self.calculate_ridge_cut_angle(pitch)
        bird_mouth = self.calculate_bird_mouth_cut(pitch)
        tail_cut_angle, overhang_length = self.calculate_tail_cut(pitch, overhang)
        
        # Create result object
        result = RafterCalculation(
            span=span,
            pitch=pitch,
            rafter_length=rafter_length,
            ridge_cut_angle=ridge_cut_angle,
            bird_mouth_cut=bird_mouth,
            tail_cut_angle=tail_cut_angle
        )
        
        self.display_cut_list(result)
        return result
    
    def display_cut_list(self, calc: RafterCalculation):
        """
        📋 Display professional cut list for construction
        
        Educational Note: This mimics what Randy's app would have provided -
        a clear, actionable list for carpenters and builders.
        """
        print("\n" + "=" * 50)
        print("📋 RAFTER CUT LIST")
        print("=" * 50)
        
        print(f"🏗️ Building Information:")
        print(f"  Span: {calc.span}")
        print(f"  Pitch: {calc.pitch} ({calc.pitch.to_angle_degrees():.1f}°)")
        print(f"  Rafter length: {calc.rafter_length / 12:.2f}' ({calc.rafter_length:.2f}\")")
        
        print(f"\n🪚 Cut Instructions:")
        print(f"  1. Ridge cut (plumb): {calc.ridge_cut_angle:.1f}°")
        print(f"  2. Bird's mouth:")
        print(f"     - Seat cut: {calc.bird_mouth_cut['seat_cut_depth']}\" deep (horizontal)")
        print(f"     - Plumb cut: {calc.bird_mouth_cut['plumb_cut_length']:.2f}\" long at {calc.bird_mouth_cut['plumb_cut_angle']:.1f}°")
        print(f"  3. Tail cut (plumb): {calc.tail_cut_angle:.1f}°")
        
        print(f"\n📦 Material Requirements:")
        total_length = calc.total_material_length()
        print(f"  Material length needed: {total_length / 12:.2f}' ({total_length:.2f}\")")
        print(f"  (Includes {((calc.material_waste_factor - 1) * 100):.0f}% waste factor)")
        
        # Suggest standard lumber sizes
        lumber_sizes = [8, 10, 12, 14, 16, 18, 20, 22, 24]
        recommended_size = None
        for size in lumber_sizes:
            if (size * 12) >= total_length:
                recommended_size = size
                break
        
        if recommended_size:
            print(f"  Recommended lumber: 2x8 or 2x10 @ {recommended_size}' length")
        else:
            print(f"  ⚠️  Special order lumber required (over 24')")

    def interactive_calculator(self):
        """
        🎮 Interactive rafter calculator interface
        
        Educational Note: This provides the user experience that Randy's
        original app would have had - simple input, powerful calculations.
        """
        print("\n🎮 INTERACTIVE RAFTER CALCULATOR")
        print("Enter your building specifications:")
        
        try:
            # Get building span
            span_feet = int(input("🏗️ Building span (feet): "))
            span_inches = float(input("🏗️ Additional inches (0-11.99): "))
            
            # Get roof pitch
            pitch_rise = float(input("📐 Roof pitch rise (inches per 12\"): "))
            
            # Optional overhang
            overhang_input = input("🏠 Overhang in inches (default 18): ")
            overhang = float(overhang_input) if overhang_input else 18.0
            
            # Perform calculation
            result = self.complete_rafter_calculation(
                span_feet, span_inches, pitch_rise, 12.0, overhang
            )
            
            # Offer to draw diagram
            if HAS_PLOTTING:
                draw_diagram = input("\n📊 Draw rafter diagram? (y/n): ").lower().startswith('y')
                if draw_diagram:
                    self.draw_rafter_diagram(result)
            
            return result
            
        except ValueError as e:
            print(f"❌ Invalid input: {e}")
            print("Please enter numeric values only.")
        except KeyboardInterrupt:
            print("\n👋 Calculation cancelled.")
    
    def draw_rafter_diagram(self, calc: RafterCalculation):
        """
        📊 Visual rafter diagram (requires matplotlib)
        
        Educational Note: Visual representation helps understand the geometry
        and validates the mathematical calculations.
        """
        if not HAS_PLOTTING:
            print("📦 Matplotlib not available for diagrams")
            return
        
        fig, ax = plt.subplots(1, 1, figsize=(12, 8))
        
        # Calculate key points
        span_inches = calc.span.to_inches()
        run_inches = span_inches / 2.0
        rise_inches = (run_inches / calc.pitch.run) * calc.pitch.rise
        
        # Draw the rafter triangle
        # Ridge point
        ridge_x = run_inches / 12.0  # Convert to feet for display
        ridge_y = rise_inches / 12.0
        
        # Wall points
        left_wall_x = 0
        right_wall_x = span_inches / 12.0
        wall_y = 0
        
        # Draw rafter lines
        ax.plot([left_wall_x, ridge_x], [wall_y, ridge_y], 'b-', linewidth=3, label='Left Rafter')
        ax.plot([ridge_x, right_wall_x], [ridge_y, wall_y], 'b-', linewidth=3, label='Right Rafter')
        
        # Draw span line
        ax.plot([left_wall_x, right_wall_x], [wall_y, wall_y], 'k-', linewidth=2, label='Span')
        
        # Draw rise line
        ax.plot([ridge_x, ridge_x], [wall_y, ridge_y], 'r--', linewidth=1, label='Rise')
        
        # Draw run line
        ax.plot([left_wall_x, ridge_x], [wall_y, wall_y], 'g--', linewidth=1, label='Run')
        
        # Annotations
        ax.annotate(f'Span: {calc.span}', xy=(span_inches/24/12, -rise_inches/24/12), 
                   ha='center', fontsize=10)
        ax.annotate(f'Rise: {rise_inches:.1f}"', xy=(ridge_x + 0.5, rise_inches/24/12), 
                   ha='left', fontsize=10)
        ax.annotate(f'Rafter: {calc.rafter_length/12:.2f}\'', 
                   xy=(ridge_x/2, ridge_y/2), ha='center', fontsize=10, 
                   bbox=dict(boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.7))
        ax.annotate(f'Pitch: {calc.pitch} ({calc.pitch.to_angle_degrees():.1f}°)', 
                   xy=(ridge_x - 1, ridge_y - 0.5), ha='center', fontsize=10,
                   bbox=dict(boxstyle="round,pad=0.3", facecolor="lightblue", alpha=0.7))
        
        ax.set_xlim(-2, span_inches/12.0 + 2)
        ax.set_ylim(-2, rise_inches/12.0 + 2)
        ax.set_xlabel('Distance (feet)')
        ax.set_ylabel('Height (feet)')
        ax.set_title('🏗️ Rafter Geometry Diagram')
        ax.grid(True, alpha=0.3)
        ax.legend()
        ax.set_aspect('equal')
        
        plt.tight_layout()
        plt.show()

def demo_calculations():
    """
    🎯 Demonstration of common rafter calculations
    
    Educational showcase of typical construction scenarios
    """
    print("🎯 RAFTER WIZARD DEMONSTRATION")
    print("Common construction scenarios with step-by-step calculations")
    
    wizard = RafterWizard()
    
    scenarios = [
        {
            "name": "Small Shed (8' span, 4/12 pitch)",
            "span_feet": 8, "span_inches": 0.0,
            "pitch_rise": 4.0, "overhang": 12.0
        },
        {
            "name": "Residential House (24' span, 6/12 pitch)",
            "span_feet": 24, "span_inches": 0.0,
            "pitch_rise": 6.0, "overhang": 18.0
        },
        {
            "name": "Steep Roof Cabin (16' span, 12/12 pitch)",
            "span_feet": 16, "span_inches": 0.0,
            "pitch_rise": 12.0, "overhang": 24.0
        }
    ]
    
    for i, scenario in enumerate(scenarios, 1):
        print(f"\n{'='*60}")
        print(f"📋 SCENARIO {i}: {scenario['name']}")
        print(f"{'='*60}")
        
        wizard.complete_rafter_calculation(
            scenario["span_feet"], scenario["span_inches"],
            scenario["pitch_rise"], 12.0, scenario["overhang"]
        )

def main():
    """
    🏗️ Main RafterWizard Educational Interface
    """
    print("🏗️ RAFTER WIZARD - Educational Recreation")
    print("Applied Construction Mathematics Platform")
    print("Original concept by Randy - Android 2.2 era innovation!")
    print("\nDemonstrating real-world application of:")
    print("• Pythagorean theorem")
    print("• Trigonometry")  
    print("• Practical problem solving")
    print("• Construction mathematics")
    
    wizard = RafterWizard()
    
    while True:
        print("\n" + "="*50)
        print("🎮 RAFTER WIZARD MENU")
        print("="*50)
        print("1. 🧮 Interactive Calculator")
        print("2. 🎯 Demo Calculations") 
        print("3. 📚 Educational Information")
        print("4. 🚪 Exit")
        
        choice = input("\nSelect option (1-4): ").strip()
        
        if choice == "1":
            wizard.interactive_calculator()
        elif choice == "2":
            demo_calculations()
        elif choice == "3":
            print("\n📚 EDUCATIONAL INFORMATION")
            print("This tool recreates Randy's RafterWizard Android app")
            print("demonstrating how theoretical mathematics solves real problems.")
            print("\nKey concepts:")
            print("• Right triangle geometry in construction")
            print("• Trigonometric functions for angles")
            print("• Unit conversions and precision")
            print("• User interface design for tradespeople")
        elif choice == "4":
            print("👋 Thanks for exploring construction mathematics!")
            break
        else:
            print("❌ Invalid choice. Please select 1-4.")

if __name__ == "__main__":
    main()