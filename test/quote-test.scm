(display-line `(list ,(+ 1 2) 4))

(display-line
  (let ((name 'a))
    `(list ,name ',name)))

(display-line `(a ,(+ 1 2) ,@(map abs '(4 -5 6)) b))

(display-line `((foo ,(- 10 3)) ,@(cdr '(c)) . ,(car '(cons))))

(display-line `(10 5 ,(sqrt 4) ,@(map sqrt '(16 9)) 8))

(display-line `(a `(b ,(+ 1 2) ,(foo ,(+ 1 3) d) e) f))

(display-line
  (let ((name1 'x)
        (name2 'y))
    `(a `(b ,,name1 ,',name2 d) e)))

