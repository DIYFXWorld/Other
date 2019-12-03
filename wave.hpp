#ifndef	wave_hpp
#define	wave_hpp

// コメントアウトを解除してどれか一つを有効にしてください。
#define		USE_DOUBLE
//#define		USE_FLOAT
//#define		USE_INTEGER

#ifdef	USE_DOUBLE
	typedef	double	TYPE_DATA;
#endif

#ifdef	USE_FLOAT
	typedef	float		TYPE_DATA;
#endif

#ifdef	USE_INTEGER
	typedef	short		TYPE_DATA;
#endif

#include	<string.h>
#include	<stdio.h>
#include	<iostream>

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
		std::cout << message << std::endl;
		exit( 0 );
	}
};

struct wave
{
  int 				fs;						// サンプリングレート
  int 				bits;					// 精度
  int 				length; 			// データの長さ
	int					channel;			// チャンネル数

	TYPE_DATA		*l, *r;

	wave() : l( NULL ), r( NULL ) {}

	wave( int _fs, int _bits, int _length, int _channel = 1 ) :
		fs( _fs ), bits( _bits ), length( _length ), channel( _channel ),
		l( NULL ), r( NULL )
	{
		create();
	}

	~wave() { destroy_l(); destroy_r(); }

	wave& operator = ( const wave& v )
	{
		if( this != &v )
		{
			fs			= v.fs;
			bits		= v.bits;
			length	= v.length;
			channel = v.channel;

			create();

			for( int i( 0 ); i < length; ++i )
				l[ i ] = v.l[ i ];

			if( channel > 1 )
				for( int i( 0 ); i < length; ++i )
					r[ i ] = v.r[ i ];
		}
		return *this;
	}

	bool operator == ( const wave& v ) const
	{
		if( this != &v )
		{
			if( fs			!= v.fs )				return false;
			if( bits		!= v.bits )			return false;
			if( length	!= v.length )		return false;
			if( channel != v.channel )	return false;

			for( int i( 0 ); i < length; ++i )
				if( l[ i ] != v.l[ i ] ) return false;

			if( channel > 1 )
				for( int i( 0 ); i < length; ++i )
					if( r[ i ] != v.r[ i ] ) return false;
		}
		return true;
	}

	bool operator != ( const wave& v ) const	{ return !( v == *this ); }

	void create_l( int len )
	{ 
		if( channel > 0 )
		{
			destroy_l();
			l = new TYPE_DATA[ len ];
		}
	}

	void create_r( int len )
	{ 
		if( channel > 1 )
		{
			destroy_r();
			r = new TYPE_DATA[ len ];
		}
	}

	void create()
	{
		create_l( length );
		create_r( length );
	}

	void destroy_l()	{ if( l )	delete [] l;	l = NULL; }
	void destroy_r()	{ if( r )	delete [] r;	r = NULL; }

	void clear( const TYPE_DATA& v = 0 )
	{
		if( l )
			for( int i( 0 ); i < length; ++i )
				l[ i ] = v;

		if( r )
			for( int i( 0 ); i < length; ++i )
				r[ i ] = v;
	}

	TYPE_DATA&	operator [] ( int i )				{ return l[ i ]; }
	TYPE_DATA&	operator [] ( int i ) const	{ return l[ i ]; }
};

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

	wav_header(){} 

	wav_header( const wave& w ) : 
		riff_chunk_ID			{ 'R', 'I', 'F', 'F' },
		file_format_type	{ 'W', 'A', 'V', 'E' },
		fmt_chunk_ID			{ 'f', 'm', 't', ' ' },
		data_chunk_ID 		{ 'd', 'a', 't', 'a' }
	{
		int	bytes = w.bits / 8;

		fmt_chunk_size		= 16;
  	wave_format_type	= 1;
		riff_chunk_size		= 36 + w.length * bytes * w.channel;
	  bits_per_sample		= w.bits;
		samples_per_sec		= w.fs;
		channel						= w.channel;
	  bytes_per_sec			= w.fs * bytes * w.channel;
	  block_size				= bytes * w.channel;
  	data_chunk_size		= w.length * bytes * w.channel;
	}
};

void create( wave& w, const wav_header& wh, bool read_body = true )
{
	int	bytes	= wh.bits_per_sample / 8;

	w.fs			= wh.samples_per_sec;
  w.bits		= wh.bits_per_sample;
  w.length	= wh.data_chunk_size / bytes / wh.channel;
	w.channel	= wh.channel;

	if( read_body )	w.create();
}

void read_8bit_mono( file_ptr& fp, wave& w )
{
	unsigned char	v;

	for( int i( 0 ); i < w.length; ++i )
  {
    fp.read( v );
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	w.l[ i ] = ( TYPE_DATA )( ( v - 128.0 ) / 128.0 );
#endif
#ifdef	USE_INTEGER
  	w.l[ i ] = v;
#endif
  }
}

void write_8bit_mono( file_ptr& fp, const wave& w )
{
	unsigned char	v;

	for( int i( 0 ); i < w.length; ++i )
  {
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	v = ( unsigned char )( ( w.l[ i ] + 1.0 ) / 2.0 * 256.0 );
#endif
#ifdef	USE_INTEGER
  	v = w.l[ i ];
#endif
  	fp.write( v );
  }

  if( ( w.length % 2 ) == 1 ) // 奇数データ
  {
    v = 0;	fp.write( v );
  }
}

void read_8bit_stereo( file_ptr& fp, wave& w )
{
  unsigned char v;

	for( int i( 0 ); i < w.length; ++i )
  {
    fp.read( v );
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	w.l[ i ] = ( TYPE_DATA )( ( v - 128.0 ) / 128.0 );
#endif
#ifdef	USE_INTEGER
  	w.l[ i ] = v;
#endif
    fp.read( v );
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	w.r[ i ] = ( TYPE_DATA )( ( v - 128.0 ) / 128.0 );
#endif
#ifdef	USE_INTEGER
  	w.r[ i ] = v;
#endif
  }
}

void write_8bit_stereo( file_ptr& fp, const wave& w )
{
	unsigned char	v;

  for( int i( 0 ); i < w.length; ++i )
  {
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	v = ( unsigned char )( ( w.l[ i ] + 1.0 ) / 2.0 * 256.0 );
#endif
#ifdef	USE_INTEGER
  	v = w.l[ i ];
#endif
  	fp.write( v );
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	v = ( unsigned char )( ( w.r[ i ] + 1.0 ) / 2.0 * 256.0 );
#endif
#ifdef	USE_INTEGER
  	v = w.r[ i ];
#endif
  	fp.write( v );
  }
}

void read_16bit_mono( file_ptr& fp, wave& w )
{
	short	v;

  for( int i( 0 ); i < w.length; ++i )
  {
  	fp.read( v );
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	w.l[ i ] = ( TYPE_DATA )( v / 32768.0 );
#endif
#ifdef	USE_INTEGER
  	w.l[ i ] = v;
#endif
  }
}

void write_16bit_mono( file_ptr& fp, const wave& w )
{
	short	v;

	for( int i( 0 ); i < w.length; ++i )
  {
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	v = w.l[ i ] * 32768.0;
#endif
#ifdef	USE_INTEGER
  	v = w.l[ i ];
#endif

  	fp.write( v ); 
  }
}

void read_16bit_stereo( file_ptr& fp, wave& w )
{
	short	v;

	for( int i( 0 ); i < w.length; ++i )
  {
  	fp.read( v );
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	w.l[ i ] = ( TYPE_DATA )( v / 32768.0 );
#endif
#ifdef	USE_INTEGER
  	w.l[ i ] = v;
#endif

  	fp.read( v );
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	w.r[ i ] = ( TYPE_DATA )( v / 32768.0 );
#endif
#ifdef	USE_INTEGER
  	w.r[ i ] = v;
#endif
  }
}

void write_16bit_stereo( file_ptr& fp, const wave& w )
{
	short	v;

	for( int i( 0 ); i < w.length; ++i )
  {
#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	v = w.l[ i ] * 32768.0;
#endif
#ifdef	USE_INTEGER
  	v = w.l[ i ];
#endif
  	fp.write( v ); 

#if	defined( USE_DOUBLE ) || defined( USE_FLOAT )
  	v = w.r[ i ] * 32768.0;
#endif
#ifdef	USE_INTEGER
  	v = w.r[ i ];
#endif
  	fp.write( v ); 
  }
}

void save( const char *file_name, const wave& w )
{
	wav_header	wh( w );
  file_ptr	fp( file_name, "wb" );
	fp.write( wh );

	if( ( w.bits ==  8 ) && ( w.channel == 1 ) )	write_8bit_mono   ( fp, w );
	if( ( w.bits ==  8 ) && ( w.channel == 2 ) )	write_8bit_stereo ( fp, w );
	if( ( w.bits == 16 ) && ( w.channel == 1 ) )	write_16bit_mono  ( fp, w );
	if( ( w.bits == 16 ) && ( w.channel == 2 ) )	write_16bit_stereo( fp, w );
}

void _save( const char *file_name, const wave& w )
{
	char	buf[ 1024 ];

	for( int i( 0 ); i < sizeof( buf ); ++i )
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
	save( buf, w );
}

void load( const char *file_name, wave& w, bool read_body = true )
{
	wav_header	wh;
  file_ptr	fp( file_name, "rb" );
	fp.read( wh );

	create( w, wh, read_body );

	if( read_body )
	{
		if( ( w.bits ==  8 ) && ( w.channel == 1 ) )	read_8bit_mono   ( fp, w );
		if( ( w.bits ==  8 ) && ( w.channel == 2 ) )	read_8bit_stereo ( fp, w );
		if( ( w.bits == 16 ) && ( w.channel == 1 ) )	read_16bit_mono  ( fp, w );
		if( ( w.bits == 16 ) && ( w.channel == 2 ) )	read_16bit_stereo( fp, w );
	}
}

#endif
