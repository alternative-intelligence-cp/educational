; ========================================================
; Tiny LISP Examples
; A playground for our beautiful, minimal LISP interpreter!
; ========================================================

; 1. Basic Arithmetic
(print (quote ---Basic-Arithmetic---))
(define r (+ 10 (* 2 5)))
(print (quote Result-of-10+2*5:))
(print r)
(print (quote ---))


; 2. Conditionals
(print (quote ---Conditionals---))
(define check_val 10)
(print (if (> check_val 5) (quote greater-than-five) (quote less-or-equal)))
(print (quote ---))


; 3. Lambda Expressions (Anonymous functions)
(print (quote ---Lambda-and-Scope---))
(define make-adder (lambda (n) (lambda (x) (+ x n))))
(define add-five (make-adder 5))
(define add-ten (make-adder 10))

(print (quote add-five-20->))
(print (add-five 20))
(print (quote add-ten-20->))
(print (add-ten 20))
(print (quote ---))


; 4. Recursion! (The real magic)
(print (quote ---Recursion:-Factorials---))

(define fact 
    (lambda (n) 
        (if (<= n 1) 
            1 
            (* n (fact (- n 1))))))

(print (quote Factorial-of-5:))
(print (fact 5))
(print (quote Factorial-of-10:))
(print (fact 10))
(print (quote ---))


; 5. List Processing (Car, Cdr, Cons)
(print (quote ---List-Processing---))

; Define a list of numbers
(define my-list (quote (1 2 3 4 5)))
(print (quote List:))
(print my-list)

; First item (car)
(print (quote First-Item-(car):))
(print (car my-list))

; Rest of the list (cdr)
(print (quote Rest-of-the-list-(cdr):))
(print (cdr my-list))

; Prepend (cons)
(print (quote Prepend-0-(cons):))
(print (cons 0 my-list))
(print (quote ---))


; 6. Higher-order functions: Map
(print (quote ---Higher-Order-Functions:-Map---))

; Map applies a function to every item in a list
(define map-list 
    (lambda (fn lst)
        (if (null? lst)
            (quote ())
            (cons (fn (car lst)) (map-list fn (cdr lst))))))

(define square (lambda (x) (* x x)))

(print (quote Square-every-item:))
(print (map-list square my-list))
