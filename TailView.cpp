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
 * $Id: TailView.cpp,v 1.28 2003/11/25 08:48:28 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include <io.h>
#include <stdio.h>


#include "Tail.h"

#include "TailDoc.h"
#include "TailView.h"
#include "keywordlist.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "hotlistdisplay.h"
#include "commdlg.h"
#include "tallywindow.h"
#include "utils.h"
#include "settings.h"

#include "workspacedlg.h"

#include "eventlogthread.h"
#include "filereaderthread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define KEYWORD_FILE    "keywords.cfg"

#define LINE_PAD        (256)

UINT ReaderThread (LPVOID pParams);

extern CTailApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTailView

IMPLEMENT_DYNCREATE(CTailView, CView)

BEGIN_MESSAGE_MAP(CTailView, CView)
	//{{AFX_MSG_MAP(CTailView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(IDM_IGNORE_STARTUP, OnIgnoreStartup)
	ON_COMMAND(IDM_DISPLAY_HOTLIST, OnDisplayHotlist)
	ON_COMMAND(IDM_SETFONT, OnSetfont)
	ON_COMMAND(IDM_CONFIG_PLUGINS, OnConfigPlugins)
	ON_COMMAND(IDM_TALLY, OnTally)
	ON_UPDATE_COMMAND_UI(IDM_IGNORE_STARTUP, OnUpdateIgnoreStartup)
	ON_COMMAND(ID_VIEW_GO, OnViewGo)
	ON_COMMAND(ID_VIEW_STOP, OnViewStop)
	ON_COMMAND(IDM_HIGLIGHT_LINE, OnHiglightLine)
	ON_UPDATE_COMMAND_UI(IDM_HIGLIGHT_LINE, OnUpdateHiglightLine)
	ON_COMMAND(IDM_WORDWRAP, OnWordwrap)
	ON_UPDATE_COMMAND_UI(IDM_WORDWRAP, OnUpdateWordwrap)
	ON_WM_DESTROY()
	ON_COMMAND(IDM_SET_BACKGROUND, OnSetBackground)
	ON_COMMAND(IDM_SET_HIGHLIGHT_COLOUR, OnSetHighlightColour)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*IMPLEMENT_DYNCREATE(CViewThread, CWinThread)

BEGIN_MESSAGE_MAP(CViewThread, CWinThread)
	//{{AFX_MSG_MAP(CVCiewThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() */



/////////////////////////////////////////////////////////////////////////////
// CTailView construction/destruction


/////////////////////////////////////////////////////////////////////////////
// 
//
CTailView::CTailView()
{
  CTailEmailEvent* pEvent = NULL;
  m_hThread = NULL;
  m_pThread = NULL;
  m_bUseTimeout = FALSE;
  m_ppszList = NULL;
  m_lNumKeywords = 0;
  m_bEventsVisible = FALSE;
  m_bPaused = FALSE;

  m_bTallyVisible = FALSE;
  m_pTallyWindow = NULL;

  memset (&m_szTailFile[0], 0, sizeof (m_szTailFile));
  memset (&m_szDieEvent[0], 0, sizeof (m_szDieEvent));

  m_pSettings = theApp.GetSettings ();
  
  LoadKeywords ();
}

/////////////////////////////////////////////////////////////////////////////
// 
//
CTailView::~CTailView()
{
  CTailApp* pApp = (CTailApp*) AfxGetApp ();

  if (m_ppszList)
  {
    for (int i = 0; i < m_lNumKeywords; i++)
    {
      if (m_ppszList[i])
      {
        free (m_ppszList[i]);
        m_ppszList[i] = NULL;
      }
    }

    free (m_pstKeywordList);
    m_pstKeywordList = NULL;

    free (m_ppszList);
    m_ppszList = NULL;

    // Warlock, added bTallyVisible check
    if (m_bTallyVisible)
    {
	    if (AfxIsValidAddress (m_pTallyWindow, sizeof (TallyWindow)) && m_pTallyWindow->GetSafeHwnd ())
	    {
        delete m_pTallyWindow;
      }
    }

  }
}

BOOL CTailView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTailView drawing

void CTailView::OnDraw(CDC* pDC)
{
	CTailDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CTailView diagnostics

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::AssertValid() const
{
	CView::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

/////////////////////////////////////////////////////////////////////////////
// 
//
CTailDoc* CTailView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTailDoc)));
	return (CTailDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTailView message handlers

/////////////////////////////////////////////////////////////////////////////
// 
//
int CTailView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  RECT  rc;
  CHARFORMAT cf;
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  int aiBarParts[] = {120, 360, 360, -1};

  if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

  GetClientRect (&rc);

  rc.top = 100;
  m_ctlStatusBar.Create (WS_CHILD | WS_VISIBLE, rc, this, 2);
  m_ctlStatusBar.SetParts (4, &aiBarParts[0]);
  m_ctlStatusBar.SetText ("Loaded.", 0, 0);
    
  GetClientRect (&rc);
  rc.top -= 100;

  m_ctlEdit.Create 	( WS_CHILD | WS_VISIBLE | 
                      ES_MULTILINE | ES_READONLY | 
                      ES_AUTOVSCROLL | ES_AUTOHSCROLL | 
                      WS_VSCROLL | WS_HSCROLL, 
                      rc, this, 1);



  if (!m_pSettings->GetGotFont())
  {
    // Set up some defaults.
    m_ctlEdit.GetDefaultCharFormat (cf);

    strcpy (cf.szFaceName, "Courier New");
    cf.dwMask |= CFM_FACE | CFM_BOLD;
    cf.dwEffects = cf.dwEffects & ~CFE_BOLD ;
    m_ctlEdit.SetDefaultCharFormat (cf);
  }
  else
  {
    SetViewFont (&m_pSettings->GetLogFont());
  }

  m_ctlEdit.SetBackgroundColor (FALSE, m_pSettings->GetBackGroundColour());

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::SetViewFont (
  LOGFONT*        pstLogFont)
{
  CHARFORMAT stFormat;
  int nPoint; // Warlock

  m_ctlEdit.GetDefaultCharFormat (stFormat);

  // Warlock, added font size changing capability
  nPoint = -MulDiv( 72, pstLogFont->lfHeight, m_ctlEdit.GetDC()->GetDeviceCaps(LOGPIXELSY) );
  stFormat.yHeight= MulDiv( nPoint, 1440, 72 ); // Twips

  stFormat.bCharSet = pstLogFont->lfCharSet; 
  stFormat.bPitchAndFamily = pstLogFont->lfPitchAndFamily;     
  strcpy (stFormat.szFaceName, pstLogFont->lfFaceName);     

  stFormat.dwMask |= CFM_UNDERLINE | CFM_BOLD | CFM_ITALIC | CFM_FACE;

  if (pstLogFont->lfUnderline)
  {
    stFormat.dwEffects |= CFE_UNDERLINE;
  }
  else
  {
    stFormat.dwEffects &= ~CFE_UNDERLINE;
  }

  if (pstLogFont->lfItalic)
  {
    stFormat.dwEffects |= CFE_ITALIC;
  }
  else
  {
    stFormat.dwEffects &= ~CFE_ITALIC;
  }

  if (pstLogFont->lfWeight > 500)
  {
    stFormat.dwEffects |= CFE_BOLD;
  }
  else
  {
    stFormat.dwEffects &= ~CFE_BOLD;
  }

  m_ctlEdit.SetDefaultCharFormat (stFormat);

  m_pSettings->SetGotFont (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
  m_ctlEdit.MoveWindow (0, 0, cx, cy - 20);	
  m_ctlStatusBar.MoveWindow (0, cy - 20, cx, 20);	

  ScrollToBottom (m_ctlEdit.GetSafeHwnd ());

  m_ctlEdit.SetTargetDevice (NULL, m_pSettings->GetWordWrap() ? 0 : 1);
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  CTailDoc* pDoc = NULL;

  memset (&m_stParams, 0, sizeof (READER_PARAMS));
  memset (&m_stELParams, 0, sizeof (EVENT_LOG_THREAD_PARAMS));

  pDoc = (CTailDoc*) GetDocument ();

  sprintf (m_szDieEvent, "%s%08ld", DIE_EVENT_NAME, (DWORD)(void*) this);

  strcpy (m_szTailFile, pDoc->m_strTailFile); 

  strcpy (m_stParams.szFileName, m_szTailFile); 

  m_stParams.hEdit = m_ctlEdit.GetSafeHwnd ();
  m_stParams.bUseTimeout = m_bUseTimeout;
  m_stParams.nTimeout = m_pSettings->GetTimeout ();
  m_stParams.bIgnoreHotStartup = m_pSettings->GetIgnoreHotStartup ();
  m_stParams.pView = this;
  m_stParams.bDebug = m_pSettings->GetDebug ();
  strcpy (m_stParams.szDieEvent, m_szDieEvent);
  strcpy (m_stParams.szReloadEvent, RELOAD_EVENT_NAME);
  m_stParams.bPaused = FALSE;

/*  strcpy (m_stELParams.szFileName, pDoc->m_strTailFile); 

  m_stELParams.hEdit = m_ctlEdit.GetSafeHwnd ();
  m_stELParams.bUseTimeout = m_bUseTimeout;
  m_stELParams.nTimeout = pSettings->GetTimeout ();
  m_stELParams.bIgnoreHotStartup = pSettings->GetIgnoreHotStartup ();
  m_stELParams.pView = this;
  m_stELParams.bDebug = pSettings->GetDebug ();
  strcpy (m_stELParams.szDieEvent, m_szDieEvent);
  m_stELParams.bPaused = FALSE; */

  if (!m_pThread && !pDoc->m_strTailFile.IsEmpty ())
  {
    m_pThread = AfxBeginThread ((AFX_THREADPROC) ReaderThread, (LPVOID) &m_stParams);

    // Comment out the above line and uncomment the following line to see event log monitoring in action.
//    m_pThread = AfxBeginThread ((AFX_THREADPROC) EventLogThread, (LPVOID) &m_stELParams);
  }

  // Add ourself to the workpace dialog. 
  // The NULL parameter concerns me here, as it'll try to add itself to 
  // the default workspace.
  theApp.m_pWorkspace->AddWorkspaceFile (m_stParams.szFileName, this, NULL);

  SetWindowText (m_szTailFile);
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::Update (
  void)
{
}


/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::Refresh ()
{
  HANDLE    hFile = NULL;
  DWORD     dwSize = 0;
  char*     pszBuffer;
  DWORD     dwBytesRead;
  BOOL      bReadOK = FALSE;
  char      szMsg[1024] = "";

  memset (&m_stParams, 0, sizeof (READER_PARAMS));

  if (!pDoc->m_strTailFile || pDoc->m_strTailFile == "")
  {
    ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), "PANIC: Document has no file!", "Tail for Win32", MB_OK | MB_ICONSTOP);
    return;
  }

  hFile = CreateFile (m_szTailFile, 
                      GENERIC_READ, 
                      FILE_SHARE_READ | FILE_SHARE_WRITE, 
                      NULL, 
                      OPEN_EXISTING, //OPEN_ALWAYS, 
                      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_POSIX_SEMANTICS, 
                      NULL);

  int i = GetLastError ();

  if (hFile == INVALID_HANDLE_VALUE)
  {
    sprintf (szMsg, "Could not open '%s'.", m_szTailFile);
    ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), szMsg, "Tail for Win32", MB_OK | MB_ICONSTOP);

    return;
  }

  dwSize = GetFileSize (hFile, NULL);

  dwSize++;

  pszBuffer = (char*) malloc (dwSize);

  ASSERT (pszBuffer);

  memset ((void*) pszBuffer, 0, dwSize);

  bReadOK = ReadFile (hFile, (LPVOID) pszBuffer, dwSize - 1, &dwBytesRead, NULL);
  
  CloseHandle (hFile);     	

  if (bReadOK)
  {
    m_ctlEdit.SetWindowText (pszBuffer);

    ScrollToBottom (m_ctlEdit.GetSafeHwnd ());

    if (m_pSettings->GetBeepOnChanges ())
    {
      MessageBeep (MB_ICONINFORMATION);  
    }
  }

  free ((void*) pszBuffer); 
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void UpdateStatusBar (
  CTailView* pView,
  char*      pszText)
{
  char szBuf[1024] = "";
  char szTS[1024] = "";

  pView->m_ctlStatusBar.SetText (pszText, 0, 0);  
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void UpdateStatusBar (
  CStatusBarCtrl* pStatus,
  long            lMatches,
  BOOL            bMatch)
{
  char szBuf[1024] = "";
  char szTS[1024] = "";

  GetTimeStamp (szTS);

  sprintf (szBuf, "Last updated: %s", szTS);
  pStatus->SetText (szBuf, 0, 0);

  if (bMatch)
  {
    sprintf (szBuf, "Last match: %s Total matches: %ld", szTS, lMatches);
    pStatus->SetText (szBuf, 1, 0);  
  }
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void UpdateStatusBarPause (
  CStatusBarCtrl* pStatus,
  BOOL            bPaused)
{
  if (bPaused)
  {
    pStatus->SetText ("Paused", 3, 0);
  }
  else
  {
    pStatus->SetText ("", 3, 0);
  }
}

/////////////////////////////////////////////////////////////////////////////
// 
//
// This function determines whether a keyword is to be actioned,
// and by which plugin, and then calls the appropriate plugin's 
// handler function.
static void FireEvent (
  CTailView*  pView,
  const char* pszKeyword,
  const char* pszLine)
{
  CKeywordListItem* pKeyword = NULL;
  CKeywordPlugin*   pPlugin = NULL;
  CTailApp*         theApp = (CTailApp*) AfxGetApp ();
  int               i = 0;
  int               j = 0;

  // Loop over the keywords.
  if (pKeyword = pView->m_pSettings->FindKeyword (pszKeyword))
  {    
    // Only fire the event if this keyword is active. 
    // We should only be here if that is so, but it makes
    // the logic look a little more obvious.
    if (pKeyword->Active ())
    {
      // Loop over all plugins for this keyword.
      while (pPlugin = pKeyword->FindPlugin (j++))
      {
        // Only fire the event if the plugin is active for this keyword.
        if (pPlugin->Active ())
        {
          LogMessage ("Firing plugin '%s' for keyword '%s'...", pPlugin->GetShortName (), pszKeyword);

          pPlugin->m_pPlugin->Action ((const char*) pszKeyword, (const char*) pszLine, (const char*) pView->m_stParams.szFileName);
        }
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// 
//
// PP: This should really be named 'UpdateView' or similar,
// as its function has grown significantly.
BOOL InsertText (
  CTailView*      pView,
  char*           pszText,
  BOOL            bDoColouring,
  long*           plMatchCount,
  BOOL*           pbMatch)
{
  CHARFORMAT stCF;
  CHARFORMAT2 stCF2;
  char szLine[2048] = "";
  char szLineBuffer[2048] = "";
  char* pszPtr;
  char* pszStart;
  char* pszLineEnd;
  char* pszToken;
  char** ppszKeywords;
  KEYWORD_LIST* pstKeywords = NULL;
  BOOL bFound = FALSE;
  BOOL bEnd = FALSE;
  BOOL bUpdated = FALSE;
  BOOL bShownLine = FALSE;
  DWORD dwIndex = -1;
  char  szTS[TIMESTAMP_LEN + 1] = "";
  long lLineLen = 0;
  CKeywordListItem* pKeyword;
  long lNumKeywords = 0;

  CTailApp* theApp = (CTailApp*) AfxGetApp ();

  GetTimeStamp (szTS);

  if (!bDoColouring)
  {
//    if (pView->m_ctlEdit.GetSafeHwnd())
    {
      pView->m_ctlEdit.ReplaceSel (pszText, FALSE);    
    }

    bUpdated = TRUE;

    if (!pView->m_pSettings->GetPaused () && !pView->m_bPaused)
    {
      ScrollToBottom (pView->m_ctlEdit.GetSafeHwnd());
    }

    if (pView->m_pSettings->GetBeepOnChanges() && bUpdated)
    {
      MessageBeep (MB_ICONINFORMATION);  
    }  

    return bUpdated;
  }

  memset (&stCF, 0, sizeof (CHARFORMAT));
  memset (&stCF2, 0, sizeof (CHARFORMAT2));

  pView->m_ctlEdit.SetSel (-1, -1);

  pszPtr = pszText;

  // Chew off a line at a time.
  while (1)
  {
    long lSelStart = 0;
    long lSelEnd = 0;

    long lLineSelStart = 0;
    long lLineSelEnd = 0;

    bShownLine = FALSE;

    //PP: NEW ***
    bFound = FALSE;

    pView->m_ctlEdit.SetSel (-1, -1);

    // Find the end of line.
    pszLineEnd = strchr (pszPtr, '\n');

    memset (szLine, 0, sizeof (szLine));

    if (pszLineEnd)
    {
      // Take off the CR+LF.
      lLineLen = MIN ((sizeof (szLine) - 1), (pszLineEnd - pszPtr + 1)) - 2;
      strncpy (szLine, pszPtr, MIN ((sizeof (szLine) - 1), (pszLineEnd - pszPtr + 1)));
    }
    else
    {
      lLineLen = MIN ((sizeof (szLine) - 1), (strlen (pszPtr)));
      strncpy (szLine, pszPtr, MIN ((sizeof (szLine) - 1), (strlen (pszPtr))));

      bEnd = TRUE;
    }
    
    // PP: This needs moving around.
    if (pView->m_pSettings->GetShowAllLines ())
    {
      if (pView->m_pSettings->GetHighlightLine() && pView->m_pSettings->GetHighlightWholeLine ())
      {
        if (pszLineEnd && ((strlen (szLine) + 1) <= LINE_PAD))
        {
          memset (&szLineBuffer[0], ' ', LINE_PAD);

          strncpy (szLineBuffer, szLine, lLineLen);

          if (pszLineEnd)
          {
            szLineBuffer[LINE_PAD - 1] = 0x0d;
            szLineBuffer[LINE_PAD]     = 0x0a;
            szLineBuffer[LINE_PAD + 1] = '\0';
          }
          else
          {
            szLineBuffer[LINE_PAD - 1] = '\0';
          } 
        }
        else
        {
          strcpy (szLineBuffer, szLine);
        }
      }
      else
      {
          strcpy (szLineBuffer, szLine);      
      }

//      LogMessage ("Writing '%s'", szLineBuffer);

//      pView->m_ctlEdit.ReplaceSel (szLine, FALSE);
      pView->m_ctlEdit.ReplaceSel (szLineBuffer, FALSE);

      bUpdated = TRUE;
      bShownLine = TRUE; // Warlock, added
    }

    // Get the keyword list.
    ppszKeywords = pView->m_ppszList;
    pstKeywords = pView->m_pstKeywordList;

    pszPtr = &szLine[0];

    lNumKeywords = pView->m_pSettings->GetNumKeywords ();

//    if (ppszKeywords && *ppszKeywords)
//    if (pView->lNumKeywords)
    if (lNumKeywords)
    {
      // Loop over each keyword.
//      while (*ppszKeywords)

      dwIndex = -1;

//      for (int iIndex = 0; iIndex < pView->lNumKeywords; iIndex++)
      for (int iIndex = 0; iIndex < lNumKeywords; iIndex++)
      {
        CString msg;

        pKeyword = pView->m_pSettings->FindKeyword (pstKeywords[iIndex].szKeyword);

        pszStart = pszPtr = &szLine[0];

        dwIndex++;

        // Search for the keyword on the line.
        if (pKeyword && pKeyword->Active ())
        {
//        while (pszToken = strstr (pszStart, *ppszKeywords))
//      while (pszToken = strstr (pszStart, pstKeywords[iIndex].szKeyword)) // Warlock, commented out, changed to below
        while (pszToken = strstr (pszPtr, pstKeywords[iIndex].szKeyword)) // Warlock, using pszPtr, fix multiple keywords per line
//        while ((pszToken = strstr (pszPtr, pKeyword->Keyword ())) // Warlock, using pszPtr, fix multiple keywords per line
//              && pKeyword->Active ()) 
        {
          // We've got another match.
          (*plMatchCount)++;
          pstKeywords[iIndex].dwMatches++;
          strcpy (pstKeywords[iIndex].szTimestamp, szTS);

          // Call the event associated with this keyword.
//          FireEvent (pView, *ppszKeywords, szLine);
          FireEvent (pView, pstKeywords[iIndex].szKeyword, szLine);
          
          // if (!theApp->m_bShowAllLines && !bShownLine && !pstKeywords[iIndex].bExclude) // Warlock, commented out
          if (!pView->m_pSettings->GetShowAllLines() && !bShownLine && pstKeywords[iIndex].bExclude) // Warlock, show keyword lines
          {
            if ((strlen (szLine) + 1) <= LINE_PAD)
            {
              memset (&szLineBuffer[0], ' ', LINE_PAD);

              strncpy (szLineBuffer, szLine, strlen (szLine));

              szLineBuffer[LINE_PAD - 1] = '\0';
            }

            pView->m_ctlEdit.ReplaceSel (szLine, FALSE);
//            pView->m_ctlEdit.ReplaceSel (szLineBuffer, FALSE);

//            pView->m_ctlEdit.GetSel (lOrgSelStart, lOrgSelEnd);

            bUpdated = TRUE;
            bShownLine = TRUE;
          }

          pView->m_ctlEdit.SetSel (-1, -1);

          pView->m_ctlEdit.GetSel (lSelStart, lSelEnd);

//          lSelStart -= (strlen (szLine) - (pszToken - pszStart));
//          lSelEnd -= (strlen (szLine) - ((pszToken - pszStart) + strlen (*ppszKeywords)));
//          lSelEnd -= (strlen (szLine) - ((pszToken - pszStart) + strlen (pstKeywords[iIndex].szKeyword)));
//          lSelStart -= (strlen (szLine) - (pszToken - pszStart) - 1); // Warlock, - 1
     
          lSelStart -= (strlen (szLine) - (pszToken - pszStart)); // PP: No minus at all!
          lSelEnd -= (strlen (szLine) - ((pszToken - pszStart) + strlen (pstKeywords[iIndex].szKeyword)) - 1); // Warlock, - 1
  
          if (lLineSelStart == 0)
          {
            lLineSelStart = lSelStart - (pszToken - pszStart);
            lLineSelEnd = lLineSelStart + strlen (szLine);
          }
          
          pView->m_ctlEdit.SetSel (lSelStart, lSelEnd);

          pView->m_ctlEdit.GetSelectionCharFormat (stCF);

          stCF.dwMask = CFM_COLOR | CFM_BOLD;
          stCF.dwEffects = CFE_BOLD;

          stCF.crTextColor = pKeyword->Colour ();

          pView->m_ctlEdit.SetSelectionCharFormat (stCF);

          pszPtr = pszToken + strlen (*ppszKeywords);
//          pszPtr = pszToken + strlen (pstKeywords[iIndex].szKeyword);

          // pszStart = pszPtr; // Warlock, commented out, fix multiple keywords per line

          bFound = TRUE;

          *pbMatch = TRUE;
        }
        } // if (pKeyword->Active ())

        // Get the next keyword.
//        ppszKeywords++;
      }

      // Highlight the entire line.
      if (bFound && pView->m_pSettings->GetHighlightLine())
      {
          pView->m_ctlEdit.SetSel (lLineSelStart, lLineSelEnd);

          stCF2.cbSize = sizeof (stCF2);
          stCF2.dwMask = CFM_PROTECTED|CFM_BACKCOLOR;
          stCF2.crBackColor = pView->m_pSettings->GetHighlightColour();

          SendMessage(pView->m_ctlEdit, EM_SETCHARFORMAT, (WPARAM)SCF_SELECTION, (LPARAM)&stCF2);
      } 
    }

    if (bEnd)
    {
      break;
    }

    // Poke the pointer on by one.
    pszPtr = pszLineEnd + 1;

    bFound = FALSE;

    // We've reached the end of the string - don't bother going round again.
    if (!*pszPtr)
    {
      break;
    }
  }   

  // Update the stats.
  UpdateStatusBar (&pView->m_ctlStatusBar, *plMatchCount, *pbMatch);
  
  // Update the tally window.
  // Warlock, added bTallyVisible check
//  if (pView->bTallyVisible)
//  if (theApp->m_bTallyVisible)
  {
    if (AfxIsValidAddress (pView->m_pTallyWindow, sizeof (TallyWindow)) && pView->m_pTallyWindow->GetSafeHwnd ())
    {
      pView->m_pTallyWindow->UpdateMatches ();
    }
  }

  if (!pView->m_pSettings->GetPaused() && !pView->m_bPaused)
  {
    ScrollToBottom (pView->m_ctlEdit.GetSafeHwnd());// hwndEdit);
  }

  if (pView->m_pSettings->GetBeepOnChanges() && bUpdated)
  {
    MessageBeep (MB_ICONINFORMATION);  
  } 

  return bUpdated;
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::CopyText()
{  
  m_ctlEdit.Copy ();
}


/////////////////////////////////////////////////////////////////////////////
// 
//
void ScrollToBottom (
  HWND hwnd)
{
  CTailApp* theApp = NULL;

  theApp = (CTailApp*) AfxGetApp ();

  ::SendMessage (hwnd, WM_VSCROLL, (WPARAM) SB_BOTTOM, 0);

  /* 4.1.2 Added version check to fix W2K bug in RichEdit control. */
  if (! ((theApp->m_fVersion >= 5.0) && (theApp->m_dwPlatformID == VER_PLATFORM_WIN32_NT)) ) // Warlock, scroll to bottom, compiled with MSVC2002, tested on Win2K-SP? and WinXP-SP1
  {
    ::SendMessage (hwnd, WM_VSCROLL, (WPARAM) SB_PAGEUP, (LPARAM) 0); 
  }
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::ReloadKeywords (
  void)
{
  char* pszItem = NULL;
  FILE* fp = NULL;
  char szLine[MAX_KEYWORD_LEN + 1] = "";
  char szAppPath[_MAX_PATH] = "";
  char szDrive[_MAX_DRIVE] = "";
  char szPath[_MAX_PATH] = "";
  char szConfigFile[_MAX_PATH] = "";
  char** ppszNewList = NULL;
  KEYWORD_LIST* pstNewKeywordList = NULL;
  long lKeywordCount = 0;
  int i = 0;

  lKeywordCount = m_pSettings->GetNumKeywords();

  if (lKeywordCount)
  {
    i = 0;

    LogMessage ("Debug: Found %ld keywords in memory.", lKeywordCount);

    ppszNewList = (char**) malloc ((lKeywordCount + 1) * sizeof (char*));

    pstNewKeywordList = (KEYWORD_LIST*) malloc (lKeywordCount * sizeof (KEYWORD_LIST));

    memset (pstNewKeywordList, 0, (lKeywordCount * sizeof (KEYWORD_LIST)));

    while (i < lKeywordCount)
    {
      strcpy (szLine, m_pSettings->FindKeyword(i)->Keyword());

      ppszNewList[i] = (char*) malloc (sizeof (szLine));
      *ppszNewList[i] = '\0';

      strcpy (pstNewKeywordList[i].szKeyword, szLine);
      pstNewKeywordList[i].bExclude = TRUE;
      strcpy (ppszNewList[i], szLine);

      // Copy the previous list members.
      if (m_ppszList)
      {
        for (int j = 0; j < m_lNumKeywords; j++)
        {
          if (m_ppszList[j])
          {
            if (strcmp (m_ppszList[j], pstNewKeywordList[i].szKeyword) == 0)
            {
              pstNewKeywordList[i].dwMatches = m_pstKeywordList[i].dwMatches;
              strcpy (pstNewKeywordList[i].szTimestamp, m_pstKeywordList[i].szTimestamp);
            }
          }
        }
      }

      i++;
    }
  }

  LogMessage ("Debug: There were %ld keywords, now %ld.", 
              m_dwNumKeywords,
              lKeywordCount);


  // Delete the old list.
  if (m_ppszList)
  {
    for (int i = 0; i < m_lNumKeywords; i++)
    {
      if (m_ppszList[i])
      {
        free (m_ppszList[i]);
        m_ppszList[i] = NULL;
      }
    }

    free (m_pstKeywordList);
    m_pstKeywordList = NULL;

    free (m_ppszList);
    m_ppszList = NULL;
  } 

  // And re-assign.

  m_dwNumKeywords = lKeywordCount;
  m_lNumKeywords = m_dwNumKeywords;

  m_pstKeywordList = pstNewKeywordList;
  m_ppszList = ppszNewList;
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::LoadKeywords (
  void)
{
  char* pszItem = NULL;
  char szLine[MAX_KEYWORD_LEN + 1] = "";
  
  if (m_lNumKeywords = m_pSettings->GetNumKeywords())
  {
    int i = 0;

    LogMessage ("Debug: Found %ld keywords in memory.", m_lNumKeywords);

    m_ppszList = (char**) malloc ((m_lNumKeywords + 1) * sizeof (char*));

    m_dwNumKeywords = m_lNumKeywords;
    m_pstKeywordList = (KEYWORD_LIST*) malloc (m_lNumKeywords * sizeof (KEYWORD_LIST));

    memset (m_pstKeywordList, 0, (m_lNumKeywords * sizeof (KEYWORD_LIST)));

    while (i < m_lNumKeywords)
    {
      strcpy (szLine, m_pSettings->FindKeyword(i)->Keyword());

      m_ppszList[i] = (char*) malloc (sizeof (szLine));
      *m_ppszList[i] = '\0';

      strcpy (m_pstKeywordList[i].szKeyword, szLine);
      m_pstKeywordList[i].bExclude = TRUE;
      strcpy (m_ppszList[i++], szLine);
    }
  }
}

BOOL CTailView::LoadPlugins (
  void)
{
  HINSTANCE hMod = NULL;
  char szAppPath[_MAX_PATH] = "";
  char szPlugin[_MAX_PATH] = "";
	struct _finddata_t file_info;
	char szFileSpec[_MAX_FNAME] = "";
	long hFind = 0;
  CPlugin* pPlugin = NULL;

  GetAppPath (szAppPath);

  strcat (szAppPath, PLUGIN_DIR);

  LogMessage ("Looking for plugins in '%s'", szAppPath);

  sprintf (szFileSpec, "%s*.%s", szAppPath, PLUGIN_EXT);

  // Get the first file.
	if (-1L != (hFind = _findfirst (szFileSpec, &file_info)))
	{
    sprintf (szPlugin, "%s%s", szAppPath, file_info.name);

    pPlugin = new CPlugin;

    LogMessage ("Loading plugin at '%s'", szPlugin);

    if (!pPlugin->Load (szPlugin))
    {
      delete pPlugin;
      pPlugin = NULL;
    }
    else
    {
      m_Plugins.AddPlugin (pPlugin);
    }

		while (_findnext (hFind, &file_info) == 0)
		{
      pPlugin = new CPlugin;

      sprintf (szPlugin, "%s%s", szAppPath, file_info.name);

      LogMessage ("Loading plugin at '%s'", szPlugin);

      if (!pPlugin->Load (szPlugin))
      {
        delete pPlugin;
        pPlugin = NULL;
      }
      else
      {
        m_Plugins.AddPlugin (pPlugin);
      }
		}
	}
	
	_findclose (hFind);

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnDisplayHotlist() 
{
	CHotListDisplay stDlg;

  stDlg.ppszList = m_ppszList;
  stDlg.lNumKeywords = m_lNumKeywords;
  stDlg.m_pPlugins = &m_Plugins;

  stDlg.DoModal ();
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnSetfont() 
{
  CFontDialog stFontDlg;
//  CTailApp* theApp = NULL;
  CHARFORMAT stCF;
  LOGFONT lf;

  int nPoint; // Warlock

//  theApp = (CTailApp*) AfxGetApp ();

  m_ctlEdit.GetDefaultCharFormat (stCF);

  lf = m_pSettings->GetLogFont ();

  // Warlock, added font size changing capability
  nPoint = MulDiv( stCF.yHeight, 72, 1440 );
  lf.lfHeight = -MulDiv(nPoint, m_ctlEdit.GetDC()->GetDeviceCaps(LOGPIXELSY), 72);
  
  lf.lfCharSet = stCF.bCharSet;
  lf.lfPitchAndFamily = stCF.bPitchAndFamily;
  strcpy (lf.lfFaceName, stCF.szFaceName); 

  stFontDlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
  stFontDlg.m_cf.lpLogFont = &lf;

  if (IDOK == stFontDlg.DoModal ())
  {
    // Warlock
    // Point = 72 / PPI * -Height
    // Twips = Point * 1440 / 72
    nPoint = -MulDiv( 72, stFontDlg.m_cf.lpLogFont->lfHeight, m_ctlEdit.GetDC()->GetDeviceCaps(LOGPIXELSY) );
    stCF.yHeight= MulDiv( nPoint, 1440, 72 ); // Twips

    stCF.bCharSet = stFontDlg.m_cf.lpLogFont->lfCharSet;     
    stCF.bPitchAndFamily = stFontDlg.m_cf.lpLogFont->lfPitchAndFamily;
    strcpy (stCF.szFaceName, stFontDlg.m_cf.lpLogFont->lfFaceName); 

    stCF.dwMask |= CFM_UNDERLINE | CFM_BOLD | CFM_ITALIC | CFM_FACE;

    if (stFontDlg.m_cf.lpLogFont->lfUnderline)
    {
      stCF.dwEffects |= CFE_UNDERLINE;
    }
    else
    {
      stCF.dwEffects &= ~CFE_UNDERLINE;
    }

    if (stFontDlg.m_cf.lpLogFont->lfItalic)
    {
      stCF.dwEffects |= CFE_ITALIC;
    }
    else
    {
      stCF.dwEffects &= ~CFE_ITALIC;
    }

    if (stFontDlg.m_cf.lpLogFont->lfWeight > 500)
    {
      stCF.dwEffects |= CFE_BOLD;
    }
    else
    {
      stCF.dwEffects &= ~CFE_BOLD;
    }

    m_ctlEdit.SetDefaultCharFormat (stCF);

    m_pSettings->SetLogFont (lf);

    m_pSettings->SetGotFont (TRUE);
  }
}

/////////////////////////////////////////////////////////////////////////////
// 
//
BOOL CTailView::AddEvent (
  char* pszEvent, 
  char* pszSeverity, 
  long  lLine)
{
  if (m_bEventsVisible)
  {
    return FALSE;
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnConfigPlugins() 
{
	CPluginConfig stDlg;

  stDlg.m_pPlugins = m_pSettings->GetPlugins ();

  stDlg.DoModal ();
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnTally() 
{
  if (!m_bTallyVisible || (!AfxIsValidAddress (m_pTallyWindow, sizeof (TallyWindow))) || (!m_pTallyWindow->GetSafeHwnd ()))
  {
    m_pTallyWindow = new TallyWindow ();

    m_pTallyWindow->Create (IDD_TALLY, this);

    m_pTallyWindow->Update (m_lNumKeywords, &m_stParams.szFileName[0], m_pstKeywordList);

    m_pTallyWindow->ShowWindow (SW_SHOW);

    m_bTallyVisible = TRUE;
  }
  else
  {
    if (AfxIsValidAddress (m_pTallyWindow, sizeof (TallyWindow)) && m_pTallyWindow->GetSafeHwnd ())
    {
      m_pTallyWindow->ShowWindow (SW_SHOW);
    }
  } 
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnIgnoreStartup() 
{
  m_pSettings->SetIgnoreHotStartup (!m_pSettings->GetIgnoreHotStartup());	
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnUpdateIgnoreStartup(CCmdUI* pCmdUI) 
{
  if (pCmdUI->m_pMenu)
    pCmdUI->SetCheck (m_pSettings->GetIgnoreHotStartup() ? MF_CHECKED : MF_UNCHECKED);		
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnViewGo() 
{
  this->m_bPaused = FALSE;
  ScrollToBottom (m_ctlEdit.GetSafeHwnd ());
  UpdateStatusBarPause (&this->m_ctlStatusBar, this->m_bPaused);
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnViewStop() 
{
  this->m_bPaused = TRUE;	
  UpdateStatusBarPause (&this->m_ctlStatusBar, this->m_bPaused);
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnHiglightLine() 
{
  m_pSettings->SetHighlightLine (!m_pSettings->GetHighlightLine());	
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void CTailView::OnUpdateHiglightLine(CCmdUI* pCmdUI) 
{
  if (pCmdUI->m_pMenu)
  {
    pCmdUI->SetCheck (m_pSettings->GetHighlightLine() ? MF_CHECKED : MF_UNCHECKED);		
  }
}

/////////////////////////////////////////////////////////////////////////////
// 
//
void ReloadKeywords (
  CTailView*      pView)
{
  if (pView)
  {
    pView->ReloadKeywords ();

    if (AfxIsValidAddress (pView->m_pTallyWindow, sizeof (TallyWindow)) && pView->m_pTallyWindow->GetSafeHwnd ())
    {
      pView->m_pTallyWindow->Update (pView->m_lNumKeywords, 
                                      &pView->m_stParams.szFileName[0], 
                                      pView->m_pstKeywordList);
    }
  }
}

void CTailView::OnWordwrap() 
{
  m_pSettings->SetWordWrap (!m_pSettings->GetWordWrap());			

  m_ctlEdit.SetTargetDevice (NULL, m_pSettings->GetWordWrap() ? 0 : 1);
}

void CTailView::OnUpdateWordwrap(CCmdUI* pCmdUI) 
{
  if (pCmdUI->m_pMenu)
    pCmdUI->SetCheck (m_pSettings->GetWordWrap() ? MF_CHECKED : MF_UNCHECKED);		
}

void CTailView::OnDestroy() 
{
  // Kill the worker thread.

  HANDLE hDie;

  hDie = OpenEvent (EVENT_ALL_ACCESS, FALSE, m_szDieEvent);

  SetEvent (hDie);

  // Wait for the die to occur.
  if (m_pThread && (WAIT_TIMEOUT == WaitForSingleObject (m_pThread->m_hThread, 2000)))
  {
    AfxMessageBox ("(Debug Warning) Could Not Kill Thread!");
  }

  CloseHandle (hDie);

	CView::OnDestroy();
	
  // Remove ourself from the workspace dialog.
  theApp.m_pWorkspace->RemoveWorkspaceFile (this);	
}

void CTailView::OnSetBackground() 
{
  CColorDialog dlg (m_pSettings->GetBackGroundColour(), CC_FULLOPEN);
  
  if (IDOK == dlg.DoModal())
  {	
    m_pSettings->SetBackGroundColour(dlg.GetColor ());

    m_ctlEdit.SetBackgroundColor (FALSE, m_pSettings->GetBackGroundColour());	
  }
}

void CTailView::OnSetHighlightColour() 
{
  CColorDialog dlg (m_pSettings->GetHighlightColour(), CC_FULLOPEN);
  
  if (IDOK == dlg.DoModal())
  {	
    m_pSettings->SetHighlightColour(dlg.GetColor ());
  }
}
