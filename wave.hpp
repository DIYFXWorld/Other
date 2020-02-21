#ifndef WAVE_HPP_
#define WAVE_HPP_

#include	<cstring>
#include	<cstdio>
#include	<cstdlib>

class file_ptr
{
private:

	FILE	*fp;

	file_ptr( const file_ptr& );
	file_ptr& operator = ( const file_ptr& );

public:

	file_ptr( const char* file_name, const char* mode ) :
		fp( 0 )
	{
		open( file_name, mode );
	}

	file_ptr( FILE* p = 0 ) :
		fp( p )
	{
	}

	~file_ptr() { close(); }

	operator bool ( void ) const		{ return fp; }

	void open( const char* file_name, const char* mode )
	{
		close();

		fp = fopen( file_name, mode );

		if( !fp )	error_exit( "open file failed." );
	}

	void close( void )
	{
		if( fp ) fclose( fp );
		fp = 0;
	}

	template <typename T>
	void write( const T& v )
	{
		if( fwrite( &v, sizeof( T ), 1, fp ) != 1 )
			error_exit( "write failed." );
	}

	template <typename T>
	void read( T& v )
	{
		if( fread( &v, sizeof( T ), 1, fp ) != 1 )
			error_exit( "read failed." );
	}

	template <typename T>
	void write( const T* v, int size )
	{
		if( fwrite( v, sizeof( T ) * size, 1, fp ) != 1 )
			error_exit( "write failed." );
	}

	template <typename T>
	void read( T* v, int size )
	{
		if( fread( v, sizeof( T ) * size, 1, fp ) != 1 )
			error_exit( "read failed." );
	}

	void error_exit( const char* message )
	{
		puts( message );
		exit( 0 );
	}
};

struct unit_data
{
	short	Value;

	////////// double
	unit_data( const double& v ) 				{ *this = v; }
	unit_data& operator = ( const double& v ){ Value = ( short )( v * 32768 ); return *this; }
	operator double () const 						{ return Value / 32768.0; }

	////////// float
	unit_data( const float& v )					{ *this = v; }
	unit_data& operator = ( const float& v ) { Value = ( short )( v * 32768 ); return *this; }
	operator float () const 						{ return Value / 32768.f; }

	////////// int
	unit_data( const int& v ) 					{ *this = v; }
	unit_data& operator = ( const int& v )		{ Value = v; return *this; }
	operator int () const								{ return Value; }

	////////// short
	unit_data( const short& v ) 				{ *this = v; }
	unit_data& operator = ( const short& v )	{ Value = v; return *this; }
	operator short () const							{ return Value; }

	////////// char
	unit_data( const char& v )					{ *this = v; }
	unit_data& operator = ( const char& v )	{ Value = ( short )( v * 256 ); return *this; }
	operator char () const							{ return ( char )( Value / 256 ); }

	////////// unsigned char
	unit_data( const unsigned char& v )					{ *this = v; }
	unit_data& operator = ( const unsigned char& v ) { Value = ( short )( ( v - 128 ) * 256 ); return *this; }
	operator unsigned char () const							{ return ( unsigned char )( Value / 256 + 128 ); }
};

template <typename T>
struct wave
{
  int 	fs;						// sampling frwquency
  int 	length; 			//
	int		channel;			// 1 or 2

	T			*L, *R;

	wave():
		fs( 0 ), length( 0 ), channel( 0 ),
		L( NULL ), R( NULL )
	{
	}

	wave( int _fs, int _length, int _channel = 1 ):
		fs( _fs ), length( _length ), channel( _channel ),
		L( NULL ), R( NULL )
	{
		create();
	}

	wave( const wave& src, bool copy = true ):
		fs( src.fs ), length( src.length ), channel( src.channel ),
		L( NULL ), R( NULL )
	{
		if( copy )	*this = src;
		else 				create();
	}

	~wave() { destroy_l(); destroy_r(); }

	wave& operator = ( const wave& v )
	{
		if( this != &v )
		{
			fs			= v.fs;
			length	= v.length;
			channel = v.channel;

			create();

			for( int i( 0 ); i < length; ++i )
				L[ i ] = v.L[ i ];

			if( channel == 2 )
				for( int i( 0 ); i < length; ++i )
					R[ i ] = v.R[ i ];
		}
		return *this;
	}

	bool operator == ( const wave& v ) const
	{
		if( this != &v )
		{
			if( fs			!= v.fs )				return false;
			if( length	!= v.length )		return false;
			if( channel != v.channel )	return false;

			for( int i( 0 ); i < length; ++i )
				if( L[ i ] != v.L[ i ] ) return false;

			if( channel == 2 )
				for( int i( 0 ); i < length; ++i )
					if( R[ i ] != v.R[ i ] ) return false;
		}
		return true;
	}

	bool operator != ( const wave& v ) const	{ return !( v == *this ); }

	void create_l( int len )
	{
		if( channel > 0 )
		{
			destroy_l();
			L = new T[ len ];
		}
	}

	void create_r( int len )
	{
		if( channel > 1 )
		{
			destroy_r();
			R = new T[ len ];
		}
	}

	void create()
	{
		create_l( length );
		create_r( length );
	}

	void destroy_l()	{ if( L )	delete [] L;	L = NULL; }
	void destroy_r()	{ if( R )	delete [] R;	R = NULL; }

	void clear( const T& v = 0 )
	{
		if( L )
			for( int i( 0 ); i < length; ++i )
				L[ i ] = v;

		if( R )
			for( int i( 0 ); i < length; ++i )
				R[ i ] = v;
	}

	      T&	operator [] ( int i )				{ return L[ i ]; }
	const T&	operator [] ( int i ) const	{ return L[ i ]; }
};

using wave_d = wave<double>;
using wave_f = wave<float>;
using wave_i = wave<int>;
using wave_s = wave<short>;
using wave_c = wave<char>;
using wave_uc = wave<unsigned char>;

struct wav_header
{
  char		riff_chunk_ID[ 4 ];
  long		riff_chunk_size;
  char		file_format_type[ 4 ];
  char		fmt_chunk_ID[ 4 ];
  long		fmt_chunk_size;
  short		wave_format_type;
  short 	channel;
  long 		samples_per_sec;
  long 		bytes_per_sec;
  short 	block_size;
  short 	bits_per_sample;
  char 		data_chunk_ID[ 4 ];
  long 		data_chunk_size;

  wav_header() {}

  template<typename T>
	wav_header( const wave<T>& w = wave<T>(), int bits = 16 ) :
		riff_chunk_ID			{ 'R', 'I', 'F', 'F' },
		file_format_type	{ 'W', 'A', 'V', 'E' },
		fmt_chunk_ID			{ 'f', 'm', 't', ' ' },
		data_chunk_ID 		{ 'd', 'a', 't', 'a' }
	{
		int	bytes = bits / 8;

		fmt_chunk_size		= 16;
  	wave_format_type	= 1;
		riff_chunk_size		= 36 + w.length * bytes * w.channel;
	  bits_per_sample		= bits;
		samples_per_sec		= w.fs;
		channel						= w.channel;
	  bytes_per_sec			= w.fs * bytes * w.channel;
	  block_size				= bytes * w.channel;
  	data_chunk_size		= w.length * bytes * w.channel;
	}
};

template<typename T>
void read_8bit_mono( file_ptr& fp, wave<T>& w )
{
	unsigned char	v;

	for( int i( 0 ); i < w.length; ++i )
  {
    fp.read( v );
  	w.L[ i ] = unit_data( v );
  }
}

template<typename T>
void write_8bit_mono( file_ptr& fp, const wave<T>& w )
{
	unsigned char	v;

	for( int i( 0 ); i < w.length; ++i )
  {
  	v = unit_data( w.L[ i ] );
  	fp.write( v );
  }

	if( ( w.length % 2 ) == 1 ) // make it even
  {
    v = 0;	fp.write( v );
  }
}

template<typename T>
void read_8bit_stereo( file_ptr& fp, wave<T>& w )
{
  unsigned char v;

	for( int i( 0 ); i < w.length; ++i )
  {
    fp.read( v );
    w.L[ i ] = unit_data( v );

    fp.read( v );
    w.R[ i ] = unit_data( v );
  }
}

template<typename T>
void write_8bit_stereo( file_ptr& fp, const wave<T>& w )
{
	unsigned char	v;

  for( int i( 0 ); i < w.length; ++i )
  {
  	v = unit_data( w.L[ i ] );
  	fp.write( v );

  	v = unit_data( w.R[ i ] );
  	fp.write( v );
  }
}

template<typename T>
void read_16bit_mono( file_ptr& fp, wave<T>& w )
{
	short	v;

  for( int i( 0 ); i < w.length; ++i )
  {
  	fp.read( v );
  	w.L[ i ] = unit_data( v );
  }
}

template<typename T>
void write_16bit_mono( file_ptr& fp, const wave<T>& w )
{
	short	v;

	for( int i( 0 ); i < w.length; ++i )
  {
  	v = ( short )unit_data( w.L[ i ] );
  	fp.write( v );
  }
}

template<typename T>
void read_16bit_stereo( file_ptr& fp, wave<T>& w )
{
	short	v;

	for( int i( 0 ); i < w.length; ++i )
  {
  	fp.read( v );
  	w.L[ i ] = unit_data( v );

  	fp.read( v );
  	w.R[ i ] = unit_data( v );
  }
}

template<typename T>
void write_16bit_stereo( file_ptr& fp, const wave<T>& w )
{
	short	v;

	for( int i( 0 ); i < w.length; ++i )
  {
  	v = unit_data( w.L[ i ] );
  	fp.write( v );

  	v = unit_data( w.R[ i ] );
  	fp.write( v );
  }
}

template<typename T>
void save( const char *file_name, const wave<T>& w, int bits = 16 )
{
	wav_header	wh( w, bits );
  file_ptr	fp( file_name, "wb" );
	fp.write( wh );

	if( ( bits ==  8 ) && ( w.channel == 1 ) )	write_8bit_mono   ( fp, w );
	if( ( bits ==  8 ) && ( w.channel == 2 ) )	write_8bit_stereo ( fp, w );
	if( ( bits == 16 ) && ( w.channel == 1 ) )	write_16bit_mono  ( fp, w );
	if( ( bits == 16 ) && ( w.channel == 2 ) )	write_16bit_stereo( fp, w );
}

template<typename T>
void _save( const char *file_name, const wave<T>& w, int bits = 16 )
{
	char	buf[ 1024 ];

	// exchange file ext
	{
		for( unsigned int i( 0 ); i < sizeof( buf ); ++i )
			buf[ i ] = 0;

		strcpy( buf, file_name );

		for( int i( sizeof( buf )-1 ); i >= 0; --i )
		{
			if( buf[ i ] == '.' )
			{
				buf[ i ] = 0;
				break;
			}
			else if( buf[ i ] == '\\' )
				break;
		}
		strcat( buf, ".wav" );
	}

	save( buf, w, bits );
}

template<typename T>
void load( const char *file_name, wave<T>& w )
{
	wav_header	wh;
  file_ptr	fp( file_name, "rb" );
	fp.read( wh );

	w.fs			= wh.samples_per_sec;
  w.length	= wh.data_chunk_size / ( wh.bits_per_sample / 8 ) / wh.channel;
	w.channel	= wh.channel;

	w.create();

	if( ( wh.bits_per_sample ==  8 ) && ( w.channel == 1 ) )	read_8bit_mono   ( fp, w );
	if( ( wh.bits_per_sample ==  8 ) && ( w.channel == 2 ) )	read_8bit_stereo ( fp, w );
	if( ( wh.bits_per_sample == 16 ) && ( w.channel == 1 ) )	read_16bit_mono  ( fp, w );
	if( ( wh.bits_per_sample == 16 ) && ( w.channel == 2 ) )	read_16bit_stereo( fp, w );
}

struct wav_info
{
	int	fs;
	int	bits;
	int length;
	int	channel;
};

wav_info get_wav_info( const char *file_name )
{
	wav_header	wh;
  file_ptr	fp( file_name, "rb" );
	fp.read( wh );

	wav_info	wi;
	wi.fs			= wh.samples_per_sec;
  wi.bits		= wh.bits_per_sample;
  wi.length	= wh.data_chunk_size / ( wh.bits_per_sample / 8 ) / wh.channel;
	wi.channel	= wh.channel;

	return wi;
}

#endif /* WAVE_HPP_ */
