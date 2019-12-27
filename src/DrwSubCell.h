// DrwSubCell.h: interface for the CDrwSubCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRWSUBFIELD_H__4B17585B_5FDE_4F37_B57F_9FBDFA529F29__INCLUDED_)
#define AFX_DRWSUBFIELD_H__4B17585B_5FDE_4F37_B57F_9FBDFA529F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawingCell.h"

class CDrwSubCell : public CDrawingCell
{
public:
	CDrwSubCell();
	virtual ~CDrwSubCell();

	virtual	DWORD	GetPenStyle()		{ return PS_DOT; }
	virtual	int		GetPenWidth()		{ return 1; }
	virtual void	Draw( CDC* pDC , bool bSelected , bool bCurrent );
	virtual void	SetRect( CRect& rect );
};

#endif // !defined(AFX_DRWSUBFIELD_H__4B17585B_5FDE_4F37_B57F_9FBDFA529F29__INCLUDED_)
