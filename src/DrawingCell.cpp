// DrawingCell.cpp: implementation of the CDrawingCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sudoku.h"
#include "Cell.h"
#include "DrawingCell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawingCell::CDrawingCell()
{
	m_pCell = NULL;
	m_Type = eCell;
}

CDrawingCell::~CDrawingCell()
{

}

void CDrawingCell::Draw( CDC* pDC , bool bSelected , bool bCurrent )
{
	if( bSelected )
	{
		//set to blue
		CPen pen( GetPenStyle() , GetPenWidth() , RGB( 0, 0 , 255 ) );
		CPen* pOldPen = pDC->SelectObject( &pen );
		pDC->Rectangle( m_Rect.left , m_Rect.top, m_Rect.right , m_Rect.bottom );
		pDC->SelectObject( pOldPen );
	}

	if( !m_Text.IsEmpty() )
	{
		CSize size = pDC->GetTextExtent( m_Text );
		if( bCurrent )
		{
			COLORREF OldColor = pDC->SetTextColor( RGB( 0, 255 , 0 ) );
			pDC->TextOut( ( m_Rect.right + m_Rect.left ) / 2 , ( m_Rect.top + m_Rect.bottom - ftoint( 0.95 * size.cy ) ) / 2 , m_Text );
			pDC->SetTextColor( OldColor );
		}
		else
		{
			DWORD color;
			if( bSelected )
				color = RGB( 0, 0 , 255 );
			else
			{
				if( m_pCell->IsInitial() )
					color = RGB( 0 , 0 , 0 );
				else
					color = RGB( 128 , 128 , 128 );
			}

			COLORREF OldColor = pDC->SetTextColor( color );
			pDC->TextOut( ( m_Rect.right + m_Rect.left ) / 2 , ( m_Rect.top + m_Rect.bottom - ftoint( 0.95 * size.cy ) ) / 2 , m_Text );
			pDC->SetTextColor( OldColor );
		}
	}
}

void CDrawingCell::SetRect( CRect& rect )
{
	m_Rect = rect;
}