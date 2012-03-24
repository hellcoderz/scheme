(display
    (call-with-current-continuation
      (lambda (exit)
        (for-each (lambda (x)
                    (if (negative? x)
                      (exit x)))
                  '(54 0 37 -3 245 19))
        #t)))
(newline)

(display
  (+ 1 (call/cc
         (lambda (k)
           (+ 2 (k 3))))))
(newline)

(define r #f)
(+ 1 (call/cc
       (lambda (k)
         (set! r k)
         (+ 2 (k 3)))))



