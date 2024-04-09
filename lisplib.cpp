// liblisp

const char *lisplib =
"\
(defun append (x y)\n\
   (cond ((null x) y)\n\
         (t (cons (car x) (append (cdr x) y)))\n\
   ) )\n\
\n\
(defun member (a lat)\n\
   (cond ((null lat) nil)\n\
         ((eq (car lat) a) t)\n\
         (t (member a (cdr lat)))\n\
   ) )\n\
\n\
(defun eqn (n1 n2)\n\
   (cond ((zerop n2) (zerop n1))\n\
         ((zerop n1) nil)\n\
         (t (eqn (sub1 n1) (sub1 n2)))\n\
   ) )\n\
\n\
(defun eqan (a1 a2)\n\
   (cond ((and (numberp a1) (numberp a2)) (eqn a1 a2))\n\
         ((or (numberp a1) (numberp a2)) nil)\n\
         (t (eq a1 a2))\n\
   ) )\n\
\n\
(defun equal (s1 s2)\n\
   (cond ((atom x) (eq x y))\n\
         ((atom y) nil)\n\
         ((equal (car x) (car y)) (equal (cdr x) (cdr y)))\n\
         (t nil)\n\
   ) )\n\
\n\
(defun equal2 (s1 s2)\n\
   (cond ((and (not (atom s1)) (not (atom s2)))\n\
              (and (equal2 (car s1) (car s2)) (equal2 (cdr s1) (cdr s2))))\n\
          ((and (atom s1) (atom s2)) (eqan s1 s2))\n\
          (t nil)\n\
   ) )\n\
\n\
(defun subst (old new lat)\n\
   (cond ((null lat) ())\n\
         ((eq (car lat) old) (cons new (cdr lat)))\n\
         (t (cons (car lat) (subst old new (cdr lat))))\n\
   )  )\n\
\n\
(defun length (lat)\n\
   (cond ((null lat) 0)\n\
         (t (add1 (length (cdr lat))))\n\
   ) )\n\
\n\
(defun intersect (set1 set2)\n\
   (cond ((null set1) ())\n\
     ((member (car set1) set2) (cons (car set1) (intersect (cdr set1) set2)))\n\
     (t (intersect (cdr set1) set2))\n\
   ) )\n\
\n\
(defun mapcar (fn x)\n\
   (cond ((null x) nil)\n\
         (t (cons (funcall fn (car x)) (mapcar fn (cdr x))))\n\
   ) )\n\
\n\
(defun maplist (fn x)\n\
   (cond ((null x) nil)\n\
         (t (cons (funcall fn x) (maplist fn (cdr x))))\n\
   ) )\n\
\n\
(defun mapc (fn x)\n\
 (prog ()\n\
a  (cond ((atom x) (return x)))\n\
   (funcall fn (car x))\n\
   (setq x (cdr x))\n\
   (go a)\n\
 )\n\
)\n\
\n\
(defun map (fn x)\n\
 (prog ()\n\
   loop (cond ((atom x) (return x))\n\
              ((null x) (return nil))\n\
        )\n\
        (funcall fn x)\n\
        (setq x (cdr x))\n\
        (go loop)\n\
 )\n\
)\n\
";