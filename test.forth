: fib dup 1 > if dup 1- fib swap 2- fib + then ;
: fib2 0 1 rot begin dup 0> while -rot tuck + rot 1- repeat drop drop ;

: cr '\n' emit ;

variable num
30 num !

num @ begin dup . 1- dup 0= until cr
num @ begin dup 0> while dup . 1- repeat cr

num @ fib . cr
num @ fib2 . cr

num @ begin dup num @ swap - fib  . 1- dup 0 = until cr
num @ begin dup num @ swap - fib2 . 1- dup 0 = until cr
