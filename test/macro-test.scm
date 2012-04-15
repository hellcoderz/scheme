(define-macro when
              (lambda (test . branch)
                (list 'if test (cons 'begin branch))))
(define test 1)
(when (< test 89)
  (display-line "line 1")
  (display-line "line 2")
  (display-line "done"))

(when (< test 0)
  (display-line "never here")
  (display-line "never here 2"))

(define-macro unless
  (lambda (test . branch)
    (cons 'when
          (cons (list 'not test) branch))))

(unless (< test 89)
  (display-line "never here")
  (display-line "never here 2"))

(unless (< test 0)
  (display-line "line 1")
  (display-line "line 2")
  (display-line "done"))


(define-macro my-or-wrong
   (lambda (x y)
     (list 'if x x y)))
(display-line (my-or-wrong 1 2))
(display-line (my-or-wrong #f 2))
(my-or-wrong
  (begin
    (display-line "doing first argument")
    #t)
  2)

(define-macro
  my-or-wrong2
  (lambda (x y)
    (list 'let (list (list 'temp x))
          (list 'if 'temp 'temp y))))
(my-or-wrong2
  (begin
    (display-line "2doing first argument")
    #t)
  2)
(define temp 3)
(display-line (my-or-wrong2 #f temp))

(define-macro
  my-or
  (lambda (x y)
    (let ((temp (gensym)))
      (list 'let (list (list temp x))
            (list 'if temp temp y)))))
(display-line (my-or #f temp))

