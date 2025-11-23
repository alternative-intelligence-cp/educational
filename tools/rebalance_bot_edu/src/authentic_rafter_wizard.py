#!/usr/bin/env python3
"""
RANDY'S AUTHENTIC RAFTER WIZARD - EDUCATIONAL RECREATION
=======================================================

This is an educational recreation of Randy's original Android app algorithms,
extracted from the actual APK (60052.apk) found on Amazon Appstore.

CRISIS INNOVATION BACKGROUND:
Randy built this during a challenging period when he needed practical 
construction mathematics for real-world roofing projects. The algorithms 
demonstrate sophisticated applied mathematics using trigonometry, the 
Pythagorean theorem, and practical engineering constraints.

EDUCATIONAL VALUE:
- Applied trigonometry in real-world contexts
- Pythagorean theorem for rafter length calculations
- Engineering tolerances and safety factors
- Iterative refinement algorithms
- Practical unit conversions and fraction handling

Original JavaScript algorithms preserved and converted to Python for education.
"""

import math
from typing import Dict, Tuple, Optional
from dataclasses import dataclass


class MathPlusEducational:
    """Educational recreation of Randy's MathPlus.js mathematical utilities."""
    
    @staticmethod
    def degrees_to_radians(angle_degrees: float) -> float:
        """Convert degrees to radians."""
        return angle_degrees * (math.pi / 180)
    
    @staticmethod
    def radians_to_degrees(angle_radians: float) -> float:
        """Convert radians to degrees."""
        return angle_radians * (180 / math.pi)
    
    @staticmethod
    def ratio_to_degrees(rise: float, run: float) -> float:
        """Convert rise/run ratio to slope angle in degrees."""
        return MathPlusEducational.radians_to_degrees(math.atan(rise / run))
    
    @staticmethod
    def degrees_to_ratio(slope_degrees: float, span: float) -> float:
        """Convert slope angle to rise for given span."""
        return math.tan(MathPlusEducational.degrees_to_radians(slope_degrees)) * span
    
    @staticmethod
    def decimal_to_fraction(decimal: float, fraction_unit: int = 16) -> str:
        """Convert decimal to fraction string (Randy's original algorithm)."""
        whole = math.floor(decimal)
        fraction = decimal - whole
        
        step = 1 / fraction_unit
        num = 0
        
        while ((num + 1) * step) <= fraction:
            num += 1
        
        if num != fraction:
            if ((num * step) + (step * 0.5)) <= fraction:
                num += 1
        
        if num * step >= 1:
            whole += 1
            num = 0
        
        # Format output
        whole_string = "" if whole == 0 else f"{whole} "
        
        if num == 0:
            fraction_string = ""
        else:
            gcd = math.gcd(fraction_unit, num)
            numerator = num // gcd
            denominator = fraction_unit // gcd
            fraction_string = f"{numerator}/{denominator}"
        
        if whole_string == "" and fraction_string == "":
            return "0"
        else:
            return (whole_string + fraction_string).strip()
    
    @staticmethod
    def inches_to_feet_and_inches(inches: float) -> str:
        """Convert inches to feet and inches with fractions."""
        feet = math.floor(inches / 12)
        num_inches = inches % 12
        
        inch_string = "inch" if 0 < num_inches <= 1 else "inches"
        fraction_inches = MathPlusEducational.decimal_to_fraction(num_inches)
        
        return f"{feet} foot {fraction_inches} {inch_string}"
    
    @staticmethod
    def tri_side_from_sines(angle_degrees: float, right_angle: float, opposite_side: float) -> float:
        """Calculate hypotenuse from angle and opposite side using sine rule."""
        return opposite_side / math.sin(MathPlusEducational.degrees_to_radians(angle_degrees))
    
    @staticmethod
    def pythagorean(side_a: float, side_b: float) -> float:
        """Calculate hypotenuse using Pythagorean theorem."""
        return math.sqrt(side_a**2 + side_b**2)


@dataclass
class RafterSpecifications:
    """Input specifications for rafter calculation."""
    name: str
    plate_to_center: float      # Distance from outside plate to center of ridge
    plate_to_peak: float        # Height from plate to peak
    board_width: float          # Width of rafter lumber
    plate_width: float          # Width of wall plate
    boxing_projection: float    # Overhang projection including boxing
    fascia_thickness: float     # Thickness of fascia board
    ridge_thickness: float      # Thickness of ridge board


class AuthenticRafterWizard:
    """
    Educational recreation of Randy's authentic RafterWizard algorithms.
    
    This class implements the exact mathematical calculations Randy developed
    for his Android construction app, preserving the crisis-time innovations
    that demonstrate applied mathematics in real-world contexts.
    """
    
    def __init__(self, specs: RafterSpecifications):
        self.specs = specs
        self.math_plus = MathPlusEducational()
        
        # Initialize calculation results
        self.results = {}
        
        # Perform Randy's authentic calculation sequence
        self._calculate_rafter_dimensions()
    
    def _calculate_rafter_dimensions(self):
        """Randy's authentic calculation sequence with iterative refinement."""
        
        # Initial rough dimensions (Randy's approach)
        width = self.specs.board_width
        slope_degrees = self.math_plus.ratio_to_degrees(
            self.specs.plate_to_peak, 
            self.specs.plate_to_center
        )
        slope_ratio = self.math_plus.degrees_to_ratio(slope_degrees, 12)
        plumb_height = self._calc_plumb_height(slope_degrees, width)
        seat_cut_width = self.specs.plate_width
        plumb_cut_depth = self._calc_plumb_cut_depth(seat_cut_width, slope_degrees)
        hap = self._calc_rafter_stand(plumb_cut_depth, plumb_height)
        projection = self._calc_projection(
            self.specs.boxing_projection, 
            self.specs.fascia_thickness
        )
        rise = self._calc_rafter_rise(
            self.specs.plate_to_peak, 
            hap, 
            self._calc_peak_allowance(self.specs.ridge_thickness, slope_degrees)
        )
        run = self._calc_rafter_run(self.specs.plate_to_center, self.specs.ridge_thickness)
        body_length = self._calc_body_length(rise, run)
        
        # Randy's iterative refinement algorithm
        last_length = body_length
        new_length = 0
        iteration_count = 0
        
        while last_length != new_length and iteration_count < 10:  # Safety limit
            iteration_count += 1
            last_length = new_length
            
            slope_degrees = self.math_plus.ratio_to_degrees(rise, run)
            slope_ratio = self.math_plus.degrees_to_ratio(slope_degrees, 12)
            plumb_height = self._calc_plumb_height(slope_degrees, width)
            plumb_cut_depth = self._calc_plumb_cut_depth(seat_cut_width, slope_degrees)
            hap = self._calc_rafter_stand(plumb_cut_depth, plumb_height)
            rise = self._calc_rafter_rise(
                self.specs.plate_to_peak,
                hap,
                self._calc_peak_allowance(self.specs.ridge_thickness, slope_degrees)
            )
            body_length = self._calc_body_length(rise, run)
            new_length = body_length
        
        # Final calculations
        tail_length = self._calc_tail_length(projection, slope_degrees)
        top_length = self._calc_top_length(body_length, tail_length)
        total_length = self._calc_total_length(top_length, slope_degrees, width)
        
        # Store results
        self.results = {
            'name': self.specs.name,
            'slope_degrees': slope_degrees,
            'slope_ratio': slope_ratio,
            'width': width,
            'height_to_peak': self.specs.plate_to_peak,
            'plumb_height': plumb_height,
            'plumb_cut_depth': plumb_cut_depth,
            'seat_cut_width': seat_cut_width,
            'hap': hap,
            'projection': projection,
            'boxing_projection': self.specs.boxing_projection,
            'body_length': body_length,
            'tail_length': tail_length,
            'top_length': top_length,
            'total_length': total_length,
            'fascia_thickness': self.specs.fascia_thickness,
            'ridge_thickness': self.specs.ridge_thickness,
            'run': run,
            'rise': rise,
            'iterations': iteration_count
        }
    
    def _calc_plumb_height(self, slope_degrees: float, rafter_width: float) -> float:
        """Calculate plumb height using Randy's trigonometric approach."""
        return rafter_width / math.cos(self.math_plus.degrees_to_radians(slope_degrees))
    
    def _calc_rafter_stand(self, plumb_cut_depth: float, plumb_height: float) -> float:
        """Calculate height above plate (HAP) - Randy's key measurement."""
        return plumb_height - plumb_cut_depth
    
    def _calc_plumb_cut_depth(self, seat_cut_width: float, slope_degrees: float) -> float:
        """Calculate plumb cut depth using tangent function."""
        return seat_cut_width * math.tan(self.math_plus.degrees_to_radians(slope_degrees))
    
    def _calc_peak_allowance(self, ridge_thickness: float, slope_degrees: float) -> float:
        """Calculate allowance for ridge board thickness at peak."""
        run = ridge_thickness / 2
        return math.tan(self.math_plus.degrees_to_radians(slope_degrees)) * run
    
    def _calc_rafter_rise(self, plate_to_peak: float, rafter_stand: float, peak_allowance: float) -> float:
        """Calculate effective rafter rise."""
        return plate_to_peak - rafter_stand - peak_allowance
    
    def _calc_rafter_run(self, outside_to_center: float, ridge_thickness: float) -> float:
        """Calculate effective rafter run."""
        return outside_to_center - (ridge_thickness * 0.5)
    
    def _calc_body_length(self, rise: float, run: float) -> float:
        """Calculate body length using Pythagorean theorem - Randy's core calculation."""
        return math.sqrt(rise**2 + run**2)
    
    def _calc_projection(self, boxing_projection: float, fascia_thickness: float) -> float:
        """Calculate actual projection accounting for fascia."""
        return boxing_projection - fascia_thickness
    
    def _calc_tail_length(self, projection: float, slope_degrees: float) -> float:
        """Calculate tail length using Randy's trigonometric approach."""
        return self.math_plus.tri_side_from_sines(slope_degrees, 90, projection)
    
    def _calc_top_length(self, body_length: float, tail_length: float) -> float:
        """Calculate top length (longpoint to shortpoint)."""
        return body_length + tail_length
    
    def _calc_total_length(self, top_length: float, slope_degrees: float, board_width: float) -> float:
        """Calculate total rafter length including angle cuts."""
        return top_length + (math.tan(self.math_plus.degrees_to_radians(slope_degrees)) * board_width)
    
    def get_educational_summary(self) -> Dict:
        """Get comprehensive educational summary of calculations."""
        return {
            'specifications': {
                'name': self.specs.name,
                'plate_to_center': self.specs.plate_to_center,
                'plate_to_peak': self.specs.plate_to_peak,
                'board_width': self.specs.board_width,
                'plate_width': self.specs.plate_width,
                'boxing_projection': self.specs.boxing_projection,
                'fascia_thickness': self.specs.fascia_thickness,
                'ridge_thickness': self.specs.ridge_thickness
            },
            'calculations': self.results,
            'educational_notes': {
                'pythagorean_application': f"Body length = √(rise² + run²) = √({self.results['rise']:.3f}² + {self.results['run']:.3f}²) = {self.results['body_length']:.3f}\"",
                'trigonometry_application': f"Slope angle = arctan(rise/run) = arctan({self.results['rise']:.3f}/{self.results['run']:.3f}) = {self.results['slope_degrees']:.2f}°",
                'iterative_refinement': f"Converged after {self.results['iterations']} iterations",
                'engineering_precision': "Randy's algorithm accounts for ridge board thickness, fascia placement, and cut allowances",
                'crisis_innovation': "Built under pressure for real construction projects - demonstrates applied mathematics mastery"
            }
        }
    
    def print_results(self):
        """Print formatted results matching Randy's original app output style."""
        print(f"\n🏗️  RANDY'S AUTHENTIC RAFTER WIZARD - {self.results['name']}")
        print("=" * 60)
        
        print(f"\n📐 SLOPE CALCULATIONS:")
        print(f"   Slope Angle: {self.results['slope_degrees']:.2f}°")
        print(f"   Slope Ratio: {self.math_plus.decimal_to_fraction(self.results['slope_ratio'])}/12")
        
        print(f"\n📏 KEY MEASUREMENTS:")
        print(f"   Plumb Height: {self.math_plus.decimal_to_fraction(self.results['plumb_height'])}\"")
        print(f"   Plumb Cut Depth: {self.math_plus.decimal_to_fraction(self.results['plumb_cut_depth'])}\"")
        print(f"   Height Above Plate (HAP): {self.math_plus.decimal_to_fraction(self.results['hap'])}\"")
        print(f"   Seat Cut Width: {self.math_plus.decimal_to_fraction(self.results['seat_cut_width'])}\"")
        
        print(f"\n🔢 PYTHAGOREAN CALCULATION:")
        print(f"   Rise: {self.math_plus.decimal_to_fraction(self.results['rise'])}\"")
        print(f"   Run: {self.math_plus.decimal_to_fraction(self.results['run'])}\"")
        print(f"   Body Length: {self.math_plus.decimal_to_fraction(self.results['body_length'])}\"")
        print(f"   (√{self.results['rise']:.3f}² + {self.results['run']:.3f}² = {self.results['body_length']:.3f})")
        
        print(f"\n📐 FINAL LENGTHS:")
        print(f"   Tail Length: {self.math_plus.decimal_to_fraction(self.results['tail_length'])}\"")
        print(f"   Top Length: {self.math_plus.decimal_to_fraction(self.results['top_length'])}\"")
        print(f"   Total Length: {self.math_plus.decimal_to_fraction(self.results['total_length'])}\"")
        
        print(f"\n⚙️  ALGORITHM INFO:")
        print(f"   Iterations: {self.results['iterations']} (Randy's refinement process)")
        print(f"   Projection: {self.math_plus.decimal_to_fraction(self.results['projection'])}\"")


def educational_demo():
    """Demonstrate Randy's authentic algorithms with educational examples."""
    
    print("🎓 RANDY'S AUTHENTIC RAFTER WIZARD - EDUCATIONAL DEMONSTRATION")
    print("=" * 70)
    print("\nThis recreation uses Randy's actual algorithms extracted from his")
    print("original Android APK (60052.apk) found on Amazon Appstore.")
    print("\nCRISIS INNOVATION: Built during challenging times for real construction projects")
    print("MATHEMATICAL MASTERY: Applied trigonometry, Pythagorean theorem, iterative refinement")
    
    # Example 1: Randy's Standard Rafter
    print(f"\n{'='*70}")
    print("EXAMPLE 1: Standard Gable Rafter (Randy's typical project)")
    print("=" * 70)
    
    specs1 = RafterSpecifications(
        name="Standard Gable Rafter",
        plate_to_center=144.0,    # 12 feet span
        plate_to_peak=72.0,       # 6 feet rise  
        board_width=3.5,          # 2x8 lumber
        plate_width=3.5,          # 2x4 plate
        boxing_projection=18.0,   # 18" overhang
        fascia_thickness=0.75,    # 1" fascia
        ridge_thickness=1.5       # 2x ridge board
    )
    
    rafter1 = AuthenticRafterWizard(specs1)
    rafter1.print_results()
    
    # Example 2: Steep Slope Rafter
    print(f"\n\n{'='*70}")
    print("EXAMPLE 2: Steep Slope Rafter (Challenging geometry)")
    print("=" * 70)
    
    specs2 = RafterSpecifications(
        name="Steep Slope Rafter",
        plate_to_center=120.0,    # 10 feet span
        plate_to_peak=120.0,      # 10 feet rise (45° slope)
        board_width=5.5,          # 2x12 lumber
        plate_width=5.5,          # 2x6 plate
        boxing_projection=24.0,   # 24" overhang
        fascia_thickness=1.0,     # Thicker fascia
        ridge_thickness=1.5       # 2x ridge board
    )
    
    rafter2 = AuthenticRafterWizard(specs2)
    rafter2.print_results()
    
    print(f"\n\n🧠 EDUCATIONAL INSIGHTS:")
    print("=" * 70)
    print("1. Randy's iterative algorithm refines calculations for precision")
    print("2. Trigonometry calculates all cut angles and depths")
    print("3. Pythagorean theorem determines exact rafter lengths")
    print("4. Engineering allowances account for material thicknesses")
    print("5. Crisis-time development demonstrates applied mathematics mastery")
    print(f"\n🎯 This is authentic Randy innovation preserved for education!")


if __name__ == "__main__":
    educational_demo()