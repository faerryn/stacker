: ack
  over 0= if
    swap drop 1+
  else
    dup 0= if
      drop 1- 1 ack
    else
      over swap 1- ack swap 1- swap ack
    then
  then ;

0 begin dup 3 < while
0 begin dup 6 < while
2dup ack . '\t' emit
1+ repeat drop cr
1+ repeat drop


bye
