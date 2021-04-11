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
 * $Id: TailDoc.cpp,v 1.8 2003/11/25 08:15:14 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "Tail.h"

#include "TailDoc.h"
#include "tailview.h"
#include "settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//DWORD WINAPI ReaderThread (LPVOID pParams);

/////////////////////////////////////////////////////////////////////////////
// CTailDoc

IMPLEMENT_DYNCREATE(CTailDoc, CDocument)

BEGIN_MESSAGE_MAP(CTailDoc, CDocument)
	//{{AFX_MSG_MAP(CTailDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTailDoc construction/destruction

CTailDoc::CTailDoc()
{
	// TODO: add one-time construction code here
  m_pThread = NULL;
  m_szEventName[0] = '\0';
//  m_pActiveView = NULL;
}

CTailDoc::~CTailDoc()
{
//  if (m_pThread)
//  {
//    m_pThread->ExitInstance ();
//  }
}


/////////////////////////////////////////////////////////////////////////////
// CTailDoc serialization

void CTailDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTailDoc diagnostics

#ifdef _DEBUG
void CTailDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTailDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTailDoc commands

BOOL CTailDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
  CTailApp* pApp = (CTailApp*) AfxGetApp ();
  CSettings* pSettings = pApp->GetSettings ();

  m_strTailFile = CString (lpszPathName);

  pApp = (CTailApp*) AfxGetApp ();

  if ((!m_strTailFile || (m_strTailFile == "")) && pSettings->GetAutoLoad ())
  {
    m_strTailFile = AfxGetApp()->GetProfileString (_T("Recent File List"), _T("File 1"), NULL);
  }

  SetTitle (m_strTailFile);
  SetPathName (m_strTailFile, FALSE);

/*  if (m_pActiveView == NULL)
  {
    m_pActiveView = GetActiveView ();
  } */

	return TRUE;
}

void CTailDoc::OnCloseDocument() 
{
  HANDLE hDie;

  hDie = OpenEvent (EVENT_ALL_ACCESS, FALSE, m_szEventName);

  SetEvent (hDie);

// Wait for the die to occur.
  if (m_pThread && (WAIT_TIMEOUT == WaitForSingleObject (m_pThread->m_hThread, 2000)))
  {
    AfxMessageBox ("(Debug Warning) Could Not Kill Thread!");
  }

  CloseHandle (hDie);

	CDocument::OnCloseDocument();
}

BOOL CTailDoc::OnNewDocument() 
{
  CTailApp* pApp = (CTailApp*) AfxGetApp();
  CSettings* pSettings = pApp->GetSettings ();

	// TODO: Add your specialized code here and/or call the base class
  if (!m_strTailFile || (m_strTailFile == ""))
  {
    // Don't load the file.
    return FALSE;

    pApp = (CTailApp*) AfxGetApp ();  

    if (pSettings->GetAutoLoad())
    {  
      m_strTailFile = AfxGetApp()->GetProfileString (_T("Recent File List"), _T("File1"), NULL);
    }
    else
    {
      return FALSE;
    } 
  }

  SetTitle (m_strTailFile);

  // Interesting fix. 
  m_strPathName = CString (m_strTailFile);

  SetPathName (m_strTailFile, FALSE);

/*  if (m_pActiveView == NULL)
  {
    m_pActiveView = GetActiveView ();
  } */

	return CDocument::OnNewDocument();
}

void CTailDoc::ShowAnotherWindow()
{
    CTailMultiDocTemplate* pTempl = (CTailMultiDocTemplate*)  GetDocTemplate();

    CFrameWnd* pFrame = pTempl->CreateNewFrame (RUNTIME_CLASS (CTailView), this, NULL);

    // Initialize & show the new window
    pTempl->InitialUpdateFrame(pFrame, this);
}

CTailView* CTailDoc::GetActiveView (void)
{
  CTailView* pView = NULL;
  POSITION pos = 0;

  pos = GetFirstViewPosition ();

  while (pos != NULL)
  {
    pView = (CTailView*) GetNextView (pos);
  }

  return pView;
}