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
 * $Id: MainFrm.cpp,v 1.15 2003/11/25 08:15:13 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "Tail.h"

#include "MainFrm.h"

#include "tailview.h"
#include "keywordlist.h"
#include "Splash.h"
#include "settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_NOTIFY_CALLBACK  (WM_USER + 1)

extern CTailApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
  ON_WM_CREATE()
	ON_COMMAND(IDM_ALWAYS_TOP, OnAlwaysTop)
	ON_UPDATE_COMMAND_UI(IDM_ALWAYS_TOP, OnUpdateAlwaysTop)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_CLOSE()
	ON_COMMAND(IDM_SHOW_HOT_LINES, OnShowHotLines)
	ON_UPDATE_COMMAND_UI(IDM_SHOW_HOT_LINES, OnUpdateShowHotLines)
	ON_COMMAND(IDM_AUTOLOAD, OnAutoload)
	ON_UPDATE_COMMAND_UI(IDM_AUTOLOAD, OnUpdateAutoload)
	ON_WM_SIZE()
	ON_COMMAND(IDM_VIEW_WORKSPACE, OnViewWorkspace)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_WORKSPACE, OnUpdateViewWorkspace)
	ON_WM_SIZE()
	ON_COMMAND(IDM_NEW_WORKSPACE, OnNewWorkspace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
  m_bAlwaysOnTop = FALSE;		
  m_bMaxChild = FALSE;		

//  m_pWorkspace = NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  RECT rc;
  CMDIChildWnd* pChild = NULL;
  LOGFONT stLogFont;
  HKEY hKey = 0;
  DWORD dwSize = 0;
  DWORD dwType = 0;
  CString strLastFile;
  char* pszLastFile = NULL;
  CSettings* pSettings = NULL;

  if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// if (!m_wndToolBar.Create(this) ||
	if (!m_wndToolBar.Create(this, WS_CHILD | CBRS_TOP) || // Warlock, don't show toolbar, takes away valuable room
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndViewToolBar.Create(this) ||
		!m_wndViewToolBar.LoadToolBar(IDR_VIEW_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

  memset (&stLogFont, 0, sizeof (LOGFONT));

	// Do the main toolbar.
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
  ::SetWindowLong (m_wndToolBar, GWL_STYLE, ::GetWindowLong (m_wndToolBar, GWL_STYLE) | 0x32);

    // Make the toolbar flat.
  ::SetWindowLong (m_wndToolBar, GWL_STYLE, ::GetWindowLong (m_wndToolBar, GWL_STYLE) | 0x800);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

    // And the view toolbar.
	m_wndViewToolBar.SetBarStyle(m_wndViewToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

  ::SetWindowLong (m_wndViewToolBar, GWL_STYLE, ::GetWindowLong (m_wndViewToolBar, GWL_STYLE) | 0x32);

    // Make the toolbar flat.
  ::SetWindowLong (m_wndViewToolBar, GWL_STYLE, ::GetWindowLong (m_wndViewToolBar, GWL_STYLE) | 0x800);

	m_wndViewToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	// Set the position.
	// RECT rec;
	// rec.left = 300;
	// const RECT rec = {300, 255, 0, 0}; // Warlock, experimenting with toolbar location
	// LPCRECT lprec = &rec;

	DockControlBar(&m_wndViewToolBar);
	//DockControlBar( &m_wndViewToolBar, (UINT) 0, lprec );
	//DockControlBar( &m_wndViewToolBar, (UINT) AFX_IDW_DOCKBAR_LEFT, lprec );
	//DockControlBar( &m_wndViewToolBar, m_wndToolBar.m_pDockBar, lprec );

  // Create the workspace window.
	if (!m_WorkspaceBar.Create(this, &m_Workspace, CString ("Workspace"), IDD_WORKSPACE))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

  m_WorkspaceBar.SetBarStyle (m_WorkspaceBar.GetBarStyle () | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_WorkspaceBar.EnableDocking (CBRS_ALIGN_ANY);
	DockControlBar (&m_WorkspaceBar);

  theApp.m_pWorkspace = &m_Workspace;

  // Get on with initialising the app.
  pSettings = theApp.GetSettings ();

  pSettings->Load ();

  // Load the plugins.
  pSettings->LoadPlugins ();

  // Load the keyword.
  pSettings->LoadKeywords ();
  pSettings->LoadKeywordFile ();

  m_bMaxChild = pSettings->GetMaxChild ();

//  memset (&stLogfont, 0, sizeof (LOGFONT));

  /*
  rc.left = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("Left"), 0);
  rc.top = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("Top"), 0);
  rc.right = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("Right"), 640);
  rc.bottom = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("Bottom"), 480);


  m_bAlwaysOnTop = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("AlwaysOnTop"), 0);
  m_bMaxChild = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("Maximised"), 0);

  theApp.m_bBeepOnChanges      = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("BeepOnChange"),       0);
  theApp.m_nTimeout            = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("Timeout"),            1000);
  theApp.m_bIgnoreHotStartup   = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("IgnoreHotStartup"),   TRUE);
  theApp.m_bShowAllLines       = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("ShowAllLines"),       TRUE);
  theApp.m_bAutoLoad           = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("AutoLoadLastFile"),   TRUE);
  theApp.m_bDebug              = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("Debug"),              FALSE);
  theApp.m_bHighlightLine      = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("HighlightLine"),      TRUE);
  theApp.m_bHighlightWholeLine = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("HighlightWholeLine"), FALSE);
  theApp.m_lHighlightColour    = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("HighlightColour"),    0x00D0D0D0);
  theApp.m_lBackGroundColour   = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("BackGroundColour"),   ::GetSysColor(COLOR_WINDOW));
  theApp.m_bShowWorkspace      = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("ShowWorkspace"),      TRUE);
  theApp.m_bWordWrap           = AfxGetApp ()->GetProfileInt (_T("Settings"), _T("WordWrap"),           FALSE);
  */

  rc = pSettings->GetWindowRect ();

  MoveWindow (&rc);

  ShowControlBar (&m_WorkspaceBar, pSettings->GetShowWorkspace (), FALSE);

  SetTop ();


  /*
  LPCSTR lpszRegKey = NULL;

  lpszRegKey = theApp.m_pszRegistryKey;

  long lRtn;

  dwSize = sizeof (LOGFONT);

  if ((ERROR_SUCCESS == RegOpenKey (HKEY_CURRENT_USER, "Software", &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, lpszRegKey, &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, "Tail for Win32", &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, "Settings", &hKey)))
  {
    if (ERROR_SUCCESS == ( lRtn = RegQueryValueEx (hKey, "Font", NULL, &dwType, (LPBYTE) &stLogFont, &dwSize)))
    {
      theApp.m_bGotFont = TRUE;
      memcpy ((void*) &theApp.stLogFont, (void*) &stLogFont, sizeof (LOGFONT));
    }
    else
    {
      theApp.m_bGotFont = FALSE;
    }
  }

  */

  CMenu* pMenu;

  pMenu = GetMenu ();

  pMenu->EnableMenuItem (IDM_NEW_WORKSPACE, TRUE);

/*
  // Get the last file that was loaded.
  strLastFile = AfxGetApp()->GetProfileString (_T("Recent File List"), _T("File 1"), NULL);
  
  if (!strLastFile.IsEmpty () && theApp.m_bAutoLoad)
  {    
    m_Workspace.AddWorkspaceFile (strLastFile.GetBuffer (10), 
                                  m_Workspace.m_pDefaultWorkspace);
  }
*/

	// CG: The following line was added by the Splash Screen component.
	// CSplashWnd::ShowSplashScreen(this); // Warlock, confuses me :) all the whiteness looks like a window
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::SetTop (void)
{
  ModifyStyleEx (m_bAlwaysOnTop ? 0 : WS_EX_TOOLWINDOW, m_bAlwaysOnTop ? WS_EX_TOOLWINDOW : 0, 0);
  
  SetWindowPos (m_bAlwaysOnTop ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, 
                SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
}

void CMainFrame::OnAlwaysTop() 
{
  m_bAlwaysOnTop = !m_bAlwaysOnTop;

  SetTop ();
}

void CMainFrame::OnUpdateAlwaysTop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  if (pCmdUI->m_pMenu)
    pCmdUI->SetCheck (m_bAlwaysOnTop ? MF_CHECKED : MF_UNCHECKED);


}

void CMainFrame::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  if (pCmdUI->m_pMenu)
    pCmdUI->Enable (TRUE);	
}

void CMainFrame::OnEditCopy() 
{
  CMDIChildWnd* pChild = NULL;
  CTailView* pView = NULL;

  pChild = MDIGetActive();

  if (!pChild)
    return;

// Get the active view attached to the active MDI child// window.
  pView = (CTailView*) pChild->GetActiveView ();

  if (pView)
    pView->CopyText ();

}

void CMainFrame::OnClose() 
{
  RECT rc;
  CTailApp* app;
  WINDOWPLACEMENT wp;
  CMDIChildWnd* pChild = NULL;
//  BOOL bMax = FALSE;
  CSettings* pSettings = NULL;

  GetWindowRect (&rc);

  CTailView* pView = NULL;

  pChild = MDIGetActive();

  app = (CTailApp*) AfxGetApp ();

  pSettings = app->GetSettings ();

  if (pChild)
  {
    pChild->GetWindowPlacement (&wp);

    pSettings->SetMaxChild (wp.showCmd == SW_SHOWMAXIMIZED);
  }

  pSettings->SetWindowRect (rc);

  pSettings->Save ();

/*  if (app)
  {
    app->WriteProfileInt (_T("Settings"), _T("Left"), rc.left);
    app->WriteProfileInt (_T("Settings"), _T("Top"), rc.top);
    app->WriteProfileInt (_T("Settings"), _T("Right"), rc.right);
    app->WriteProfileInt (_T("Settings"), _T("Bottom"), rc.bottom);

    app->WriteProfileInt (_T("Settings"), _T("AlwaysOnTop"), m_bAlwaysOnTop);
    app->WriteProfileInt (_T("Settings"), _T("BeepOnChange"), theApp.m_bBeepOnChanges);
    app->WriteProfileInt (_T("Settings"), _T("Timeout"), theApp.m_nTimeout);
    app->WriteProfileInt (_T("Settings"), _T("Maximised"), bMax);
    app->WriteProfileInt (_T("Settings"), _T("IgnoreHotStartup"), theApp.m_bIgnoreHotStartup);
    app->WriteProfileInt (_T("Settings"), _T("ShowAllLines"), theApp.m_bShowAllLines);
    app->WriteProfileInt (_T("Settings"), _T("AutoLoadLastFile"), theApp.m_bAutoLoad);
    app->WriteProfileInt (_T("Settings"), _T("HighlightLine"), theApp.m_bHighlightLine);
    app->WriteProfileInt (_T("Settings"), _T("HighlightWholeLine"), theApp.m_bHighlightWholeLine);
    app->WriteProfileInt (_T("Settings"), _T("HighlightColour"), theApp.m_lHighlightColour);
    app->WriteProfileInt (_T("Settings"), _T("ShowWorkspace"), theApp.m_bShowWorkspace);
    app->WriteProfileInt (_T("Settings"), _T("WordWrap"), theApp.m_bWordWrap);
    app->WriteProfileInt (_T("Settings"), _T("BackGroundColour"), theApp.m_lBackGroundColour);

    LPCSTR lpszRegKey = NULL;
    HKEY hKey = NULL;

    lpszRegKey = theApp.m_pszRegistryKey;

    if (theApp.m_bGotFont)
    {
      if ((ERROR_SUCCESS == RegOpenKey (HKEY_CURRENT_USER, "Software", &hKey))
      && (ERROR_SUCCESS == RegOpenKey (hKey, lpszRegKey, &hKey))
      && (ERROR_SUCCESS == RegOpenKey (hKey, "Tail for Win32", &hKey))
      && (ERROR_SUCCESS == RegOpenKey (hKey, "Settings", &hKey)))
      {
        RegSetValueEx (hKey, "Font", NULL, REG_BINARY, (LPBYTE) &theApp.stLogFont, sizeof (LOGFONT));
      }
    }
  } */

  pSettings->SaveKeywords ();

  Shell_NotifyIcon (NIM_DELETE,&m_stNotifyIcon);     

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnShowHotLines() 
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CSettings* pSettings = theApp->GetSettings ();

  pSettings->SetShowAllLines (!pSettings->m_bShowAllLines);	
}

void CMainFrame::OnUpdateShowHotLines(CCmdUI* pCmdUI) 
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CSettings* pSettings = theApp->GetSettings ();

  if (pCmdUI->m_pMenu)
  {
    pCmdUI->SetCheck (pSettings->GetShowAllLines() ? MF_UNCHECKED : MF_CHECKED);	
  }
}

void CMainFrame::OnAutoload() 
{
  CSettings* pSettings = theApp.GetSettings ();

  pSettings->SetAutoLoad (!pSettings->GetAutoLoad());	
}

void CMainFrame::OnUpdateAutoload(CCmdUI* pCmdUI) 
{
  CSettings* pSettings = theApp.GetSettings ();

  if (pCmdUI->m_pMenu)
  {
    pCmdUI->SetCheck (pSettings->GetAutoLoad() ? MF_CHECKED : MF_UNCHECKED);	
  }
}

void CMainFrame::OnUpdateViewWorkspace(CCmdUI* pCmdUI) 
{
  CSettings* pSettings = theApp.GetSettings ();

  if (pCmdUI->m_pMenu)
  {
    pCmdUI->SetCheck (pSettings->GetShowWorkspace() ? MF_CHECKED : MF_UNCHECKED);	
  }
}

void CMainFrame::OnNewWorkspace() 
{
  theApp.m_pWorkspace->AddNewWorkspace ();	
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	
  if (SIZE_MINIMIZED == nType)
  {
/*    m_stNotifyIcon.cbSize = sizeof (NOTIFYICONDATA);
    m_stNotifyIcon.hWnd = GetSafeHwnd();
    m_stNotifyIcon.uID = IDI_APPICON;
    m_stNotifyIcon.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;
    m_stNotifyIcon.uCallbackMessage = WM_NOTIFY_CALLBACK;
    m_stNotifyIcon.hIcon = AfxGetApp()->LoadIcon (IDI_APPICON);
    
    GetWindowText (m_stNotifyIcon.szTip, 200);
    
    Shell_NotifyIcon (NIM_ADD,&m_stNotifyIcon);
    
    ShowWindow (SW_MINIMIZE);
    ShowWindow (SW_HIDE); */
  }	
}

// New window proc to handle tray icon callback.
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
  switch (message)
  {
  case WM_NOTIFY_CALLBACK:
//    ShowWindow (SW_SHOWNA);

    if (lParam == WM_LBUTTONDBLCLK)
    {
      SetForegroundWindow();
    }

    if (lParam == WM_RBUTTONDOWN)
    {
      CPoint point;
      CMenu* pmenu;
      CMenu* pPopup;

      GetCursorPos(&point);

      pmenu = GetSystemMenu(FALSE);
      //     VERIFY(menu.LoadMenu(IDR_POPUP_MENU));
      
//      if (pPopup = pmenu->GetSubMenu(0))
      pPopup = pmenu;
      {
        //      if (!m_hWndPrevious)
        //        pPopup->EnableMenuItem(ID_UNDO, MF_GRAYED|MF_BYCOMMAND);
        SetForegroundWindow();
        pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON, 
                                point.x, 
                                point.y,
                                this);
      }
    }

    break;

  }

	return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnViewWorkspace() 
{
  CSettings* pSettings = theApp.GetSettings ();

	if(::IsWindow(m_WorkspaceBar))
	{
		ShowControlBar(&m_WorkspaceBar, !pSettings->m_bShowWorkspace, FALSE);
	}
  
  pSettings->m_bShowWorkspace = !pSettings->m_bShowWorkspace;
}

