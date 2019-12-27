// DirDialog.h : header file
//

#ifndef DIR_DLG_INCLUDED
#define DIR_DLG_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// CDirDialog dialog

class CDirDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CDirDialog)

public:
	CDirDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	CString	m_path;
 
protected:
	//{{AFX_MSG(CDirDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CBrowser
{
public:
	BOOL	Browse(CString& strPath, BOOL bUncAllowed = FALSE);
};

#undef CDS_DLL

#endif // DIR_DLG_INCLUDED