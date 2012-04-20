(display-line
  (macroexpand-1
    '(do ((vec (make-vector 5))
          (i 0 (+ i 1)))
       ((= i 5) vec)
       (vector-set! vec i i))))

(display-line
    (do ((vec (make-vector 5))
          (i 0 (+ i 1)))
       ((= i 5) vec)
       (vector-set! vec i i)))

(display-line
   (let ((x '(1 3 5 7 9)))
      (macroexpand-1
        '(do ((x x (cdr x))
              (sum 0 (+ sum (car x))))
             ((null? x) sum)))))

(display-line
    (let ((x '(1 3 5 7 9)))
        (do ((x x (cdr x))
              (sum 0 (+ sum (car x))))
             ((null? x) sum))))

