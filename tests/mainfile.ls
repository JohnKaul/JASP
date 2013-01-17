;#include        licenceheader.lh
;#ifndef USEALERT
    ;#error "USEALERT" has not been defined!
    ;; ;@ if the USEALERT hasnt been defined, stop processing
    ;#include print-line.lh
    ;#include print-string.lh
;#endif

;#ifdef USEALERT
;@ use this block if the USEALERT has been given.
(defun sayHello ( str )
  (alert str) 
  )
;#endif

;@define _TEST (defun SETTEST () (alert "Test Passed!"))
;@ this next line should be replaced with the above.
;@ _TEST

; MYTEST

(defun myfoo ( / )
  (setq string1 "Hello"
        string2 " world!")
  ;#ifdef USEALERT
  (sayHello (strcat string1 string2))
  ;#endif
  ;#ifndef USEALERT
  (print-line string1)
  (print-string string2)
  ;#endif
  (princ)
 )

;#include licenceheader.lh
