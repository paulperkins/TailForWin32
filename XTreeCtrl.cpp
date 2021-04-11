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
 * $Id: XTreeCtrl.cpp,v 1.3 2003/11/21 13:26:20 paulperkins Exp $
 * 
 */

// XTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "tail.h"
#include "XTreeCtrl.h"
#include "workspace.h"
#include "workspacedlg.h"
#include "workspacelistdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// XTreeCtrl

CXTreeCtrl::CXTreeCtrl()
{
  m_pWorkspaceDlg = NULL;
}

CXTreeCtrl::~CXTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CXTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CXTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// XTreeCtrl message handlers

void CXTreeCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
  // Do a reverse reflection of the right-click.
  // i.e. send the message back where it should be in the 
  // first place - the parent window.
  m_pWorkspaceDlg->DblClick ();
  
	*pResult = 1;
}

void CXTreeCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  // Do a reverse reflection of the right-click.
  // i.e. send the message back where it should be in the 
  // first place - the parent window.
  m_pWorkspaceDlg->RButton ();

	*pResult = 0;
} 

///////////////////////////////////////////////////////////////////////////////
// AddTreeItem
//
HTREEITEM CXTreeCtrl::AddTreeItem (
	HTREEITEM	htvParent, 
	char*		pszText,
	int			nNormalImage,
	int			nSelectedImage,
	void*		pData)
{
	TV_INSERTSTRUCT	tvi;

	memset (&tvi, 0, sizeof (TV_INSERTSTRUCT));

	tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	if (pData)
	{
		tvi.item.mask |= TVIF_PARAM;
		tvi.item.lParam  = (long) pData;
	} 

	tvi.hParent      = htvParent;
	tvi.hInsertAfter = TVI_LAST;		  
	tvi.item.pszText = pszText;
	tvi.item.cchTextMax = strlen (tvi.item.pszText);
	tvi.item.iImage  =	nNormalImage; 
	tvi.item.iSelectedImage  =	nSelectedImage; 

	return InsertItem (&tvi);
}	

///////////////////////////////////////////////////////////////////////////////
// DelTreeItem
//
BOOL CXTreeCtrl::DelTreeItem (
	HTREEITEM	hti)
{
	return DeleteItem (hti);
}	
