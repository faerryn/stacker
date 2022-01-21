: 3dup >r 2dup r@ -rot r> ;
: 3drop drop drop drop ;

: star '*' ;

: r110
  star = >r
  star = >r
  star = >r
  r> r> r>

  3dup invert and and >r
  3dup swap invert and and >r
  3dup rot invert and and >r
  3dup swap invert or or invert >r
  3dup invert or or invert >r

  3drop
  r> r> r> r> r> or or or or

  if star else ' ' then
;

: r110map
  1 -
  begin
    dup 1 >
  while
    rot 2dup +
    dup 1 + c@ >r
    dup c@ >r
    dup 1 - c@ >r
    drop

    -rot 2dup +
    r> r> r> r110
    swap c!

    1 -
  repeat
  drop drop drop ;

: r110scroller
  dup alloc 2dup swap ' ' fill
  swap dup alloc 2dup swap ' ' fill
  2dup + 2 - star swap c!
  swap
  begin
    2dup type
    rot swap 3dup r110map
    key '\n' <>
  until
  cr
  drop free free ;

256 r110scroller

bye
