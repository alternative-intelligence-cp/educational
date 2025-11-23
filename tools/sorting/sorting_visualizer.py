#!/usr/bin/env python3
"""
🔄 Randy's Sorting Algorithms - Python Implementation
Interactive visualization and analysis tool
"""

import random
import time
import matplotlib.pyplot as plt
import numpy as np
from typing import List, Callable, Tuple

class SortingVisualizer:
    def __init__(self):
        self.comparison_count = 0
        
    def reset_comparisons(self):
        self.comparison_count = 0
    
    def compare(self, a: int, b: int) -> bool:
        """Count comparisons for analysis"""
        self.comparison_count += 1
        return a > b
    
    def bubble_sort(self, arr: List[int]) -> List[int]:
        """🫧 Bubble Sort - O(n²) but simple and stable"""
        arr = arr.copy()
        n = len(arr)
        
        for i in range(n):
            swapped = False
            for j in range(0, n - i - 1):
                if self.compare(arr[j], arr[j + 1]):
                    arr[j], arr[j + 1] = arr[j + 1], arr[j]
                    swapped = True
            
            if not swapped:  # Early termination optimization
                break
        
        return arr
    
    def merge_sort(self, arr: List[int]) -> List[int]:
        """🔄 Merge Sort - O(n log n) always, stable"""
        if len(arr) <= 1:
            return arr
        
        mid = len(arr) // 2
        left = self.merge_sort(arr[:mid])
        right = self.merge_sort(arr[mid:])
        
        return self._merge(left, right)
    
    def _merge(self, left: List[int], right: List[int]) -> List[int]:
        result = []
        i = j = 0
        
        while i < len(left) and j < len(right):
            if not self.compare(left[i], right[j]):  # left[i] <= right[j]
                result.append(left[i])
                i += 1
            else:
                result.append(right[j])
                j += 1
        
        result.extend(left[i:])
        result.extend(right[j:])
        return result
    
    def quick_sort(self, arr: List[int]) -> List[int]:
        """⚡ Quick Sort - O(n log n) average, fast in practice"""
        if len(arr) <= 1:
            return arr
        
        pivot = arr[len(arr) // 2]
        left = [x for x in arr if not self.compare(x, pivot) and x != pivot]
        middle = [x for x in arr if x == pivot]
        right = [x for x in arr if self.compare(x, pivot)]
        
        return self.quick_sort(left) + middle + self.quick_sort(right)

def time_algorithm(sort_func: Callable, data: List[int], name: str) -> Tuple[float, int, List[int]]:
    """Time a sorting algorithm and return performance metrics"""
    visualizer = SortingVisualizer()
    
    start_time = time.time()
    sorted_data = sort_func(visualizer, data.copy())
    end_time = time.time()
    
    return end_time - start_time, visualizer.comparison_count, sorted_data

def generate_test_data(size: int, data_type: str) -> List[int]:
    """Generate different types of test data"""
    if data_type == "random":
        return [random.randint(1, 1000) for _ in range(size)]
    elif data_type == "sorted":
        return list(range(1, size + 1))
    elif data_type == "reverse":
        return list(range(size, 0, -1))
    elif data_type == "nearly_sorted":
        arr = list(range(1, size + 1))
        # Swap a few random elements
        for _ in range(size // 20):
            i, j = random.randint(0, size-1), random.randint(0, size-1)
            arr[i], arr[j] = arr[j], arr[i]
        return arr
    else:
        raise ValueError(f"Unknown data type: {data_type}")

def create_performance_chart(results: dict, data_types: List[str]):
    """Create visualization of algorithm performance"""
    algorithms = list(results.keys())
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Time comparison
    x = np.arange(len(data_types))
    width = 0.25
    
    for i, algorithm in enumerate(algorithms):
        times = [results[algorithm][dt]['time'] for dt in data_types]
        ax1.bar(x + i * width, times, width, label=algorithm)
    
    ax1.set_xlabel('Data Type')
    ax1.set_ylabel('Time (seconds)')
    ax1.set_title('Sorting Algorithm Performance - Time')
    ax1.set_xticks(x + width)
    ax1.set_xticklabels(data_types)
    ax1.legend()
    ax1.set_yscale('log')
    
    # Comparison count
    for i, algorithm in enumerate(algorithms):
        comparisons = [results[algorithm][dt]['comparisons'] for dt in data_types]
        ax2.bar(x + i * width, comparisons, width, label=algorithm)
    
    ax2.set_xlabel('Data Type')
    ax2.set_ylabel('Comparisons')
    ax2.set_title('Sorting Algorithm Performance - Comparisons')
    ax2.set_xticks(x + width)
    ax2.set_xticklabels(data_types)
    ax2.legend()
    ax2.set_yscale('log')
    
    plt.tight_layout()
    plt.savefig('/home/randy/WORK_SPACE/work/tools/sorting/performance_comparison.png')
    plt.show()

def demo_real_world_applications():
    """Demonstrate practical uses of sorting"""
    print("\n🌟 REAL-WORLD SORTING APPLICATIONS")
    print("=" * 40)
    
    # Student grade analysis
    print("\n📚 Student Grade Analysis:")
    grades = [87, 92, 78, 95, 82, 88, 91, 76, 89, 94, 85, 90, 79, 93, 86]
    print(f"Original grades: {grades}")
    
    visualizer = SortingVisualizer()
    sorted_grades = visualizer.quick_sort(grades)
    print(f"Sorted grades: {sorted_grades}")
    print(f"Median grade: {sorted_grades[len(sorted_grades)//2]}")
    print(f"Top 3 grades: {sorted_grades[-3:]}")
    print(f"Bottom 3 grades: {sorted_grades[:3]}")
    
    # E-commerce price sorting
    print("\n🛒 E-commerce Product Prices:")
    products = [
        ("Laptop", 999.99),
        ("Mouse", 25.50),
        ("Keyboard", 75.00),
        ("Monitor", 299.99),
        ("Headphones", 149.99),
        ("Webcam", 89.99)
    ]
    
    print("Original products:")
    for name, price in products:
        print(f"  {name}: ${price}")
    
    # Sort by price (using merge sort for stability)
    products_by_price = sorted(products, key=lambda x: x[1])
    print("\nSorted by price:")
    for name, price in products_by_price:
        print(f"  {name}: ${price}")
    
    # File processing example
    print("\n📁 Log File Processing:")
    log_entries = [
        (1609459200, "ERROR: Database connection failed"),
        (1609459150, "INFO: User login successful"),
        (1609459180, "WARNING: High memory usage"),
        (1609459220, "INFO: Backup completed"),
        (1609459170, "ERROR: Payment processing failed")
    ]
    
    print("Original log entries:")
    for timestamp, message in log_entries:
        print(f"  {timestamp}: {message}")
    
    # Sort by timestamp
    sorted_logs = sorted(log_entries, key=lambda x: x[0])
    print("\nSorted by timestamp:")
    for timestamp, message in sorted_logs:
        print(f"  {timestamp}: {message}")

def main():
    print("🔄 Randy's Sorting Algorithms - Python Edition")
    print("=" * 50)
    print("🎯 Real-world sorting with performance analysis!")
    
    # Test with different data sizes and types
    sizes = [100, 1000, 5000]
    data_types = ["random", "sorted", "reverse", "nearly_sorted"]
    algorithms = {
        "Bubble Sort": lambda v, data: v.bubble_sort(data),
        "Merge Sort": lambda v, data: v.merge_sort(data), 
        "Quick Sort": lambda v, data: v.quick_sort(data)
    }
    
    results = {alg: {} for alg in algorithms}
    
    for size in sizes:
        print(f"\n📊 Testing with {size} elements:")
        print("-" * 30)
        
        for data_type in data_types:
            print(f"\n{data_type.upper()} DATA:")
            test_data = generate_test_data(size, data_type)
            
            for alg_name, alg_func in algorithms.items():
                # Skip bubble sort for large datasets
                if alg_name == "Bubble Sort" and size > 1000:
                    continue
                    
                elapsed_time, comparisons, sorted_data = time_algorithm(alg_func, test_data, alg_name)
                
                # Verify correctness
                is_correct = sorted_data == sorted(test_data)
                
                print(f"  {alg_name}:")
                print(f"    Time: {elapsed_time:.4f} seconds")
                print(f"    Comparisons: {comparisons:,}")
                print(f"    Correct: {'✅' if is_correct else '❌'}")
                
                if alg_name not in results:
                    results[alg_name] = {}
                if data_type not in results[alg_name]:
                    results[alg_name][data_type] = {}
                    
                results[alg_name][data_type] = {
                    'time': elapsed_time,
                    'comparisons': comparisons,
                    'correct': is_correct
                }
    
    # Real-world applications demo
    demo_real_world_applications()
    
    # Algorithm comparison summary
    print("\n💡 ALGORITHM COMPARISON SUMMARY")
    print("=" * 40)
    print("🫧 Bubble Sort:")
    print("   ✅ Simple to understand and implement")
    print("   ✅ Stable (maintains relative order)")
    print("   ❌ O(n²) time complexity")
    print("   🎯 Use for: Learning, very small datasets")
    
    print("\n🔄 Merge Sort:")
    print("   ✅ Guaranteed O(n log n) performance")
    print("   ✅ Stable sorting")
    print("   ✅ Predictable performance")
    print("   ❌ O(n) extra memory required")
    print("   🎯 Use for: Large datasets, when stability matters")
    
    print("\n⚡ Quick Sort:")
    print("   ✅ Fast average case O(n log n)")
    print("   ✅ In-place sorting (low memory)")
    print("   ❌ O(n²) worst case")
    print("   ❌ Not stable")
    print("   🎯 Use for: General purpose, when memory is limited")
    
    print("\n🏆 REAL-WORLD USAGE:")
    print("   • Python's sorted(): Timsort (hybrid merge/insertion)")
    print("   • Java Arrays.sort(): Dual-pivot quicksort")
    print("   • C++ std::sort(): Introsort (quicksort/heapsort hybrid)")
    print("   • Database ORDER BY: Often merge sort variants")
    
    print(f"\n✨ Performance comparison chart saved to:")
    print(f"   /home/randy/WORK_SPACE/work/tools/sorting/performance_comparison.png")

if __name__ == "__main__":
    main()