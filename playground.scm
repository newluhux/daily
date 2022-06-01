(define vhello "Hello world")
(define fhello (lambda ()
		 "Hello World"))

(define hello
  (lambda (name)
    (string-append "Hello " name "!")))

(define sum3
  (lambda(a b c)
    (+ a b c)))

(define (hello s)
  (string-append "Hello" s "!"))

(define (inc n)
  (+ n 1))

(define (dec n)
  (- n 1))

(define cheng
  (lambda (a b c)
    (if (and (> a 0) (> b 0) (> c 0))
	(* a b c)
	#f)))

(define c
  (lambda (a b c)
    (cond
     ((not (null? a)) "a")
     ((not (null? b)) "b")
     ((not (null? c)) "c")
     (else "nop"))))

