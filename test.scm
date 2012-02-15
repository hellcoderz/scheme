(define (double n)
  (* 2 n))

(define (map f seq)
  (if (null? seq)
    '()
    (cons (f (car seq))
          (map f (cdr seq)))))

(map double '(1 2 3))

