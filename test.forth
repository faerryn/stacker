: fib 0 1 rot begin dup 0> while -rot tuck + rot 1- repeat drop drop ;
: testfib 0 begin dup 10 < while dup fib . 1+ repeat cr ;
testfib

: fact dup 0= if drop 1 else dup 1- fact * then ;
: testfact 0 begin dup 10 < while dup fact . 1+ repeat cr ;
testfact

: ack
over 0= if swap drop 1+
else dup 0= if drop 1- 1 ack
else over swap 1- ack swap 1- swap ack
then then ;
: testack 3 3 ack . cr ;
testack

bye
