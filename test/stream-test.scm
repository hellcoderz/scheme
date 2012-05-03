(load-lib "stream")

(define stream-123
  (stream-cons
    1
    (stream-cons
      2
      (stream-cons
        3
        (stream-null)))))

(display-line (stream-car stream-123))
(display-line (stream-car (stream-cdr stream-123)))
(display-line (stream-car (stream-cdr (stream-cdr stream-123))))

(define (iter f x)
    (stream-cons x (iter f (f x))))

(define nats
  (iter (lambda (x) (+ 1 x)) 0))

(define (stream-add a b)
  (stream-cons
    (+ (stream-car a) (stream-car b))
    (stream-add (stream-cdr a) (stream-cdr b))))

(define evens
  (stream-add nats nats))

(display-line (stream-car evens))
(display-line (stream-car (stream-cdr (stream-cdr evens))))

(define s (stream 1 (/ 1 0) 2 4 (+ 89 9)))
(display-line (stream-car s))
(display-line (stream-car (stream-cdr (stream-cdr (stream-cdr  (stream-cdr s))))))
; (display-line (stream-car (stream-cdr s)))

(define s (stream 1 (begin (display-line "only once") 2) (/ 200 0)))
(display-line (stream-car (stream-cdr s)))
(display-line (stream-car (stream-cdr s)))

(define s
    (let ((a 100) (b 2))
      (stream a b)))
(display-line (stream-car s))

(define odds (stream-map (lambda (x) (- x 1)) evens))
(display-line (stream-car (stream-cdr (stream-cdr odds))))

(display-line (stream-ref odds 100))
(display-line (stream-ref evens 10000))
(display-line (stream-ref evens 100000))
