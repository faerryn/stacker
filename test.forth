: fib dup 1 > if dup 1- fib swap 2- fib + then ;
: fib2 0 1 rot begin dup 0> while -rot tuck + rot 1- repeat drop drop ;

: cr 10 emit ;

10 begin dup . 1- dup 0= until cr
10 begin dup 0> while dup . 1- repeat cr

10 fib . cr
10 fib2 . cr

10 begin dup 10 swap - fib . 1- dup 0 = until cr
10 begin dup 10 swap - fib2 . 1- dup 0 = until cr
