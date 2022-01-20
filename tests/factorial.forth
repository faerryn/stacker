: fact dup 0= if drop 1 else dup 1- fact * then ;
: test 0 begin dup 10 < while dup fact . 1+ repeat cr ;
test
bye

