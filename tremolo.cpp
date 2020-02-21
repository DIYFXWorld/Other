#include  "wave.hpp"
#include  <cmath>

main()
{
  wave_f  src;

  load( "_aruhinokuregatanokotodearu.wav", src ); // �t�@�C������ǂݍ���

  wave_f  dst( src.fs, src.length );              // �o�̓f�[�^�̓��ꕨ

  float   freq = 1.f;                             // ���g��
  float   t = (2.f*M_PI) / src.fs * freq;

  for( auto i( 0 ); i < src.length; ++i )
  {
    dst[ i ] = src[ i ] * sin( t * i );           // �g����������
  }

  save( "toremolo.wav", dst );                    // �t�@�C���֏����o��
}

