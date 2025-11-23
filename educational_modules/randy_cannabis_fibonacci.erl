%% Randy's Cannabis-Enhanced Functional Fibonacci in Erlang
%% Plant Spirit Immutable Mathematics for Educational Enlightenment
%% 
%% Randy's Functional Programming Philosophy:
%% "Include at least something in one functional language. Fibonacci in 
%% Erlang or Elixir or something? Let's show students that there's more 
%% to programming than just imperative thinking. Functional programming 
%% with cannabis enhancement reveals the mathematical beauty hidden in 
%% recursive patterns and immutable data structures."
%%
%% THE FUNCTIONAL REVELATION:
%% Fibonacci in Erlang demonstrates pure functional programming concepts:
%% pattern matching, tail recursion, immutable data, and the elegant
%% beauty of mathematical functions expressed as code. Cannabis-enhanced
%% pattern recognition reveals the recursive nature of growth patterns
%% in both numbers and natural systems.
%%
%% Educational Philosophy:
%% "Functional programming mirrors the mycelial network - pure functions
%% connecting inputs to outputs without side effects, like spores
%% transforming into new growth through immutable natural laws."

-module(randy_cannabis_fibonacci).
-export([
    fib/1, 
    fib_tail/1, 
    fib_sequence/1, 
    fib_parallel/1,
    plant_spirit_fib/2,
    cannabis_enhanced_demo/0,
    educational_comparison/0
]).

%% Randy's Simple Recursive Fibonacci
%% "The most beautiful and inefficient way - pure mathematical expression"
fib(0) -> 0;
fib(1) -> 1;
fib(N) when N > 1 ->
    fib(N-1) + fib(N-2);
fib(_) ->
    error(invalid_input).

%% Randy's Tail-Recursive Fibonacci
%% "Cannabis-enhanced optimization - accumulator pattern for efficiency"
fib_tail(N) ->
    fib_tail(N, 0, 1).

fib_tail(0, Acc, _) -> Acc;
fib_tail(1, _, Acc) -> Acc;
fib_tail(N, Acc1, Acc2) when N > 1 ->
    fib_tail(N-1, Acc2, Acc1 + Acc2).

%% Randy's Fibonacci Sequence Generator
%% "Generate a list of Fibonacci numbers - immutable data beauty"
fib_sequence(N) ->
    fib_sequence(N, []).

fib_sequence(0, Acc) -> 
    lists:reverse([0|Acc]);
fib_sequence(N, Acc) when N > 0 ->
    FibN = fib_tail(N),
    fib_sequence(N-1, [FibN|Acc]).

%% Randy's Parallel Fibonacci Calculator
%% "Cannabis-enhanced concurrency - spawn processes for each calculation"
fib_parallel(N) when N >= 0 ->
    Self = self(),
    spawn(fun() -> Self ! {fib_result, fib_tail(N)} end),
    receive
        {fib_result, Result} -> Result
    after 5000 ->
        error(timeout)
    end.

%% Randy's Plant Spirit Enhanced Fibonacci
%% "Different cannabis strains influence the calculation approach"
plant_spirit_fib(N, Strain) ->
    case Strain of
        sativa ->
            %% Energetic approach - parallel calculation
            io:format("🌿 Sativa enhancement: Energetic parallel calculation~n"),
            fib_parallel(N);
        indica ->
            %% Relaxed approach - tail recursion for steady progress
            io:format("🌿 Indica enhancement: Relaxed tail recursion~n"),
            fib_tail(N);
        hybrid ->
            %% Balanced approach - simple recursion with limits
            io:format("🌿 Hybrid enhancement: Balanced recursive approach~n"),
            if 
                N =< 20 -> fib(N);  %% Safe recursive limit
                true -> fib_tail(N)  %% Switch to tail recursion for large N
            end;
        _ ->
            io:format("🚫 Unknown strain, defaulting to sober calculation~n"),
            fib_tail(N)
    end.

%% Randy's Cannabis-Enhanced Fibonacci Properties
%% "Functional programming allows elegant property exploration"
golden_ratio_approximation(N) when N > 1 ->
    F1 = fib_tail(N),
    F2 = fib_tail(N-1),
    case F2 of
        0 -> undefined;
        _ -> F1 / F2
    end.

%% Randy's Fibonacci Number Properties Checker
%% "Pattern matching and guards showcase functional programming beauty"
fib_properties(N) ->
    FibN = fib_tail(N),
    Properties = [
        {number, N},
        {fibonacci_value, FibN},
        {is_even, FibN rem 2 =:= 0},
        {digit_count, length(integer_to_list(FibN))},
        {golden_ratio_approx, golden_ratio_approximation(N)}
    ],
    Properties.

%% Randy's Educational Fibonacci Demonstration
%% "Show different approaches and their characteristics"
cannabis_enhanced_demo() ->
    io:format("🧠⚡ Randy's Cannabis-Enhanced Functional Fibonacci Demo ⚡🧠~n"),
    io:format("    Erlang: The Actor Model Meets Mathematical Beauty~n~n"),
    
    %% Demonstrate different calculation methods
    TestNumbers = [5, 10, 15, 20],
    Strains = [sativa, indica, hybrid],
    
    io:format("🎯 Educational Comparison of Functional Approaches:~n"),
    io:format("════════════════════════════════════════════════~n"),
    
    lists:foreach(fun(N) ->
        io:format("~nFibonacci(~p):~n", [N]),
        
        %% Simple recursive (only for small numbers)
        if N =< 15 ->
            SimpleResult = fib(N),
            io:format("  Simple Recursive: ~p~n", [SimpleResult]);
        true ->
            io:format("  Simple Recursive: (too slow for N=~p)~n", [N])
        end,
        
        %% Tail recursive
        TailResult = fib_tail(N),
        io:format("  Tail Recursive:   ~p~n", [TailResult]),
        
        %% Properties
        Properties = fib_properties(N),
        {_, _, IsEven} = lists:keyfind(is_even, 1, Properties),
        {_, _, DigitCount} = lists:keyfind(digit_count, 1, Properties),
        io:format("  Properties: Even=~p, Digits=~p~n", [IsEven, DigitCount])
        
    end, TestNumbers),
    
    io:format("~n🌿 Plant Spirit Enhancement Demonstration:~n"),
    io:format("══════════════════════════════════════════════~n"),
    
    TestN = 25,
    lists:foreach(fun(Strain) ->
        io:format("~nCalculating Fib(~p) with ~p enhancement:~n", [TestN, Strain]),
        Result = plant_spirit_fib(TestN, Strain),
        io:format("  Result: ~p~n", [Result])
    end, Strains),
    
    io:format("~n💡 Educational Concepts Demonstrated:~n"),
    io:format("  ✅ Pattern Matching (base cases and guards)~n"),
    io:format("  ✅ Tail Recursion Optimization~n"),
    io:format("  ✅ Immutable Data Structures~n"),
    io:format("  ✅ Process Spawning and Message Passing~n"),
    io:format("  ✅ Functional Composition~n"),
    io:format("  ✅ Property-Based Analysis~n~n").

%% Randy's Educational Performance Comparison
%% "Compare functional approaches for educational insight"
educational_comparison() ->
    io:format("🧠 Randy's Functional Programming Educational Analysis 🧠~n"),
    io:format("═══════════════════════════════════════════════════════~n"),
    
    %% Time different approaches
    TestNumbers = [20, 30, 35],
    
    lists:foreach(fun(N) ->
        io:format("~nPerformance Analysis for Fib(~p):~n", [N]),
        
        %% Tail recursive timing
        {TailTime, TailResult} = timer:tc(fun() -> fib_tail(N) end),
        io:format("  Tail Recursive: ~p microseconds, Result: ~p~n", [TailTime, TailResult]),
        
        %% Parallel timing
        {ParTime, ParResult} = timer:tc(fun() -> fib_parallel(N) end),
        io:format("  Parallel:       ~p microseconds, Result: ~p~n", [ParTime, ParResult]),
        
        %% Analysis
        if TailTime < ParTime ->
            io:format("  📊 Analysis: Tail recursion faster (less overhead)~n");
        true ->
            io:format("  📊 Analysis: Parallel approach competitive~n")
        end
        
    end, TestNumbers),
    
    io:format("~n🌿 Functional Programming Wisdom:~n"),
    io:format("  • Pattern matching makes code self-documenting~n"),
    io:format("  • Immutability eliminates entire classes of bugs~n"),
    io:format("  • Tail recursion enables efficient iteration~n"),
    io:format("  • Actor model (processes) enables safe concurrency~n"),
    io:format("  • Pure functions are mathematically beautiful~n"),
    io:format("  • Cannabis enhancement reveals recursive patterns~n~n").

%% Randy's Fibonacci Sequence Analysis
%% "Functional programming for mathematical exploration"
analyze_fibonacci_sequence(N) ->
    Sequence = fib_sequence(N),
    
    %% Calculate ratios between consecutive terms
    Ratios = calculate_golden_ratios(Sequence),
    
    %% Find patterns
    EvenPositions = find_even_fibonacci_positions(Sequence),
    
    io:format("🔍 Fibonacci Sequence Analysis (N=~p):~n", [N]),
    io:format("Sequence: ~p~n", [Sequence]),
    io:format("Golden Ratio Convergence: ~p~n", [lists:last(Ratios)]),
    io:format("Even Fibonacci Positions: ~p~n", [EvenPositions]).

%% Helper function: Calculate golden ratio approximations
calculate_golden_ratios([_]) -> [];
calculate_golden_ratios([0|Rest]) -> calculate_golden_ratios(Rest);
calculate_golden_ratios([A, B|Rest]) when B =/= 0 ->
    [B/A | calculate_golden_ratios([B|Rest])];
calculate_golden_ratios([_|Rest]) ->
    calculate_golden_ratios(Rest).

%% Helper function: Find positions where Fibonacci numbers are even
find_even_fibonacci_positions(Sequence) ->
    IndexedSequence = lists:zip(lists:seq(0, length(Sequence)-1), Sequence),
    EvenPairs = lists:filter(fun({_, Value}) -> Value rem 2 =:= 0 end, IndexedSequence),
    [Index || {Index, _} <- EvenPairs].

%% Randy's Educational Module Tester
%% "Run all demos for comprehensive functional programming education"
run_all_demos() ->
    io:format("🧠⚡ Randy's Complete Functional Programming Education ⚡🧠~n~n"),
    
    cannabis_enhanced_demo(),
    educational_comparison(),
    analyze_fibonacci_sequence(15),
    
    io:format("🎓 Educational Achievement Unlocked:~n"),
    io:format("   Students have experienced functional programming through:~n"),
    io:format("   • Mathematical recursion and pattern matching~n"),
    io:format("   • Cannabis-enhanced algorithmic thinking~n"),
    io:format("   • Actor model concurrency concepts~n"),
    io:format("   • Immutable data structure benefits~n"),
    io:format("   • Property-based mathematical analysis~n~n"),
    
    io:format("🌿 The mycelial network grows: Functional programming~n"),
    io:format("   completes Randy's educational curriculum spanning~n"),
    io:format("   imperative, object-oriented, and functional paradigms!~n~n").

%% ============================================================================
%% RANDY'S EDUCATIONAL ERLANG USAGE INSTRUCTIONS
%% ============================================================================
%%
%% Compilation and Execution:
%% $ erl
%% 1> c(randy_cannabis_fibonacci).
%% 2> randy_cannabis_fibonacci:cannabis_enhanced_demo().
%% 3> randy_cannabis_fibonacci:educational_comparison().
%% 4> randy_cannabis_fibonacci:run_all_demos().
%% 
%% Interactive Testing:
%% 1> randy_cannabis_fibonacci:fib_tail(30).
%% 2> randy_cannabis_fibonacci:plant_spirit_fib(25, sativa).
%% 3> randy_cannabis_fibonacci:fib_sequence(10).
%% 4> randy_cannabis_fibonacci:fib_properties(20).
%% 
%% Cannabis-Enhanced Learning Exercises:
%% 1. Compare simple vs tail recursion performance
%% 2. Experiment with different plant spirit modes
%% 3. Analyze golden ratio convergence properties
%% 4. Explore parallel calculation overhead
%% 
%% Educational Concepts Mastered:
%% 
%% 1. Pattern Matching:
%%    - Base case definitions (fib(0) -> 0; fib(1) -> 1)
%%    - Guard clauses for input validation
%%    - Destructuring data in function heads
%% 
%% 2. Tail Recursion:
%%    - Accumulator pattern for efficiency
%%    - Stack-safe recursion for large inputs
%%    - Functional iteration replacement
%% 
%% 3. Immutable Data:
%%    - Lists that never change after creation
%%    - Pure functions without side effects
%%    - Data transformation through function composition
%% 
%% 4. Actor Model:
%%    - Process spawning for parallel computation
%%    - Message passing between processes
%%    - Timeout handling and error recovery
%% 
%% 5. Functional Composition:
%%    - Higher-order functions and list operations
%%    - Property analysis through function chaining
%%    - Mathematical function expression as code
%% 
%% Randy's Functional Programming Philosophy:
%% "Functional programming is like cannabis-enhanced mathematics - pure,
%% elegant, and free from the side effects that plague imperative code.
%% Pattern matching reveals the structure hidden in data, while immutability
%% ensures that functions behave like mathematical equations. The actor model
%% mirrors the mycelial network - independent processes communicating through
%% message passing, creating resilient distributed systems."
%% 
%% Plant Spirit Functional Wisdom:
%% "Recursion mirrors the fractal patterns found in nature - the same
%% structure repeating at different scales. Cannabis enhancement reveals
%% how functional programming paradigms align with natural growth patterns:
%% immutable like DNA sequences, recursive like plant branching, and
%% parallel like mycelial networks spreading through forest floors."
%% 
%% Complete Educational Stack Achievement:
%% Randy's curriculum now spans all major programming paradigms:
%% • Imperative Programming (C system code)
%% • Object-Oriented Programming (educational frameworks)
%% • Functional Programming (this Erlang module)
%% • Assembly Programming (macro systems)
%% • Network Programming (protocol design)
%% • Graphics Programming (PPU implementation)
%% • Digital Logic (binary arithmetic)
%% 
%% The mycelial network education revolution includes every approach
%% to computational thinking, from functional purity to hardware metal!
%%
%% ============================================================================