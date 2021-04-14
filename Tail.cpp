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
 * $Id: Tail.cpp,v 1.10 2003/11/25 08:15:14 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "Tail.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "TailDoc.h"
#include "TailView.h"
#include "Splash.h"

#include "utils.h"

#include "Plugin.h"
#include "keywordlist.h"
#include "settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTailApp

BEGIN_MESSAGE_MAP(CTailApp, CWinApp)
	//{{AFX_MSG_MAP(CTailApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_BEEP, OnFileBeep)
	ON_UPDATE_COMMAND_UI(ID_FILE_BEEP, OnUpdateFileBeep)
	ON_COMMAND(IDM_PAUSED, OnPaused)
	ON_UPDATE_COMMAND_UI(IDM_PAUSED, OnUpdatePaused)
	ON_COMMAND(IDM_IGNORE_STARTUP, OnIgnoreStartup)
	ON_UPDATE_COMMAND_UI(IDM_IGNORE_STARTUP, OnUpdateIgnoreStartup)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


CTailMultiDocTemplate::CTailMultiDocTemplate ( 
  UINT nIDResource, 
  CRuntimeClass* pDocClass, 
  CRuntimeClass* pFrameClass, 
  CRuntimeClass* pViewClass ) :  CMultiDocTemplate (nIDResource, pDocClass, pFrameClass, pViewClass)
{

}

CFrameWnd* CTailMultiDocTemplate::CreateNewFrame (
  CRuntimeClass *pNewViewClass,
  CDocument *pDoc, 
  CFrameWnd *pOther)
{
  CRuntimeClass* pOldViewClass = m_pViewClass;

  m_pViewClass=pNewViewClass;

  CFrameWnd* pFrame = CMultiDocTemplate::CreateNewFrame(pDoc, pOther);

  m_pViewClass=pOldViewClass;

  return pFrame;
}


/////////////////////////////////////////////////////////////////////////////
// CTailApp construction

CTailApp::CTailApp()
{
  char szVersionString[255] = "";


	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
/*  m_bBeepOnChanges = FALSE;		
  m_bPaused = FALSE;		
  m_nTimeout = 500;
  m_pPlugins = new CPluginList;
  m_pKeywords = new CKeywordList;


  m_pWorkspace = NULL;
  m_bShowWorkspace = FALSE; */

//  m_bDebug = DebugFilePresent ();

  ::LoadString (AfxGetApp ()->m_hInstance, IDS_VERSION, szVersionString, sizeof (szVersionString) - 1);

  LogMessage ("Starting %s...", szVersionString);

}

CTailApp::~CTailApp()
{
  LogMessage ("Terminating...");

  delete m_pSettings;
/*
  delete (CPluginList*) m_pPlugins;
  delete (CKeywordList*) m_pKeywords; */
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTailApp object

CTailApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTailApp initialization

BOOL CTailApp::InitInstance()
{
	// CG: The following block was added by the Splash Screen component.
\
	{
\
		CCommandLineInfo cmdInfo;
\
		ParseCommandLine(cmdInfo);
\

\
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
\
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	// Now deprecated.
	//Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Objektiv"));

//  VersionCheck ();

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

  m_pSettings = new CSettings;

//  m_pSettings->Load ();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

//	CMultiDocTemplate* pDocTemplate;
//	m_pDocTemplate = new CMultiDocTemplate(
	m_pDocTemplate = new CTailMultiDocTemplate(
		IDR_TAILTYPE,
		RUNTIME_CLASS(CTailDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTailView));
	AddDocTemplate(m_pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	m_pMainFrame = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

  DragAcceptFiles ((HWND)(this->GetMainWnd()->GetSafeHwnd()), TRUE);

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// GetSettings
//
CSettings* CTailApp::GetSettings (
	void)
{
  return m_pSettings;
}

// App command to run the dialog
void CTailApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}



/////////////////////////////////////////////////////////////////////////////
// CTailApp commands
  

void CTailApp::OnFileBeep() 
{
  CSettings* pSettings = GetSettings ();

  pSettings->SetBeepOnChanges (!pSettings->GetBeepOnChanges());
}

void CTailApp::OnUpdateFileBeep(CCmdUI* pCmdUI) 
{
  CSettings* pSettings = GetSettings ();

  if (pCmdUI->m_pMenu)
    pCmdUI->SetCheck (pSettings->GetBeepOnChanges() ? MF_CHECKED : MF_UNCHECKED);		
}

BOOL CTailApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

BOOL CAboutDlg::OnInitDialog() 
{
  char szVersion[255] = "";
  char szMsg[1024] = "";
  int count = 0;

	CDialog::OnInitDialog();
	
  ::LoadString (AfxGetApp ()->m_hInstance, IDS_VERSION, szVersion, sizeof (szVersion) - 1);

  GetDlgItem (IDC_VERSION) -> SetWindowText (szVersion);


  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CTailApp::OnPaused() 
{
  CSettings* pSettings = GetSettings ();

  pSettings->SetPaused (!pSettings->GetPaused());
}

void CTailApp::OnUpdatePaused(CCmdUI* pCmdUI) 
{
  CSettings* pSettings = GetSettings ();

  if (pCmdUI->m_pMenu)
    pCmdUI->SetCheck (pSettings->GetPaused() ? MF_CHECKED : MF_UNCHECKED);		
}

void CTailApp::OnIgnoreStartup() 
{
  CSettings* pSettings = GetSettings ();

  pSettings->SetIgnoreHotStartup (!pSettings->GetIgnoreHotStartup());			
}

void CTailApp::OnUpdateIgnoreStartup(CCmdUI* pCmdUI) 
{
  CSettings* pSettings = GetSettings ();

  if (pCmdUI->m_pMenu)
    pCmdUI->SetCheck (pSettings->GetIgnoreHotStartup() ? MF_CHECKED : MF_UNCHECKED);		
}

