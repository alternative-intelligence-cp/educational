#!/usr/bin/env python3
"""
Tiny LISP Interpreter

An educational interpreter showcasing the fundamental concepts of programming language design:
1. Lexical Analysis (Tokenizing)
2. Parsing (Building an Abstract Syntax Tree)
3. Environments (Scoping and Variables)
4. Evaluation (Recursively computing the AST)
"""

import math
import operator as op
import sys

# =====================================================================
# 1. Lexical Analysis (Tokenizing)
# =====================================================================
def tokenize(chars: str) -> list:
    """
    Convert a string of characters into a list of tokens.
    
    LISP syntax uses S-expressions, which are heavily reliant on parentheses. 
    By simply adding spaces around parentheses, we can split the string by whitespace 
    and get a perfect list of tokens!
    """
    # E.g., "(add 2 (mul 3 4))" -> ["(", "add", "2", "(", "mul", "3", "4", ")", ")"]
    return chars.replace('(', ' ( ').replace(')', ' ) ').split()


# =====================================================================
# 2. Parsing (Building an AST)
# =====================================================================
def parse(tokens: list):
    """
    Read a sequence of tokens and construct an Abstract Syntax Tree (AST).
    
    The AST is represented using native Python nested lists.
    For example: ["+", 1, ["*", 2, 3]]
    """
    if len(tokens) == 0:
        raise SyntaxError('unexpected EOF')
    
    token = tokens.pop(0)
    
    if token == '(':
        # It's a list. Recursively parse all inner tokens until we hit ')'
        L = []
        while tokens[0] != ')':
            L.append(parse(tokens))
        tokens.pop(0) # pop off ')'
        return L
    elif token == ')':
        raise SyntaxError('unexpected )')
    else:
        # It's an atom (a number or a symbol)
        return atom(token)

def atom(token: str):
    """Numbers become integers/floats; everything else is a symbol (string)."""
    try: 
        return int(token)
    except ValueError:
        try: 
            return float(token)
        except ValueError:
            return str(token)


# =====================================================================
# 3. Environments
# =====================================================================
class Env(dict):
    """
    An environment is a dictionary of {'var': val} pairs, with a reference 
    to an `outer` environment. This allows for lexical scoping! 
    If a variable isn't found in the local scope, we check the outer scope.
    """
    def __init__(self, params=(), args=(), outer=None):
        self.update(zip(params, args))
        self.outer = outer
        
    def find(self, var):
        """Find the innermost Env where the variable appears."""
        if var in self:
            return self
        elif self.outer is not None:
            return self.outer.find(var)
        else:
            raise NameError(f"unbound variable '{var}'")

def standard_env() -> Env:
    """An environment loaded with some standard procedures."""
    env = Env()
    env.update(vars(math)) # sin, cos, sqrt, pi, ...
    env.update({
        '+':op.add, '-':op.sub, '*':op.mul, '/':op.truediv, 
        '>':op.gt, '<':op.lt, '>=':op.ge, '<=':op.le, '=':op.eq, 
        'abs':     abs,
        'append':  op.add,  
        'apply':   lambda proc, args: proc(*args),
        'begin':   lambda *x: x[-1],
        'car':     lambda x: x[0] if x else [],
        'cdr':     lambda x: x[1:] if x else [], 
        'cons':    lambda x,y: [x] + y,
        'eq?':     op.is_, 
        'expt':    pow,
        'equal?':  op.eq, 
        'length':  len, 
        'list':    lambda *x: list(x), 
        'list?':   lambda x: isinstance(x, list), 
        'map':     map,
        'max':     max,
        'min':     min,
        'not':     op.not_,
        'null?':   lambda x: x == [], 
        'number?': lambda x: isinstance(x, (int, float)),  
        'print':   print,
        'procedure?': callable,
        'round':   round,
        'symbol?': lambda x: isinstance(x, str),
    })
    return env

global_env = standard_env()

class Procedure:
    """A user-defined Scheme procedure (Closure)."""
    def __init__(self, params, body, env):
        self.params, self.body, self.env = params, body, env
    def __call__(self, *args): 
        # When called, we evaluate the body in a NEW environment
        # that binds the parameters to the arguments, mapped to the parent scope!
        return evaluate(self.body, Env(self.params, args, self.env))


# =====================================================================
# 4. Evaluation
# =====================================================================
def evaluate(x, env=global_env):
    """
    Evaluate an AST expression in an environment.
    This function is recursive, mimicking how LISP intrinsically computes values.
    """
    if isinstance(x, str):        # variable reference
        return env.find(x)[x]
    elif not isinstance(x, list): # constant (number)
        return x
    
    op_code, *args = x
    
    if op_code == 'quote':        # (quote exp) -> raw list, do not evaluate
        return args[0]
    elif op_code == 'if':         # (if test conseq alt)
        (test, conseq, alt) = args
        exp = (conseq if evaluate(test, env) else alt)
        return evaluate(exp, env)
    elif op_code == 'define':     # (define symbol exp)
        (symbol, exp) = args
        env[symbol] = evaluate(exp, env)
    elif op_code == 'set!':       # (set! symbol exp)
        (symbol, exp) = args
        env.find(symbol)[symbol] = evaluate(exp, env)
    elif op_code == 'lambda':     # (lambda (var...) body) -> Procedure
        (params, body) = args
        return Procedure(params, body, env)
    else:                         # (proc arg...) -> Procedure call
        # Evaluate the procedure name
        proc = evaluate(op_code, env)
        # Recursively evaluate all arguments
        vals = [evaluate(arg, env) for arg in args]
        # Call the procedure
        return proc(*vals)


# =====================================================================
# 5. REPL & Output formatting
# =====================================================================
def lispstr(exp):
    """Convert a Python object back into a LISP-readable string."""
    if isinstance(exp, list):
        return '(' + ' '.join(map(lispstr, exp)) + ')'
    else:
        return str(exp)

def repl(prompt='lispy> '):
    """A prompt-read-eval-print loop."""
    print("Welcome to Tiny LISP Interpreter!")
    print("Type Ctrl+C or Ctrl+D to exit.")
    while True:
        try:
            line = input(prompt)
            if not line.strip(): continue
            val = evaluate(parse(tokenize(line)))
            if val is not None: 
                print(lispstr(val))
        except (KeyboardInterrupt, EOFError):
            print("\nGoodbye!")
            break
        except Exception as e:
            print(f"Error: {e}")

def remove_comments(code: str) -> str:
    lines = code.split('\n')
    cleaned = []
    for line in lines:
        if ';' in line:
            line = line.split(';', 1)[0]
        cleaned.append(line)
    return '\n'.join(cleaned)

def run_file(filepath):
    """Load and execute a .lisp file."""
    with open(filepath, 'r') as f:
        code = f.read()
    
    code = remove_comments(code)
    
    # We wrap multiple top-level expressions in a (begin ...) block
    # so it forms a single valid AST.
    code = f"(begin {code})"
    try:
        val = evaluate(parse(tokenize(code)))
        if val is not None:
            print(lispstr(val))
    except Exception as e:
        print(f"Error running {filepath}: {e}")

if __name__ == '__main__':
    if len(sys.argv) > 1:
        run_file(sys.argv[1])
    else:
        repl()
