// SudokuView.h : interface of the CSudokuView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUDOKUVIEW_H__8A8F4C28_FB4B_4F31_9E7E_2EAA04D13FD8__INCLUDED_)
#define AFX_SUDOKUVIEW_H__8A8F4C28_FB4B_4F31_9E7E_2EAA04D13FD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cell.h"
#include "DrawingCell.h"
#include "DrwSubCell.h"

#include <list>

class CSudokuGame;

class CSudokuView : public CView
{
protected: // create from serialization only
	CSudokuView();
	DECLARE_DYNCREATE(CSudokuView)

// Attributes
public:
	CSudokuDoc* GetDocument();

protected:

	CSudokuGame*	m_pGame;

public:
	CSudokuGame* GetGame()	{ return m_pGame; }

	CPoint	m_Origin;
	int		m_Scale;//isotropic integer(!) scale
	CRect	m_PlayRect;
	CPoint	m_OutputTopLeft;
	CPoint	m_TitleBottomCenter;
	int		m_OutPutIdx;

	int		m_ScreenWidth;
	int		m_ScreenHeight;

	std::list<CDrawingCell*>	m_listCells;
	CDrawingCell*				m_pSelectedCell;
	CDrawingCell*				m_pCurrentCell;

	CDrawingCell				m_DrwCells[COL_CNT][ROW_CNT];
	CDrwSubCell					m_DrwSubCells[COL_CNT][ROW_CNT];

	CString						m_OutputText;

	inline	CDrawingCell*	GetCell( int col , int row )			{ return &m_DrwCells[col][row]; }
	inline	CDrwSubCell*	GetSubCell( int col , int row )	{ return &m_DrwSubCells[col][row]; }

	void	IncreaseRect( CRect& rect );

	void	SetCell( CDrawingCell* pNewCell );

	void	SetSize( int cx , int cy , bool bPrinting );

	void	Draw( CDC* pDC );

	void	SaveSubStrings();
	void	LoadSubStrings();

	double	m_PrintingFontFactor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSudokuView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetGame( CSudokuGame* pGame );
	virtual ~CSudokuView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSudokuView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetstartpos();
	afx_msg void OnNextNumber();
	afx_msg void OnUpdateNextNumber(CCmdUI* pCmdUI);
	afx_msg void OnComputerSolve();
	afx_msg void OnSetLevel();
	afx_msg void OnFullSolve();
	afx_msg void OnResetToInitial();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SudokuView.cpp
inline CSudokuDoc* CSudokuView::GetDocument()
   { return (CSudokuDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUDOKUVIEW_H__8A8F4C28_FB4B_4F31_9E7E_2EAA04D13FD8__INCLUDED_)
