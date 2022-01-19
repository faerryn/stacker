: fact dup 0= if drop 1 else dup 1- fact * then ;
: fib 0 1 rot begin dup 0> while -rot tuck + rot 1- repeat drop drop ;

: cr '\n' emit ;

variable num
20 num !
: NUM num @ ;

NUM fact . cr
NUM fib . cr

0 begin dup NUM < while dup fact . 1+ repeat cr
0 begin dup NUM < while dup fib  . 1+ repeat cr