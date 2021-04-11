#if !defined(AFX_EVENTDISPLAY_H__BD3333E0_A4BD_11D3_A51F_004095404B58__INCLUDED_)
#define AFX_EVENTDISPLAY_H__BD3333E0_A4BD_11D3_A51F_004095404B58__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EventDisplay.h : header file
//

#include "resource.h"       // main symbols

#define SEVERITY_FATAL    0
#define SEVERITY_WARNING  1
#define SEVERITY_INFO     2


/////////////////////////////////////////////////////////////////////////////
// CEventDisplay dialog

class CEventDisplay : public CDialog
{
// Construction
public:
	CEventDisplay(CWnd* pParent = NULL);   // standard constructor

  BOOL AddEvent (int iSeverity, char* pszText, char* pszKeyword); 

  long lMaxItem;

// Dialog Data
	//{{AFX_DATA(CEventDisplay)
	enum { IDD = IDD_EVENT_DISPLAY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventDisplay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
  CListCtrl* m_pListCtrl;
  CImageList m_stImageList;

  int iRedIcon;
  int iAmberIcon;
  int iGreenIcon;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventDisplay)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTDISPLAY_H__BD3333E0_A4BD_11D3_A51F_004095404B58__INCLUDED_)
