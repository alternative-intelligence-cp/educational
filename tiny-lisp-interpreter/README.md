# Tiny LISP Interpreter

Welcome to the **Tiny LISP Interpreter**! 

If you want to truly understand how programming languages work under the hood, writing a LISP interpreter is a classical "rite of passage" in Computer Science. It demystifies parsing, execution, and memory, peeling back the magic of programming into a few elegant concepts.

## Why LISP?

LISP (List Processing) has a trivially simple syntax composed entirely of S-expressions (parenthesized lists). Because the syntax *is* the Abstract Syntax Tree (AST), we don't have to write complex parser grammars like we would for C or Python. We can focus purely on the beautiful concepts of **Lexical Scoping**, **Recursion**, and **Evaluation**.

## How it Works

The interpreter is entirely contained in `interpreter.py` and is divided into 5 simple phases:

1. **Lexical Analysis (`tokenize`)**:
   We take a string of source code and split it into tokens.
   `"(add 2 (mul 3 4))"` $\rightarrow$ `['(', 'add', '2', '(', 'mul', '3', '4', ')', ')']`

2. **Parsing (`parse`)**:
   We convert the flat list of tokens into a nested Python list that represents our AST.
   `['(', 'add', '2', '(', 'mul', '3', '4', ')', ')']` $\rightarrow$ `['add', 2, ['mul', 3, 4]]`

3. **Environments (`Env`)**:
   An environment is just a dictionary mapping variable names to their values (like `x=10` or `+ = operator.add`). By giving environments an `outer` reference to their parent scope, we instantly get Lexical Scoping and Closures!

4. **Evaluation (`evaluate`)**:
   A recursive function that takes an AST node and computes it. 
   - If it's a variable, we look it up in the `Env`.
   - If it's an `if` statement, we evaluate the test and then selectively evaluate the consequence or alternative.
   - If it's a function call, we recursively evaluate its arguments, and then call the function!

5. **REPL**:
   The Read-Eval-Print-Loop lets you interact with the interpreter live.

## Running the Code

You can start the interactive REPL:
```bash
./interpreter.py
```

Or you can run the included examples file to see features like Lambdas, Recursion, and Higher-Order functions in action:
```bash
./interpreter.py examples.lisp
```

## Challenge!
Try to expand the interpreter! Here are some fun exercises:
- Add support for string literals `"hello world"`.
- Implement `cond` for multi-branch conditionals.
- Add macro support!
