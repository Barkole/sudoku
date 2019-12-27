// DirDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"

#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT	iMSGHelp;
static UINT	iMSGFind;
static UINT	iMSGShareViolation;
static UINT	iMSGFileOK;
static UINT	iMSGListboxChange;
static UINT	iMSGColorOK;
static UINT	iMSGSetRGB;

/////////////////////////////////////////////////////////////////////////////
// CDirDialog

IMPLEMENT_DYNAMIC(CDirDialog, CFileDialog)

static CDirDialog*	pDlg = NULL;
static BOOL			bFirstTime = TRUE;

static UINT APIENTRY OFNHookProcOldStyle(
	HWND	/*hdlg*/,	// handle to dialog box window
	UINT	uiMsg,	// message identifier
	WPARAM	wParam,	// message parameter
	LPARAM	lParam	// message parameter
	)
{
	LPOPENFILENAME		pOFN;
	int					i, iItem;
	char				buf[MAX_PATH];

	if (uiMsg == WM_INITDIALOG) 
	{
		pOFN = (LPOPENFILENAME)lParam;
		pDlg = (CDirDialog*)pOFN->lCustData;
		pDlg->GetDlgItem(stc1)->ShowWindow(SW_HIDE);
		pDlg->GetDlgItem(stc2)->ShowWindow(SW_HIDE);
		pDlg->GetDlgItem(stc3)->ShowWindow(SW_HIDE);
		pDlg->GetDlgItem(edt1)->ShowWindow(SW_HIDE);
		pDlg->GetDlgItem(lst1)->ShowWindow(SW_HIDE);
		pDlg->GetDlgItem(cmb1)->ShowWindow(SW_HIDE);
		if (pOFN->lpstrFile[0])
			pDlg->m_path = pOFN->lpstrFile;
		pDlg->SetDlgItemText(edt2, pDlg->m_path);
		pDlg->SetDlgItemText(edt1, "junk");
		return 0;
	}

	if (uiMsg == iMSGFileOK) 
	{
		pOFN = (LPOPENFILENAME)lParam;
		pDlg->GetDlgItemText(edt2, pOFN->lpstrFile, pOFN->nMaxFile);
		if (pOFN->lpstrFile[0])
			return 0;
		else
			return 1;
	}

	if (uiMsg == iMSGListboxChange && pDlg) 
	{
		if ((UINT)wParam == cmb2) 
		{
			pDlg->m_path = "";
			pDlg->SendDlgItemMessage(lst2, LB_GETTEXT, 0, (LPARAM)(LPCTSTR)buf);
			pDlg->m_path += buf;
			if (buf[0] == '\\')
				pDlg->m_path += '\\';
			pDlg->SetDlgItemText(edt2, pDlg->m_path);
			pDlg->SetDlgItemText(edt1, "junk");
		}
		if ((UINT)wParam == lst2) {
			iItem = LOWORD(lParam);
			pDlg->m_path = "";
			for (i=0;i <= iItem;i++) {
				pDlg->SendDlgItemMessage(lst2, LB_GETTEXT, i, (LPARAM)(LPCTSTR)buf);
				pDlg->m_path += buf;
				if (i == 0 && buf[0] == '\\')
					pDlg->m_path += '\\';
				if (i != iItem && i != 0)
					pDlg->m_path += '\\';
			}
			pDlg->SetDlgItemText(edt2, pDlg->m_path);
			pDlg->SetDlgItemText(edt1, "junk");
		}
		return 1;
	}

	return 0;
}

CDirDialog::CDirDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd):
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags | OFN_ENABLETEMPLATE | OFN_OVERWRITEPROMPT, lpszFilter, pParentWnd)
{
	if (bFirstTime == TRUE) {
		bFirstTime = FALSE;
		iMSGHelp			= RegisterWindowMessage(HELPMSGSTRING);  //All Dialogs
		iMSGFind			= RegisterWindowMessage(FINDMSGSTRING);  //Find & Replace
		iMSGShareViolation	= RegisterWindowMessage(SHAREVISTRING);  //Open & Save
		iMSGFileOK			= RegisterWindowMessage(FILEOKSTRING);   //Open & Save
		iMSGListboxChange	= RegisterWindowMessage(LBSELCHSTRING);  //Open & Save
		iMSGColorOK			= RegisterWindowMessage(COLOROKSTRING);  //Color
		iMSGSetRGB			= RegisterWindowMessage(SETRGBSTRING);   //Color
	}

	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_DIR);
	m_ofn.hInstance = AfxGetResourceHandle();

	m_ofn.Flags &= ~OFN_EXPLORER;
	m_ofn.Flags |= OFN_ENABLEHOOK;
	m_ofn.lpfnHook = OFNHookProcOldStyle;
	m_ofn.lCustData = (long)this;

	m_path = m_ofn.lpstrFile;
	m_ofn.lpstrFile[0] = 0;
	pDlg = NULL;
}

BEGIN_MESSAGE_MAP(CDirDialog, CFileDialog)
	//{{AFX_MSG_MAP(CDirDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowser dialog

BOOL CBrowser::Browse(CString& strPath, BOOL bUncAllowed)
{
	char		szPath[MAX_PATH + 1];
	BOOL		bRes = FALSE;

	CString		title;
	lstrcpy(szPath, LPCTSTR(strPath.Left(MAX_PATH)));

	CDirDialog	dlg(TRUE, NULL, szPath,
					OFN_HIDEREADONLY |
					OFN_OVERWRITEPROMPT |
					OFN_NOCHANGEDIR,
					NULL, NULL);

	title.LoadString(IDS_CHOOSE_DIR);
	
	dlg.m_ofn.nMaxFile = MAX_PATH;
	dlg.m_ofn.lpstrTitle = title;

	if (dlg.DoModal() == IDOK) {
		CString strTmp = dlg.GetPathName();
		char c = strTmp[0];
		if (bUncAllowed == FALSE) {
			if (((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) && (strTmp[1] == ':')) {
				strPath = strTmp;
				bRes = TRUE;
			} else {

				AfxMessageBox(IDS_NO_UNC);

			}
		} else {
			strPath = strTmp;
			bRes = TRUE;
		}
	}

	return bRes;
}

