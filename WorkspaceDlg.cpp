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
 * $Id: WorkspaceDlg.cpp,v 1.3 2003/11/21 14:03:33 paulperkins Exp $
 * 
 */

// WorkspaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tail.h"
#include "taildoc.h"
#include "WorkspaceDlg.h"
#include "workspacelistdlg.h"
#include "xtreectrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WORKSPACE_ICON_TOP        (0)
#define WORKSPACE_ICON_WORKSPACE  (1)
#define WORKSPACE_ICON_FOLDER     (2)
#define WORKSPACE_ICON_FILE       (3)
#define WORKSPACE_ICON_DOWN_FILE  (4)

extern CTailApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg dialog


CWorkspaceDlg::CWorkspaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkspaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkspaceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

//  m_pTree = NULL;

  m_htiRoot = 0;
  m_htiNonWorkspace = 0;
  m_lAnonymousWorkspaceCount = 0;

  m_pDefaultWorkspace = new CWorkspace;

  strcpy (m_pDefaultWorkspace->m_szName, "Non-Workspace Files");

  m_pWorkspaces = new CObList;

//  AddWorkspace (m_pDefaultWorkspace->m_szName);

  m_pWorkspaces->AddTail ((CObject*) m_pDefaultWorkspace);
}

CWorkspaceDlg::~CWorkspaceDlg (void)
{
  POSITION stPos;

  for (stPos = m_pWorkspaces->GetHeadPosition (); stPos != NULL; ) 
  {
    delete (CWorkspace*) m_pWorkspaces->GetNext (stPos);
  } 

  delete m_pWorkspaces;
}

void CWorkspaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkspaceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkspaceDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkspaceDlg)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(IDM_TN_RENAMEWORKSPACE, OnWorkspaceRenameWorkspace)
	ON_COMMAND(IDM_TI_ADD_TO_WORKSPACE, OnFileAddToWorkspace)
	ON_COMMAND(IDM_TN_ADDFILETOWORKSPACE, OnWorkspaceAddFileToWorkspace)
	ON_COMMAND(IDM_TN_REMOVEWORKSPACE, OnWorkspaceRemoveWorkspace)
	ON_COMMAND(IDM_TN_SAVEWORKSPACE, OnWorkspaceSaveWorkspace)
	ON_COMMAND(IDM_TN_SAVEWORKSPACEAS, OnWorkspaceSaveWorkspaceAs)
	ON_COMMAND(IDM_TI_DEL_FROM_WORKSPACE, OnFileDeleleteFromWorkspace)
	ON_COMMAND(IDM_TI_ADDNEWWORKSPACE, OnTopLevelAddNewWorkspace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::Create (
  LPCTSTR lpszClassName, 
  LPCTSTR lpszWindowName, 
  DWORD dwStyle, 
  const RECT& rect, 
  CWnd* pParentWnd, 
  UINT nID, 
  CCreateContext* pContext) 
{
	return CDialog::Create(IDD, pParentWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::OnInitDialog (
  void) 
{
  HTREEITEM htiWS1 = 0;

  CDialog::OnInitDialog();

	m_stImageList.Create (IDB_TREE_BUTTONS, 16, 5, RGB(255, 0, 255));

  m_Tree.SetImageList (&m_stImageList, TVSIL_NORMAL);

  m_htiRoot         = m_Tree.AddTreeItem (NULL, _T("Workspaces"), WORKSPACE_ICON_TOP, WORKSPACE_ICON_TOP, NULL);
  m_htiNonWorkspace = m_Tree.AddTreeItem (m_htiRoot, _T("Non-Workspace Files"), WORKSPACE_ICON_WORKSPACE, WORKSPACE_ICON_WORKSPACE, (void*) m_pDefaultWorkspace);

  m_Tree.Expand (m_htiRoot, TVE_EXPAND);
  m_Tree.Expand (m_htiNonWorkspace, TVE_EXPAND);

  m_Tree.m_pWorkspaceDlg = this;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnSize (
  UINT nType, 
  int cx, 
  int cy) 
{
	CRect rc;

	CDialog::OnSize(nType, cx, cy);

	GetClientRect (rc);
	rc.DeflateRect (5,5);

  m_Tree.MoveWindow (rc);	
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
int CWorkspaceDlg::OnCreate (
  LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_Tree.Create (WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT, // | TVS_EDITLABELS,
		                  CRect(0, 0, 0, 0), 
                      this, 
                      100))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;		// fail to create
	}

	m_Tree.ModifyStyleEx (0, WS_EX_CLIENTEDGE);
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::DblClick (
  void)
{
  CObject* pObject = NULL;

  HTREEITEM hItem = m_Tree.GetSelectedItem ();

	if (hItem) 
  {
		pObject = (CObject*) m_Tree.GetItemData (hItem);

    if (pObject)
    {
      if (pObject->IsKindOf (RUNTIME_CLASS (CWorkspaceItem)))
      {
        CWorkspaceItem* pItem = NULL;

  		  if (pItem = (CWorkspaceItem*) m_Tree.GetItemData (hItem))
        {
          pItem->Activate ();
        }
      }

      if (pObject->IsKindOf (RUNTIME_CLASS (CWorkspace)))
      {
        CWorkspace* pItem = NULL;

  		  if (pItem = (CWorkspace*) m_Tree.GetItemData (hItem))
        {
          pItem->Activate ();
        }
      }

    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::RButton (
  void)
{
  CWorkspaceItem* pWorkspaceItem = NULL;
  CWorkspace*     pWorkspace = NULL;
  CMenu           menu;
  CMenu*          pPopup = NULL;
  CPoint          point;
  DWORD           dwSel = 0;
  HTREEITEM       hItem = 0;
  CObject*        pObject = NULL;

  menu.LoadMenu(IDR_TREE_MENU);

  GetCursorPos (&point);

  // Work out which sub-menu to load.
  // This is a *real* bodge, but I can't be bothered to work out a 
  // more appropriate way at the moment.
  if (hItem = m_Tree.GetSelectedItem ())
  {		
    pObject = (CObject*) m_Tree.GetItemData (hItem);

    if (pObject)
    {
      if (pObject->IsKindOf (RUNTIME_CLASS (CWorkspaceItem)))
      {
        pWorkspaceItem = (CWorkspaceItem*) m_Tree.GetItemData (hItem);

        if (FindWorkspaceItem (pWorkspaceItem))
        {
          pPopup = menu.GetSubMenu(0);
        }
      }

      if (pObject->IsKindOf (RUNTIME_CLASS (CWorkspace)))
      {
		    pWorkspace = (CWorkspace*) m_Tree.GetItemData (hItem);

        if (FindWorkspace (pWorkspace))
        {
          // Not yet - it'll only end in tears!
          //pPopup = menu.GetSubMenu(1);
        }
      }
    }

    // Must be the top-level of the tree.
    if (!pObject)
    {
      pPopup = menu.GetSubMenu(2);
    }

    if (pPopup)
    {
      dwSel = pPopup->TrackPopupMenu (TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, 
                                      point.x, 
                                      point.y,
                                      this);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::AddWorkspaceFile (
  char*       pszFilename, 
  CWorkspace* pWorkspace)
{
  CTailDoc* pDoc = NULL;
  CTailView* pView = NULL;

  if (!FindFile (pWorkspace, pszFilename))
  {
    if (pDoc = (CTailDoc*) theApp.OpenDocumentFile (pszFilename))
    {
      // See if a new view was created before creating another.
//      if (!pDoc->GetActiveView ())
      {
        pDoc->ShowAnotherWindow ();
      }

      // Get the latest view for this document.
      pView = pDoc->GetActiveView ();

      AddWorkspaceFile (pszFilename, pDoc, pView, pWorkspace);
    }
  }

  return (pDoc != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::AddWorkspaceFile (
  char*       pszFilename, 
  CTailDoc*   pDoc,
  CTailView*  pView,
  CWorkspace* pWorkspace)
{
  CWorkspaceItem* pNewItem = NULL;

  // Check if the file has already ben added to this workspace.
  if (FindFile (pWorkspace, pszFilename))
  {
    return FALSE;
  }

  pNewItem = new CWorkspaceItem;

  pNewItem->m_pDoc  = pDoc;
  pNewItem->m_pView = pView;

//  pDoc->m_pWorkspace = pWorkspace;

  strcpy (pNewItem->m_szTailFile, pszFilename);

  if (!pWorkspace)
  {
    pNewItem->m_hti = m_Tree.AddTreeItem (m_htiNonWorkspace, pszFilename, WORKSPACE_ICON_FILE, WORKSPACE_ICON_FILE, (void*) pNewItem);

    m_pDefaultWorkspace->m_pItems->AddTail ((CObject*) pNewItem);
    
    m_Tree.Expand (m_htiNonWorkspace, TVE_EXPAND);
  }
  else
  {
    pNewItem->m_hti = m_Tree.AddTreeItem (pWorkspace->m_hti, pszFilename, WORKSPACE_ICON_FILE, WORKSPACE_ICON_FILE, (void*) pNewItem);

    pWorkspace->AddItem (pNewItem);
    
    m_Tree.Expand (pWorkspace->m_hti, TVE_EXPAND);
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::AddWorkspaceFile (
  char*       pszFilename, 
  CTailView*  pView,
  CWorkspace* pWorkspace)
{
  CWorkspaceItem* pNewItem = NULL;

  // Check if the file has already ben added to this workspace.
  if (FindFile (pWorkspace, pszFilename))
  {
    return FALSE;
  }

  pNewItem = new CWorkspaceItem;

  pNewItem->m_pView = pView;

  strcpy (pNewItem->m_szTailFile, pszFilename);

  if (!pWorkspace)
  {
    pNewItem->m_hti = m_Tree.AddTreeItem (m_htiNonWorkspace, pszFilename, WORKSPACE_ICON_FILE, WORKSPACE_ICON_FILE, (void*) pNewItem);

    m_pDefaultWorkspace->m_pItems->AddTail ((CObject*) pNewItem);
    
    m_Tree.Expand (m_htiNonWorkspace, TVE_EXPAND);
  }
  else
  {
    pNewItem->m_hti = m_Tree.AddTreeItem (pWorkspace->m_hti, pszFilename, WORKSPACE_ICON_FILE, WORKSPACE_ICON_FILE, (void*) pNewItem);

    pWorkspace->AddItem (pNewItem);
    
    m_Tree.Expand (pWorkspace->m_hti, TVE_EXPAND);
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::RemoveWorkspaceFile (
  CTailView* pView)
{
  POSITION stPos;
  CWorkspaceItem* pItem = NULL;
  CWorkspace* pSearch = NULL;

  // Loop over all workspaces to find the view.
  for (stPos = m_pWorkspaces->GetHeadPosition (); stPos != NULL; ) 
  {
    pSearch = (CWorkspace*) m_pWorkspaces->GetNext (stPos);

    if (pItem = pSearch->FindView (pView))
    {
      m_Tree.DelTreeItem (pItem->m_hti);

      pSearch->RemoveItem (pItem);

      delete (CWorkspaceItem*) pItem;

      return TRUE;
    }
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::RemoveWorkspaceFile (
  CTailView*  pView,
  CWorkspace* pWorkspace)
{
  CWorkspaceItem* pItem = NULL;
  CWorkspace* pSearch = NULL;

  pSearch = pWorkspace;

  if (!pWorkspace)
  {
    pSearch = m_pDefaultWorkspace;
  }

  if (pItem = pSearch->FindView (pView))
  {
    m_Tree.DelTreeItem (pItem->m_hti);

    pSearch->RemoveItem (pItem);

    delete (CWorkspaceItem*) pItem;
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::RemoveWorkspaceFile (
  CTailDoc*   pDoc,
  CWorkspace* pWorkspace)
{
  CWorkspaceItem* pItem = NULL;
  CWorkspace* pSearch = NULL;

  pSearch = pWorkspace;

  if (!pWorkspace)
  {
    pSearch = m_pDefaultWorkspace;
  }

  if (pItem = pSearch->FindDocument (pDoc))
  {
    m_Tree.DelTreeItem (pItem->m_hti);

    pSearch->RemoveItem (pItem);

    delete (CWorkspaceItem*) pItem;
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::AddWorkspace (
  char*       pszName)
{
  CWorkspace* pNewItem = NULL;

  pNewItem = new CWorkspace;

  strcpy (pNewItem->m_szName, pszName);

  pNewItem->m_hti = m_Tree.AddTreeItem (m_htiRoot, 
                                pszName, 
                                WORKSPACE_ICON_WORKSPACE, 
                                WORKSPACE_ICON_WORKSPACE, 
                                (void*) pNewItem);

//  m_DefaultWorkspace.m_pItems->AddTail ((CObject*) pNewItem);
  
  m_Tree.Expand (m_htiRoot, TVE_EXPAND);

  m_pWorkspaces->AddTail ((CObject*) pNewItem);


  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
BOOL CWorkspaceDlg::AddNewWorkspace (
  void)
{
  char szWkspName[1024] = "";

  sprintf (szWkspName, "Workspace %ld", ++m_lAnonymousWorkspaceCount);

  AddWorkspace (szWkspName);

  return TRUE;
}

/*void CWorkspaceDlg::OnTreeItemAddToWorkspace() 
{
  CWorkspaceListDlg stDlg;
  CWorkspaceItem* pItem = NULL;

  HTREEITEM hItem = m_Tree.GetSelectedItem ();

  if (hItem && (pItem = (CWorkspaceItem*) m_Tree.GetItemData (hItem)))
  {
    stDlg.DoModal ();
  }
} */

/*void CWorkspaceDlg::OnTreeItemDelFromWorkspace() 
{
  CWorkspaceItem* pItem = NULL;

  HTREEITEM hItem = m_Tree.GetSelectedItem ();

	if (hItem && (pItem = (CWorkspaceItem*) m_Tree.GetItemData (hItem)))
  {
    char szText[1024];

    sprintf (szText, pItem->GetFilename ());
    ::AfxMessageBox (szText);
  }	
} */

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
// See if this file already exists in the given workspace.
CWorkspaceItem* CWorkspaceDlg::FindFile (
  CWorkspace* pWorkspace,
  char*       pszFilename)
{
  POSITION stPos;
  CWorkspace* pSearch = NULL;
  CWorkspaceItem* pFound = NULL;

  pSearch = pWorkspace;

  if (!pWorkspace)
  {
    pSearch = m_pDefaultWorkspace;
  }

  for (stPos = pSearch->m_pItems->GetHeadPosition (); stPos != NULL; ) 
  {
    pFound = (CWorkspaceItem*) pSearch->m_pItems->GetNext (stPos);

    if (strcmp (pFound->GetFilename (), pszFilename) == 0)
    {
      return pFound;
    }
  }

  return NULL;

}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
CWorkspace* CWorkspaceDlg::FindWorkspace (
  CWorkspace* pWorkspace)
{
  POSITION stPos;
  CWorkspace* pFound = NULL;

  for (stPos = m_pWorkspaces->GetHeadPosition (); stPos != NULL; ) 
  {
    pFound = (CWorkspace*) m_pWorkspaces->GetNext (stPos);

    if (pFound == pWorkspace)
    {
      return pWorkspace;
    }
  }

  return NULL;

}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
// Find the workspace containing the given workspace item.
CWorkspace* CWorkspaceDlg::GetWorkspace (
  CWorkspaceItem* pWorkspaceItem)
{
  POSITION        stPos;
  CWorkspace*     pFoundWksp = NULL;
  CWorkspaceItem* pFoundItem = NULL;

  for (stPos = m_pWorkspaces->GetHeadPosition (); stPos != NULL; ) 
  {
    pFoundWksp = (CWorkspace*) m_pWorkspaces->GetNext (stPos);

    if (pFoundWksp->FindItem (pWorkspaceItem))
    {
      return pFoundWksp;
    }
  }

  return NULL;

}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
CWorkspaceItem* CWorkspaceDlg::FindWorkspaceItem (
  CWorkspaceItem* pWorkspaceItem)
{
  POSITION stPos;
  CWorkspace* pFound = NULL;
  CWorkspaceItem* pFoundItem = NULL;

  for (stPos = m_pWorkspaces->GetHeadPosition (); stPos != NULL; ) 
  {
    pFound = (CWorkspace*) m_pWorkspaces->GetNext (stPos);

    if (pFound->FindItem (pWorkspaceItem))
    {
      return pWorkspaceItem;
    }
  }

  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
CWorkspace* CWorkspaceDlg::FindWorkspace (
  long lIndex)
{
  POSITION stPos;

  if ((stPos = m_pWorkspaces->FindIndex (lIndex)) != NULL)
  {
      return (CWorkspace*) m_pWorkspaces->GetAt (stPos);
  }

  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnWorkspaceRenameWorkspace() 
{
  ::AfxMessageBox ("Rename workspace");	

	m_Tree.ModifyStyleEx (0, TVS_EDITLABELS);
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnWorkspaceAddFileToWorkspace() 
{
  ::AfxMessageBox ("Add file to workspace");	
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnWorkspaceRemoveWorkspace() 
{
  ::AfxMessageBox ("Delete workspace");	
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnWorkspaceSaveWorkspace() 
{
  ::AfxMessageBox ("Save workspace");	
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnWorkspaceSaveWorkspaceAs() 
{
  ::AfxMessageBox ("Save workspace as...");	
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnFileAddToWorkspace() 
{
  CWorkspaceListDlg stDlg;
  CWorkspaceItem* pItem = NULL;
  CWorkspace* pWorkspace = NULL;
  int nItem = 0;

  HTREEITEM hItem = m_Tree.GetSelectedItem ();

  if (hItem && (pItem = (CWorkspaceItem*) m_Tree.GetItemData (hItem)))
  {
    nItem = stDlg.DoModal ();

    if (nItem != -1)
    {
      if (pWorkspace = FindWorkspace (nItem))
      {
        AddWorkspaceFile ((char*) pItem->GetFilename (), pWorkspace);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg 
//
void CWorkspaceDlg::OnFileDeleleteFromWorkspace() 
{
  CWorkspaceItem* pItem = NULL;
  HTREEITEM       hItem = NULL;

  hItem = m_Tree.GetSelectedItem ();

  if (hItem && (pItem = (CWorkspaceItem*) m_Tree.GetItemData (hItem)))
  {
    // This *should* do everything we need.
    pItem->Close ();
  }
}

void CWorkspaceDlg::OnTopLevelAddNewWorkspace() 
{
  AddNewWorkspace ();
}
