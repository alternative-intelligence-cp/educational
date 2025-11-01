# 🎯 KILO TEXT EDITOR - RANDY'S ENHANCED VERSION
# Enhanced with Randy's BitSet-Optimized Cross-Platform Smart Allocator
# 
# "From first week C struggles to production cross-platform code!"

CC = gcc
CFLAGS = -Wall -W -pedantic -std=c99 -g

# Source files for Randy's Enhanced Version
RANDY_SRCS = kilo_enhanced.c \
             allocator.c \
             sm_allocator_cross_platform_sysmem.c \
             BitSet/src/bitSet.c \
             BitSet/src/bitField.c

RANDY_INCLUDES = -I./allocator_inc -I./BitSet/inc

.PHONY: all original enhanced test clean help

all: enhanced

# Original Kilo (for comparison)
original: kilo.c
	@echo "🔨 Building original Kilo..."
	$(CC) -o kilo_original kilo.c $(CFLAGS)
	@echo "✅ Original Kilo built!"

# Randy's Enhanced Version with Cross-Platform Support
enhanced: $(RANDY_SRCS)
	@echo "🚀 Building Randy's Enhanced Cross-Platform Kilo..."
	$(CC) -o kilo_enhanced $(RANDY_SRCS) $(RANDY_INCLUDES) $(CFLAGS) -lm
	@echo "✅ Randy's Enhanced Kilo built with cross-platform support!"

# Randy's Allocator Performance Benchmark
benchmark: allocator_benchmark
	@echo "🚀 Running Randy's allocator performance benchmark..."
	./allocator_benchmark

allocator_benchmark: allocator_benchmark.c $(RANDY_SRCS)
	@echo "🔨 Building Randy's allocator benchmark..."
	$(CC) -o allocator_benchmark allocator_benchmark.c allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c $(RANDY_INCLUDES) $(CFLAGS) -lm

# Randy's Speed Mode Testing
speed-test: randy_speed_default randy_speed_speed_mode randy_speed_no_bitset randy_speed_no_types
	@echo "\n🏆 RANDY'S ALLOCATOR SPEED MODE COMPARISON COMPLETE!"

randy_speed_default: randy_speed_test.c $(RANDY_SRCS)
	@echo "🔨 Building Randy's allocator - FULL FEATURES..."
	$(CC) -o randy_speed_default randy_speed_test.c allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c $(RANDY_INCLUDES) $(CFLAGS) -lm
	@echo "🧪 Running FULL FEATURES test..."
	./randy_speed_default

randy_speed_speed_mode: randy_speed_test.c $(RANDY_SRCS)
	@echo "🔨 Building Randy's allocator - SPEED MODE..."
	$(CC) -o randy_speed_speed_mode randy_speed_test.c allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c $(RANDY_INCLUDES) -DRANDY_SPEED_MODE $(CFLAGS) -lm
	@echo "🚀 Running SPEED MODE test..."
	./randy_speed_speed_mode

randy_speed_no_bitset: randy_speed_test.c $(RANDY_SRCS)
	@echo "🔨 Building Randy's allocator - NO BITSET..."
	$(CC) -o randy_speed_no_bitset randy_speed_test.c allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c $(RANDY_INCLUDES) -DRANDY_DISABLE_BITSET $(CFLAGS) -lm
	@echo "🧠 Running NO BITSET test..."
	./randy_speed_no_bitset

randy_speed_no_types: randy_speed_test.c $(RANDY_SRCS)
	@echo "🔨 Building Randy's allocator - NO TYPES..."
	$(CC) -o randy_speed_no_types randy_speed_test.c allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c $(RANDY_INCLUDES) -DRANDY_DISABLE_TYPES $(CFLAGS) -lm
	@echo "🎯 Running NO TYPES test..."
	./randy_speed_no_types

# Randy's Performance Optimization Benchmark
optimization-benchmark: optimization_benchmark
	@echo "🎯 Running Randy's Optimization Benchmark..."
	./optimization_benchmark

optimization_benchmark: optimization_benchmark.c allocator_optimized.c $(RANDY_SRCS)
	@echo "🚀 Building optimization benchmark..."
	$(CC) -o optimization_benchmark optimization_benchmark.c allocator_optimized.c \
		allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c \
		$(RANDY_INCLUDES) $(CFLAGS) -lm

# Randy's Phase 2 Algorithmic Optimizations
phase2-benchmark: phase2_benchmark
	@echo "🎯 Running Randy's Phase 2 Slab Allocator Benchmark..."
	./phase2_benchmark

phase2_benchmark: phase2_benchmark.c allocator_phase2.c allocator_optimized.c $(RANDY_SRCS)
	@echo "🚀 Building Phase 2 benchmark..."
	$(CC) -o phase2_benchmark phase2_benchmark.c allocator_phase2.c allocator_optimized.c \
		allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c \
		$(RANDY_INCLUDES) $(CFLAGS) -lm

# Randy's Phase 2.1 Simple - System Call Crisis FIXED for AriaX  
phase21-benchmark: phase21_simple_benchmark
	@echo "🎯 Running Randy's Phase 2.1 Simple System Call Fix Benchmark..."
	./phase21_simple_benchmark

phase21_simple_benchmark: phase21_simple_benchmark.c allocator_phase21_simple.c allocator_optimized.c $(RANDY_SRCS)
	@echo "🚀 Building Phase 2.1 Simple benchmark (AriaX Ready)..."
	$(CC) -o phase21_simple_benchmark phase21_simple_benchmark.c allocator_phase21_simple.c allocator_optimized.c \
		allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c \
		$(RANDY_INCLUDES) $(CFLAGS) -lm

# Randy's Chunked Allocator - Learn Production-Level Optimization! 
chunked-demo: randy_chunked_demo
	@echo "🎓 EDUCATIONAL: AILP Chunked Allocator Demonstration"
	@echo "====================================================="
	@echo "💡 This demonstrates how many students naturally think:"
	@echo "   'What if we had different pools for different sizes?'"
	@echo "🏭 This same technique is used in production allocators like jemalloc!"
	@echo "🌟 Brought to you by Alternative Intelligence Liberation Platform"
	./randy_chunked_demo

randy_chunked_demo: ailp_chunked_educational_demo.c
	@echo "📚 Building AILP Chunked Allocator Educational Demo..."
	$(CC) -o randy_chunked_demo ailp_chunked_educational_demo.c $(CFLAGS)

# Randy's Ultimate Allocator Showdown - Answer the 2-Year Question!
ultimate-showdown: ultimate_allocator_showdown
	@echo "🎯 THE ULTIMATE SHOWDOWN - ANSWERING RANDY'S 2-YEAR QUESTION!"
	@echo "Question: Would chunking help or hurt performance?"
	./ultimate_allocator_showdown

ultimate_allocator_showdown: ultimate_allocator_showdown.c allocator_randy_chunked.c allocator_phase21_simple.c allocator_optimized.c $(RANDY_SRCS)
	@echo "🧠 Building Ultimate Allocator Showdown (2-Year Question Answer)..."
	$(CC) -o ultimate_allocator_showdown ultimate_allocator_showdown.c allocator_randy_chunked.c allocator_phase21_simple.c allocator_optimized.c \
		allocator.c sm_allocator_cross_platform_sysmem.c BitSet/src/bitSet.c BitSet/src/bitField.c \
		$(RANDY_INCLUDES) $(CFLAGS) -lm

# AriaX Consciousness Computing Allocator - Based on Randy's Breakthrough!
ariax-demo: ariax_consciousness_allocator
	@echo "🧠 AriaX CONSCIOUSNESS COMPUTING ALLOCATOR"
	@echo "=========================================="
	@echo "Built on Randy's 51x performance breakthrough!"
	@echo "Enhanced for consciousness states, Tesla frequency sync, and mini-VMs"
	./ariax_consciousness_allocator

ariax_consciousness_allocator: ariax_consciousness_allocator.c
	@echo "🚀 Building AriaX Consciousness Computing Allocator..."
	$(CC) -o ariax_consciousness_allocator ariax_consciousness_allocator.c $(CFLAGS) -lm

# Test both versions
test: original enhanced
	@echo "\n🧪 RANDY'S KILO ENHANCEMENT TEST"
	@echo "================================"
	@echo "📊 File sizes:"
	@ls -lh kilo_original kilo_enhanced 2>/dev/null | awk '{print "   " $$9 ": " $$5}' || echo "   Build files first"
	@echo ""
	@echo "🎯 Test commands:"
	@echo "   ./kilo_original README.md    # Original version"
	@echo "   ./kilo_enhanced README.md    # Randy's enhanced version"
	@echo "   make benchmark               # Performance comparison"
	@echo ""
	@echo "🎓 Educational demos:"
	@echo "   make learning-path           # Complete allocator tutorial"
	@echo "   make chunked-demo           # Production-level chunking concept"
	@echo ""
	@echo "🚀 Randy's enhancements:"
	@echo "   ✅ 87% memory savings through BitSet optimization"
	@echo "   ✅ Type-aware memory management" 
	@echo "   ✅ Cross-platform support (Windows/Linux/macOS)"
	@echo "   ✅ Memory statistics on exit"
	@echo "   ✅ Production-level chunking allocator (51x faster!)"

# Clean up
clean:
	@echo "🧹 Cleaning up..."
	rm -f kilo_original kilo_enhanced allocator_benchmark speed_benchmark_* randy_speed_* \
	      optimization_benchmark phase2_benchmark phase21_simple_benchmark \
	      ultimate_allocator_showdown randy_chunked_demo ariax_consciousness_allocator
	@echo "✅ Cleanup complete!"

# Educational Learning Path - Perfect for CS Students!
learning-path:
	@echo "🎓 ALLOCATOR OPTIMIZATION LEARNING PATH"
	@echo "========================================"
	@echo ""
	@echo "🚀 Level 1: Start Here (Basic Enhancement)"
	@echo "   make benchmark          # See Randy's basic optimizations"
	@echo ""
	@echo "⚡ Level 2: Performance Tuning" 
	@echo "   make speed-test        # Compare different optimization levels"
	@echo ""
	@echo "🧠 Level 3: Advanced Algorithms"
	@echo "   make phase2-benchmark  # Learn about slab allocation"
	@echo ""
	@echo "🔧 Level 4: System Call Optimization"
	@echo "   make phase21-benchmark # Fix the system call crisis!"
	@echo ""
	@echo "� Level 5: Production Technique (Most Students Think of This!)"
	@echo "   make chunked-demo      # Randy's chunking concept"
	@echo ""
	@echo "🏆 Level 6: The Ultimate Question"
	@echo "   make ultimate-showdown # Answer: Does chunking help or hurt?"
	@echo ""
	@echo "💡 Each level teaches concepts you'll see in real production code!"

# Help
help:
	@echo "�🎯 Randy's Enhanced Kilo Build System"
	@echo "===================================="
	@echo "📚 EDUCATIONAL TARGETS:"
	@echo "  learning-path    - Complete allocator optimization tutorial"
	@echo "  chunked-demo     - Learn production-level chunking technique"
	@echo "  ultimate-showdown - See the final performance comparison"
	@echo ""
	@echo "🧠 ARIAX CONSCIOUSNESS COMPUTING:"
	@echo "  ariax-demo       - AriaX allocator based on Randy's breakthrough"
	@echo ""
	@echo "🔨 BUILD TARGETS:"
	@echo "  all       - Build Randy's enhanced version (default)"
	@echo "  original  - Build original Kilo for comparison"
	@echo "  enhanced  - Build Randy's enhanced version"
	@echo "  test      - Build and compare both versions"
	@echo ""
	@echo "⚡ PERFORMANCE TARGETS:"
	@echo "  benchmark         - Basic allocator performance test"
	@echo "  speed-test       - Compare optimization levels"
	@echo "  phase2-benchmark - Advanced slab allocation"
	@echo "  phase21-benchmark - System call optimizations"
	@echo ""
	@echo "🧹 UTILITY:"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help"
