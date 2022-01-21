: fib 0 1 rot begin dup 0> while -rot tuck + rot 1- repeat drop drop ;
0 begin dup 10 < while dup fib . 1+ repeat drop cr

bye
