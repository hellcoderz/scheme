(define (eat-whitespace in)
  (let ((c (peek-char in)))
    (cond
      ((eof-object? c) c)
      ((char-whitespace? c)
         (read-char in)
         (eat-whitespace in))
      (else c))))

(define (read-word in)
  (define (iter in)
    (let ((c (peek-char in)))
      (if (char-alphabetic? c)
        (cons (read-char in)
              (iter in))
        '())))
  (let ((c (eat-whitespace in)))
    (if (eof-object? c)
      c
      (list->string (iter in)))))

(define (read-periodic-table file)
  (define (build in)
    (let ((abbr (read-word in))
          (full-name (read-word in)))
      (if (or (eof-object? abbr)
              (eof-object? full-name))
        '()
        (cons (make-entry abbr full-name)
              (build in)))))
  (let ((in (open-input-port file)))
    (let ((table (build in)))
        (close-input-port in)
        table)))

(define (make-entry abbr full-name)
  (list abbr full-name))

(define (first-entry table)
  (car table))

(define (rest-entries table)
  (cdr table))

(define (entry-abbr entry)
  (car entry))

(define (entry-full-name entry)
  (cadr entry))

(define (truncate-word word start)
  (let ((end (string-length word)))
    (substring word start end)))

(define (first-n word len)
  (if (> len (string-length word))
    ""
    (substring word 0 len)))

(define (string-empty? str)
  (string=? str ""))

(define (periodic-word? table word)
  (define (iter entries)
    (if (null? entries)
      #f
      (let ((abbr (entry-abbr (first-entry entries))))
        (if (string-ci=? abbr (first-n word (string-length abbr)))
          (or (periodic-word? table
                            (truncate-word word (string-length abbr)))
              (iter (rest-entries entries)))
          (iter (rest-entries entries))))))
  (if (string-empty? word)
    #t
   (iter table)))

(define (read-dict-word in)
  (define (iter in)
    (let ((c (peek-char in)))
      (if (not (char-whitespace? c))
        (cons (read-char in)
              (iter in))
        '())))
  (let ((c (eat-whitespace in)))
    (if (eof-object? c)
      c
      (list->string (iter in)))))

(define (dict-periodic-percentage table dict)
  (define (iter total periodic in)
    (let ((word (read-dict-word in)))
      (cond
        ((eof-object? word) 
         (write periodic) (write ",") (write total) (write ",") (/ periodic total))
        (else
          (let ((i (if (periodic-word? table word) 1 0)))
            (iter (inc total)
                  (+ periodic i)
                  in))))))
  (let ((in (open-input-port dict)))
    (let ((percentage (iter 0 0 in)))
      (close-input-port in)
      percentage)))

(define (timeit fn . args)
  (let ((start (runtime)))
    (write (apply fn args))
    (write ",")
    (write (- (runtime) start))))

(define (runtest periodic-table dict)
  (let ((tbl (read-periodic-table periodic-table)))
    (timeit dict-periodic-percentage tbl dict)))

(runtest "periodic/periodic-table" "periodic/words")
