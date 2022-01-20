: r@ r> dup >r ;

: -rot rot rot ;
: tuck swap over ;

: 2swap >r -rot r> -rot ;
: 2dup over over ;
: 2over >r >r 2dup >r >r 2swap ;
: 2drop drop drop ;

: 0= 0 = ;
: 0< 0 < ;
: 0> 0 > ;
: 1+ 1 + ;
: 1- 1 - ;
: 2+ 2 + ;
: 2- 2 - ;
: 2* 2 * ;
: 2/ 2 / ;

: true 0 invert ;
: false 0 ;

: ?dup dup dup 0= if drop then ;

: negate invert 1+ ;
: abs dup 0 < if negate then ;
: min 2dup > if swap then drop ;
: max 2dup < if swap then drop ;

: /rem 2dup / -rot rem ;
: /mod 2dup / -rot mod ;

: */ -rot * swap / ;
: */rem -rot * swap /rem ;
: */mod -rot * swap /mod ;

: ? @ . ;
: +! tuck @ + ! ;

: spaces begin dup 0> while ' ' emit 1- repeat ;
: cr '\n' emit ;

: type begin dup 0> while swap dup c@ emit 1+ swap 1- repeat drop drop ;
