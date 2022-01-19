: fib dup 1 < if dup 1 switch - fib switch 2 switch - fib + then ;

: tuck switch over ;
: -rot rot rot ;
: fib2 0 1 rot begin dup 0 < while -rot tuck + rot 1 switch - repeat drop drop ;

: cr 10 emit ;

10 begin dup . 1 switch - dup 0 = until cr
10 begin - dup 0 < while dup . 1 switch repeat cr

10 fib . cr
10 fib2 . cr

10 begin dup 10 - fib . 1 switch - dup 0 = until cr
10 begin dup 10 - fib2 . 1 switch - dup 0 = until cr
