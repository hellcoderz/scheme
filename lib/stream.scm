(define stream-null
  (delay (cons 'stream 'null)))

(define (stream-null? stream)
  (eq? (force stream-null)
       (force stream)))

(define-macro
  (stream-cons obj stream)
      `(delay (cons (delay ,obj) (delay ,stream))))

(define (stream-car stream)
  (force (car (force stream))))

(define (stream-cdr stream)
  (force (cdr (force stream))))

(define-macro
  (stream . objs)
     (fold-right (lambda (a b) `(stream-cons ,a ,b))
                  `stream-null
                  objs))

(define (stream-map proc . streams)
  (define (stream-map streams)
    (if (any stream-null? streams)
      stream-null
      (stream-cons (apply proc (map stream-car streams))
                   (stream-map (map stream-cdr streams)))))
  (stream-map streams))

(define (stream-ref stream n)
  (if (zero? n)
    (stream-car stream)
    (stream-ref (stream-cdr stream) (- n 1))))
