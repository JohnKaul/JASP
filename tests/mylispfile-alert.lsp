
(defun sayHello ( str )
  (alert str) 
  )


; (defun SETTEST () (alert "Test Passed!"))

(defun myfoo ( / )
  (setq string1 "Hello"
        string2 " world!")
  (sayHello (strcat string1 string2))
  (princ)
 )

