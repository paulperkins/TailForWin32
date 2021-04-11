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
 * $Id: Keywordlist.h,v 1.4 2003/11/25 08:15:13 paulperkins Exp $
 * 
 */


#ifndef __TAIL_KEYWORD_LIST__
#define __TAIL_KEYWORD_LIST__

#define KEYWORD_NAME_SIZE (1024)
#define EVENT_PARAM_SIZE (1024)

extern BOOL LoadKeywordFile (
  void);

extern BOOL LoadKeywordReg (
  void);

extern BOOL SaveKeywordsReg (
  void);

class CPlugin;

class CKeywordPlugin
{
public:
  CKeywordPlugin (void);
  CKeywordPlugin (CPlugin* pPlugin);
  ~CKeywordPlugin (void);

  const char* GetShortName (void);
  BOOL Active (void);
  void SetActive (BOOL bActive);

  CPlugin* m_pPlugin;
  BOOL     m_bActive;
};

class CKeywordListItem
{
public:
  CKeywordListItem (void);
  CKeywordListItem (const char* pszKeyword, BOOL bActive = FALSE);
  CKeywordListItem (const char* pszKeyword, BOOL bActive, COLORREF cr);
  ~CKeywordListItem (void);

  CKeywordPlugin* AddPlugin (const char* pszPlugin);
  CKeywordPlugin* AddPlugin (CPlugin* pPlugin);  
  
  BOOL Active (void);
  const char* Keyword (void);
  COLORREF Colour (void);

  void SetActive (BOOL bActive);
  void SetColour (COLORREF cr);

  CKeywordPlugin* FindPlugin (const char* pszShortName);
  CKeywordPlugin* FindPlugin (long lIndex);
  CKeywordPlugin* FindPlugin (CPlugin* pPlugin);  

private:
  char     m_szKeyword[KEYWORD_NAME_SIZE + 1];
  DWORD    m_dwCount;
  BOOL     m_bActive;
  COLORREF m_crColour;
  CObList* m_pPlugins;
};

class CKeywordList
{
public:
  CKeywordList (void);
  ~CKeywordList (void);

  BOOL Load (CPluginList* pPlugins);
  BOOL LoadFile (CPluginList* pPlugins);
  BOOL Save (void);

  CKeywordListItem* AddKeyword (CKeywordListItem* pKeyword);
  CKeywordListItem* AddKeyword (const char* pszKeyword);
  CKeywordListItem* AddKeyword (const char* pszKeyword, BOOL bActive);
  CKeywordListItem* AddKeyword (const char* pszKeyword, BOOL bActive, COLORREF cr);
  CKeywordListItem* FindKeyword (const char* pszName);
  CKeywordListItem* FindKeyword (long lIndex);
  BOOL DeleteKeyword (long lIndex);

  long GetNumKeywords (void);

private:
  CObList* m_pItems;

};

#endif // #ifndef __TAIL_KEYWORD_LIST__
