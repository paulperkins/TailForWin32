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
 * $Id: WorkspaceDlg.h,v 1.3 2003/11/21 14:04:19 paulperkins Exp $
 * 
 */

#if !defined(AFX_WORKSPACEDLG_H__D4910840_04CA_4061_8F85_F2B79C886889__INCLUDED_)
#define AFX_WORKSPACEDLG_H__D4910840_04CA_4061_8F85_F2B79C886889__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceDlg.h : header file
//

#include "xtreectrl.h"
#include "workspace.h"

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg dialog

class CTailDoc;
class CTailView;

class CWorkspaceDlg : public CDialog
{
// Construction
public:
	CWorkspaceDlg(CWnd* pParent = NULL);   // standard constructor
	~CWorkspaceDlg (void);

  void RButton (void);
  void DblClick (void);
//  void OnRButtonDown (UINT nFlags, CPoint point);

  CXTreeCtrl  m_Tree;
  CImageList  m_stImageList;

  HTREEITEM m_htiRoot;
  HTREEITEM m_htiNonWorkspace;

  CWorkspace* m_pDefaultWorkspace;

  BOOL AddWorkspaceFile (char* pszFilename, CTailDoc* pDoc, CTailView* pView, CWorkspace* pWorkspace);
  BOOL AddWorkspaceFile (char* pszFilename, CTailView* pView, CWorkspace* pWorkspace);
  BOOL AddWorkspaceFile (char* pszFilename, CWorkspace* pWorkspace);
  BOOL RemoveWorkspaceFile (CTailDoc* pDoc, CWorkspace* pWorkspace);
  BOOL RemoveWorkspaceFile (CTailView* pView, CWorkspace* pWorkspace);
  BOOL RemoveWorkspaceFile (CTailView* pView);

  CWorkspaceItem* FindFile (CWorkspace* pWorkspace, char* pszFilename);
  CWorkspace* GetWorkspace (CWorkspaceItem* pWorkspaceItem);

  void ActivateItem (CWorkspaceItem* pItem);
  BOOL AddWorkspace (char* pszName);
  BOOL AddNewWorkspace (void);

  CWorkspace* FindWorkspace (CWorkspace* pWorkspace);
  CWorkspaceItem* FindWorkspaceItem (CWorkspaceItem* pWorkspaceItem);
  CWorkspace* FindWorkspace (long lIndex);

  CObList* m_pWorkspaces;


// Dialog Data
	//{{AFX_DATA(CWorkspaceDlg)
	enum { IDD = IDD_WORKSPACE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
  
  long m_lAnonymousWorkspaceCount;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorkspaceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWorkspaceRenameWorkspace();
	afx_msg void OnFileAddToWorkspace();
	afx_msg void OnWorkspaceAddFileToWorkspace();
	afx_msg void OnWorkspaceRemoveWorkspace();
	afx_msg void OnWorkspaceSaveWorkspace();
	afx_msg void OnWorkspaceSaveWorkspaceAs();
	afx_msg void OnFileDeleleteFromWorkspace();
	afx_msg void OnTopLevelAddNewWorkspace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEDLG_H__D4910840_04CA_4061_8F85_F2B79C886889__INCLUDED_)
