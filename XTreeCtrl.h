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
 * $Id: XTreeCtrl.h,v 1.2 2003/11/21 10:27:55 paulperkins Exp $
 * 
 */

#if !defined(AFX_XTREECTRL_H__8CEF80B1_F048_4410_93E6_D38674D10989__INCLUDED_)
#define AFX_XTREECTRL_H__8CEF80B1_F048_4410_93E6_D38674D10989__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXTreeCtrl window

class CWorkspaceDlg;

class CXTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CXTreeCtrl();

  CWorkspaceDlg*  m_pWorkspaceDlg;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXTreeCtrl();

  HTREEITEM AddTreeItem (HTREEITEM htvParent, char* pszText, int nNormalImage,int nSelectedImage, void* pData);
  BOOL DelTreeItem (HTREEITEM	hti);



	// Generated message map functions
protected:
	//{{AFX_MSG(CXTreeCtrl)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XTREECTRL_H__8CEF80B1_F048_4410_93E6_D38674D10989__INCLUDED_)
