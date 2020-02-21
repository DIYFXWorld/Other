#include  "wave.hpp"
#include  <cmath>

main()
{
  int       fs      = 44100;   // �T���v�����O���[�g
  int       freq    = 1000;    // �T�C���g�̎��g��
  int       length  = 5;       // �����f�[�^�̒���(�b)
  float     gain   = 0.2;      // �Q�C��

  wave_f  src(  fs, fs*length, 2 ); // ��3������2���w�肷��ƃX�e���I

  double  s = (2.0*M_PI) / fs * freq;

  for( auto i( 0 ); i < src.length; ++i )
  {
    float  v = sin( s * i ) * gain;
    src.L[ i ] =  v;    // L channel
    src.R[ i ] =  v;    // R channel
  }

  save( "wave_sine_st_8bit.wav", src, 8 );  // 8�r�b�g�t�H�[�}�b�g
//  save( "wave_sine_st_16bit.wav", src );    // 16�r�b�g�t�H�[�}�b�g(�f�t�H���g)
}
