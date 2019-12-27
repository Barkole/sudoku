// Sudoku.h : main header file for the SUDOKU application
//

#if !defined(AFX_SUDOKU_H__934B8993_00E5_4E89_BA1A_BDA5BB33B52F__INCLUDED_)
#define AFX_SUDOKU_H__934B8993_00E5_4E89_BA1A_BDA5BB33B52F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSudokuApp:
// See Sudoku.cpp for the implementation of this class
//

class CSudokuApp : public CWinApp
{
public:
	CSudokuApp();

protected:
	int		m_MaxStepLevel;//level for stepwise solving
	int		m_MaxCreateLevel;//level for creating a new game
	int		m_MinCellCnt;//minimum cells to leave for creating a new game

	int		m_Version;//version of usp-file

	static DWORD m_NextGameId;

	CString	m_UserPath;

public:
	int&	GetMaxStepLevel()				{ return m_MaxStepLevel; }
	void	SetMaxStepLevel( int lev )		{ m_MaxStepLevel = lev; }
	int&	GetMaxCreateLevel()				{ return m_MaxCreateLevel; }
	void	SetMaxCreateLevel( int lev )	{ m_MaxCreateLevel = lev; }
	int&	GetMinCellCnt()					{ return m_MinCellCnt; }
	void	SetMinCellCnt( int m )			{ m_MinCellCnt = m; }
	DWORD	GetFreeGameId()					{ return ++m_NextGameId; }

	CString& GetUserPath()					{ return m_UserPath; }

	static LPCTSTR CSudokuApp::LevelToString( int level );

	DWORD	GetTime();
	BOOL	SaveUserprofile();
	BOOL	LoadUserprofile();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSudokuApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSudokuApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUDOKU_H__934B8993_00E5_4E89_BA1A_BDA5BB33B52F__INCLUDED_)
