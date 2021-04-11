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
 * $Id: HotListDisplay.h,v 1.4 2003/11/12 08:53:44 paulperkins Exp $
 * 
 */

#if !defined(AFX_HOTLISTDISPLAY_H__02AD9D32_5BF9_11D3_B352_004095435FF2__INCLUDED_)
#define AFX_HOTLISTDISPLAY_H__02AD9D32_5BF9_11D3_B352_004095435FF2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HotListDisplay.h : header file
//

#include "resource.h"       // main symbols
#include "xlistctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CHotListDisplay dialog

class CHotListDisplay : public CDialog
{
// Construction
public:
	CHotListDisplay(CWnd* pParent = NULL);   // standard constructor
  char** ppszList;
  long lNumKeywords;
  CPluginList* m_pPlugins;
  CStringArray m_sa;

  void FillList ();

// Dialog Data
	//{{AFX_DATA(CHotListDisplay)
	enum { IDD = IDD_KEYWORDS };
	CXListCtrl	m_List;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHotListDisplay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHotListDisplay)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeywordDel();
	afx_msg void OnKeywordAdd();
	virtual void OnOK();
	//}}AFX_MSG
  afx_msg LRESULT OnCheckbox(WPARAM, LPARAM);
  afx_msg LRESULT OnComboSelection(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOTLISTDISPLAY_H__02AD9D32_5BF9_11D3_B352_004095435FF2__INCLUDED_)
