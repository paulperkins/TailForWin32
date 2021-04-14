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
 * $Id: TallyWindow.cpp,v 1.7 2003/11/17 07:29:46 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "tail.h"
#include "TallyWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





/////////////////////////////////////////////////////////////////////////////
// TallyWindow dialog


TallyWindow::TallyWindow(CWnd* pParent /*=NULL*/)
	: CDialog(TallyWindow::IDD, pParent)
{
	//{{AFX_DATA_INIT(TallyWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  bInited = FALSE;
}


void TallyWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TallyWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TallyWindow, CDialog)
	//{{AFX_MSG_MAP(TallyWindow)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TallyWindow message handlers

int TallyWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText (pszFilename);
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void TallyWindow::UpdateMatches (void)
{
  CListCtrl* pList = NULL;
  char szBuf[1024] = "";
  LVITEM lvi;

  memset (&lvi, 0, sizeof (LVITEM));

  pList = (CListCtrl*) GetDlgItem (IDC_LIST_TALLY);

  pList->DeleteAllItems ();

  for (int i = 0; i < (int) dwNumKeywords; i++)
  {
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem = i;
    lvi.pszText = &pstKeywordList[i].szKeyword[0];
    lvi.cchTextMax = strlen (pstKeywordList[i].szKeyword) + 1;
    lvi.lParam = (LPARAM) &pstKeywordList[i];

    pList->InsertItem (&lvi);

    // Insert the keyword.
//    pList->InsertItem (i, pstKeywordList[i].szKeyword);

    // Insert the match timestamp.
    sprintf_s (szBuf, sizeof(szBuf), "%ld", pstKeywordList[i].dwMatches);
    pList->SetItemText(i, 1, szBuf);

    // Insert the match count.
    pList->SetItemText(i, 2, pstKeywordList[i].szTimestamp);
  }
}

void TallyWindow::Update (
  long          lNewNumKeywords, 
  char*         pszNewFilename, 
  KEYWORD_LIST* pstNewKeywordList)
{
  dwNumKeywords  = lNewNumKeywords;
  pstKeywordList = pstNewKeywordList;
  pszFilename    = pszNewFilename;

  FillList ();
}

void TallyWindow::FillList (
  void)
{
  CListCtrl* pList = NULL;
  char szBuf[1024] = "";
  LVITEM lvi;

  memset (&lvi, 0, sizeof (LVITEM));

  pList = (CListCtrl*) GetDlgItem (IDC_LIST_TALLY);

  pList->DeleteAllItems ();

  if (!bInited)
  {
   pList->InsertColumn (0, "Keyword", LVCFMT_LEFT, 100);
   pList->InsertColumn (1, "Matches", LVCFMT_LEFT, 100);
   pList->InsertColumn (2, "Last Match", LVCFMT_LEFT, 100);
  }

  for (int i = 0; i < (int) dwNumKeywords; i++)
  {
    // Insert the keyword.
    // Use the full monty method so that we can add
    // item data.
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem = i;
    lvi.pszText = &pstKeywordList[i].szKeyword[0];
    lvi.cchTextMax = strlen (pstKeywordList[i].szKeyword) + 1;
    lvi.lParam = (LPARAM) &pstKeywordList[i];

    pList->InsertItem (&lvi);

    // Insert the match timestamp.
    sprintf_s (szBuf, sizeof(szBuf), "%ld", pstKeywordList[i].dwMatches);
    pList->SetItemText(i, 1, szBuf);

    // Insert the match count.
    pList->SetItemText(i, 2, pstKeywordList[i].szTimestamp);
  }

  bInited = TRUE;
}

BOOL TallyWindow::OnInitDialog() 
{
	FillList ();

	return TRUE;
}

// Function to sort the list view control items when header control selected.
int CALLBACK SortItems(
  LPARAM lParam1, 
  LPARAM lParam2, 
  LPARAM lParamSort)
{
  KEYWORD_LIST* pListItem1;
  KEYWORD_LIST* pListItem2;

  if (lParam1 && lParam2)
  {
    pListItem1 = (KEYWORD_LIST*) lParam1;
    pListItem2 = (KEYWORD_LIST*) lParam2;

    switch (lParamSort)
    {
    // Sort by keyword name.
    case 0:
      return (strcmp (pListItem1->szKeyword, pListItem2->szKeyword));

    // Sort by number of hits.
    case 1:
      if (pListItem1->dwMatches > pListItem2->dwMatches)
      {
        return -1;
      }
      else if (pListItem1->dwMatches < pListItem2->dwMatches)
      {
        return 1;
      }
      else
      {
        return 0;
      }

    // Sort by timestamp.
    case 2:
      return (strcmp (pListItem2->szTimestamp, pListItem1->szTimestamp)); 
    }    
  }

  return 0;
}

// Trap WM_NOTIFY messages so that we can sort the list view control 
// contents when header control clicked.
BOOL TallyWindow::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
  CListCtrl* pList;
  CHeaderCtrl* pHdr;
  NMHDR* pnmhdr;

  pnmhdr = (NMHDR*) lParam;

  pList = (CListCtrl*) GetDlgItem (IDC_LIST_TALLY);

  pHdr = pList->GetHeaderCtrl ();

  // If the 'from' HWND is that of the header, then sort the list control.
  if (pnmhdr->hwndFrom == pHdr->GetSafeHwnd ())
  {
    pList->SortItems (SortItems, (LPARAM) ((LPNMLISTVIEW) lParam)->iItem);
  }
  
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void TallyWindow::OnDestroy() 
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();;
	
  CDialog::OnDestroy();
}

void TallyWindow::OnClose() 
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();;
	
	CDialog::OnClose();
}

void TallyWindow::OnOK() 
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();;
	
	CDialog::OnOK();
}
