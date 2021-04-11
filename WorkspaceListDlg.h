#if !defined(AFX_WORKSPACELISTDLG_H__03139FC7_3A34_43B5_A338_E19E09E623DE__INCLUDED_)
#define AFX_WORKSPACELISTDLG_H__03139FC7_3A34_43B5_A338_E19E09E623DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceListDlg dialog

class CWorkspaceListDlg : public CDialog
{
// Construction
public:
	CWorkspaceListDlg(CWnd* pParent = NULL);   // standard constructor
	~CWorkspaceListDlg (void);

  void FillList (void);


  CObList* m_pItems;

// Dialog Data
	//{{AFX_DATA(CWorkspaceListDlg)
	enum { IDD = IDD_WORKSPACE_LIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorkspaceListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACELISTDLG_H__03139FC7_3A34_43B5_A338_E19E09E623DE__INCLUDED_)
