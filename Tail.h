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
 * $Id: Tail.h,v 1.10 2003/11/25 08:15:14 paulperkins Exp $
 * 
 */

#if !defined(AFX_TAIL_H__92759CE5_A775_11D1_A186_006097AE8476__INCLUDED_)
#define AFX_TAIL_H__92759CE5_A775_11D1_A186_006097AE8476__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "tallywindow.h"

#define PLUGIN_EXT  "dll"
#define PLUGIN_DIR  "plugins\\"

/////////////////////////////////////////////////////////////////////////////
// CTailApp:
// See Tail.cpp for the implementation of this class
//

class CTailMultiDocTemplate : public CMultiDocTemplate
{
public:
  CTailMultiDocTemplate( UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass );

  CFrameWnd* CreateNewFrame (CRuntimeClass *pNewViewClass, CDocument *pDoc, CFrameWnd *pOther);

};

class CSettings;
class CPluginList;
class CKeywordList;
class CWorkspaceDlg;
class CMainFrame;

class CTailApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CTailApp();
	~CTailApp();

  DWORD VersionCheck (void);

  CSettings* GetSettings (void);

  float m_fVersion;
  DWORD m_dwPlatformID;

/*  BOOL m_bBeepOnChanges;
  BOOL m_bIgnoreHotStartup;
  BOOL m_bShowAllLines;
  BOOL m_bDebug;
  BOOL m_bGotFont;
  int m_nTimeout;
  DWORD m_dwFunctions;
  BOOL m_bAutoLoad;
  BOOL m_bPaused;
  BOOL m_bHighlightLine;
  BOOL m_bHighlightWholeLine;
  long m_lHighlightColour;
  long m_lBackGroundColour;
  BOOL m_bShowWorkspace;
  BOOL m_bWordWrap;

  LOGFONT stLogFont;

  CPluginList*   m_pPlugins;
  CKeywordList*  m_pKeywords; */

  CWorkspaceDlg* m_pWorkspace;

//  CMultiDocTemplate* m_pDocTemplate;
  CTailMultiDocTemplate* m_pDocTemplate;
  CMainFrame* m_pMainFrame;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTailApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTailApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileBeep();
	afx_msg void OnUpdateFileBeep(CCmdUI* pCmdUI);
	afx_msg void OnPaused();
	afx_msg void OnUpdatePaused(CCmdUI* pCmdUI);
	afx_msg void OnIgnoreStartup();
	afx_msg void OnUpdateIgnoreStartup(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

  CSettings* m_pSettings;
};


void LogMessage (
  char* pszMessage,
  ...);

char* GetAppPath (
  char* pszPath);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAIL_H__92759CE5_A775_11D1_A186_006097AE8476__INCLUDED_)
