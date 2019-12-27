// DrwSubCell.cpp: implementation of the CDrwSubCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sudoku.h"
#include "DrwSubCell.h"
#include "Cell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrwSubCell::CDrwSubCell()
{
	m_Type = eSubCell;
}


CDrwSubCell::~CDrwSubCell()
{
}

void CDrwSubCell::Draw( CDC* pDC , bool bSelected , bool bCurrent )
{
	if( !m_pCell || !m_pCell->IsFree() )
		return;

	CDrawingCell::Draw( pDC , bSelected , bCurrent );
}

void CDrwSubCell::SetRect( CRect& rect )
{
	m_Rect = rect;
}