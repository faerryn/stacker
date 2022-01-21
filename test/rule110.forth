: star '*' ;
: space ' ' ;

: 3dup >r 2dup r@ -rot r> ;

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

  drop drop drop
  r> r> r> r> r> or or or or

  if star else space then
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
  rot swap
  begin
    2dup type
    rot swap
    3dup r110map
    key '\n' <>
  until
  cr
  drop drop drop ;

dup 1 =
if
  64
  dup alloc swap 2dup space fill
  dup alloc swap 2dup space fill
  2dup + 2 - star swap c!
  rot swap

  3dup
  r110scroller
  drop free free
else
  dup 2 =
  if
    drop drop drop
    dup alloc swap 2dup space fill

    3dup
    r110scroller
    drop free
  else
    "USAGE: rule110 [START]\n" over swap type free
  then
then

bye
