(define (gen-random-list n)
  (cond
    ((zero? n) '())
    (else (cons (random-in-range 0 n)
               (gen-random-list (dec n))))))

(define seq (gen-random-list 10000))

(define (timeit)
  (let ((start (runtime)))
    (display (apply <= (quick-sort seq <)))
    (newline)
    (display (- (runtime) start))
    (newline)))

(timeit)
