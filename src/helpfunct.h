
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <afxtempl.h>

unsigned random32( unsigned z );
unsigned long random64( unsigned long z );

//void random_test(unsigned long i,unsigned zmax);

int	compare(const void *a, const void *b); //für die Lib-Funktion qsort

int	compare2(const void *a, const void *b); //für die Lib-Funktion qsort

template<class T>    const T& Min_(const T& x, const T& y) {return x < y ? x : y;}

template<class T>    const T& Max_(const T& x, const T& y) {return x > y ? x : y;}

//result is an Array of 4 strings

int ftoint( float x );
int ftoint( double x );	

void Swap( int& x, int& y );

class CIntArray : public CArray< int , int& >
{
public:
	void operator=( const CIntArray& src );
};

class CBits  
{
public:
	CBits( DWORD size );
	virtual ~CBits();
	void SetBit( const DWORD& at );
	BOOL GetBit( const DWORD& at );

protected:
	BYTE*	m_Buffer;
};

void ColorToString( int color , CString& res );
void GetBorderStr( double x , CString& res , int digits , char sign );

void Shuffle( void** ppPtr , int size );


#endif