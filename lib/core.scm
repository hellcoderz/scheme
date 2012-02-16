;
; core libaray for SC Scheme
; Charles Lee<zombiefml@gmail.com>
; 
; 2012-2-16, initial version
;

; NOTES:
;
; due to implementation limitations, SC Scheme does not support
; var-args, many functions are thus simplified to take the least
; amount of arguments.
;

; number functions
(define (number? x)
  (integer? x))

(define (inc n)
  (+ 1 n))

(define (dec n)
  (- n 1))

(define (zero? n)
  (eq? n 0))


; boolean functions
(define (false? b)
  (eq? b #f))

(define (not b)
  (if (false? b)
    #t
    #f))

(define (true? b)
  (not (false? b)))


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

