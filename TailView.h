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
 * $Id: TailView.h,v 1.15 2003/11/25 08:48:28 paulperkins Exp $
 * 
 */

#if !defined(AFX_TAILVIEW_H__92759CEF_A775_11D1_A186_006097AE8476__INCLUDED_)
#define AFX_TAILVIEW_H__92759CEF_A775_11D1_A186_006097AE8476__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "taildoc.h"
#include "Eventlist.h"
#include "Plugin.h"

#include "richeditctrlex.h"

#include "common.h"

#include "filereaderthread.h"
#include "eventlogthread.h"
#include "settings.h"

#include "eventdisplay.h"


class TallyWindow;

class CTailView : public CView
{
protected: // create from serialization only
	CTailView();
	DECLARE_DYNCREATE(CTailView)

// Attributes
public:
	CTailDoc* GetDocument();

  void Update (void);
  HANDLE m_hThread;
  CWinThread* m_pThread;
  CRichEditCtrlEx m_ctlEdit;
  CStatusBarCtrl m_ctlStatusBar;
  READER_PARAMS m_stParams;
  EVENT_LOG_THREAD_PARAMS m_stELParams;
  HANDLE hDie;
  CPluginList m_Plugins;
  char** m_ppszList;
  long m_lNumKeywords;
  KEYWORD_LIST*   m_pstKeywordList; // Our own counts.
  BOOL m_bEventsVisible;
  BOOL m_bPaused;
  CSettings* m_pSettings;
  
  BOOL m_bTallyVisible;
  TallyWindow* m_pTallyWindow; 
  char m_szTailFile[_MAX_FNAME];
  char m_szDieEvent[_MAX_FNAME];

  BOOL AddEvent (char* pszEvent, char* pszSeverity, long lLine);

  void ReloadKeywords (void);


private:
  void Refresh ();
  BOOL LoadPlugins (void);
  void LoadKeywords (void);
  void SetViewFont (LOGFONT* pstLogFont);

  CTailDoc* pDoc;

  DWORD           m_dwNumKeywords;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTailView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	void CopyText();
	virtual ~CTailView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
  BOOL m_bUseTimeout;

protected:

private:

// Generated message map functions
protected:
	//{{AFX_MSG(CTailView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIgnoreStartup();
	afx_msg void OnDisplayHotlist();
	afx_msg void OnSetfont();
	afx_msg void OnConfigPlugins();
	afx_msg void OnTally();
	afx_msg void OnUpdateIgnoreStartup(CCmdUI* pCmdUI);
	afx_msg void OnViewGo();
	afx_msg void OnViewStop();
	afx_msg void OnHiglightLine();
	afx_msg void OnUpdateHiglightLine(CCmdUI* pCmdUI);
	afx_msg void OnWordwrap();
	afx_msg void OnUpdateWordwrap(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnSetBackground();
	afx_msg void OnSetHighlightColour();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TailView.cpp
inline CTailDoc* CTailView::GetDocument()
   { return (CTailDoc*)m_pDocument; }
#endif

BOOL InsertText (
  CTailView*      pView,
  char*           pszText,
  BOOL            bDoColouring,
  long*           plMatchCount,
  BOOL*           pbMatch);

void ScrollToBottom (
  HWND hwnd);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAILVIEW_H__92759CEF_A775_11D1_A186_006097AE8476__INCLUDED_)
