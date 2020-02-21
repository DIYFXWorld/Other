#include  "wave.hpp"
#include  <cmath>

main()
{
  int       fs      = 44100;   // サンプリングレート
  int       freq    = 1000;    // サイン波の周波数
  int       length  = 5;       // 音声データの長さ(秒)
  float     gain   = 0.2;      // ゲイン

  wave_f  src(  fs, fs*length, 2 ); // 第3引数に2を指定するとステレオ

  double  s = (2.0*M_PI) / fs * freq;

  for( auto i( 0 ); i < src.length; ++i )
  {
    float  v = sin( s * i ) * gain;
    src.L[ i ] =  v;    // L channel
    src.R[ i ] =  v;    // R channel
  }

  save( "wave_sine_st_8bit.wav", src, 8 );  // 8ビットフォーマット
//  save( "wave_sine_st_16bit.wav", src );    // 16ビットフォーマット(デフォルト)
}
