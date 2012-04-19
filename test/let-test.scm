(let* ((x 7)
       (y (+ x 1))
       (z (* x y)))
  (display-line x)
  (display-line y)
  (display-line z))

(letrec ((even?
           (lambda (n)
             (if (zero? n)
               #t
               (odd? (- n 1)))))
         (odd?
           (lambda (n)
             (if (zero? n)
               #f
               (even? (- n 1))))))
  (display-line (even? 88)))

