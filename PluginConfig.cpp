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
 * $Id: PluginConfig.cpp,v 1.4 2003/11/13 08:29:10 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "Tail.h"
#include "PluginConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPluginConfig dialog


CPluginConfig::CPluginConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPluginConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPluginConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPluginConfig)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPluginConfig, CDialog)
	//{{AFX_MSG_MAP(CPluginConfig)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CONFIGURE, OnConfigure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPluginConfig message handlers

int CPluginConfig::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL CPluginConfig::OnInitDialog() 
{
  CListBox* pList = NULL;
  int i = 0;
  char* pszName = NULL;
  CPlugin* pPlugin = NULL;

	CDialog::OnInitDialog();
	
  pList = (CListBox*) GetDlgItem (IDC_PLUGIN_LIST);

  while (pPlugin = m_pPlugins->FindPlugin (i))
  {
    pszName = pPlugin->GetName ();
    pList->AddString (pszName);
    pList->SetItemData (i, (DWORD) pPlugin);
    i++;
  }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPluginConfig::OnConfigure() 
{
  int iSel = 0;
  CListBox* pList = NULL;
  CPlugin* pPlugin = NULL;

  pList = (CListBox*) GetDlgItem (IDC_PLUGIN_LIST);
 	
  if (LB_ERR != (iSel = pList->GetCurSel()))
  {
    pPlugin = (CPlugin*) pList->GetItemData (iSel);

    if (pPlugin)
    {
      pPlugin->Configure (m_hWnd);
    }
  }
}
