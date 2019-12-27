#include "stdafx.h"
#include <stdlib.h>
#include "helpfunct.h"
#include <math.h>

DWORD		dwRandNum;
__int64		RandNum64;

DWORD&		GetRandNum32()
{
	return dwRandNum;
}

__int64&	GetRandNum64()
{
	return RandNum64;
}

unsigned random32( unsigned zmax )	//gibt eine Zufallszahl zwischen 0 und zmax-1 zurück
{						
	return ((( dwRandNum = dwRandNum * 214013L + 2531011L ) >> 16 ) % zmax );
	// x = ax + b in modulo-ring 2^32
	// der Rekursivaufruf x = ax + b definiert eine Untergruppe des restklassen-rings mod 2^32
	// für a = 4k + 1 und b ungerade ist diese Untergruppe der ganze Ring, d.h es kommt
	// erst nach genau 2^32 Aufrufen zur Wiederholung
}

unsigned long random64( unsigned long zmax )	//gibt eine Zufallszahl zwischen 0 und zmax-1 zurück
{	
	RandNum64 = RandNum64 * ((__int64)20000014013)  + ((__int64)3344072531011);
	return ( (unsigned long)(RandNum64 >> 32) ) % zmax;
	// x = ax + b in modulo-ring 2^64
	// der Rekursivaufruf x = ax + b definiert eine Untergruppe des restklassen-rings mod 2^64
	// für a = 4k + 1 und b ungerade ist diese Untergruppe der ganze Ring, d.h es kommt
	// erst nach genau 2^64 Aufrufen zur Wiederholung
}

void Shuffle( void** ppPtr , int size )
{
	if( size < 1 )
		return;

	int		i, r;
	void*	tmp;
	for( i = size-1 ; i >= 0 ; i-- )
	{
		r = random64( i + 1 );
		tmp = ppPtr[r];
		ppPtr[r] = ppPtr[i];
		ppPtr[i] = tmp;
	}
}

int compare(const void *a, const void *b) //für die Lib-Funktion qsort
{
	return(*(int*)a-*(int*)b);
}

int compare2(const void *a, const void *b) //für die Lib-Funktion qsort
{
	return(*(int*)b-*(int*)a);
}

int ftoint( float x )
{
	float y = x < 0.0f ? -0.5f : 0.5f;

	return (int)( x + y );
}

int ftoint( double x )
{
	double y = x < 0.0 ? -0.5 : 0.5;

	return (int)( x + y );
}

void Swap( int& x, int& y )
{
	int z = x;
	x = y;
	y = z;
}

void CIntArray::operator=( const CIntArray& src )
{
	if( &src == this )
		return;

	int size = src.GetSize();
	if( size == 0 )
		return;

	SetSize( size );

	memcpy( GetData() , src.GetData() , size * sizeof(int) );
}

CBits::CBits( DWORD size )
{
	if( size == 0 )
	{
		ASSERT(0);
		return;
	}

	int bufsize = (int)ceil( (double)(size) / 8 );
	m_Buffer = new BYTE[bufsize];
	memset( m_Buffer , 0 , bufsize );
}

CBits::~CBits()
{
	delete[] m_Buffer;
}

void CBits::SetBit( const DWORD& at )
{
	m_Buffer[at/8] |= 1 << (at%8);
}

BOOL CBits::GetBit( const DWORD& at )
{
	return ( m_Buffer[at/8] & 1 << (at%8) ) != 0;
}

void GetBorderStr( double x , CString& res , int digits , char sign )
{
	CString fstr = sign;

#ifdef _DEBUG
	if( sign != '>' && sign != '<' )
	{
		ASSERT(0);
		return;
	}
#endif

	double scale = pow( 10, digits );
	x *= scale;

	if( floor( x + 0.5 ) != ceil( x ) )
	{
		if( sign == '<' )
			fstr += '=';
	}
	else
	{
		if( sign == '>' )
			fstr += '=';
	}

	switch( digits )
	{
	case 0:
		fstr += "%1.0f";
		break;
	case 1:
		fstr += "%1.1f";
		break;
	case 2:
		fstr += "%1.2f";
		break;
	case 3:
		fstr += "%1.3f";
		break;
	case 4:
		fstr += "%1.4f";
		break;
	default:
		ASSERT(0);
		return;
	}
	
	res.Format( fstr, x / scale );
}

void ColorToString( int color , CString& res )
{
	res.LoadString( 8010 + (DWORD)color );
}
