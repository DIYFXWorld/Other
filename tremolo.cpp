#include  "wave.hpp"
#include  <cmath>

main()
{
  wave_f  src;

  load( "_aruhinokuregatanokotodearu.wav", src ); // ファイルから読み込み

  wave_f  dst( src.fs, src.length );              // 出力データの入れ物

  float   freq = 1.f;                             // 周波数
  float   t = (2.f*M_PI) / src.fs * freq;

  for( auto i( 0 ); i < src.length; ++i )
  {
    dst[ i ] = src[ i ] * sin( t * i );           // トレモロ効果
  }

  save( "toremolo.wav", dst );                    // ファイルへ書き出し
}

