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

(define (random-in-range low high)
  (+ low (remainder (random) (- high low))))


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

(define (append! seq . rest)
  (reduce-right
    (lambda (x y)
      (cond
        ((null? x) y)
        (else (set-cdr! (last-pair x) y) x)))
    '()
    (cons seq rest)))


(define (reverse seq)
  (define (iter in out)
    (if (pair? in)
      (iter (cdr in) (cons (car in) out))
      out))
  (if (list? seq)
      (iter seq '())))

(define (map proc seq . rest)
  (define (map1 fn seq)
    (cond
      ((null? seq) '())
      (else (cons (fn (car seq))
                  (map1 fn (cdr seq))))))
  (cond
    ((null? seq) '())
    (else (cons (apply proc (map1 car (cons seq rest)))
                (apply map proc (cdr seq) (map1 cdr rest))))))

(define (append-map proc seq . rest)
  (apply append (apply map proc seq rest)))

(define (for-each proc seq . rest)
  (cond
    ((null? seq) #t)
    (else
      (apply proc (car seq) (map car rest))
      (apply for-each proc (cdr seq) (map cdr rest)))))

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

(define (find pred seq)
  (cond
    ((null? seq) #f)
    ((pred (car seq)) (car seq))
    (else (find pred (cdr seq)))))

(define (find-tail pred seq)
  (cond
    ((null? seq) #f)
    ((pred (car seq)) seq)
    (else (find-tail pred (cdr seq)))))

(define (member-procedure pred)
  (lambda (obj seq)
    (cond
      ((null? seq) #f)
      ((pred obj (car seq)) seq)
      (else (maker obj (cdr seq))))))

(define memq (member-procedure eq?))

(define memv (member-procedure eqv?))

(define member (member-procedure equal?))

(define (reduce-left fn initial seq)
  (define (reduce-iter val rest)
    (if (null? rest)
      val
      (reduce-iter
        (fn val (car rest))
        (cdr rest))))
  (cond
    ((null? seq) initial)
    ((null? (cdr seq)) (car seq))
    (else (reduce-iter
            (fn (car seq) (cadr seq))
            (cddr seq)))))

(define (reduce-right fn initial seq)
  (define (iter seq)
    (cond
      ((null? (cddr seq)) (fn (car seq)
                              (cadr seq)))
      (else (fn (car seq)
                (iter (cdr seq))))))
  (cond
    ((null? seq) initial)
    ((null? (cdr seq)) (car seq))
    (else (iter seq))))

(define (fold-left fn initial seq)
  (define (iter val rest)
    (cond
      ((null? rest) val)
      (else (iter (fn val (car rest))
                  (cdr rest)))))
  (cond
    ((null? seq) initial)
    (else (iter (fn initial (car seq))
                (cdr seq)))))

(define (fold-right fn initial seq)
  (define (iter seq)
    (cond
      ((null? (cdr seq)) (fn (car seq) initial))
      (else (fn (car seq)
                (iter (cdr seq))))))
  (cond
    ((null? seq) initial)
    (else (iter seq))))

(define (any pred seq . rest)
  (define (any1 pred seq)
    (if (null? seq) #f
      (let ((val (pred (car seq))))
        (if val val
          (any1 pred (cdr seq))))))
  (let ((args (cons seq rest)))
    (cond
      ((any1 null? args) #f)
      ((apply pred (map car args)) #t)
      (else (apply any pred (map cdr args))))))

(define (every pred seq . rest)
  (define (iter args last-val)
    (if (any null? args)
      last-val
      (let ((val (apply pred (map car args))))
        (if (not val)
          #f
          (iter (map cdr args) val)))))
  (iter (cons seq rest) #t))

(define (circular-list . objs)
  (cond
    ((null? objs)
     (let ((seq (list '())))
       (set-cdr! seq seq)
       seq))
    (else
      (let ((pair (last-pair objs)))
        (set-cdr! pair objs)
        pair))))

(define (quick-sort seq proc)
  (cond
    ((null? seq) '())
    ((null? (cdr seq)) seq)
    (else
      (let ((rnd (list-ref seq
                   (random-in-range 0 (length seq)))))
        (append
          (quick-sort (filter (lambda (e) (proc e rnd)) seq) proc)
          (filter (lambda (e) (not (or (proc e rnd) (proc rnd e)))) seq)
          (quick-sort (filter (lambda (e) (proc rnd e)) seq) proc))))))

(define (merge-sort seq proc)
  (define (merge x y)
    (cond
      ((null? x) y)
      ((null? y) x)
      ((proc (car x) (car y)) (cons (car x) (merge (cdr x) y)))
      ((proc (car y) (car x)) (cons (car y) (merge x (cdr y))))
      (else (cons (car x)
                  (cons (car y) (merge (cdr x) (cdr y)))))))
  (cond
    ((null? seq) '())
    ((null? (cdr seq)) seq)
    (else
      (let ((len (length seq)))
        (let ((mid (quotient len 2)))
          (merge
            (merge-sort (sublist seq 0 mid) proc)
            (merge-sort (sublist seq mid len) proc)))))))

(define sort merge-sort)

(define call/cc call-with-current-continuation)

