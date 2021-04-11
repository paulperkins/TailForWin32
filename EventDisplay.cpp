// EventDisplay.cpp : implementation file
//

#include "stdafx.h"
#include "Tail.h"
#include "EventDisplay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventDisplay dialog


CEventDisplay::CEventDisplay(CWnd* pParent /*=NULL*/)
	: CDialog(CEventDisplay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventDisplay)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  Create (IDD_EVENT_DISPLAY);

  lMaxItem = 0;
}


void CEventDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventDisplay)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventDisplay, CDialog)
	//{{AFX_MSG_MAP(CEventDisplay)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventDisplay message handlers

BOOL CEventDisplay::OnInitDialog() 
{
  RECT rc;
  HICON hIcon = NULL;

  CDialog::OnInitDialog();
	
	GetClientRect (&rc);

  m_pListCtrl = (CListCtrl*) GetDlgItem (IDC_EVENT_LIST);

  if (m_pListCtrl)
  {
    m_pListCtrl->SetWindowPos (NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 0);	
  
    m_stImageList.Create (32, 32, 0, 3, 1);

    hIcon = AfxGetApp()->LoadIcon (IDI_EVENT_RED); 
	  iRedIcon = m_stImageList.Add (hIcon);
    DeleteObject (hIcon);
    
    hIcon = AfxGetApp()->LoadIcon (IDI_EVENT_AMBER); 
	  iAmberIcon = m_stImageList.Add (hIcon);
    DeleteObject (hIcon);
    
    hIcon = AfxGetApp()->LoadIcon (IDI_EVENT_GREEN); 
	  iGreenIcon = m_stImageList.Add (hIcon);
    DeleteObject (hIcon);

    m_pListCtrl->SetImageList (&m_stImageList, LVSIL_NORMAL);

    m_pListCtrl->InsertColumn (0, "");
    m_pListCtrl->InsertColumn (1, "Time");
    m_pListCtrl->InsertColumn (2, "Keyword");
  }

  lMaxItem = 0;

  AddEvent (SEVERITY_FATAL,"Fatal", "test");
  AddEvent (SEVERITY_INFO, "Info", "test2");
  AddEvent (SEVERITY_WARNING, "Warning", "test3");
  AddEvent (SEVERITY_FATAL,"Fatal2", "test4");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventDisplay::OnSize(UINT nType, int cx, int cy) 
{ 
  RECT rc;

	CDialog::OnSize(nType, cx, cy);
	
  GetClientRect (&rc);

  m_pListCtrl = (CListCtrl*) GetDlgItem (IDC_EVENT_LIST);

  if (m_pListCtrl)
  {
    m_pListCtrl->SetWindowPos (NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 0);	
  }
}

BOOL CEventDisplay::AddEvent (
  int   iSeverity, 
  char* pszText,
  char* pszKeyword)
{
  int iIndex = 0;
  int iRtn = 0;
  LV_ITEM stItem;
  char szTime[256] = "";

  switch (iSeverity)
  {
  case SEVERITY_WARNING:
    iIndex = 1;
    break;
  
  case SEVERITY_INFO:
    iIndex = 2;
    break;

  default:
  case SEVERITY_FATAL:
    iIndex = 0;
    break;
  }

  FormatTime (szTime, sizeof (szTime));
  memset (&stItem, 0, sizeof (LV_ITEM));

  // Insert text and icon.
  stItem.iItem = lMaxItem;
  stItem.iImage = iIndex;
  stItem.pszText = pszText;
  stItem.cchTextMax = 256;
  stItem.mask = LVIF_TEXT | LVIF_IMAGE;

//  iRtn = m_pListCtrl->InsertItem (lMaxItem++, pszText, iIndex);
  iRtn = m_pListCtrl->InsertItem (&stItem);

  stItem.mask = LVIF_TEXT;

  // Insert time.
  stItem.iSubItem = 1;
  stItem.pszText = szTime;

  iRtn = m_pListCtrl->InsertItem (&stItem);

  // Insert keyword.
  stItem.iSubItem = 2;
  stItem.pszText = pszKeyword;

  iRtn = m_pListCtrl->InsertItem (&stItem);


/*
  UINT   mask; 
    int    iItem; 
    int    iSubItem; 
    UINT   state; 
    UINT   stateMask; 
    LPTSTR  pszText; 
    int    cchTextMax; 
    int    iImage;       // index of the list view item’s icon 
    LPARAM lParam;       // 32-bit value to associate with item 
} LV_ITEM; 
 

  iRtn = m_pListCtrl->InsertItem (lMaxItem++, pszText, iIndex);
*/

  lMaxItem++;

  return TRUE;
}

