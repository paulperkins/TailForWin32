/*
 * Tail for Win32 - a Windows version of the UNIX 'tail -f' command.
 * 
 * Author: Paul Perkins (paul@objektiv.org.uk)
 * 
 * Copyright(c)
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
 * Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * $Id: TallyWindow.h,v 1.4 2003/11/17 07:29:46 paulperkins Exp $
 * 
 */

#if !defined(AFX_TALLYWINDOW_H__8C33AE82_A076_4BF6_8AC0_F9F07D212C5E__INCLUDED_)
#define AFX_TALLYWINDOW_H__8C33AE82_A076_4BF6_8AC0_F9F07D212C5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TallyWindow.h : header file
//

#include "resource.h"

#include "common.h"

/////////////////////////////////////////////////////////////////////////////
// TallyWindow dialog

class TallyWindow : public CDialog
{
// Construction
public:
	TallyWindow(CWnd* pParent = NULL);   // standard constructor
  void UpdateMatches (void);
  void FillList (void);
  void Update (long lNewNumKeywords, char* pszNewFilename, KEYWORD_LIST* pstNewKeywordList);

  DWORD         dwNumKeywords;
  KEYWORD_LIST* pstKeywordList;
  char*			pszFilename;
  BOOL      bInited;

// Dialog Data
	//{{AFX_DATA(TallyWindow)
	enum { IDD = IDD_TALLY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TallyWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TallyWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALLYWINDOW_H__8C33AE82_A076_4BF6_8AC0_F9F07D212C5E__INCLUDED_)
