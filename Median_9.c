#include  <stdio.h>
#include  <stdlib.h>
#include  <time.h>

struct Unit
{
  int v0, v1, v2;
};

Unit  Unit_A, Unit_B, Unit_C;


int Median_9()
{
  int   tmp;
  Unit  tmp_u;
  int   min, max;

#define swap_( t, a, b )  t = a; a = b; b = t

  // step1

  if( Unit_A.v0 < Unit_A.v1 ) { swap_( tmp, Unit_A.v0, Unit_A.v1 ); }
  if( Unit_A.v0 < Unit_A.v2 ) { swap_( tmp, Unit_A.v0, Unit_A.v2 ); }
  if( Unit_A.v1 < Unit_A.v2 ) { swap_( tmp, Unit_A.v1, Unit_A.v2 ); }

  if( Unit_B.v0 < Unit_B.v1 ) { swap_( tmp, Unit_B.v0, Unit_B.v1 ); }
  if( Unit_B.v0 < Unit_B.v2 ) { swap_( tmp, Unit_B.v0, Unit_B.v2 ); }
  if( Unit_B.v1 < Unit_B.v2 ) { swap_( tmp, Unit_B.v1, Unit_B.v2 ); }

  if( Unit_C.v0 < Unit_C.v1 ) { swap_( tmp, Unit_C.v0, Unit_C.v1 ); }
  if( Unit_C.v0 < Unit_C.v2 ) { swap_( tmp, Unit_C.v0, Unit_C.v2 ); }
  if( Unit_C.v1 < Unit_C.v2 ) { swap_( tmp, Unit_C.v1, Unit_C.v2 ); }

  // step2

  if( Unit_A.v1 < Unit_B.v1 ) { swap_( tmp_u, Unit_A, Unit_B ); }
  if( Unit_A.v1 < Unit_C.v1 ) { swap_( tmp_u, Unit_A, Unit_C ); }
  if( Unit_B.v1 < Unit_C.v1 ) { swap_( tmp_u, Unit_B, Unit_C ); }

#define _v_ Unit_B.v2
#define _w_ Unit_B.v0
#define _x_ Unit_B.v1
#define _y_ Unit_A.v2
#define _z_ Unit_C.v0

  // case 1

  if( _y_ <= _x_ && _x_ <= _z_ )  return _x_;
  if( _z_ <= _x_ && _x_ <= _y_ )  return _x_;

  // case 2

  if( _x_ < _y_ && _x_ < _z_ )
  {

    if( _y_ < _z_ ) min = _y_;
    else            min = _z_;

    if( _w_ < min ) min = _w_;

    return min;
  }

  // case 3

  if( _x_ > _y_ && _x_ > _z_ )
  {
    if( _y_ > _z_ ) max = _y_;
    else            max = _z_;

    if( _v_ > max ) max = _v_;

    return max;
  }

  return _x_;
}

int main()
{
  srand( ( unsigned )time( NULL ) );

  Unit_A.v0 = rand() % 100; Unit_A.v1 = rand() % 100; Unit_A.v2 = rand() % 100;
  Unit_B.v0 = rand() % 100; Unit_B.v1 = rand() % 100; Unit_B.v2 = rand() % 100;
  Unit_C.v0 = rand() % 100; Unit_C.v1 = rand() % 100; Unit_C.v2 = rand() % 100;

  int v = Median_9();

  printf( "%d, %d, %d\n", Unit_A.v0, Unit_A.v1, Unit_A.v2 );
  printf( "%d, %d, %d\n", Unit_B.v0, Unit_B.v1, Unit_B.v2 );
  printf( "%d, %d, %d\n", Unit_C.v0, Unit_C.v1, Unit_C.v2 );

  printf( ">%d\n", v );
}
