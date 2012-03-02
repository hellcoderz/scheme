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

(define (sqrt-iter guess x)
  (if (good-enough? guess x)
    guess
    (sqrt-iter (improve guess x)
               x)))

(define (improve guess x)
  (average guess (/ x guess)))

(define (average x y)
  (/ (+ x y) 2))

(define (good-enough? guess x)
  (< (abs (- (square guess) x)) 0.0000000001))

(define (square x)
  (* x x))

(define (square-root x)
  (sqrt-iter 1.0 x))

(define (another-good-enough? guess x)
  (let ((next (improve guess x)))
    (< (abs (- guess next)) 0.000000001)))

(define (double x) (+ x x))

(define (cube-root x)
  (define (cube-root-iter guess x)
    (if (good-enough? guess x)
      guess
      (cube-root-iter (improve guess x) x)))
  (define (improve x y)
    (/ (+ (/ y (square x))
          (double x))
       3))
  (define (good-enough? guess x)
    (let ((next (improve guess x)))
      (< (abs (- guess next)) 0.0000000001)))
  (cube-root-iter 1.0 x))

(define (A x y)
  (cond ((= y 0) 0)
        ((= x 0) (* 2 y))
        ((= y 1) 2)
        (else (A (- x 1)
                 (A x (- y 1))))))


(define (f n)
  (cond ((< n 3) n)
        (else (+ (f (- n 1))
                 (* 2 (f (- n 2)))
                 (* 3 (f (- n 3)))))))
(define (f-iter n)
  (define (iter k n a b c)
    (cond ((< n 3) n)
          ((> k n) a)
          (else (iter (+ k 1)
                      n
                      (+ a
                         (* b 2)
                         (* c 3))
                      a
                      b))))
  (iter 3 n 2 1 0))

(define (pascal r c)
  (cond ((= c 1) 1)
        ((= c r) 1)
        (else (+ (pascal (- r 1) (- c 1))
                 (pascal (- r 1) c)))))

(define (fast-expt b n)
  (define (iter b n ret)
    (cond ((zero? n) ret)
          ((even? n) (iter (* b b) (quotient n 2) ret))
          (else (iter b (- n 1) (* ret b)))))
  (iter b n 1))

