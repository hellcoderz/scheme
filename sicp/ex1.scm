(define (square x) (* x x))

(define (square-sum x y)
  (+ (square x)
     (square y)))

(define (f a b c)
  (cond
    ((and (<= a b) (<= a c)) (square-sum b c))
    ((and (<= b a) (<= b c)) (square-sum a c))
    ((and (<= c a) (<= c b)) (square-sum a b))))

(define (a-plus-abs-b a b)
  ((if (< b 0) - +) a b))

(define (infinite-loop) (infinite-loop))
; test returns 0 if the interpreter uses normal-order evaluation,
; trap into an infinite loop if the interpreter uses applicative-
; order evaluation.
(define (test x y)
  (if (= x 0)
    0
    y))
