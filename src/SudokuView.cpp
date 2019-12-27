// SudokuView.cpp : implementation of the CSudokuView class
//

#include "stdafx.h"
#include "Sudoku.h"

#include "SudokuDoc.h"
#include "SudokuView.h"
#include "Cell.h"
#include "DrawingCell.h"
#include "DrwSubCell.h"
#include "SudokuGame.h"
#include "SetLevelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSudokuView

IMPLEMENT_DYNCREATE(CSudokuView, CView)

BEGIN_MESSAGE_MAP(CSudokuView, CView)
	//{{AFX_MSG_MAP(CSudokuView)
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_SETSTARTPOS, OnSetstartpos)
	ON_COMMAND(ID_NEXT_NUMBER, OnNextNumber)
	ON_UPDATE_COMMAND_UI(ID_NEXT_NUMBER, OnUpdateNextNumber)
	ON_COMMAND(ID_COMPUTERSOLVE, OnComputerSolve)
	ON_COMMAND(ID_LEVEL, OnSetLevel)
	ON_COMMAND(ID_FULLSOLVE, OnFullSolve)
	ON_COMMAND(ID_RESET_TO_INITIAL, OnResetToInitial)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSudokuView construction/destruction

CSudokuView::CSudokuView()
{
	m_pSelectedCell = NULL;
	m_pCurrentCell = NULL;
	m_pGame = NULL;
	m_ScreenWidth = 0;
	m_ScreenHeight = 0;
	m_OutPutIdx = -1;
	m_PrintingFontFactor = 1.0;
}

CSudokuView::~CSudokuView()
{
}

BOOL CSudokuView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSudokuView drawing

void CSudokuView::OnDraw(CDC* pDC)
{
	if( pDC->m_bPrinting )
	{
		int PaperWidth = pDC->GetDeviceCaps( HORZRES );
		int PaperHeight = pDC->GetDeviceCaps( VERTRES );
		SetSize( PaperWidth , PaperHeight , true );
		m_PrintingFontFactor = 1.0;
		if( m_ScreenWidth && m_ScreenHeight )
			m_PrintingFontFactor = 1.35 * __min( PaperWidth/m_ScreenWidth , PaperHeight/m_ScreenHeight );
		Draw( pDC );
		SetSize( m_ScreenWidth , m_ScreenHeight , false );
		return;
	}

	Draw( pDC );
}

void CSudokuView::Draw( CDC* pDC )
{
	CSudokuDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	//draw lattice:
	int i;
	//fat lines:
	int PenWidth = 3;
	if( pDC->IsPrinting() )
		PenWidth = ftoint( PenWidth * m_PrintingFontFactor );

	CPen penThick( PS_SOLID , PenWidth , RGB(0,0,0) );
	CPen* pOldPen = pDC->SelectObject( &penThick );
	CPoint	pt1 = m_Origin;
	CPoint	pt2 = m_Origin;
	pt2.y += ROW_CNT * m_Scale;
	int dx = COLS_IN_FIELD * m_Scale;
	for( i = -1 ; i < HOR_FIELD_CNT ; i++ )
	{
		pDC->MoveTo( pt1 );
		pDC->LineTo( pt2 );
		pt1.x += dx;
		pt2.x += dx;	
	}
	pt1 = m_Origin;
	pt2 = m_Origin;
	pt2.x += COL_CNT * m_Scale;
	int dy = ROWS_IN_FIELD * m_Scale;
	for( i = -1 ; i < VER_FIELD_CNT ; i++ )
	{
		pDC->MoveTo( pt1 );
		pDC->LineTo( pt2 );
		pt1.y += dy;
		pt2.y += dy;	
	}

	//thinn lines:
	PenWidth = 1;
	if( pDC->IsPrinting() )
		PenWidth = ftoint( PenWidth * m_PrintingFontFactor );
	CPen penThinn( PS_SOLID , PenWidth , RGB(0,0,0) );
	pDC->SelectObject( &penThinn );
	pt1 = m_Origin;
	pt2 = m_Origin;
	pt2.y += ROW_CNT * m_Scale;
	dx = m_Scale;
	for( i = 1 ; i < COL_CNT ; i++ )
	{
		pt1.x += dx;
		pt2.x += dx;
		if( i % VER_FIELD_CNT == 0 )
			continue;
		pDC->MoveTo( pt1 );
		pDC->LineTo( pt2 );	
	}
	pt1 = m_Origin;
	pt2 = m_Origin;
	pt2.x += COL_CNT * m_Scale;
	dy =  m_Scale;
	for( i = 1 ; i < ROW_CNT ; i++ )
	{
		pt1.y += dy;
		pt2.y += dy;
		if( i % HOR_FIELD_CNT == 0 )
			continue;
		pDC->MoveTo( pt1 );
		pDC->LineTo( pt2 );	
	}

	LOGFONT logfont;
	CFont* pOldFont = pDC->GetCurrentFont();
	pOldFont->GetLogFont( &logfont );
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfQuality = NONANTIALIASED_QUALITY;
	pDC->SetTextAlign( TA_CENTER | TA_TOP );

	//big Font:
	CFont BigFont;
	logfont.lfWidth = 0;
	logfont.lfHeight = 56;
	if( pDC->IsPrinting() )
		logfont.lfHeight = ftoint( logfont.lfHeight * m_PrintingFontFactor );

	logfont.lfWeight = 400;
	strcpy( logfont.lfFaceName, "Arial" );        
	BigFont.CreateFontIndirect( &logfont );

	//small font
	CFont SmallFont;
	logfont.lfWidth = 0;
	logfont.lfHeight = 16;
	if( pDC->IsPrinting() )
		logfont.lfHeight = ftoint( logfont.lfHeight * m_PrintingFontFactor );
	logfont.lfWeight = 400;
	strcpy( logfont.lfFaceName, "Arial" );        
	SmallFont.CreateFontIndirect( &logfont );

	//draw the fields
	std::list<CDrawingCell*>::iterator	it;
	CDrawingCell*					pDrwCell;
	for( it = m_listCells.begin() ; it != m_listCells.end() ; it++ )
	{
		pDrwCell = *it;

		if( pDrwCell->IsSubCell() )
			pDC->SelectObject( &SmallFont );
		else
			pDC->SelectObject( &BigFont );

		pDrwCell->Draw( pDC , pDrwCell == m_pSelectedCell , pDrwCell == m_pCurrentCell );
	}

	//output:
	if( !pDC->IsPrinting() )
	{
		CToPlay* pToPlay = m_pGame ? m_pGame->GetOutput( m_OutPutIdx ) : NULL;
		if( pToPlay )
			m_OutputText = pToPlay->GetDesc();
		else
			m_OutputText.Empty();

		if( !m_OutputText.IsEmpty() )
		{
			CFont OutputFont;
			logfont.lfWidth = 0;
			logfont.lfHeight = 20;
			logfont.lfWeight = 400;
			strcpy( logfont.lfFaceName, "Arial" );        
			OutputFont.CreateFontIndirect( &logfont );
			pDC->SelectObject( &OutputFont );
			pDC->SetTextAlign( TA_LEFT | TA_BOTTOM );
			pDC->TextOut( m_OutputTopLeft.x , m_OutputTopLeft.y , m_OutputText );
		}	
	}

	//title:
	if( pDoc )
	{
		CString title = pDoc->GetTitle();
		if( !title.IsEmpty() )
		{
			int len = title.GetLength();
			if( len > 4 && title.Right( 4 ) == ".sdk" )
				title = title.Left( len - 4 );

			CFont TitleFont;
			logfont.lfWidth = 0;
			logfont.lfHeight = 36;
			if( pDC->IsPrinting() )
				logfont.lfHeight = ftoint( logfont.lfHeight * m_PrintingFontFactor );
			logfont.lfWeight = 600;
			strcpy( logfont.lfFaceName, "Arial" );        
			TitleFont.CreateFontIndirect( &logfont );
			pDC->SelectObject( &TitleFont );
			pDC->SetTextAlign( TA_CENTER | TA_BOTTOM );
			pDC->TextOut( m_TitleBottomCenter.x , m_TitleBottomCenter.y , title );
		}
	}

	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldFont );
}

/////////////////////////////////////////////////////////////////////////////
// CSudokuView printing

BOOL CSudokuView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSudokuView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSudokuView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSudokuView diagnostics

#ifdef _DEBUG
void CSudokuView::AssertValid() const
{
	CView::AssertValid();
}

void CSudokuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSudokuDoc* CSudokuView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSudokuDoc)));
	return (CSudokuDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSudokuView message handlers

void CSudokuView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( !m_PlayRect.PtInRect( point ) )
	{
		SetCell( NULL );
		return;
	}

	int col = ( point.x - m_PlayRect.left ) / m_Scale;
	int row = ( point.y - m_PlayRect.top ) / m_Scale;
	if( col < 0 || col >= COL_CNT || row < 0 || row >= ROW_CNT )
	{
		SetCell( NULL );
		return;
	}

	CDrawingCell* pDrwCell = GetCell( col , row );
	if( !pDrwCell )
	{
		ASSERT(0);
		SetCell( NULL );
		return;
	}

	CDrwSubCell* pSubDrwCell = GetSubCell( col , row );
	if( !pSubDrwCell )
	{
		ASSERT(0);
		SetCell( NULL );
		return;
	}

	CDrawingCell* pNewCell = pDrwCell;

	CCell* pCell = pDrwCell->GetDataCell();
	if( pCell && pCell->IsFree() )
	{
		CRect* pRect = pSubDrwCell->GetRect();
		if( pRect && pRect->PtInRect( point ) )
			pNewCell = pSubDrwCell;
	}

	SetCell( pNewCell );
	
	CView::OnMouseMove(nFlags, point);
}

void CSudokuView::SetCell( CDrawingCell* pNewCell )
{
	if( pNewCell == m_pSelectedCell )
		return;

	if( m_pSelectedCell )
	{
		CRect rect = *m_pSelectedCell->GetRect();
		IncreaseRect( rect );
		InvalidateRect( &rect );
	}

	m_pSelectedCell = pNewCell;

	if( m_pSelectedCell )
	{
		CRect rect = *m_pSelectedCell->GetRect();
		IncreaseRect( rect );
		InvalidateRect( &rect );
	}
}

void CSudokuView::OnSize( UINT nType, int cx, int cy ) 
{
	CView::OnSize(nType, cx, cy);

	m_ScreenWidth = cx;
	m_ScreenHeight = cy;

	SetSize( cx , cy , false );
}

void CSudokuView::SetSize( int cx , int cy , bool bPrinting ) 
{
	if( !m_pGame )
		return;
	
	//calc scale
	//borders:
	//3 units at top + 4 at bottom (room left for output)
	//2 units on the right and left side
	int dx = 1 + COL_CNT;
	int dy = 2 + ROW_CNT;
	if( bPrinting )
	{
		dx += 1;
		dy += 1;
	}

	double ScaleX = cx / dx;
	double ScaleY = cy / dy;
	m_Scale = __min( ScaleX , ScaleY );//force isotropic scaling ( m_ScaleX == m_ScaleY )

	//set offset:
	long& OriginX = m_Origin.x;
	long& OriginY = m_Origin.y;
	OriginX = ( cx - COL_CNT * m_Scale ) / 2;
	OriginY = ( cy - ROW_CNT * m_Scale ) / 2;

	//set PlayRect:
	m_PlayRect.left = OriginX;
	m_PlayRect.right = OriginX + COL_CNT * m_Scale;
	m_PlayRect.top = OriginY - ftoint( 0.2 * m_Scale );
	m_PlayRect.bottom = OriginY + ROW_CNT * m_Scale - ftoint( 0.2 * m_Scale );

	//set the rects of the fields
	std::list<CDrawingCell*>::iterator	it;
	CDrawingCell*					pDrwCell;
	CRect							rect;
	CCell*							pCell;
	int								col, row;
	double							SubShiftX = 0.25;
	double							SubShiftYtop = 0.08;
	double							SubShiftYbot = 0.25;
	for( it = m_listCells.begin() ; it != m_listCells.end() ; it++ )
	{
		pDrwCell = *it;
		pCell = pDrwCell->GetDataCell();
		if( !pCell )
		{
			ASSERT(0);
			continue;
		}

		col = pCell->m_Col;
		row = pCell->m_Row;

		if( pDrwCell->IsSubCell() )
		{
			rect.top = OriginY + ftoint( m_Scale * ( SubShiftYtop + row ) );
			rect.bottom = OriginY + ftoint( m_Scale * ( SubShiftYbot + row ) );
			rect.left = OriginX + ftoint( m_Scale * ( SubShiftX + col ) );
			rect.right = OriginX + ftoint( m_Scale * ( -SubShiftX + col + 1 ) );
		}
		else
		{
			rect.top = OriginY + m_Scale * row;
			rect.bottom = rect.top + m_Scale;
			rect.left = OriginX + m_Scale * col;
			rect.right = rect.left + m_Scale;	
		}

		pDrwCell->SetRect( rect );
	}

	m_OutputTopLeft.x = m_Origin.x;
	m_OutputTopLeft.y = m_Origin.y + ftoint( (0.5 + ROW_CNT) * m_Scale );

	m_TitleBottomCenter.x = m_Origin.x + ftoint( 0.5 * ROW_CNT * m_Scale );
	m_TitleBottomCenter.y = m_Origin.y - ftoint( 0.25 * m_Scale );
}

void CSudokuView::SetGame( CSudokuGame* pGame )
{
	m_pGame = pGame;
	
	m_listCells.clear();
	m_OutPutIdx = -1;
	m_pCurrentCell = NULL;

	CCell* pCell;
	int val;

	int i, j;
	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			if( pGame )
			{
				pCell = pGame->GetCell( i , j );
				m_DrwCells[i][j].SetDataCell( pCell );
				if( pCell )
				{
					val = pCell->GetVal();
					if( val )
						m_DrwCells[i][j].m_Text.Format( "%d" , val );
					else
						m_DrwCells[i][j].m_Text.Empty();
				}
				else
					m_DrwCells[i][j].m_Text.Empty();

				m_DrwSubCells[i][j].SetDataCell( pCell );
				m_DrwSubCells[i][j].m_Text.Empty();
			}
			m_listCells.push_back( &m_DrwCells[i][j] );
			m_listCells.push_back( &m_DrwSubCells[i][j] );
		}
	}
}

void CSudokuView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	CSudokuDoc* pDoc = GetDocument();
	if( !pDoc )
		SetGame( NULL );
	else
		SetGame( pDoc->GetGame() );	
}

void CSudokuView::IncreaseRect( CRect& rect )
{
	const int add = 3;
	rect.right += add;
	rect.left -= add;
	rect.top -= add;
	rect.bottom += add;
}

void CSudokuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( m_pSelectedCell )
	{
		if( nChar == VK_DELETE )
		{
			if( !m_pSelectedCell->m_Text.IsEmpty() && m_pGame )
			{
				m_pGame->ResetMovesToMake();
				m_pGame->ResetForbidden();
			}

			m_pSelectedCell->m_Text = "";
			InvalidateRect( m_pSelectedCell->GetRect() );
			if( !m_pSelectedCell->IsSubCell() )
			{
				CCell* pCell = m_pSelectedCell->GetDataCell();
				if( pCell )
					pCell->Erase();
			}
		}
		else
		{
			char c = (char)nChar;
			char buf[2];
			buf[0] = c;
			buf[1] = 0;
			int val = atoi( buf );
			m_pGame->ResetForbidden();

			if( val > 0 && val <= VAL_CNT )
			{
				CCell* pCell = m_pSelectedCell->GetDataCell();
				if( pCell && pCell->IsFree( val , NULL , false ) )
				{
					InvalidateRect( m_pSelectedCell->GetRect() );
					m_pCurrentCell = NULL;
					if( m_pSelectedCell->IsSubCell() )
					{
						if( -1 == m_pSelectedCell->m_Text.Find( c ) )
						{
							m_pSelectedCell->m_Text += c;	
						}
					}
					else
					{
						if( m_pSelectedCell->m_Text.IsEmpty() && m_pGame )
							m_pGame->ResetMovesToMake();
						m_pSelectedCell->m_Text = c;
						pCell->SetVal( val );
					}
				}	
			}
		}
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSudokuView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CSudokuDoc* pDoc = GetDocument();

	switch( lHint )
	{
	case UH_NEW_GAME:
		if( !pDoc )
			SetGame( NULL );
		else
			SetGame( pDoc->GetGame() );
		LoadSubStrings();
		m_OutputText.Empty();
		Invalidate();
		break;

	case UH_SAVE_SUBSTRINGS:
		SaveSubStrings();
		break;

	case UH_REDRAW:
		Invalidate();
	}		
}

void CSudokuView::SaveSubStrings()
{
	CSudokuDoc* pDoc = GetDocument();

	int i, j;
	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			pDoc->m_SubStrings[i][j] = m_DrwSubCells[i][j].m_Text;
		}
	}
}

void CSudokuView::LoadSubStrings()
{
	CSudokuDoc* pDoc = GetDocument();

	int i, j;
	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			m_DrwSubCells[i][j].m_Text = pDoc->m_SubStrings[i][j];
		}
	}
}

void CSudokuView::OnSetstartpos() 
{
	if( !m_pGame )
		return;

	m_pGame->SetAsInitialPos();
	
	Invalidate();
}

void CSudokuView::OnNextNumber() 
{
	m_pCurrentCell = NULL;

	if( !m_pGame )
		return;

	m_OutPutIdx++;
	CToPlay* pCur = m_pGame->GetOutput( m_OutPutIdx );
	if( !pCur )
		return;

	std::list<CDrawingCell*>::iterator	it;
	CCell*								pCell = pCur->GetCell();
	CDrawingCell*						pDrwCell;

	if( pCell )
	{
		pCell->SetVal( pCur->GetVal() );
		for( it = m_listCells.begin() ; it != m_listCells.end() ; it++ )
		{
			pDrwCell = *it;
			if( !pDrwCell->IsSubCell() && pDrwCell->GetDataCell() == pCell )
			{
				m_pCurrentCell = pDrwCell;
				m_pCurrentCell->m_Text.Format( "%d" , pCell->GetVal() );
				break;
			}
		}
	}

	CForbidAction* pAction = pCur->GetAction();
	if( pAction )
	{
		std::vector<sValCellPair>::iterator	jt;
		for( jt = pAction->ForbiddenValues.begin() ; jt != pAction->ForbiddenValues.end() ; jt++ )
		{
			sValCellPair& pair = *jt;
			pCell = pair.pCell;
			for( it = m_listCells.begin() ; it != m_listCells.end() ; it++ )
			{
				pDrwCell = *it;
				if( pDrwCell->IsSubCell() && pDrwCell->GetDataCell() == pCell )
				{
					pDrwCell->m_Text.Format( "%s" , (LPCTSTR)pAction->GetForbiddenValStr() );
					break;
				}
			}
		}
	}

	Invalidate();
}

void CSudokuView::OnUpdateNextNumber(CCmdUI* pCmdUI) 
{
	if( m_pGame )
	{
		int size = m_pGame->m_MovesToMake.size();
		if( size > 0 && m_OutPutIdx < size - 1 )
		{
			pCmdUI->Enable();
			return;
		}
	}
	
	pCmdUI->Enable( FALSE );	
}

void CSudokuView::OnComputerSolve() 
{
	if( !m_pGame )
		return;

	m_OutputText.Empty();

	m_OutPutIdx = -1;

	int NumberOfSolutions = 0;
	int UsedLevel;

	CSudokuApp* pApp = (CSudokuApp*)AfxGetApp();
	if( !pApp )
	{
		ASSERT(0);
		return;
	}

	int MaxLevel = pApp->GetMaxStepLevel();

	if( m_pGame->Solve( MaxLevel , UsedLevel , NumberOfSolutions , true ) < 0 )
	{
		AfxMessageBox( "Widerspruch! Keine Lösung möglich" );
		return;
	}

	int FoundCellsCnt = m_pGame->m_MovesToMake.size();
	if( FoundCellsCnt == 0 )
	{
		AfxMessageBox( "Keine Felder auf diesem Level gefunden" );
		return;
	}
}

void CSudokuView::OnSetLevel() 
{
	CSudokuApp* pApp = (CSudokuApp*)AfxGetApp();
	if( !pApp )
	{
		ASSERT(0);
		return;
	}

	CSetLevelDlg dlg;
	dlg.m_StepLevel = pApp->GetMaxStepLevel();
	dlg.m_CreateLevel = pApp->GetMaxCreateLevel();
	dlg.m_MinCellCnt = pApp->GetMinCellCnt();

	if( dlg.DoModal() == IDOK )
	{
		pApp->SetMaxStepLevel( dlg.m_StepLevel );
		pApp->SetMaxCreateLevel( dlg.m_CreateLevel );
		pApp->SetMinCellCnt( dlg.m_MinCellCnt );
	}
}

void CSudokuView::OnFullSolve() 
{
	if( !m_pGame )
		return;

	CWaitCursor wc;

	int NumberOfSolutions = m_pGame->DoComputerSolution( false , false );
	if( NumberOfSolutions == 0 )
		m_OutputText = "Keine Lösung.";
	else if( NumberOfSolutions == 1 )
		m_OutputText = "Genau eine Lösung.";
	else//NumberOfSolutions > 1
		m_OutputText.Format( "%d Lösungen." , NumberOfSolutions );

	CSudokuDoc* pDoc = GetDocument();
	if( pDoc )
		pDoc->ClearSubStrings();

	SetGame( m_pGame );

	Invalidate();
}

void CSudokuView::OnResetToInitial() 
{
	if( m_pGame )
		m_pGame->ResetToInitialPos();

	CSudokuDoc* pDoc = GetDocument();
	if( pDoc )
		pDoc->ClearSubStrings();

	SetGame( m_pGame );

	Invalidate();
}
