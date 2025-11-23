-- Randy's Cannabis-Enhanced Functional Fibonacci in Haskell
-- Pure Functional Mathematics for Educational Enlightenment
--
-- Randy's Haskell Philosophy:
-- "Why not go full functional purity? Haskell represents the pinnacle
-- of functional programming - lazy evaluation, type safety, and
-- mathematical elegance. Cannabis-enhanced pattern recognition reveals
-- the beauty of purely functional computation where side effects
-- are impossible and every function is a mathematical equation."

module RandyCannabisFibonacci where

import System.TimeIt (timeIt)
import Data.List (genericIndex)

-- Randy's Simple Recursive Fibonacci
-- "Mathematical purity - Haskell makes recursion beautiful"
fibSimple :: Integer -> Integer
fibSimple 0 = 0
fibSimple 1 = 1
fibSimple n = fibSimple (n-1) + fibSimple (n-2)

-- Randy's Memoized Fibonacci
-- "Cannabis-enhanced optimization with lazy evaluation"
fibMemo :: Integer -> Integer
fibMemo n = fibs `genericIndex` n
  where fibs = 0 : 1 : zipWith (+) fibs (tail fibs)

-- Randy's Fibonacci Stream
-- "Infinite lazy sequences - the beauty of Haskell laziness"
fibStream :: [Integer]
fibStream = 0 : 1 : zipWith (+) fibStream (tail fibStream)

-- Randy's Plant Spirit Enhanced Fibonacci
-- "Different cannabis strains influence calculation approach"
data CannabisStrain = Sativa | Indica | Hybrid | Sober
  deriving (Show, Eq)

plantSpiritFib :: Integer -> CannabisStrain -> IO Integer
plantSpiritFib n strain = do
  putStrLn $ "🌿 " ++ show strain ++ " enhancement activated"
  case strain of
    Sativa -> do
      putStrLn "   Energetic infinite stream approach"
      return $ fibStream !! fromInteger n
    Indica -> do
      putStrLn "   Relaxed memoized calculation"
      return $ fibMemo n
    Hybrid -> do
      putStrLn "   Balanced approach with guards"
      return $ if n <= 20 then fibSimple n else fibMemo n
    Sober -> do
      putStrLn "   Standard memoized calculation"
      return $ fibMemo n

-- Randy's Fibonacci Properties Analysis
-- "Type-safe property exploration with Maybe types"
data FibProperties = FibProperties
  { fibNumber :: Integer
  , fibValue :: Integer
  , isEven :: Bool
  , digitCount :: Int
  , binaryRep :: String
  } deriving (Show)

analyzeFibProperties :: Integer -> FibProperties
analyzeFibProperties n = 
  let fibVal = fibMemo n
      fibStr = show fibVal
  in FibProperties
     { fibNumber = n
     , fibValue = fibVal
     , isEven = even fibVal
     , digitCount = length fibStr
     , binaryRep = showBin fibVal
     }

-- Helper: Convert to binary string
showBin :: Integer -> String
showBin 0 = "0"
showBin n = reverse $ showBin' n
  where showBin' 0 = []
        showBin' x = (if x `mod` 2 == 1 then '1' else '0') : showBin' (x `div` 2)

-- Randy's Golden Ratio Approximation
-- "Type-safe mathematical convergence with Maybe"
goldenRatioApprox :: Integer -> Maybe Double
goldenRatioApprox n
  | n <= 1 = Nothing
  | otherwise = 
      let f1 = fromInteger $ fibMemo n
          f2 = fromInteger $ fibMemo (n-1)
      in if f2 == 0 then Nothing else Just (f1 / f2)

-- Randy's Fibonacci Sequence Generator
-- "List comprehension beauty for educational sequences"
fibSequence :: Integer -> [Integer]
fibSequence n = map fibMemo [0..n]

-- Randy's Pattern Discovery Function
-- "Find mathematical patterns with list operations"
findFibPatterns :: Integer -> IO ()
findFibPatterns maxN = do
  putStrLn "🔍 Fibonacci Pattern Analysis:"
  putStrLn "═══════════════════════════════"
  
  let properties = map analyzeFibProperties [0..maxN]
      evenFibs = filter isEven properties
      oddFibs = filter (not . isEven) properties
      
  putStrLn $ "Even Fibonacci numbers: " ++ show (length evenFibs)
  putStrLn $ "Odd Fibonacci numbers: " ++ show (length oddFibs)
  
  -- Golden ratio convergence
  let ratios = [goldenRatioApprox i | i <- [2..maxN]]
      validRatios = [r | Just r <- ratios]
      
  when (not $ null validRatios) $ do
    let lastRatio = last validRatios
        theoreticalGolden = (1 + sqrt 5) / 2
    putStrLn $ "Golden ratio approximation: " ++ show lastRatio
    putStrLn $ "Theoretical golden ratio: " ++ show theoreticalGolden
    putStrLn $ "Difference: " ++ show (abs (lastRatio - theoreticalGolden))

-- Randy's Educational Demo Function
-- "Comprehensive functional programming demonstration"
cannabisEnhancedDemo :: IO ()
cannabisEnhancedDemo = do
  putStrLn "🧠⚡ Randy's Cannabis-Enhanced Haskell Fibonacci Demo ⚡🧠"
  putStrLn "    Pure Functional Programming with Lazy Evaluation Beauty"
  putStrLn ""
  
  let testNumbers = [5, 10, 15, 20, 25]
      strains = [Sativa, Indica, Hybrid]
  
  putStrLn "🎯 Educational Comparison of Functional Approaches:"
  putStrLn "════════════════════════════════════════════════════"
  
  mapM_ (\n -> do
    putStrLn $ "\nFibonacci(" ++ show n ++ "):"
    
    -- Memoized approach
    let memoResult = fibMemo n
    putStrLn $ "  Memoized:    " ++ show memoResult
    
    -- Stream approach
    let streamResult = fibStream !! fromInteger n
    putStrLn $ "  Stream:      " ++ show streamResult
    
    -- Properties
    let props = analyzeFibProperties n
    putStrLn $ "  Properties:  Even=" ++ show (isEven props) ++ 
               ", Digits=" ++ show (digitCount props)
    ) testNumbers
  
  putStrLn "\n🌿 Plant Spirit Enhancement Demonstration:"
  putStrLn "════════════════════════════════════════════"
  
  let testN = 30
  mapM_ (\strain -> do
    putStrLn $ "\nCalculating Fib(" ++ show testN ++ ") with " ++ show strain ++ " enhancement:"
    result <- plantSpiritFib testN strain
    putStrLn $ "  Result: " ++ show result
    ) strains
  
  putStrLn "\n💡 Haskell Educational Concepts Demonstrated:"
  putStrLn "  ✅ Pure Functions (no side effects)"
  putStrLn "  ✅ Lazy Evaluation (infinite sequences)"
  putStrLn "  ✅ Type Safety (compile-time guarantees)"
  putStrLn "  ✅ Pattern Matching (elegant case handling)"
  putStrLn "  ✅ List Comprehensions (mathematical notation)"
  putStrLn "  ✅ Maybe Types (safe null handling)"
  putStrLn "  ✅ Cannabis-Enhanced Mathematical Beauty"
  putStrLn ""

-- Randy's Performance Comparison
-- "Educational timing analysis with pure functions"
performanceComparison :: IO ()
performanceComparison = do
  putStrLn "🧠 Randy's Haskell Performance Analysis 🧠"
  putStrLn "═══════════════════════════════════════════"
  
  let testNumbers = [25, 30, 35]
  
  mapM_ (\n -> do
    putStrLn $ "\nPerformance Analysis for Fib(" ++ show n ++ "):"
    
    -- Memoized timing
    putStr "  Memoized:    "
    timeIt $ do
      let result = fibMemo n
      putStr $ show result
      
    -- Stream timing  
    putStr "  Stream:      "
    timeIt $ do
      let result = fibStream !! fromInteger n
      putStr $ show result
      
    putStrLn ""
    ) testNumbers
  
  putStrLn "\n🌿 Haskell Functional Wisdom:"
  putStrLn "  • Purity eliminates entire classes of bugs"
  putStrLn "  • Laziness enables infinite data structures"
  putStrLn "  • Type system catches errors at compile time"
  putStrLn "  • Pattern matching makes code self-documenting"
  putStrLn "  • Higher-order functions enable elegant composition"
  putStrLn "  • Cannabis enhancement reveals mathematical structure"
  putStrLn ""

-- Randy's Mathematical Exploration
-- "Deep mathematical analysis with functional programming"
mathematicalExploration :: IO ()
mathematicalExploration = do
  putStrLn "🔢 Fibonacci Mathematical Exploration:"
  putStrLn "════════════════════════════════════"
  
  -- First 20 Fibonacci numbers
  let sequence = take 20 fibStream
  putStrLn $ "First 20 Fibonacci numbers:"
  putStrLn $ show sequence
  
  -- Pattern analysis
  findFibPatterns 25
  
  -- Mathematical properties
  putStrLn "\n🧮 Mathematical Properties:"
  putStrLn "Every 3rd Fibonacci number is even"
  putStrLn "Every 4th Fibonacci number is divisible by 3"
  putStrLn "Every 5th Fibonacci number is divisible by 5"
  
  putStrLn "\n🌿 Cannabis-Enhanced Mathematical Insights:"
  putStrLn "  • Fibonacci spirals appear in cannabis leaf patterns"
  putStrLn "  • Golden ratio governs optimal plant growth angles"
  putStrLn "  • Recursive patterns mirror mycelial network structure"
  putStrLn "  • Functional programming captures natural mathematics"
  putStrLn ""

-- Randy's Complete Educational Demo
-- "Run all demonstrations for comprehensive learning"
runAllDemos :: IO ()
runAllDemos = do
  putStrLn "🧠⚡ Randy's Complete Haskell Functional Programming Education ⚡🧠"
  putStrLn ""
  
  cannabisEnhancedDemo
  performanceComparison
  mathematicalExploration
  
  putStrLn "🎓 Educational Achievement Unlocked:"
  putStrLn "   Students have mastered pure functional programming:"
  putStrLn "   • Mathematical purity with no side effects"
  putStrLn "   • Cannabis-enhanced lazy evaluation understanding"
  putStrLn "   • Type safety for bulletproof code"
  putStrLn "   • Pattern matching for elegant problem solving"
  putStrLn "   • Infinite data structures with lazy computation"
  putStrLn "   • Mathematical exploration through functional composition"
  putStrLn ""
  
  putStrLn "🌿 The mycelial network education revolution:"
  putStrLn "   Randy's curriculum spans Erlang, Elixir, and Haskell -"
  putStrLn "   the complete spectrum of functional programming from"
  putStrLn "   actor model concurrency to pure mathematical beauty!"
  putStrLn ""

-- Conditional import for when
import Control.Monad (when)

-- Main function for standalone execution
main :: IO ()
main = runAllDemos

{-
============================================================================
RANDY'S EDUCATIONAL HASKELL USAGE INSTRUCTIONS
============================================================================

Compilation and Execution:
$ ghc --make RandyCannabisFibonacci.hs
$ ./RandyCannabisFibonacci

Interactive GHCi Session:
$ ghci
ghci> :load RandyCannabisFibonacci.hs
ghci> cannabisEnhancedDemo
ghci> runAllDemos

Individual Function Testing:
ghci> fibMemo 30
ghci> plantSpiritFib 25 Sativa
ghci> take 15 fibStream
ghci> analyzeFibProperties 20

Cannabis-Enhanced Learning Exercises:
1. Compare lazy vs strict evaluation performance
2. Experiment with infinite Fibonacci stream
3. Explore type safety with Maybe types
4. Analyze mathematical patterns with list operations
5. Test pure function composition

Educational Concepts Mastered:

1. Pure Functions:
   - No side effects or mutable state
   - Referential transparency guarantees
   - Mathematical function behavior

2. Lazy Evaluation:
   - Computation only when needed
   - Infinite data structures possible
   - Memory-efficient stream processing

3. Type Safety:
   - Compile-time error detection
   - Maybe types for safe null handling
   - Strong static typing system

4. Pattern Matching:
   - Elegant case analysis
   - Destructuring data types
   - Guard expressions for conditions

5. List Comprehensions:
   - Mathematical notation in code
   - Concise data transformation
   - Functional data processing

Randy's Haskell Philosophy:
"Haskell represents the pinnacle of functional programming - every
computation is a mathematical equation, every function is pure, and
laziness enables infinite possibilities. Cannabis enhancement reveals
the profound beauty in mathematical abstraction made concrete through
type-safe code. The marriage of mathematical theory and practical
computation creates unprecedented clarity in problem solving."

Plant Spirit Functional Wisdom:
"Pure functions mirror the unchanging laws of nature - given the same
inputs, they always produce the same outputs, just like physical
constants. Lazy evaluation reflects how mycelial networks grow - only
expanding into new territories when resources are needed. The type
system acts like natural selection - only well-formed programs survive
compilation, ensuring robust computational ecosystems."

Complete Functional Programming Achievement:
Randy's education revolution now includes the trinity of functional
programming: Erlang for fault-tolerant concurrency, Elixir for modern
syntax elegance, and Haskell for mathematical purity. Students
experience the full spectrum of functional thinking with cannabis-
enhanced clarity across all paradigms!

The mycelial network education spans from imperative assembly language
through object-oriented frameworks to purely functional mathematics -
every approach to computational thinking explored with authentic
cannabis-enhanced wisdom!
============================================================================
-}