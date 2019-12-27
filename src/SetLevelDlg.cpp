// SetLevelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sudoku.h"
#include "SetLevelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetLevelDlg dialog


CSetLevelDlg::CSetLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetLevelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetLevelDlg)
	m_strMinCells = _T("");
	//}}AFX_DATA_INIT

	m_StepLevel = 0;
	m_CreateLevel = 0;
	m_MinCellCnt = 0;
}


void CSetLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetLevelDlg)
	DDX_Control(pDX, IDC_COMBO_STEP, m_ComboStep);
	DDX_Control(pDX, IDC_COMBO_CREATE, m_ComboCreate);
	DDX_Text(pDX, IDC_EDIT_MIN_CELLS, m_strMinCells);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetLevelDlg, CDialog)
	//{{AFX_MSG_MAP(CSetLevelDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetLevelDlg message handlers

/*void CSetLevelDlg::InitValDescPairs()
{
	int i;
	for( i = 0 ; i < MAX_LEVEL_CNT ; i++ )
	{
		m_ValDesc[i].level = i+1;
	}

	m_ValDesc[0].desc = "Es gibt immer(außer am Schluss) mindestens 5 Zellen, die mit einfachen Methoden gefüllt werden können";
	m_ValDesc[1].desc = "Es gibt immer(außer am Schluss) mindestens 3 Zellen, die mit einfachen Methoden gefüllt werden können";
	m_ValDesc[2].desc = "Es gibt immer(außer am Schluss) mindestens 2 Zellen, die mit einfachen Methoden gefüllt werden können";
	m_ValDesc[3].desc = "Es gibt immer mindestens 1 Zelle, die mit einfachen Methoden gefüllt werden kann";
	m_ValDesc[4].desc = "Indirekte Schlüsse oder einfacher Paarausschluss notwendig";
	m_ValDesc[5].desc = "2x2 Gruppenzerlegung notwendig";
	m_ValDesc[6].desc = "2x3 Gruppenzerlegung oder Kombination der Methoden aus Level 5/6";
	m_ValDesc[7].desc = "Kompliziertere Gruppenzerlegung bei weniger als 30 gesetzten Feldern oder wiederholtes ineinander einsetzen der Methoden aus level 4/5/6 oder Kreisschlüsse";
	m_ValDesc[8].desc = "Computer probiert alles aus";
}*/

BOOL CSetLevelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//InitValDescPairs();

	CSudokuApp*		pApp = (CSudokuApp*)AfxGetApp();
	if( !pApp )
		return TRUE;
	
	//fill list
	int i;
	CString	desc;
	CString line;
	int		idx;
	for( i = 0 ; i < MAX_LEVEL_CNT ; i++ )
	{
		line.Format( "Level %d (%s)" , i+1 , pApp->LevelToString( i+1 ) );
		idx = m_ComboStep.AddString( line );
		m_ComboStep.SetItemData( idx , i+1 );
		idx = m_ComboCreate.AddString( line );
		m_ComboCreate.SetItemData( idx , i+1 );
	}

	/*sValDescPair* pPair;
	for( i = 0 ; i < MAX_LEVEL_CNT ; i++ )
	{
		pPair = (sValDescPair*)m_List.GetItemDataPtr( i );
		if( pPair && pPair->level == m_Level )
		{
			m_List.SetCurSel( i );
			m_Desc = pPair->desc;
			break;
		}
	}*/

	int level;
	for( i = 0 ; i < MAX_LEVEL_CNT ; i++ )
	{
		level = m_ComboStep.GetItemData( i );

		if( level == m_StepLevel )
		{
			m_ComboStep.SetCurSel( i );
		}
		if( level == m_CreateLevel )
		{
			m_ComboCreate.SetCurSel( i );
		}
	}

	m_strMinCells.Format( "%d" , m_MinCellCnt );

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*void CSetLevelDlg::OnSelchangeList() 
{
	UpdateData();
	int idx = m_List.GetCurSel();
	if( idx < 0 )
	{
		m_Desc.Empty();
		UpdateData( FALSE );
		return;
	}

	sValDescPair* pPair = (sValDescPair*)m_List.GetItemDataPtr( idx );
	if( pPair )
	{
		m_Desc = pPair->desc;
		UpdateData( FALSE );
	}
}*/

void CSetLevelDlg::OnOK() 
{
	UpdateData();

	int idx;

	idx = m_ComboStep.GetCurSel();
	if( idx > -1 )
	{
		m_StepLevel = m_ComboStep.GetItemData( idx );
	}

	idx = m_ComboCreate.GetCurSel();
	if( idx > -1 )
	{
		m_CreateLevel = m_ComboCreate.GetItemData( idx );
	}

	m_MinCellCnt = atoi( (LPCTSTR)m_strMinCells );
	
	CDialog::OnOK();
}
