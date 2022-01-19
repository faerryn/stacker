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

: ?dup dup dup 0= if drop then ;

: negate 0 - ;
: abs dup 0 < if negate then ;
: min 2dup > if swap then drop ;
: max 2dup < if swap then drop ;

: /rem 2dup / 2swap rem ;
: /mod 2dup / 2swap mod ;

: */ -rot * / ;
: */mod -rot * /mod ;

: ? @ . ;
: +! tuck @ + ! ;
