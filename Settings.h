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
 * $Id: Settings.h,v 1.1 2003/11/25 08:13:19 paulperkins Exp $
 * 
 */

#ifndef __TAIL_SETTINGS__
#define __TAIL_SETTINGS__

#include "afxwin.h"

#define TAIL_REG_KEY    "Objektiv"

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
// This class abstracts a collection of files to be monitored.
//

class CPluginList;
class CPlugin;
class CKeywordListItem;
class CKeywordList;
class CKeyword;

class CSettings : public CObject
{
protected:
  DECLARE_DYNAMIC(CSettings)

  BOOL m_bBeepOnChanges;
  BOOL m_bIgnoreHotStartup;
  BOOL m_bShowAllLines;
  BOOL m_bDebug;
  BOOL m_bGotFont;
  int m_nTimeout;
  DWORD m_dwFunctions;
  BOOL m_bAutoLoad;
  float m_fVersion;
  DWORD m_dwPlatformID;
  BOOL m_bPaused;
  BOOL m_bHighlightLine;
  BOOL m_bHighlightWholeLine;
  long m_lHighlightColour;
  long m_lBackGroundColour;
  BOOL m_bShowWorkspace;
  BOOL m_bWordWrap;
  RECT m_rcWindow;
  BOOL m_bAlwaysOnTop;
  BOOL m_bMaxChild;

  LOGFONT m_stLogFont;

  CPluginList*   m_pPlugins;
  CKeywordList*  m_pKeywords;


public:
  CSettings (void);
  ~CSettings (void);

  BOOL Load (void);
  BOOL Save (void);

  BOOL LoadPlugins (void);
  BOOL LoadKeywords (void);
  BOOL LoadKeywordFile (void);


  void SetBeepOnChanges (BOOL bBeepOnChanges);
  void SetIgnoreHotStartup (BOOL bIgnoreHotStartup);
  void SetShowAllLines (BOOL bShowAllLines);
  void SetDebug (BOOL bDebug);
  void SetGotFont (BOOL bGotFont);
  void SetTimeout (int nTimeout);
  void SetAutoLoad (BOOL bAutoLoad);
  void SetVersion (float fVersion);
  void SetPlatformID (DWORD dwPlatformID);
  void SetPaused (BOOL bPaused);
  void SetHighlightLine (BOOL bHighlightLine);
  void SetHighlightWholeLine (BOOL bHighlightWholeLine);
  void SetHighlightColour (long lHighlightColour);
  void SetBackGroundColour (long lBackGroundColour);
  void SetShowWorkspace (BOOL bShowWorkspace);
  void SetWordWrap (BOOL bWordWrap);
  void SetAlwaysOnTop (BOOL bAlwaysOnTop);
  void SetMaxChild (BOOL bMaxChild);
  void SetWindowRect (RECT rc);
  void SetLogFont (LOGFONT lf);

  BOOL GetBeepOnChanges (void);
  BOOL GetIgnoreHotStartup (void);
  BOOL GetShowAllLines (void);
  BOOL GetDebug (void);
  BOOL GetGotFont (void);
  int  GetTimeout (void);
  BOOL GetAutoLoad (void);
  float GetVersion (void);
  DWORD GetPlatformID (void);
  BOOL GetPaused (void);
  BOOL GetHighlightLine (void);
  BOOL GetHighlightWholeLine (void);
  long GetHighlightColour (void);
  long GetBackGroundColour (void);
  BOOL GetShowWorkspace (void);
  BOOL GetWordWrap (void);
  BOOL GetAlwaysOnTop (void);
  BOOL GetMaxChild (void);
  RECT GetWindowRect (void);
  LOGFONT GetLogFont (void);
  CPluginList* GetPlugins (void);

  long GetNumPlugins (void);
  long GetNumKeywords (void);
  long FindPlugin (void);
  CPlugin* FindPlugin (long lIndex);
  CKeywordListItem* FindKeyword (long lIndex);
  CKeywordListItem* FindKeyword (const char* pszName);

  BOOL DeleteKeyword (long lIndex);
  CKeywordListItem* AddKeyword (const char* pszKeyword);

  BOOL SaveKeywords (void);

};

#endif // #ifndef __TAIL_SETTINGS__
