#include  "wave.hpp"
#include  <cmath>

// double         -1.0...0.99999  ( -1.0 <= x < 1.0 )
// float          -1.0...0.99999  ( -1.0 <= x < 1.0 )
// int            -32768...32767
// short          -32768...32767
// char           -128...127
// unsigned char  0...255

main()
{
  int     fs      = 44100;     // サンプリングレート
  int     freq    = 1000;      // 周波数
  float   length  = 5.f;       // 音声データの長さ(秒)
  float   gain    = 0.2f;      // ゲイン

  wave<float> src( fs, fs * length );

  float  t = ( 2.0*M_PI ) / fs * freq;

  for( auto i( 0 ); i < src.length; ++i )
  {
    src[ i ] =  sin( t * i ) * gain;
  }

  wave_f src2( src ); // コピーのテスト

  save( "sine.wav", src2 );

  save( "sine_8.wav", src2, 8 );
}
