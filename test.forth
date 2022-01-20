: fib 0 1 rot begin dup 0> while -rot tuck + rot 1- repeat drop drop ;
: testfib 0 begin dup 10 < while dup fib . 1+ repeat cr ;
testfib

: fact dup 0= if drop 1 else dup 1- recurse * then ;
: testfact 0 begin dup 10 < while dup fact . 1+ repeat cr ;
testfact
bye