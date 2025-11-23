#!/usr/bin/env python3
"""
🎨 RANDY'S CANVAS DRAWING FUNDAMENTALS - EDUCATIONAL MODULE
===========================================================

RANDY'S PHILOSOPHY:
"Most classes want you to use a framework to abstract it away.
But how are you supposed to learn how things are drawn on a screen? 
If you can't draw on a javascript canvas you sure as hell can't draw to a video ram buffer."

EDUCATIONAL APPROACH:
This module teaches FUNDAMENTALS FIRST:
- Direct pixel manipulation before frameworks
- Understanding coordinates and color models
- Raw drawing operations before abstractions
- Canvas concepts that transfer to ANY graphics system

EXTRACTED FROM: Randy's RafterWizard canvas drawing concepts
MODERNIZED FOR: Contemporary educational use without heavy dependencies

Randy's Core Insight: "Learn to draw yourself before letting libraries do it for you"
"""

import sys
import math
from typing import Tuple, List, Optional
from dataclasses import dataclass

# Minimal dependencies - Randy's approach
try:
    from PIL import Image, ImageDraw, ImageFont
    HAS_PIL = True
except ImportError:
    HAS_PIL = False
    print("📦 Optional: Install Pillow for advanced canvas features")
    print("    pip install Pillow")

@dataclass
class RandyColor:
    """Randy's simple color model - RGB values 0-255"""
    r: int
    g: int 
    b: int
    a: int = 255  # Alpha (transparency)
    
    def to_tuple(self) -> Tuple[int, int, int, int]:
        """Convert to RGBA tuple for PIL"""
        return (self.r, self.g, self.b, self.a)
    
    def to_hex(self) -> str:
        """Convert to hex string for web"""
        return f"#{self.r:02x}{self.g:02x}{self.b:02x}"

@dataclass 
class RandyPoint:
    """Randy's coordinate system - simple x,y points"""
    x: int
    y: int
    
    def __add__(self, other):
        return RandyPoint(self.x + other.x, self.y + other.y)

class RandyCanvas:
    """
    Randy's Educational Canvas - Learn to Draw From First Principles
    
    This is Randy's fundamentals-first approach to graphics programming.
    No magic, no frameworks - just direct pixel manipulation and 
    understanding how drawing actually works.
    """
    
    def __init__(self, width: int = 800, height: int = 600):
        self.width = width
        self.height = height
        self.background_color = RandyColor(255, 255, 255)  # White background
        
        print("🎨 Randy's Canvas Initialized")
        print(f"📏 Dimensions: {width}x{height} pixels")
        print("🎓 Ready for fundamentals-first drawing education!")
        
        # Initialize canvas based on available libraries
        if HAS_PIL:
            self.image = Image.new('RGBA', (width, height), self.background_color.to_tuple())
            self.draw = ImageDraw.Draw(self.image)
            print("✅ PIL Canvas mode enabled")
        else:
            self.pixels = [[self.background_color for _ in range(width)] for _ in range(height)]
            print("✅ Pure Python Canvas mode enabled")
    
    def set_pixel(self, x: int, y: int, color: RandyColor):
        """
        Randy's Fundamental Operation #1: Set a single pixel
        
        This is the atomic operation that everything else builds on.
        Randy's insight: "If you can't set a pixel, you can't draw anything."
        """
        if not self._is_valid_coordinate(x, y):
            return
            
        if HAS_PIL:
            self.draw.point((x, y), color.to_tuple())
        else:
            self.pixels[y][x] = color
    
    def get_pixel(self, x: int, y: int) -> RandyColor:
        """
        Randy's Fundamental Operation #2: Read a pixel
        
        Understanding what's already on the canvas is crucial for
        advanced operations like blending and collision detection.
        """
        if not self._is_valid_coordinate(x, y):
            return self.background_color
            
        if HAS_PIL:
            pixel = self.image.getpixel((x, y))
            return RandyColor(pixel[0], pixel[1], pixel[2], pixel[3] if len(pixel) == 4 else 255)
        else:
            return self.pixels[y][x]
    
    def draw_line(self, start: RandyPoint, end: RandyPoint, color: RandyColor, width: int = 1):
        """
        Randy's Line Drawing - Bresenham's Algorithm Implementation
        
        Educational Note: This shows how lines are actually drawn pixel by pixel.
        No magic - just math and iteration.
        """
        print(f"🖊️  Drawing line from ({start.x}, {start.y}) to ({end.x}, {end.y})")
        
        if HAS_PIL and width == 1:
            # Use PIL for efficiency
            self.draw.line((start.x, start.y, end.x, end.y), color.to_tuple(), width)
        else:
            # Randy's educational implementation - Bresenham's line algorithm
            dx = abs(end.x - start.x)
            dy = abs(end.y - start.y)
            sx = 1 if start.x < end.x else -1
            sy = 1 if start.y < end.y else -1
            err = dx - dy
            
            x, y = start.x, start.y
            
            while True:
                # Draw pixel (and surrounding pixels for width)
                for i in range(-width//2, width//2 + 1):
                    for j in range(-width//2, width//2 + 1):
                        self.set_pixel(x + i, y + j, color)
                
                if x == end.x and y == end.y:
                    break
                    
                e2 = 2 * err
                if e2 > -dy:
                    err -= dy
                    x += sx
                if e2 < dx:
                    err += dx
                    y += sy
    
    def draw_rectangle(self, top_left: RandyPoint, width: int, height: int, 
                      color: RandyColor, filled: bool = False):
        """
        Randy's Rectangle Drawing - Building Complex Shapes from Lines
        
        Educational Note: Shows how complex shapes are built from simple operations.
        """
        print(f"📐 Drawing {'filled ' if filled else ''}rectangle at ({top_left.x}, {top_left.y}) size {width}x{height}")
        
        if HAS_PIL:
            bottom_right = (top_left.x + width, top_left.y + height)
            if filled:
                self.draw.rectangle((top_left.x, top_left.y, *bottom_right), fill=color.to_tuple())
            else:
                self.draw.rectangle((top_left.x, top_left.y, *bottom_right), outline=color.to_tuple())
        else:
            if filled:
                # Fill the rectangle pixel by pixel
                for y in range(top_left.y, top_left.y + height):
                    for x in range(top_left.x, top_left.x + width):
                        self.set_pixel(x, y, color)
            else:
                # Draw outline using lines
                # Top line
                self.draw_line(top_left, RandyPoint(top_left.x + width, top_left.y), color)
                # Bottom line  
                self.draw_line(RandyPoint(top_left.x, top_left.y + height), 
                             RandyPoint(top_left.x + width, top_left.y + height), color)
                # Left line
                self.draw_line(top_left, RandyPoint(top_left.x, top_left.y + height), color)
                # Right line
                self.draw_line(RandyPoint(top_left.x + width, top_left.y), 
                             RandyPoint(top_left.x + width, top_left.y + height), color)
    
    def draw_circle(self, center: RandyPoint, radius: int, color: RandyColor, filled: bool = False):
        """
        Randy's Circle Drawing - Trigonometry in Action
        
        Educational Note: Shows how trigonometry creates curved shapes.
        This is the same math Randy used in RafterWizard for angle calculations.
        """
        print(f"⭕ Drawing {'filled ' if filled else ''}circle at ({center.x}, {center.y}) radius {radius}")
        
        if HAS_PIL:
            bbox = (center.x - radius, center.y - radius, center.x + radius, center.y + radius)
            if filled:
                self.draw.ellipse(bbox, fill=color.to_tuple())
            else:
                self.draw.ellipse(bbox, outline=color.to_tuple())
        else:
            # Randy's educational implementation using trigonometry
            if filled:
                # Fill circle pixel by pixel using distance formula
                for y in range(center.y - radius, center.y + radius + 1):
                    for x in range(center.x - radius, center.x + radius + 1):
                        distance = math.sqrt((x - center.x)**2 + (y - center.y)**2)
                        if distance <= radius:
                            self.set_pixel(x, y, color)
            else:
                # Draw circle outline using parametric equations
                for angle in range(360):
                    radian = math.radians(angle)
                    x = center.x + int(radius * math.cos(radian))
                    y = center.y + int(radius * math.sin(radian))
                    self.set_pixel(x, y, color)
    
    def draw_text(self, position: RandyPoint, text: str, color: RandyColor, size: int = 12):
        """
        Randy's Text Drawing - When Available
        
        Educational Note: Text rendering is actually very complex!
        This shows why understanding fundamentals matters.
        """
        print(f"✍️  Drawing text '{text}' at ({position.x}, {position.y})")
        
        if HAS_PIL:
            try:
                # Try to load a font
                font = ImageFont.truetype("arial.ttf", size)
            except:
                # Fall back to default font
                font = ImageFont.load_default()
            
            self.draw.text((position.x, position.y), text, fill=color.to_tuple(), font=font)
        else:
            print("📝 Text rendering requires PIL library")
            print(f"    Would draw: '{text}' at ({position.x}, {position.y})")
    
    def clear_canvas(self, color: Optional[RandyColor] = None):
        """
        Randy's Canvas Clearing - Start Fresh
        
        Educational Note: Understanding how to reset state is crucial.
        """
        if color is None:
            color = self.background_color
            
        print(f"🧹 Clearing canvas to {color.to_hex()}")
        
        if HAS_PIL:
            self.image = Image.new('RGBA', (self.width, self.height), color.to_tuple())
            self.draw = ImageDraw.Draw(self.image)
        else:
            self.pixels = [[color for _ in range(self.width)] for _ in range(self.height)]
    
    def save_image(self, filename: str):
        """
        Randy's Image Saving - Preserve Your Work
        
        Educational Note: Understanding file formats and data persistence.
        """
        print(f"💾 Saving canvas to {filename}")
        
        if HAS_PIL:
            self.image.save(filename)
            print(f"✅ Image saved successfully")
        else:
            print("💾 Image saving requires PIL library")
            print(f"    Would save canvas state to {filename}")
    
    def _is_valid_coordinate(self, x: int, y: int) -> bool:
        """Randy's bounds checking - prevent errors"""
        return 0 <= x < self.width and 0 <= y < self.height

# Randy's Color Palette - Predefined Colors for Education
class RandyColors:
    """Randy's standard color palette for educational use"""
    WHITE = RandyColor(255, 255, 255)
    BLACK = RandyColor(0, 0, 0)
    RED = RandyColor(255, 0, 0)
    GREEN = RandyColor(0, 255, 0)
    BLUE = RandyColor(0, 0, 255)
    YELLOW = RandyColor(255, 255, 0)
    PURPLE = RandyColor(255, 0, 255)
    CYAN = RandyColor(0, 255, 255)
    ORANGE = RandyColor(255, 165, 0)
    GRAY = RandyColor(128, 128, 128)

def randy_canvas_demo():
    """
    Randy's Canvas Drawing Demonstration
    
    Educational walkthrough showing fundamental drawing operations.
    """
    print("🎨 RANDY'S CANVAS DRAWING FUNDAMENTALS DEMO")
    print("=" * 50)
    print()
    print("Randy's Philosophy: 'Learn to draw yourself before using frameworks'")
    print("This demo shows raw drawing operations that work everywhere!")
    print()
    
    # Create Randy's educational canvas
    canvas = RandyCanvas(400, 300)
    
    print("\n🎓 LESSON 1: Basic Shapes")
    print("-" * 25)
    
    # Draw some basic shapes
    canvas.draw_rectangle(RandyPoint(50, 50), 100, 60, RandyColors.BLUE, filled=True)
    canvas.draw_circle(RandyPoint(200, 100), 40, RandyColors.RED)
    canvas.draw_line(RandyPoint(300, 50), RandyPoint(350, 150), RandyColors.GREEN, width=3)
    
    print("\n🎓 LESSON 2: Text and Complex Shapes")
    print("-" * 35)
    
    # Add some text
    canvas.draw_text(RandyPoint(10, 10), "Randy's Canvas Education", RandyColors.BLACK, 16)
    
    # Draw a more complex shape - a house!
    print("\n🏠 Drawing a house using fundamental operations:")
    house_base = RandyPoint(50, 200)
    
    # House base (rectangle)
    canvas.draw_rectangle(house_base, 80, 60, RandyColors.ORANGE, filled=True)
    
    # House roof (triangle using lines)
    roof_peak = RandyPoint(90, 180)
    canvas.draw_line(house_base, roof_peak, RandyColors.RED, width=2)
    canvas.draw_line(RandyPoint(130, 200), roof_peak, RandyColors.RED, width=2)
    
    # Door (rectangle)
    canvas.draw_rectangle(RandyPoint(70, 230), 20, 30, RandyColors.BLACK, filled=True)
    
    # Window (rectangle)
    canvas.draw_rectangle(RandyPoint(100, 210), 15, 15, RandyColors.CYAN, filled=True)
    
    print("\n🎓 LESSON 3: Mathematical Drawing")
    print("-" * 30)
    
    # Draw a sine wave to show mathematical concepts
    print("📈 Drawing sine wave using Randy's trigonometry knowledge:")
    for x in range(200, 350):
        y = 200 + int(30 * math.sin((x - 200) * 0.1))
        canvas.set_pixel(x, y, RandyColors.PURPLE)
    
    # Save the educational canvas
    canvas.save_image("randy_canvas_education.png")
    
    print("\n🎉 EDUCATIONAL DEMO COMPLETE!")
    print("=" * 40)
    print()
    print("🧠 Randy's Key Learning Points:")
    print("1. Every complex drawing is built from simple operations")
    print("2. Understanding coordinates and colors is fundamental")
    print("3. Math (trigonometry, geometry) creates interesting graphics")
    print("4. Once you know these basics, ANY graphics library makes sense")
    print()
    print("💡 Randy's Wisdom: 'If you can draw on a canvas, you can draw anywhere!'")

def interactive_canvas_tutorial():
    """
    Randy's Interactive Canvas Tutorial
    
    Let students experiment with Randy's canvas drawing concepts.
    """
    print("🎯 RANDY'S INTERACTIVE CANVAS TUTORIAL")
    print("=" * 40)
    print()
    print("Learn by doing - Randy's preferred method!")
    print()
    
    canvas = RandyCanvas(300, 200)
    
    while True:
        print("\n🎨 Randy's Canvas Commands:")
        print("1. Draw Rectangle")
        print("2. Draw Circle") 
        print("3. Draw Line")
        print("4. Set Pixel")
        print("5. Clear Canvas")
        print("6. Save Image")
        print("7. Exit Tutorial")
        
        choice = input("\nSelect operation (1-7): ").strip()
        
        try:
            if choice == "1":
                x = int(input("Rectangle X position: "))
                y = int(input("Rectangle Y position: "))
                w = int(input("Rectangle width: "))
                h = int(input("Rectangle height: "))
                color = _get_color_choice()
                filled = input("Filled? (y/n): ").lower().startswith('y')
                canvas.draw_rectangle(RandyPoint(x, y), w, h, color, filled)
                
            elif choice == "2":
                x = int(input("Circle center X: "))
                y = int(input("Circle center Y: "))
                r = int(input("Circle radius: "))
                color = _get_color_choice()
                filled = input("Filled? (y/n): ").lower().startswith('y')
                canvas.draw_circle(RandyPoint(x, y), r, color, filled)
                
            elif choice == "3":
                x1 = int(input("Start X: "))
                y1 = int(input("Start Y: "))
                x2 = int(input("End X: "))
                y2 = int(input("End Y: "))
                color = _get_color_choice()
                width = int(input("Line width (1-10): ") or "1")
                canvas.draw_line(RandyPoint(x1, y1), RandyPoint(x2, y2), color, width)
                
            elif choice == "4":
                x = int(input("Pixel X: "))
                y = int(input("Pixel Y: "))
                color = _get_color_choice()
                canvas.set_pixel(x, y, color)
                print(f"✅ Pixel set at ({x}, {y})")
                
            elif choice == "5":
                color = _get_color_choice()
                canvas.clear_canvas(color)
                
            elif choice == "6":
                filename = input("Filename (without extension): ") or "randy_canvas"
                canvas.save_image(f"{filename}.png")
                
            elif choice == "7":
                print("👋 Thanks for learning Randy's canvas fundamentals!")
                break
                
            else:
                print("❌ Invalid choice. Try again.")
                
        except ValueError:
            print("❌ Invalid input. Please enter numbers for coordinates.")
        except KeyboardInterrupt:
            print("\n👋 Tutorial cancelled.")
            break

def _get_color_choice() -> RandyColor:
    """Helper function to choose colors interactively"""
    colors = {
        '1': ('Red', RandyColors.RED),
        '2': ('Green', RandyColors.GREEN), 
        '3': ('Blue', RandyColors.BLUE),
        '4': ('Yellow', RandyColors.YELLOW),
        '5': ('Purple', RandyColors.PURPLE),
        '6': ('Orange', RandyColors.ORANGE),
        '7': ('Black', RandyColors.BLACK),
        '8': ('White', RandyColors.WHITE)
    }
    
    print("\n🎨 Choose color:")
    for key, (name, _) in colors.items():
        print(f"  {key}. {name}")
    
    choice = input("Color (1-8): ").strip()
    if choice in colors:
        return colors[choice][1]
    else:
        return RandyColors.BLACK  # Default

if __name__ == "__main__":
    print("🎨 RANDY'S CANVAS DRAWING EDUCATION")
    print("=" * 40)
    print()
    print("Choose your learning path:")
    print("1. 🎓 Demo - Watch Randy's fundamentals in action")
    print("2. 🎯 Interactive Tutorial - Learn by doing")
    print()
    
    choice = input("Select option (1-2): ").strip()
    
    if choice == "1":
        randy_canvas_demo()
    elif choice == "2":  
        interactive_canvas_tutorial()
    else:
        print("Running demo by default...")
        randy_canvas_demo()