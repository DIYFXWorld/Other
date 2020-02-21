#include  "wave.hpp"

main( int argc, char *argv[] )
{
  if( argc < 2 )
  {
    printf( "usage : wav_info file_name.wav" );
    return 0;
  }

  wav_info  wi = get_wav_info( argv[ 1 ] );

  puts( "" );

  printf( "Sampling rate : %d Hz\r\n", wi.fs );
  printf( "Sample bits   : %d bit\r\n", wi.bits );
  
  printf( "Channel       : " );
  if( wi.channel == 1 )  puts( "1 (Mono)" );
  if( wi.channel == 2 )  puts( "2 (Stereo)" );

  printf( "Time length   : %f s\r\n", (double)wi.length / wi.fs );
}

