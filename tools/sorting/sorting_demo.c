/*
 * 🔄 Randy's Practical Sorting Algorithms Demo
 * Real-world sorting with performance analysis
 * 
 * Demonstrates why algorithm choice matters for actual problems
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define MAX_SIZE 100000
#define TEST_SIZE 10000

// Timing utility
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Print array utility (first 10 elements)
void print_array(int arr[], int n, const char* label) {
    printf("%s: [", label);
    int show = n < 10 ? n : 10;
    for (int i = 0; i < show; i++) {
        printf("%d", arr[i]);
        if (i < show - 1) printf(", ");
    }
    if (n > 10) printf(", ... (%d total)", n);
    printf("]\n");
}

// Copy array utility
void copy_array(int src[], int dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

// 🫧 BUBBLE SORT - Simple but O(n²) - Good for small data or educational purposes
long bubble_sort(int arr[], int n) {
    long comparisons = 0;
    
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;  // Optimization: early exit if already sorted
        
        for (int j = 0; j < n - i - 1; j++) {
            comparisons++;
            if (arr[j] > arr[j + 1]) {
                // Swap elements
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        
        if (!swapped) break;  // Array is sorted
    }
    
    return comparisons;
}

// 🔄 MERGE SORT - O(n log n) always - Stable sort, good for large data
void merge(int arr[], int left, int mid, int right, long* comparisons) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Create temporary arrays
    int* left_arr = malloc(n1 * sizeof(int));
    int* right_arr = malloc(n2 * sizeof(int));
    
    // Copy data to temp arrays
    for (int i = 0; i < n1; i++) {
        left_arr[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        right_arr[j] = arr[mid + 1 + j];
    }
    
    // Merge the temp arrays back
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        (*comparisons)++;
        if (left_arr[i] <= right_arr[j]) {
            arr[k] = left_arr[i];
            i++;
        } else {
            arr[k] = right_arr[j];
            j++;
        }
        k++;
    }
    
    // Copy remaining elements
    while (i < n1) {
        arr[k] = left_arr[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = right_arr[j];
        j++;
        k++;
    }
    
    free(left_arr);
    free(right_arr);
}

void merge_sort_helper(int arr[], int left, int right, long* comparisons) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        merge_sort_helper(arr, left, mid, comparisons);
        merge_sort_helper(arr, mid + 1, right, comparisons);
        merge(arr, left, mid, right, comparisons);
    }
}

long merge_sort(int arr[], int n) {
    long comparisons = 0;
    merge_sort_helper(arr, 0, n - 1, &comparisons);
    return comparisons;
}

// ⚡ QUICKSORT - O(n log n) average, O(n²) worst - Fast in practice
int partition(int arr[], int low, int high, long* comparisons) {
    int pivot = arr[high];  // Choose last element as pivot
    int i = low - 1;        // Index of smaller element
    
    for (int j = low; j < high; j++) {
        (*comparisons)++;
        if (arr[j] < pivot) {
            i++;
            // Swap arr[i] and arr[j]
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    // Swap arr[i+1] and arr[high] (pivot)
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return i + 1;
}

void quick_sort_helper(int arr[], int low, int high, long* comparisons) {
    if (low < high) {
        int pi = partition(arr, low, high, comparisons);
        
        quick_sort_helper(arr, low, pi - 1, comparisons);
        quick_sort_helper(arr, pi + 1, high, comparisons);
    }
}

long quick_sort(int arr[], int n) {
    long comparisons = 0;
    quick_sort_helper(arr, 0, n - 1, &comparisons);
    return comparisons;
}

// Generate test data
void generate_random_data(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000;
    }
}

void generate_sorted_data(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
}

void generate_reverse_sorted_data(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
}

// Verify array is sorted
int is_sorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

// Performance test
void test_algorithm(const char* name, long (*sort_func)(int[], int), 
                   int original[], int n, const char* data_type) {
    int* arr = malloc(n * sizeof(int));
    copy_array(original, arr, n);
    
    printf("\n🔄 Testing %s on %s data (%d elements):\n", name, data_type, n);
    
    double start = get_time();
    long comparisons = sort_func(arr, n);
    double end = get_time();
    
    printf("   ⏱️  Time: %.4f seconds\n", end - start);
    printf("   🔍 Comparisons: %ld\n", comparisons);
    printf("   ✅ Sorted correctly: %s\n", is_sorted(arr, n) ? "Yes" : "No");
    
    free(arr);
}

// Real-world use case demo
void demo_real_world_usage() {
    printf("\n🌟 REAL-WORLD SORTING APPLICATIONS\n");
    printf("=====================================\n\n");
    
    // Student grades example
    int grades[] = {87, 92, 78, 95, 82, 88, 91, 76, 89, 94};
    int n = sizeof(grades) / sizeof(grades[0]);
    
    printf("📚 Sorting student grades:\n");
    print_array(grades, n, "Original grades");
    
    // Use quicksort for this size
    quick_sort(grades, n);
    print_array(grades, n, "Sorted grades");
    
    printf("   📊 Median grade: %d\n", grades[n/2]);
    printf("   🏆 Top 3 grades: %d, %d, %d\n", grades[n-1], grades[n-2], grades[n-3]);
    
    // File size sorting example  
    printf("\n📁 Sorting file sizes (KB):\n");
    int file_sizes[] = {1024, 256, 2048, 512, 128, 4096, 64, 8192};
    int fs_n = sizeof(file_sizes) / sizeof(file_sizes[0]);
    
    print_array(file_sizes, fs_n, "Original sizes");
    merge_sort(file_sizes, fs_n);  // Use merge sort for stability
    print_array(file_sizes, fs_n, "Sorted sizes");
    
    printf("   💾 Total size: %d KB\n", file_sizes[fs_n-1] + file_sizes[fs_n-2] + file_sizes[fs_n-3]);
}

int main() {
    srand(time(NULL));
    
    printf("🔄 Randy's Practical Sorting Algorithms\n");
    printf("======================================\n");
    printf("🎯 Teaching why algorithm choice matters!\n\n");
    
    // Demo with small data first
    printf("🧪 SMALL DATASET COMPARISON (100 elements)\n");
    printf("------------------------------------------\n");
    
    int small_data[100];
    generate_random_data(small_data, 100);
    
    print_array(small_data, 100, "Original data");
    
    test_algorithm("Bubble Sort", bubble_sort, small_data, 100, "random");
    test_algorithm("Merge Sort", merge_sort, small_data, 100, "random");  
    test_algorithm("Quick Sort", quick_sort, small_data, 100, "random");
    
    // Performance comparison with larger data
    printf("\n\n🚀 PERFORMANCE COMPARISON (10,000 elements)\n");
    printf("--------------------------------------------\n");
    
    int* large_data = malloc(TEST_SIZE * sizeof(int));
    
    // Test on different data patterns
    printf("\n📊 RANDOM DATA:\n");
    generate_random_data(large_data, TEST_SIZE);
    test_algorithm("Bubble Sort", bubble_sort, large_data, 1000, "random");  // Smaller for bubble
    test_algorithm("Merge Sort", merge_sort, large_data, TEST_SIZE, "random");
    test_algorithm("Quick Sort", quick_sort, large_data, TEST_SIZE, "random");
    
    printf("\n📈 ALREADY SORTED DATA:\n");
    generate_sorted_data(large_data, TEST_SIZE);
    test_algorithm("Bubble Sort", bubble_sort, large_data, 1000, "sorted");
    test_algorithm("Merge Sort", merge_sort, large_data, TEST_SIZE, "sorted");
    test_algorithm("Quick Sort", quick_sort, large_data, TEST_SIZE, "sorted");
    
    printf("\n📉 REVERSE SORTED DATA:\n");
    generate_reverse_sorted_data(large_data, TEST_SIZE);
    test_algorithm("Bubble Sort", bubble_sort, large_data, 1000, "reverse");
    test_algorithm("Merge Sort", merge_sort, large_data, TEST_SIZE, "reverse");
    test_algorithm("Quick Sort", quick_sort, large_data, TEST_SIZE, "reverse");
    
    // Real-world examples
    demo_real_world_usage();
    
    // Key insights
    printf("\n\n💡 KEY INSIGHTS FOR REAL PROGRAMMING:\n");
    printf("=====================================\n");
    printf("🫧 Bubble Sort: Simple to understand, good for learning or tiny datasets\n");
    printf("🔄 Merge Sort: Consistent O(n log n), stable, good for external sorting\n");
    printf("⚡ Quick Sort: Fastest average case, used in most standard libraries\n\n");
    printf("🎯 Choose based on your needs:\n");
    printf("   • Small data (< 50): Any algorithm works\n");
    printf("   • Large data: Quick sort or merge sort\n");
    printf("   • Stability needed: Merge sort\n");
    printf("   • Memory constrained: Quick sort (in-place)\n");
    printf("   • Learning/teaching: Start with bubble sort\n\n");
    
    printf("🏆 Real libraries use hybrid approaches:\n");
    printf("   • Python: Timsort (merge sort variant)\n");
    printf("   • C++ std::sort: Introsort (quicksort + heapsort)\n");
    printf("   • Java: Dual-pivot quicksort\n\n");
    
    free(large_data);
    return 0;
}