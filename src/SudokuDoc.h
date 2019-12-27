// SudokuDoc.h : interface of the CSudokuDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUDOKUDOC_H__D13DC6B6_72B0_4989_BFEF_DC5AE76BBD5E__INCLUDED_)
#define AFX_SUDOKUDOC_H__D13DC6B6_72B0_4989_BFEF_DC5AE76BBD5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSudokuGame;

#define UH_SAVE_SUBSTRINGS	0x1001
#define UH_NEW_GAME			0x1002
#define UH_REDRAW			0x1003

#include "Cell.h"


class CSudokuDoc : public CDocument
{
protected: // create from serialization only
	CSudokuDoc();
	DECLARE_DYNCREATE(CSudokuDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSudokuDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSudokuDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CSudokuGame* m_pGame;

	int			m_Version;

public:

	CString		m_SubStrings[VAL_CNT][VAL_CNT];//useredited substrings

	CSudokuGame*	GetGame()		{ return m_pGame; }
	void			ReplaceGame( CSudokuGame* pNewGame );
	void			ClearSubStrings();
	void			CreateGame();


// Generated message map functions
protected:
	//{{AFX_MSG(CSudokuDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUDOKUDOC_H__D13DC6B6_72B0_4989_BFEF_DC5AE76BBD5E__INCLUDED_)
