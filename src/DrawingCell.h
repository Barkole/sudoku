// DrawingCell.h: interface for the CDrawingCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWINGFIELD_H__E8E47E05_97A4_46D2_8CF1_AA405CE49D92__INCLUDED_)
#define AFX_DRAWINGFIELD_H__E8E47E05_97A4_46D2_8CF1_AA405CE49D92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCell;

class CDrawingCell  
{
public:
	typedef enum eType
	{
		eCell,
		eSubCell,
	};

	eType		m_Type;

	CDrawingCell();
	virtual ~CDrawingCell();

	CString				m_Text;
	CRect				m_Rect;

	CRect*				GetRect()					{ return &m_Rect; }
	virtual void		SetRect( CRect& rect );

	CCell*		m_pCell;
	CCell*		GetDataCell()		{ return m_pCell; }
	void		SetDataCell( CCell* f )		{ m_pCell = f; }

	virtual void	Draw( CDC* pDC , bool bSelected , bool bCurrent );

	virtual	DWORD	GetPenStyle()		{ return PS_SOLID; }
	virtual	int		GetPenWidth()		{ return 3; }
	bool			IsSubCell()		{ return m_Type == eSubCell; }
};

#endif // !defined(AFX_DRAWINGFIELD_H__E8E47E05_97A4_46D2_8CF1_AA405CE49D92__INCLUDED_)
