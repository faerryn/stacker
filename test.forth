: fib dup 1 < if dup 1 swap - fib swap 2 swap - fib + then ;

: tuck swap over ;
: -rot rot rot ;
: fib2 0 1 rot begin dup 0 < while -rot tuck + rot 1 swap - repeat drop drop ;

: cr 10 emit ;

10 begin dup . 1 swap - dup 0 = until cr
10 begin - dup 0 < while dup . 1 swap repeat cr

10 fib . cr
10 fib2 . cr

10 begin dup 10 - fib . 1 swap - dup 0 = until cr
10 begin dup 10 - fib2 . 1 swap - dup 0 = until cr

1 if 84 emit else 70 emit then cr
