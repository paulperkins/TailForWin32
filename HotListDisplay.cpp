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
 * $Id: HotListDisplay.cpp,v 1.7 2003/11/25 08:14:28 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "Tail.h"
#include "HotListDisplay.h"

#include "plugin.h"
#include "keywordlist.h"
#include "settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHotListDisplay dialog


CHotListDisplay::CHotListDisplay(CWnd* pParent /*=NULL*/)
	: CDialog(CHotListDisplay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHotListDisplay)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHotListDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHotListDisplay)
	DDX_Control(pDX, IDC_KEYWORD_LIST, m_List);	// comment out this line if creating dynamically
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHotListDisplay, CDialog)
	//{{AFX_MSG_MAP(CHotListDisplay)
	ON_BN_CLICKED(IDC_KEYWORD_DEL, OnKeywordDel)
	ON_BN_CLICKED(IDC_KEYWORD_ADD, OnKeywordAdd)
	//}}AFX_MSG_MAP
  ON_REGISTERED_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED, OnCheckbox)
  ON_REGISTERED_MESSAGE(WM_XLISTCTRL_COMBO_SELECTION, OnComboSelection)
END_MESSAGE_MAP()

void CHotListDisplay::FillList (
  void)
{
  CXListCtrl* pGrid = NULL;
  char szBuf[1024] = "";
  LVITEM lvi;
  CListBox* pList = NULL;
  CPlugin* pPlugin = NULL;
  CKeywordPlugin* pKeywordPlugin = NULL;
  CKeywordListItem* pKeyword = NULL;
  int i = 0;
	LV_COLUMN lvcolumn;
	CRect rect;
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CSettings* pSettings = theApp->GetSettings ();

  m_sa.RemoveAll();
	m_sa.Add(_T("Red"));
	m_sa.Add(_T("Green"));
	m_sa.Add(_T("Blue"));
	m_sa.Add(_T("White"));
	m_sa.Add(_T("Black"));

  // Lock the list to stop any flickering during the reload.
  m_List.LockWindowUpdate ();

  // Clear the list first.
  m_List.DeleteAllItems ();

  if (0 == m_List.m_HeaderCtrl.GetItemCount ())
  {
    // Get the size for calculating the header widths.
	  m_List.GetWindowRect(&rect);

	  int w = rect.Width() - 2;
	  int total_cx = 0;
    int iKeywordWidth = 0;
    int iColourWidth = 50;
    int iPluginWidth = 50;
    int iActiveWidth = 20;
    int iFrigWidth = 2;

    memset (&lvi, 0, sizeof (LVITEM));

	  m_List.SetExtendedStyle (LVS_EX_FULLROWSELECT);

    // Calculate the width of the 'keyword' header.
    iKeywordWidth = w - (theApp->GetSettings()->GetNumPlugins() * iPluginWidth) - iColourWidth - iActiveWidth - iFrigWidth;

    memset(&lvcolumn, 0, sizeof(lvcolumn));

    // Inert the first 'active' (checkbox) column.
    lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	  lvcolumn.fmt = LVCFMT_LEFT;
	  lvcolumn.pszText = "";
	  lvcolumn.iSubItem = 0;
    lvcolumn.cx = iActiveWidth;
	  total_cx += lvcolumn.cx;
	  m_List.InsertColumn(0, &lvcolumn);


    // And the keyword column.
    lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	  lvcolumn.fmt = LVCFMT_LEFT;
	  lvcolumn.pszText = "Keyword";
	  lvcolumn.iSubItem = 1;
    lvcolumn.cx = iKeywordWidth;
	  m_List.InsertColumn(1, &lvcolumn);

    // Add the colour column.
    lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	  lvcolumn.fmt = LVCFMT_LEFT;
	  lvcolumn.pszText = "Colour";
	  lvcolumn.iSubItem = 2;
    lvcolumn.cx = iColourWidth;
	  m_List.InsertColumn(2, &lvcolumn);

    // Loop over the plugins, adding a header for each.
    while (pPlugin = pSettings->FindPlugin (i))
    {
		  lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		  lvcolumn.fmt = LVCFMT_CENTER;
		  lvcolumn.pszText = pPlugin->GetShortName ();
		  lvcolumn.iSubItem = i + 3;
      lvcolumn.cx = iPluginWidth;
		  m_List.InsertColumn(i + 3, &lvcolumn);

      i++;
    } 
  }

  // Add a row per keyword.
  int j = 0;

  while (pKeyword = pSettings->FindKeyword (j))
  {
    // Active column.
    m_List.InsertItem (j, "");
    m_List.SetCheckbox(j, 0, pKeyword->Active ());  

    // Keyword column.
    m_List.SetItemText (j, 1, pKeyword->Keyword (), RGB(0,0,0), RGB(255,255,255));
    m_List.SetItemData (j, (DWORD) pKeyword); 

    // Colour column.
    
    //  R 0
    //  G 1
    //  B 2
    //  W 3
    //  B 4
    // Get the index of the chosen colour.
    if (pKeyword->Colour () ==  RGB (0, 255, 0))
    {
      m_List.SetComboBox(j, 2, 1, &m_sa, 5, 1);  
    }
    else if (pKeyword->Colour () ==  RGB (0, 0, 255))
    {
      m_List.SetComboBox(j, 2, 1, &m_sa, 5, 2);  
    }
    else if (pKeyword->Colour () ==  RGB (0, 0, 255))
    {
      m_List.SetComboBox(j, 2, 1, &m_sa, 5, 2);  
    }
    else if (pKeyword->Colour () ==  RGB (255, 255, 255))
    {
      m_List.SetComboBox(j, 2, 1, &m_sa, 5, 3);  
    }
    else if (pKeyword->Colour () ==  RGB (0, 0, 0))
    {
      m_List.SetComboBox(j, 2, 1, &m_sa, 5, 4);  
    }
    else
    {
      m_List.SetComboBox(j, 2, 1, &m_sa, 5, 0);  
    }
    
    i = 0;
    
    // And a column for each plugin.
    while (pPlugin = pSettings->FindPlugin (i))
    {      
      if (pKeywordPlugin = pKeyword->FindPlugin (pPlugin->GetShortName()))
      {
        m_List.SetCheckbox(j, i + 3, pKeywordPlugin->Active()); //pPlugin->Active());  
      }

      i++;
    }

    j++;
  }

  // Unlock the drawing.
  m_List.UnlockWindowUpdate ();
}

/////////////////////////////////////////////////////////////////////////////
// CHotListDisplay message handlers

BOOL CHotListDisplay::OnInitDialog() 
{
  CDialog::OnInitDialog();

  FillList ();

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CHotListDisplay::OnCheckbox ( 
  WPARAM nItem, 
  LPARAM nSubItem)
{
  CKeywordListItem* pKeyword = NULL;
  CPlugin* pPlugin = NULL;
  CKeywordPlugin* pKeywordPlugin = NULL;
  char szBuf[1024] = "";

  pKeyword = (CKeywordListItem*) m_List.GetItemData (nItem); 

  // Set the keyword active.
  if (nSubItem == 0)
  {
    pKeyword->SetActive (m_List.GetCheckbox (nItem, nSubItem));
  }
  else
  {
    CTailApp* theApp = (CTailApp*) AfxGetApp ();
    CSettings* pSettings = theApp->GetSettings ();

    // Find it in the app list first, as the ordering seems to be a little odd.
    pPlugin = pSettings->FindPlugin (nSubItem - 3);

    if (pKeywordPlugin = pKeyword->FindPlugin (pPlugin->GetShortName()))
    {
      pKeywordPlugin->SetActive (!pKeywordPlugin->Active());      
    }
  }

  return 0;
}

void CHotListDisplay::OnKeywordDel() 
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CKeywordListItem* pKeyword = NULL;
  CSettings* pSettings = theApp->GetSettings ();
  POSITION pos;
  int i = 0;

  pos = m_List.GetFirstSelectedItemPosition ();

  if (pos)
  {
    i = m_List.GetNextSelectedItem (pos);

    if (pKeyword = pSettings->FindKeyword (i))
    {      
      pSettings->DeleteKeyword (i);

      FillList ();
    }
  }
	
}

void CHotListDisplay::OnKeywordAdd() 
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CKeywordList* pKeywordList = NULL;
  CSettings* pSettings = theApp->GetSettings ();
  char szKeyword[1024] = "";

  GetDlgItemText (IDC_KEYWORD_NEW, szKeyword, sizeof (szKeyword) - 1);

  pSettings->AddKeyword (szKeyword);

  FillList ();
}

LRESULT CHotListDisplay::OnComboSelection(WPARAM nItem, LPARAM nSubItem)
{
  CKeywordListItem* pKeyword = NULL;
  char szText[1024] = "";

  if (pKeyword = (CKeywordListItem*) m_List.GetItemData (nItem))
  {
    strcpy_s (szText, sizeof (szText), m_List.GetComboText (nItem, nSubItem));

    if (strcmp (szText, "Red") == 0)
    {
      pKeyword->SetColour (RGB (255, 0, 0));
    }

    if (strcmp (szText, "Green") == 0)
    {
      pKeyword->SetColour (RGB (0, 255, 0));
    }

    if (strcmp (szText, "Blue") == 0)
    {
      pKeyword->SetColour (RGB (0, 0, 255));
    }

    if (strcmp (szText, "Black") == 0)
    {
      pKeyword->SetColour (RGB (0, 0, 0));
    }

    if (strcmp (szText, "White") == 0)
    {
      pKeyword->SetColour (RGB (255, 255, 255));
    }
  }

	return 0;
}

void CHotListDisplay::OnOK() 
{
  HANDLE hReload;

  hReload = OpenEvent (EVENT_ALL_ACCESS, FALSE, RELOAD_EVENT_NAME);

  // Release the threads and then reset the event automatically.
  PulseEvent (hReload);

  CloseHandle (hReload);
	
	CDialog::OnOK();
}
