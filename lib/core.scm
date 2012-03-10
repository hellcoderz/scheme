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
  (cond
    ((null? obj) #t)
    ((pair? obj) (list? (cdr obj)))
    (else #f)))

(define (length seq)
  (define (iter x len)
    (if (null? x)
      len
      (iter (cdr x) (+ 1 len))))
  (if (list? seq)
      (iter seq 0)))

(define (append seq . rest)
  (define (append-two x y)
    (cond
      ((null? x) y)
      (else (cons (car x)
                  (append-two (cdr x) y)))))
  (reduce-left append-two '() (cons seq rest)))


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

(define (list-copy seq)
  (cond
    ((null? seq) '())
    (else (cons (car seq) (list-copy (cdr seq))))))

(define (dotted-list? seq)
  (cond
    ((null? seq) #f)
    ((not (pair? seq)) #t)
    (else (dotted-list? (cdr seq)))))

(define (circular-list? seq)
  (define (two-more? seq)
    (cond
      ((null? seq) #f)
      ((null? (cdr seq)) #f)
      ((null? (cddr seq)) #f)
      (else #t)))
  (define (iter hare tortoise)
    (cond
      ((null? hare) #f)
      ((not (two-more? hare)) #f)
      ((eq? (car hare) (car tortoise)) #t)
      (else (iter (cddr hare) (cdr tortoise)))))
  (cond
    ((two-more? seq) (iter (cddr seq) seq))
    (else #f)))

(define (sublist seq start end)
  (define (iter seq i)
    (cond
      ((null? seq) '())
      ((< i start) (iter (cdr seq) (inc i)))
      ((>= i end) '())
      (else (cons (car seq)
                  (iter (cdr seq) (inc i))))))
  (cond
    ((< start 0) '())
    ((> start end) '())
    ((> end (length seq)) '())
    (else (iter seq 0))))

(define (list-head seq k)
  (sublist seq 0 k))

(define (last-pair seq)
  (cond
    ((not (pair? seq)) seq)
    ((pair? (cdr seq)) (last-pair (cdr seq)))
    (else seq)))

(define (except-last-pair seq)
  (cond
    ((not (pair? seq)) seq)
    ((not (pair? (cdr seq))) '())
    (else (cons (car seq) (except-last-pair (cdr seq))))))

(define (filter pred seq)
  (cond
    ((null? seq) '())
    ((pred (car seq)) (cons (car seq)
                            (filter pred (cdr seq))))
    (else (filter pred (cdr seq)))))

(define (remove pred seq)
  (cond
    ((null? seq) '())
    ((pred (car seq)) (remove pred (cdr seq)))
    (else (cons (car seq)
                (remove pred (cdr seq))))))

(define (partition pred seq)
  (list
    (filter pred seq)
    (remove pred seq)))

(define (delq elem seq)
  (remove (lambda (obj) (eq? obj elem)) seq))

(define (delv elem seq)
  (remove (lambda (obj) (eqv? obj elem)) seq))

(define (delete elem seq)
  (remove (lambda (obj) (equal? obj elem)) seq))

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
