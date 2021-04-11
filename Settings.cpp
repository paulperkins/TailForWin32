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
 * $Id: Settings.cpp,v 1.1 2003/11/25 08:13:19 paulperkins Exp $
 * 
 */

#include "stdafx.h"

#include "settings.h"

#include "plugin.h"
#include "keywordlist.h"
#include "utils.h"

IMPLEMENT_DYNAMIC(CSettings, CObject)

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
CSettings::CSettings (void)
{
  m_bBeepOnChanges = FALSE;
  m_bIgnoreHotStartup = FALSE;
  m_bShowAllLines = FALSE;
  m_bDebug = FALSE;
  m_bGotFont = FALSE;
  m_nTimeout =  0;
  m_dwFunctions = FALSE;
  m_bAutoLoad = FALSE;
  m_fVersion = 0.0;
  m_dwPlatformID = 0;
  m_bPaused = FALSE;
  m_bHighlightLine = FALSE;
  m_bHighlightWholeLine = FALSE;
  m_lHighlightColour = 0;
  m_lBackGroundColour = 0;
  m_bShowWorkspace = FALSE;
  m_bWordWrap = FALSE;
  m_bAlwaysOnTop = FALSE;
  m_bMaxChild = FALSE;

  memset (&m_stLogFont, 0, sizeof (LOGFONT));

  m_pPlugins = new CPluginList;
  m_pKeywords = new CKeywordList;

  memset (&m_rcWindow, 0, sizeof (RECT));

  m_bDebug = DebugFilePresent ();
}

/////////////////////////////////////////////////////////////////////////////
// CSettings 
//
CSettings::~CSettings (void)
{
  delete (CPluginList*) m_pPlugins;
  delete (CKeywordList*) m_pKeywords;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings 
//
BOOL CSettings::Load (void)
{
  CWinApp* theApp = NULL;
  LPCSTR lpszRegKey = NULL;
  long lRtn = 0;
  DWORD dwSize = 0;
  DWORD dwType = 0;
  HKEY hKey = 0;

  theApp = (CWinApp*) AfxGetApp ();

  m_rcWindow.left   = theApp->GetProfileInt (_T("Settings"), _T("Left"), 0);
  m_rcWindow.top    = theApp->GetProfileInt (_T("Settings"), _T("Top"), 0);
  m_rcWindow.right  = theApp->GetProfileInt (_T("Settings"), _T("Right"), 640);
  m_rcWindow.bottom = theApp->GetProfileInt (_T("Settings"), _T("Bottom"), 480);

  m_bAlwaysOnTop = theApp->GetProfileInt (_T("Settings"), _T("AlwaysOnTop"), 0);
  m_bMaxChild    = theApp->GetProfileInt (_T("Settings"), _T("Maximised"), 0);

  m_bBeepOnChanges      = theApp->GetProfileInt (_T("Settings"), _T("BeepOnChange"),       0);
  m_nTimeout            = theApp->GetProfileInt (_T("Settings"), _T("Timeout"),            1000);
  m_bIgnoreHotStartup   = theApp->GetProfileInt (_T("Settings"), _T("IgnoreHotStartup"),   TRUE);
  m_bShowAllLines       = theApp->GetProfileInt (_T("Settings"), _T("ShowAllLines"),       TRUE);
  m_bAutoLoad           = theApp->GetProfileInt (_T("Settings"), _T("AutoLoadLastFile"),   TRUE);
  m_bDebug              = theApp->GetProfileInt (_T("Settings"), _T("Debug"),              FALSE);
  m_bHighlightLine      = theApp->GetProfileInt (_T("Settings"), _T("HighlightLine"),      TRUE);
  m_bHighlightWholeLine = theApp->GetProfileInt (_T("Settings"), _T("HighlightWholeLine"), FALSE);
  m_lHighlightColour    = theApp->GetProfileInt (_T("Settings"), _T("HighlightColour"),    0x00D0D0D0);
  m_lBackGroundColour   = theApp->GetProfileInt (_T("Settings"), _T("BackGroundColour"),   ::GetSysColor(COLOR_WINDOW));
  m_bShowWorkspace      = theApp->GetProfileInt (_T("Settings"), _T("ShowWorkspace"),      TRUE);
  m_bWordWrap           = theApp->GetProfileInt (_T("Settings"), _T("WordWrap"),           FALSE);


  dwSize = sizeof (LOGFONT);

  if ((ERROR_SUCCESS == RegOpenKey (HKEY_CURRENT_USER, "Software", &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, TAIL_REG_KEY, &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, "Tail for Win32", &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, "Settings", &hKey)))
  {
    if (ERROR_SUCCESS == ( lRtn = RegQueryValueEx (hKey, "Font", NULL, &dwType, (LPBYTE) &m_stLogFont, &dwSize)))
    {
      m_bGotFont = TRUE;
      memcpy ((void*) &m_stLogFont, (void*) &m_stLogFont, sizeof (LOGFONT));
    }
    else
    {
      m_bGotFont = FALSE;
    }
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings 
//
BOOL CSettings::Save (void)
{
  CWinApp* theApp = NULL;
  LPCSTR lpszRegKey = NULL;
  long lRtn = 0;
  DWORD dwSize = 0;
  DWORD dwType = 0;
  HKEY hKey = 0;

  theApp = (CWinApp*) AfxGetApp ();

  theApp->WriteProfileInt (_T("Settings"), _T("Left"), m_rcWindow.left);
  theApp->WriteProfileInt (_T("Settings"), _T("Top"), m_rcWindow.top);
  theApp->WriteProfileInt (_T("Settings"), _T("Right"), m_rcWindow.right);
  theApp->WriteProfileInt (_T("Settings"), _T("Bottom"), m_rcWindow.bottom);

  theApp->WriteProfileInt (_T("Settings"), _T("AlwaysOnTop"), m_bAlwaysOnTop);
  theApp->WriteProfileInt (_T("Settings"), _T("BeepOnChange"), m_bBeepOnChanges);
  theApp->WriteProfileInt (_T("Settings"), _T("Timeout"), m_nTimeout);
  theApp->WriteProfileInt (_T("Settings"), _T("Maximised"), m_bMaxChild);
  theApp->WriteProfileInt (_T("Settings"), _T("IgnoreHotStartup"), m_bIgnoreHotStartup);
  theApp->WriteProfileInt (_T("Settings"), _T("ShowAllLines"), m_bShowAllLines);
  theApp->WriteProfileInt (_T("Settings"), _T("AutoLoadLastFile"), m_bAutoLoad);
  theApp->WriteProfileInt (_T("Settings"), _T("HighlightLine"), m_bHighlightLine);
  theApp->WriteProfileInt (_T("Settings"), _T("HighlightWholeLine"), m_bHighlightWholeLine);
  theApp->WriteProfileInt (_T("Settings"), _T("HighlightColour"), m_lHighlightColour);
  theApp->WriteProfileInt (_T("Settings"), _T("ShowWorkspace"), m_bShowWorkspace);
  theApp->WriteProfileInt (_T("Settings"), _T("WordWrap"), m_bWordWrap);
  theApp->WriteProfileInt (_T("Settings"), _T("BackGroundColour"), m_lBackGroundColour);

  if (m_bGotFont)
  {
    if ((ERROR_SUCCESS == RegOpenKey (HKEY_CURRENT_USER, "Software", &hKey))
    && (ERROR_SUCCESS == RegOpenKey (hKey, TAIL_REG_KEY, &hKey))
    && (ERROR_SUCCESS == RegOpenKey (hKey, "Tail for Win32", &hKey))
    && (ERROR_SUCCESS == RegOpenKey (hKey, "Settings", &hKey)))
    {
      RegSetValueEx (hKey, "Font", NULL, REG_BINARY, (LPBYTE) &m_stLogFont, sizeof (LOGFONT));
    }
  }

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Getters
//


/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetBeepOnChanges (void) { return m_bBeepOnChanges; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetIgnoreHotStartup (void) { return m_bIgnoreHotStartup; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetShowAllLines (void) { return m_bShowAllLines; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetDebug (void) { return m_bDebug; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetGotFont (void) { return m_bGotFont; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
int  CSettings::GetTimeout (void) { return m_nTimeout; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetAutoLoad (void) { return m_bAutoLoad; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
float CSettings::GetVersion (void) { return m_fVersion; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
DWORD CSettings::GetPlatformID (void) { return m_dwPlatformID; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetPaused (void) { return m_bPaused; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetHighlightLine (void) { return m_bHighlightLine; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetHighlightWholeLine (void) { return m_bHighlightWholeLine; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
long CSettings::GetHighlightColour (void) { return m_lHighlightColour; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
long CSettings::GetBackGroundColour (void) { return m_lBackGroundColour; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetShowWorkspace (void) { return m_bShowWorkspace; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetWordWrap (void) { return m_bWordWrap; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
RECT CSettings::GetWindowRect (void) 
{ 
  return m_rcWindow;  
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
LOGFONT CSettings::GetLogFont (void) 
{ 
  return m_stLogFont;  
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetAlwaysOnTop (void) { return m_bAlwaysOnTop; }

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::GetMaxChild (void) { return m_bMaxChild; }


/////////////////////////////////////////////////////////////////////////////
// CSettings
//
CPluginList* CSettings::GetPlugins (
  void)
{
  return m_pPlugins;
}

/////////////////////////////////////////////////////////////////////////////
// Setters
//


/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetBeepOnChanges (BOOL bBeepOnChanges)
{
  m_bBeepOnChanges = bBeepOnChanges;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetIgnoreHotStartup (BOOL bIgnoreHotStartup)
{
  m_bIgnoreHotStartup = bIgnoreHotStartup;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetShowAllLines (BOOL bShowAllLines)
{
  m_bShowAllLines = bShowAllLines;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetDebug (BOOL bDebug)
{
  m_bDebug = bDebug;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetGotFont (BOOL bGotFont)
{
  m_bGotFont = bGotFont;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetTimeout (int nTimeout)
{
  m_nTimeout = nTimeout;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetAutoLoad (BOOL bAutoLoad)
{
  m_bAutoLoad = bAutoLoad;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetVersion (float fVersion)
{
  m_fVersion = fVersion;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetPlatformID (DWORD dwPlatformID)
{
  m_dwPlatformID = dwPlatformID;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetPaused (BOOL bPaused)
{
  m_bPaused = bPaused;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetHighlightLine (BOOL bHighlightLine)
{
  m_bHighlightLine = bHighlightLine;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetHighlightWholeLine (BOOL bHighlightWholeLine)
{
  m_bHighlightWholeLine = bHighlightWholeLine;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetHighlightColour (long lHighlightColour)
{
  m_lHighlightColour = lHighlightColour;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetBackGroundColour (long lBackGroundColour)
{
  m_lBackGroundColour = lBackGroundColour;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetShowWorkspace (BOOL bWordWrap)
{
  m_bWordWrap = bWordWrap;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetWordWrap (BOOL bWordWrap)
{
  m_bWordWrap = bWordWrap;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetWindowRect (RECT rc) 
{ 
  m_rcWindow = rc;  

  if (m_rcWindow.left < 0)
  {
    m_rcWindow.left = 0;
  }

  if (m_rcWindow.top < 0)
  {
    m_rcWindow.top = 0;
  }

}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetLogFont (LOGFONT lf) 
{ 
  m_stLogFont = lf;  
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetAlwaysOnTop (BOOL bAlwaysOnTop) 
{ 
  m_bAlwaysOnTop = bAlwaysOnTop;  
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
void CSettings::SetMaxChild (BOOL bMaxChild) 
{ 
  m_bMaxChild = bMaxChild;  
}



long CSettings::GetNumPlugins (
  void)
{
  return m_pPlugins->GetNumPlugins ();
}

long CSettings::GetNumKeywords (
  void)
{
  return m_pKeywords->GetNumKeywords ();
}

CPlugin* CSettings::FindPlugin (
  long lIndex)
{
  return m_pPlugins->FindPlugin (lIndex);
}

CKeywordListItem* CSettings::FindKeyword (
  long lIndex)
{
  return m_pKeywords->FindKeyword (lIndex);
}

CKeywordListItem* CSettings::FindKeyword (
  const char* pszName)
{
  return m_pKeywords->FindKeyword (pszName);
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::LoadPlugins (
  void)
{
  return m_pPlugins->Load ();
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::LoadKeywords (
  void)
{
  return m_pKeywords->Load (m_pPlugins);
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::SaveKeywords (
  void)
{
  return m_pKeywords->Save ();
}


/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::LoadKeywordFile (
  void)
{
  return m_pKeywords->Load (m_pPlugins);
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
BOOL CSettings::DeleteKeyword (
  long lIndex)
{
  return m_pKeywords->DeleteKeyword (lIndex);
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
CKeywordListItem* CSettings::AddKeyword (
  const char* pszKeyword)
{
  return m_pKeywords->AddKeyword (pszKeyword);
}