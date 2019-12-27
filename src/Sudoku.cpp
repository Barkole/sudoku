// Sudoku.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Sudoku.h"
#include "Paths.h"

#include "MainFrm.h"
#include "SudokuDoc.h"
#include "SudokuView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __int64 RandNum64;

/////////////////////////////////////////////////////////////////////////////
// CSudokuApp

BEGIN_MESSAGE_MAP(CSudokuApp, CWinApp)
	//{{AFX_MSG_MAP(CSudokuApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSudokuApp construction

DWORD CSudokuApp::m_NextGameId = 0;

CSudokuApp::CSudokuApp()
{
	m_MaxStepLevel = 4;
	m_MaxCreateLevel = 4;
	m_MinCellCnt = 26;
	m_Version = 1;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSudokuApp object

CSudokuApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSudokuApp initialization

BOOL CSudokuApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(9);  // Load standard INI file options (including MRU)

	CreateUserPath( m_UserPath );

	if( !LoadUserprofile() )
	{
		m_MaxStepLevel = 2;
		m_MaxCreateLevel = 2;
		m_MinCellCnt = 22;
		m_NextGameId = 0;
	}

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSudokuDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSudokuView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSudokuApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSudokuApp message handlers

BOOL CSudokuApp::SaveUserprofile() 
{
	CString PathName = GetUserPath();
	PathName += "Sudoku.usp";

	CFile file;
	if( !file.Open( PathName , CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite ) )
		return FALSE;

	CArchive ar( &file, CArchive::store );

	ar << m_Version;

	ar.Write( &m_MaxStepLevel , sizeof(int) );
	ar.Write( &m_MaxCreateLevel , sizeof(int) );
	ar.Write( &m_MinCellCnt , sizeof(int) );
	ar.Write( &m_NextGameId , sizeof(DWORD) );
	
	DWORD time = GetTime();
	ar << time;

	ar.Close();
	file.Close();

	return TRUE;
}

BOOL CSudokuApp::LoadUserprofile() 
{
	CString PathName = GetUserPath();
	PathName += "Sudoku.usp";

	CFile file;
	if( !file.Open( PathName , CFile::modeRead | CFile::shareDenyWrite ) )
		return FALSE;

	CArchive ar( &file, CArchive::load );

	int Version;

	ar >> Version;
	ar.Read( &m_MaxStepLevel , sizeof(int) );
	ar.Read( &m_MaxCreateLevel , sizeof(int) );
	ar.Read( &m_MinCellCnt , sizeof(int) );
	ar.Read( &m_NextGameId , sizeof(DWORD) );
	
	//init RandNum64 from CurTime and oldtime
	DWORD oldtime;
	ar >> oldtime;
	DWORD CurTime = GetTime();
	__int64 x = oldtime * (__int64)(0x10000000);
	RandNum64 = x + CurTime;

	ar.Close();
	file.Close();

	return TRUE;
}

int CSudokuApp::ExitInstance() 
{
	SaveUserprofile();
		
	return CWinApp::ExitInstance();
}

DWORD CSudokuApp::GetTime()
{
	const unsigned long CurTime = 0x38823700;// Jan 2000
	DWORD res = time( NULL );
	if( res > CurTime )
		res -= CurTime;

	return res;
}

/*static*/ LPCTSTR CSudokuApp::LevelToString( int level )
{
	switch( level )
	{
	case 1:
		return "leicht";
	case 2:
		return "locker";
	case 3:
		return "mittel";
	case 4:
		return "anspruchsvoll";
	case 5:
		return "schwer";
	case 6:
		return "sehr schwer";
	case 7:
		return "extrem schwer";
	default:
		ASSERT(0);
		return "";
	}
}