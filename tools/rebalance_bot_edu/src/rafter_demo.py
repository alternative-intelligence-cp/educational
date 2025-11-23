#!/usr/bin/env python3

"""
🎯 RafterWizard Demo - Showcasing Randy's Construction Mathematics
"""

import sys
import os

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from rafter_wizard import RafterWizard

def main():
    print("🎯 RAFTER WIZARD DEMONSTRATION")
    print("Showcasing Randy's Applied Construction Mathematics")
    print("="*60)
    
    wizard = RafterWizard()
    
    # Demo 1: Small Shed
    print("\n📋 DEMO 1: Small Storage Shed")
    print("8-foot span, 4/12 pitch roof")
    result1 = wizard.complete_rafter_calculation(8, 0.0, 4.0, 12.0, 12.0)
    
    # Demo 2: Residential House  
    print("\n📋 DEMO 2: Residential House")
    print("24-foot span, 6/12 pitch roof")
    result2 = wizard.complete_rafter_calculation(24, 0.0, 6.0, 12.0, 18.0)
    
    # Demo 3: Steep Cabin Roof
    print("\n📋 DEMO 3: Mountain Cabin") 
    print("16-foot span, 12/12 pitch roof (45° angle)")
    result3 = wizard.complete_rafter_calculation(16, 0.0, 12.0, 12.0, 24.0)
    
    print("\n" + "="*60)
    print("🎓 EDUCATIONAL SUMMARY")
    print("="*60)
    print("Randy's RafterWizard demonstrates:")
    print("✅ Pythagorean theorem in real-world application")
    print("✅ Trigonometry for construction angles")
    print("✅ Practical problem solving with mathematics")
    print("✅ Professional-grade calculation precision")
    print("✅ User-friendly interface for tradespeople")
    print("\n💡 This is applied mathematics at its finest!")

if __name__ == "__main__":
    main()