#if !defined(AFX_SETLEVELDLG_H__8229C247_F58D_4D12_ACD7_26A76B5CF43B__INCLUDED_)
#define AFX_SETLEVELDLG_H__8229C247_F58D_4D12_ACD7_26A76B5CF43B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetLevelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetLevelDlg dialog
/* initially there was a multiline description displayed, which explains each level in detail.
But i am too lazy to maintain at every rework all description texts and think that user will develop a feeling
what every level means...
For possible later reusage all the desc-code i only commented out.
struct sValDescPair
{
	int		level;
	CString desc;
};*/

#define MAX_LEVEL_CNT 7

class CSetLevelDlg : public CDialog
{
// Construction
public:
	CSetLevelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetLevelDlg)
	enum { IDD = IDD_SET_LEVEL };
	CComboBox	m_ComboStep;
	CComboBox	m_ComboCreate;
	CString		m_strMinCells;
	//}}AFX_DATA

	/*sValDescPair	m_ValDesc[MAX_LEVEL_CNT];

	void			InitValDescPairs();*/

	int				m_StepLevel;
	int				m_CreateLevel;
	int				m_MinCellCnt;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetLevelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetLevelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETLEVELDLG_H__8229C247_F58D_4D12_ACD7_26A76B5CF43B__INCLUDED_)
