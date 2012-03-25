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
(display 
  (+ 1 (call/cc
       (lambda (k)
         (set! r k)
         (+ 2 (k 3))))))
(newline)

(display
  (call/cc (lambda (cc)
             (display "I got here\n")
             (cc "This string is passed to the continuation\n")
             (display "But not here\n"))))

(let ((start #f))
  (if (not start)
    (call/cc (lambda (cc)
               (set! start cc))))
  (display "Going to call (start)\n")
  (start 'ok)) ; infinite loop



