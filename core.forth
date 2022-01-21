: -rot rot rot ;
: tuck swap over ;

: 2swap >r -rot r> -rot ;
: 2dup over over ;
: 2over >r >r 2dup >r >r 2swap ;
: 2drop drop drop ;

: true 0 invert ;
: false 0 ;

: ?dup dup dup 0= if drop then ;

: negate invert 1 + ;
: abs dup 0 < if negate then ;
: min 2dup > if swap then drop ;
: max 2dup < if swap then drop ;

: /rem 2dup / -rot rem ;
: /mod 2dup / -rot mod ;

: */ -rot * swap / ;
: */rem -rot * swap /rem ;
: */mod -rot * swap /mod ;

: _.digit '0' + emit ;
: _.num dup 10 < if _.digit else 10 /rem swap _.num _.digit then ;
: . dup 0 < if '-' emit negate then _.num ' ' emit ;

: ? @ . ;
: +! tuck @ + ! ;

: spaces begin dup 0 > while ' ' emit 1 - repeat ;
: cr '\n' emit ;

: type
  begin
    dup 0 >
  while
    swap dup c@ emit 1 + swap 1 -
  repeat
  drop drop ;
: accept
  0
  begin
    2dup >
  while
    key
    dup '\n' <>
    if
      >r rot 2dup + r> swap c! -rot 1 +
    else
      drop swap drop dup
    then
  repeat
  -rot drop drop ;

: _debug over swap type free .s cr ;

: fill
  begin
    swap dup 0 >
    rot swap
  while
    rot 2dup c!
    1 +
    rot 1 -
    rot
  repeat
  drop drop drop ;
