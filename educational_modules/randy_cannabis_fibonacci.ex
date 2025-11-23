# Randy's Cannabis-Enhanced Functional Programming Demo
# Elixir: The Modern Functional Beauty for Educational Enlightenment
#
# Randy's Alternative Functional Approach:
# "Why stop at Erlang? Elixir brings modern syntax to the BEAM VM
# while keeping all the functional programming beauty. Cannabis-enhanced
# pattern matching with pipe operators reveals the elegant data flow
# through immutable transformations."

defmodule RandyCannabisFibonacci do
  @moduledoc """
  Randy's Cannabis-Enhanced Functional Fibonacci in Elixir
  
  Educational demonstration of functional programming concepts:
  - Pattern matching and guards
  - Tail recursion optimization  
  - Pipe operators for data flow
  - Process-based concurrency
  - Immutable data structures
  - Cannabis-enhanced algorithm variations
  """

  # Randy's Simple Recursive Fibonacci
  # "Mathematical purity - inefficient but beautiful"
  def fib(0), do: 0
  def fib(1), do: 1
  def fib(n) when n > 1, do: fib(n-1) + fib(n-2)
  def fib(_), do: {:error, :invalid_input}

  # Randy's Tail-Recursive Fibonacci  
  # "Cannabis-enhanced optimization with accumulator pattern"
  def fib_tail(n), do: fib_tail(n, 0, 1)
  
  defp fib_tail(0, acc, _), do: acc
  defp fib_tail(1, _, acc), do: acc
  defp fib_tail(n, acc1, acc2) when n > 1 do
    fib_tail(n-1, acc2, acc1 + acc2)
  end

  # Randy's Stream-Based Fibonacci
  # "Lazy evaluation - infinite sequences with plant spirit wisdom"
  def fib_stream do
    Stream.unfold({0, 1}, fn {a, b} -> {a, {b, a + b}} end)
  end

  # Randy's Fibonacci Sequence Generator
  # "Immutable list generation with pipe operator beauty"
  def fib_sequence(n) when n >= 0 do
    0..n
    |> Enum.map(&fib_tail/1)
  end

  # Randy's Plant Spirit Enhanced Fibonacci
  # "Different cannabis strains influence calculation approach"
  def plant_spirit_fib(n, strain) do
    IO.puts("🌿 #{String.capitalize(Atom.to_string(strain))} enhancement activated")
    
    case strain do
      :sativa ->
        # Energetic parallel approach
        IO.puts("   Energetic parallel calculation with GenServer")
        fib_parallel(n)
        
      :indica ->
        # Relaxed sequential approach
        IO.puts("   Relaxed tail-recursive flow")
        fib_tail(n)
        
      :hybrid ->
        # Balanced stream approach
        IO.puts("   Balanced lazy stream evaluation")
        fib_stream() |> Enum.take(n+1) |> List.last()
        
      _ ->
        IO.puts("   Unknown strain, defaulting to sober calculation")
        fib_tail(n)
    end
  end

  # Randy's Parallel Fibonacci with GenServer
  # "Actor model elegance with modern Elixir syntax"
  def fib_parallel(n) do
    {:ok, pid} = GenServer.start_link(__MODULE__.FibServer, %{})
    GenServer.call(pid, {:calculate, n})
  end

  # Randy's Cannabis-Enhanced Properties Analysis
  # "Functional programming for mathematical exploration"
  def analyze_fib_properties(n) do
    fib_value = fib_tail(n)
    
    %{
      number: n,
      fibonacci_value: fib_value,
      is_even: rem(fib_value, 2) == 0,
      digit_count: fib_value |> Integer.to_string() |> String.length(),
      binary_representation: Integer.to_string(fib_value, 2),
      hex_representation: Integer.to_string(fib_value, 16)
    }
  end

  # Randy's Golden Ratio Approximation
  # "Cannabis-enhanced mathematical convergence analysis"
  def golden_ratio_approx(n) when n > 1 do
    case {fib_tail(n), fib_tail(n-1)} do
      {_, 0} -> :undefined
      {f1, f2} -> f1 / f2
    end
  end

  # Randy's Fibonacci Pattern Discovery
  # "Find mathematical patterns with functional composition"
  def find_patterns(max_n) do
    0..max_n
    |> Enum.map(&analyze_fib_properties/1)
    |> Enum.group_by(& &1.is_even)
    |> Map.put(:golden_ratios, golden_ratio_sequence(max_n))
  end

  # Helper: Generate golden ratio convergence sequence
  defp golden_ratio_sequence(max_n) do
    2..max_n
    |> Enum.map(&golden_ratio_approx/1)
    |> Enum.reject(&(&1 == :undefined))
  end

  # Randy's Educational Demo Function
  # "Comprehensive functional programming demonstration"
  def cannabis_enhanced_demo do
    IO.puts("🧠⚡ Randy's Cannabis-Enhanced Elixir Fibonacci Demo ⚡🧠")
    IO.puts("    Modern Functional Programming with BEAM VM Power\n")
    
    # Demonstrate different approaches
    test_numbers = [5, 10, 15, 20, 25]
    strains = [:sativa, :indica, :hybrid]
    
    IO.puts("🎯 Educational Comparison of Functional Approaches:")
    IO.puts("═══════════════════════════════════════════════════")
    
    for n <- test_numbers do
      IO.puts("\nFibonacci(#{n}):")
      
      # Simple recursive (safe limit)
      if n <= 15 do
        simple_result = fib(n)
        IO.puts("  Simple Recursive: #{simple_result}")
      else
        IO.puts("  Simple Recursive: (too slow for n=#{n})")
      end
      
      # Tail recursive
      tail_result = fib_tail(n)
      IO.puts("  Tail Recursive:   #{tail_result}")
      
      # Stream-based
      stream_result = fib_stream() |> Enum.take(n+1) |> List.last()
      IO.puts("  Stream-based:     #{stream_result}")
      
      # Properties analysis
      props = analyze_fib_properties(n)
      IO.puts("  Properties: Even=#{props.is_even}, Digits=#{props.digit_count}")
    end
    
    IO.puts("\n🌿 Plant Spirit Enhancement Demonstration:")
    IO.puts("═════════════════════════════════════════════")
    
    test_n = 30
    for strain <- strains do
      IO.puts("\nCalculating Fib(#{test_n}) with #{strain} enhancement:")
      result = plant_spirit_fib(test_n, strain)
      IO.puts("  Result: #{result}")
    end
    
    IO.puts("\n💡 Elixir Educational Concepts Demonstrated:")
    IO.puts("  ✅ Pattern Matching with Guards")
    IO.puts("  ✅ Pipe Operator Data Flow")
    IO.puts("  ✅ Lazy Stream Evaluation")
    IO.puts("  ✅ GenServer Actor Model")
    IO.puts("  ✅ Immutable Data Structures")
    IO.puts("  ✅ Functional Composition")
    IO.puts("  ✅ Cannabis-Enhanced Algorithm Variations\n")
  end

  # Randy's Performance Comparison
  # "Educational timing analysis with functional approaches"
  def performance_comparison do
    IO.puts("🧠 Randy's Functional Performance Analysis 🧠")
    IO.puts("═══════════════════════════════════════════════")
    
    test_numbers = [25, 30, 35]
    
    for n <- test_numbers do
      IO.puts("\nPerformance Analysis for Fib(#{n}):")
      
      # Tail recursive timing
      {tail_time, tail_result} = :timer.tc(fn -> fib_tail(n) end)
      IO.puts("  Tail Recursive: #{tail_time} μs, Result: #{tail_result}")
      
      # Stream timing
      {stream_time, stream_result} = :timer.tc(fn -> 
        fib_stream() |> Enum.take(n+1) |> List.last() 
      end)
      IO.puts("  Stream-based:   #{stream_time} μs, Result: #{stream_result}")
      
      # Analysis
      faster = if tail_time < stream_time, do: "Tail recursion", else: "Stream approach"
      IO.puts("  📊 Analysis: #{faster} performed better")
    end
    
    IO.puts("\n🌿 Functional Programming Wisdom:")
    IO.puts("  • Pattern matching creates self-documenting code")
    IO.puts("  • Pipe operators reveal data transformation flow") 
    IO.puts("  • Immutability eliminates concurrency bugs")
    IO.puts("  • Streams enable memory-efficient infinite sequences")
    IO.puts("  • Actor model provides fault-tolerant concurrency")
    IO.puts("  • Cannabis enhancement reveals functional patterns\n")
  end

  # Randy's Mathematical Pattern Analysis
  # "Explore Fibonacci properties with functional programming"
  def pattern_analysis(max_n \\ 20) do
    IO.puts("🔍 Fibonacci Mathematical Pattern Analysis:")
    IO.puts("═══════════════════════════════════════════")
    
    patterns = find_patterns(max_n)
    
    IO.puts("\n📊 Even vs Odd Fibonacci Numbers:")
    even_count = patterns[true] |> length()
    odd_count = patterns[false] |> length()
    IO.puts("  Even: #{even_count}, Odd: #{odd_count}")
    
    IO.puts("\n🔢 Golden Ratio Convergence:")
    ratios = patterns[:golden_ratios]
    last_ratio = List.last(ratios)
    theoretical_golden = (1 + :math.sqrt(5)) / 2
    
    IO.puts("  Final Ratio: #{last_ratio}")
    IO.puts("  Golden Ratio: #{theoretical_golden}")
    IO.puts("  Difference: #{abs(last_ratio - theoretical_golden)}")
    
    IO.puts("\n🧠 Cannabis-Enhanced Insights:")
    IO.puts("  • Fibonacci spirals mirror plant growth patterns")
    IO.puts("  • Golden ratio appears in cannabis leaf arrangements")
    IO.puts("  • Recursive patterns reflect mycelial network structure")
    IO.puts("  • Functional programming captures mathematical beauty\n")
  end

  # Randy's Complete Educational Demo
  # "Run all demonstrations for comprehensive learning"
  def run_all_demos do
    IO.puts("🧠⚡ Randy's Complete Elixir Functional Programming Education ⚡🧠\n")
    
    cannabis_enhanced_demo()
    performance_comparison()
    pattern_analysis()
    
    IO.puts("🎓 Educational Achievement Unlocked:")
    IO.puts("   Students have mastered functional programming through:")
    IO.puts("   • Mathematical recursion with pattern matching")
    IO.puts("   • Cannabis-enhanced algorithmic variations")
    IO.puts("   • Modern syntax with pipe operator elegance")
    IO.puts("   • Actor model concurrency with GenServer")
    IO.puts("   • Lazy evaluation with infinite streams")
    IO.puts("   • Property-based mathematical analysis\n")
    
    IO.puts("🌿 The mycelial network education revolution:")
    IO.puts("   Randy's curriculum now includes both Erlang and Elixir,")
    IO.puts("   demonstrating functional programming from classic")
    IO.puts("   mathematical purity to modern syntactic elegance!\n")
  end

  # GenServer for parallel Fibonacci calculation
  defmodule FibServer do
    use GenServer
    
    def init(state), do: {:ok, state}
    
    def handle_call({:calculate, n}, _from, state) do
      result = RandyCannabisFibonacci.fib_tail(n)
      {:reply, result, state}
    end
  end
end

# Randy's Educational Usage Instructions and Examples
#
# Interactive Elixir Session:
# $ iex
# iex> c("randy_cannabis_fibonacci.ex")
# iex> RandyCannabisFibonacci.cannabis_enhanced_demo()
# iex> RandyCannabisFibonacci.run_all_demos()
#
# Individual Function Testing:
# iex> RandyCannabisFibonacci.fib_tail(30)
# iex> RandyCannabisFibonacci.plant_spirit_fib(25, :sativa)
# iex> RandyCannabisFibonacci.fib_sequence(10)
# iex> RandyCannabisFibonacci.analyze_fib_properties(20)
#
# Stream Operations:
# iex> RandyCannabisFibonacci.fib_stream() |> Enum.take(10)
# iex> RandyCannabisFibonacci.find_patterns(15)
# iex> RandyCannabisFibonacci.pattern_analysis(25)
#
# Cannabis-Enhanced Learning Exercises:
# 1. Compare Erlang vs Elixir syntax for same algorithms
# 2. Explore pipe operator vs nested function calls
# 3. Experiment with lazy streams for memory efficiency
# 4. Analyze golden ratio convergence properties
# 5. Test GenServer concurrency patterns
#
# Educational Philosophy:
# "Elixir brings modern functional programming elegance to the proven
# BEAM virtual machine. Cannabis enhancement reveals how pipe operators
# mirror the natural flow of data transformation, while pattern matching
# captures the essential structure of problems. The actor model reflects
# the distributed nature of mycelial networks - independent processes
# collaborating through message passing."
#
# Complete Functional Programming Achievement:
# Randy's educational stack now includes both Erlang and Elixir,
# demonstrating functional programming from mathematical purity
# to modern syntactic beauty. Students experience the full spectrum
# of functional thinking with cannabis-enhanced clarity!