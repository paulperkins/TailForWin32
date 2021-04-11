// WorkspaceListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tail.h"
#include "workspace.h"
#include "workspacedlg.h"
#include "WorkspaceListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTailApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceListDlg dialog


CWorkspaceListDlg::CWorkspaceListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkspaceListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkspaceListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_pItems = new CObList;
}

CWorkspaceListDlg::~CWorkspaceListDlg (
  void)
{
  delete m_pItems;
}


void CWorkspaceListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkspaceListDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkspaceListDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkspaceListDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWorkspaceListDlg::FillList (
  void)
{
  CListCtrl* pList = NULL;
  CWorkspace* pWorkspace = NULL;
  char szBuf[1024] = "";
  LVITEM lvi;

  memset (&lvi, 0, sizeof (LVITEM));

  pList = (CListCtrl*) GetDlgItem (IDC_WORKSPACE_LIST);

  pList->DeleteAllItems ();

  for (int i = 0; i < (int) theApp.m_pWorkspace->m_pWorkspaces->GetCount (); i++)
  {
    pWorkspace = theApp.m_pWorkspace->FindWorkspace (i);

    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem = i;
    lvi.pszText = &pWorkspace->m_szName[0];
    lvi.cchTextMax = strlen (lvi.pszText) + 1;
    lvi.lParam = (LPARAM) pWorkspace;

    pList->InsertItem (&lvi);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceListDlg message handlers

BOOL CWorkspaceListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  FillList ();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWorkspaceListDlg::OnOK() 
{
  CListCtrl*  pList = NULL;
  POSITION    pos = 0;
  int         nItem = -1;

  pList = (CListCtrl*) GetDlgItem (IDC_WORKSPACE_LIST);

  pos = pList->GetFirstSelectedItemPosition ();

  if (pos != NULL)
  {
     while (pos)
     {
        nItem = pList->GetNextSelectedItem(pos);

        // For now.
        break;
     }
  }

  // Return the index of the selected item.
  CDialog::EndDialog (nItem);
}
