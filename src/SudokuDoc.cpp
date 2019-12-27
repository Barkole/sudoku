// SudokuDoc.cpp : implementation of the CSudokuDoc class
//

#include "stdafx.h"
#include "Sudoku.h"

#include "SudokuDoc.h"

#include "SudokuGame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSudokuDoc

IMPLEMENT_DYNCREATE(CSudokuDoc, CDocument)

BEGIN_MESSAGE_MAP(CSudokuDoc, CDocument)
	//{{AFX_MSG_MAP(CSudokuDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSudokuDoc construction/destruction

CSudokuDoc::CSudokuDoc()
{
	m_pGame = NULL;
	m_Version = 1;
}

CSudokuDoc::~CSudokuDoc()
{
	delete m_pGame;
}

BOOL CSudokuDoc::OnNewDocument()
{
	if( !CDocument::OnNewDocument() )
		return FALSE;

	CreateGame();

	UpdateAllViews( NULL , UH_NEW_GAME );

	return TRUE;
}

void CSudokuDoc::ReplaceGame( CSudokuGame* pNewGame )
{
	delete m_pGame;
	m_pGame = pNewGame;
	ClearSubStrings();
	UpdateAllViews( NULL , UH_NEW_GAME );
}

void CSudokuDoc::ClearSubStrings()
{
	int i, j;
	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			m_SubStrings[i][j].Empty();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSudokuDoc serialization

void CSudokuDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSudokuDoc diagnostics

#ifdef _DEBUG
void CSudokuDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSudokuDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSudokuDoc commands

BOOL CSudokuDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	CStdioFile file;

	if( !file.Open( lpszPathName , CFile::modeRead ) )
		return FALSE;

	CString line;

	try
	{
		file.ReadString( line );
		if( line != "<--Sudoku-File-->" )
		{
			AfxMessageBox( "Is no a Sudoku-file!" );
			return FALSE;
		}

		file.ReadString( line );
		int pos = line.Find( "<--Version-->" );
		if( pos < 1 )
		{
			AfxMessageBox( "Is no a Sudoku-file!" );
			return FALSE;
		}
		CString left = line.Left( pos );
		int version = atoi( (LPCTSTR)left );

		m_pGame->LoadFromFile( file , version );

		//read user edited Substrings:
		ClearSubStrings();
		file.ReadString( line );
		ASSERT( line == "<--User Edited Substrings-->" );
		int	i, j;
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			for( i = 0 ; i < VAL_CNT ; i++ )
			{
				file.ReadString( m_SubStrings[i][j] );
			}	
		}

		UpdateAllViews( NULL , UH_NEW_GAME );
	}


	catch(...)
	{
		AfxMessageBox( "Unexpected Fileformat" );
		return FALSE;
	}


	file.Close();
	
	return TRUE;
}

BOOL CSudokuDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	CStdioFile file;

	if( !file.Open( lpszPathName , CFile::modeCreate | CFile::modeWrite ) )
		return FALSE;

	CString line;

	file.WriteString( "<--Sudoku-File-->\n" );
	line.Format( "%d<--Version-->\n" , m_Version );
	file.WriteString( (LPCTSTR)line );

	if( !m_pGame )
	{
		ASSERT(0);
		file.Abort();
		return TRUE;
	}

	m_pGame->SaveToFile( file );

	UpdateAllViews( NULL , UH_SAVE_SUBSTRINGS );

	//write user edited Substrings:
	file.WriteString( "<--User Edited Substrings-->\n" );
	int	i, j;
	for( j = 0 ; j < VAL_CNT ; j++ )
	{
		for( i = 0 ; i < VAL_CNT ; i++ )
		{
			line.Format( "%s\n" , (LPCTSTR)m_SubStrings[i][j] );
			file.WriteString( line );
		}	
	}

	file.Close();

	UpdateAllViews( NULL , UH_REDRAW );
	return TRUE;
}

void CSudokuDoc::CreateGame()
{
	CSudokuApp*		pApp = (CSudokuApp*)AfxGetApp();
	
	delete m_pGame;
	m_pGame = new CSudokuGame;

	if( m_pGame )
	{
		ClearSubStrings();

		CWaitCursor wc;
		int MinCellCnt = pApp->GetMinCellCnt();
		int level = pApp->GetMaxCreateLevel();
		if( !m_pGame->CreateGame( level , &MinCellCnt ) )
		{
			ASSERT(0);
			delete m_pGame;
			m_pGame = NULL;
			return;
		}

		m_pGame->ResetMovesToMake();

		int GameNo = pApp->GetFreeGameId();
		CString title;
		CString PathName = pApp->GetUserPath();
		title.Format( "Nr.%d, Level %d", GameNo , level /*, pApp->LevelToString( level )*/ );
		PathName += title;
		SetPathName( PathName );
		SetTitle( title );
	}
}
