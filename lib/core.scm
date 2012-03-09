;
; core libaray for SC Scheme
; Charles Lee<zombiefml@gmail.com>
; 
; 2012-2-16, initial version
;


(define (reload-core)
  (load "lib/core.scm"))

; number functions
(define (number? x)
  (real? x))

(define (exact? x)
  (integer? x))

(define (inexact? x)
  (and (real? x)
       (not (exact? x))))

(define (inc n)
  (+ 1 n))

(define (dec n)
  (- n 1))

(define (zero? n)
  (= n 0))

(define (one? n)
  (= n 1))

(define (positive? x)
  (> x 0))

(define (negative? x)
  (< x 0))

(define (even? n)
  (if (integer? n)
    (zero? (remainder n 2))
    #f))

(define (odd? n)
  (not (even? n)))

(define (max x y . z)
  (reduce-left
    (lambda (x y)
      (if (> x y) x y))
    x
    (cons x (cons y z))))

(define (min x y . z)
  (reduce-left
    (lambda (x y)
      (if (> x y) y x))
    x
    (cons x (cons y z))))

(define (gcd a b)
  (if (= b 0)
    a
    (gcd b (remainder a b))))

(define (lcm a b)
  (let ((d (gcd a b)))
    (* a (quotient b d))))


; boolean functions
(define (false? b)
  (eq? b #f))

(define (not b)
  (if (false? b)
    #t
    #f))

(define (true? b)
  (not (false? b)))


; character procedures
(define (char=? a b . rest)
  (apply
    (lambda seq
      (apply = (map char->integer seq)))
    a b rest))

(define (char<? a b . rest)
  (apply
    (lambda seq
      (apply < (map char->integer seq)))
    a b rest))

(define (char>? a b . rest)
  (apply
    (lambda seq
      (apply > (map char->integer seq)))
    a b rest))

(define (char<=? a b . rest)
  (apply
    (lambda seq
      (apply <= (map char->integer seq)))
    a b rest))

(define (char>=? a b . rest)
  (apply
    (lambda seq
      (apply >= (map char->integer seq)))
    a b rest))

(define (char-ci=? a b . rest)
  (apply char=?
        (apply 
          (lambda seq
            (map char-upcase seq))
          a b rest)))

(define (char-ci<? a b . rest)
  (apply char<?
        (apply 
          (lambda seq
            (map char-upcase seq))
          a b rest)))

(define (char-ci>? a b . rest)
  (apply char>?
        (apply 
          (lambda seq
            (map char-upcase seq))
          a b rest)))

(define (char-ci>=? a b . rest)
  (apply char>=?
        (apply 
          (lambda seq
            (map char-upcase seq))
          a b rest)))

(define (char-ci<=? a b . rest)
  (apply char<=?
        (apply 
          (lambda seq
            (map char-upcase seq))
          a b rest)))

; io functions
(define (newline . args)
  (apply display
         #\newline args))

(define (open-input-file . args)
  (apply open-input-port args))

(define (open-output-file . args)
  (apply open-output-port args))

(define (call-with-input-file file proc)
  (let ((fin (open-input-port file)))
    (let ((result (proc fin)))
      (close-input-port fin)
      result)))

(define (call-with-output-file file proc)
  (let ((fout (open-output-port file)))
    (let ((result (proc fout)))
      (close-output-port fout)
      result)))

; list functions
(define (list? obj)
  (define (null-terminate? obj)
    (cond
      ((null? obj) #t)
      ((pair? obj) (null-terminate? (cdr obj)))
      (else #f)))
  (cond
    ((null? obj) #t)
    ((pair? obj) (null-terminate? obj))
    (else #f)))

(define (length seq)
  (define (iter x len)
    (if (null? x)
      len
      (iter (cdr x) (+ 1 len))))
  (if (list? seq)
      (iter seq 0)))

(define (append seq obj)
  (if (list? seq)
      (if (null? seq)
        obj
        (cons (car seq)
              (append (cdr seq) obj)))))

(define (reverse seq)
  (define (iter in out)
    (if (pair? in)
      (iter (cdr in) (cons (car in) out))
      out))
  (if (list? seq)
      (iter seq '())))

(define (map fn seq)
  (if (null? seq)
    '()
    (cons (fn (car seq))
          (map fn (cdr seq)))))

(define (for-each fn seq)
  (if (null? seq)
    #t
    (begin
      (fn (car seq))
      (for-each fn (cdr seq)))))

(define (list-tail seq k)
  (define (at-least-k? seq k len)
    (cond
      ((null? seq) #f)
      ((= k len) #t)
      ((= k 0) #t)
      (else (at-least-k? (cdr seq) k (+ len 1)))))
  (define (skip-n seq k len)
    (cond
      ((= k len) (cdr seq))
      ((= k 0) seq)
      (else (skip-n (cdr seq) k (+ len 1)))))
  (if (at-least-k? seq k 1)
    (skip-n seq k 1)))

(define (list-ref seq k)
  (define tail (list-tail seq k))
  (if (pair? tail)
    (car tail)))

(define (reduce-left fn initial seq)
  (define (reduce-iter fn val rest)
    (if (null? rest)
      val
      (reduce-iter
        fn
        (fn val (car rest))
        (cdr rest))))
  (cond
    ((null? seq) initial)
    ((null? (cdr seq)) (car seq))
    (else (reduce-iter
            fn
            (fn (car seq) (cadr seq))
            (cddr seq)))))
