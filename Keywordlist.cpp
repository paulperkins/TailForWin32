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
 * $Id: Keywordlist.cpp,v 1.5 2003/11/25 08:15:13 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "shlwapi.h"

#include "tail.h"
#include "keywordlist.h"
#include "plugin.h"
#include "settings.h"

#define KEYWORD_FILE    "keywords.cfg"

CKeywordPlugin::CKeywordPlugin (void)
{
  m_bActive = FALSE;
  m_pPlugin = NULL;
}

CKeywordPlugin::CKeywordPlugin (CPlugin* pPlugin)
{
  m_bActive = FALSE;
  m_pPlugin = pPlugin;
}

CKeywordPlugin::~CKeywordPlugin (void)
{

}


BOOL CKeywordPlugin::Active (
  void)
{
  return m_bActive;
}

void CKeywordPlugin::SetActive (
  BOOL bActive)
{
  m_bActive = bActive;
}

const char* CKeywordPlugin::GetShortName (void)
{
  return m_pPlugin->GetShortName ();
}

CKeywordListItem::CKeywordListItem (void)
{
  m_bActive = FALSE; 
  m_dwCount = 0; 
  memset (m_szKeyword, 0, KEYWORD_NAME_SIZE); 
  m_crColour = RGB (255, 0, 0);
  
  m_pPlugins = new CObList;
}

CKeywordListItem::CKeywordListItem (const char* pszKeyword, BOOL bActive)
{
  m_bActive = bActive; 
  m_dwCount = 0; 
  m_crColour = RGB (255, 0, 0);

  strcpy (m_szKeyword, pszKeyword);
 
  m_pPlugins = new CObList;
}

CKeywordListItem::CKeywordListItem (
  const char* pszKeyword, 
  BOOL        bActive,
  COLORREF    cr)
{
  m_bActive = bActive; 
  m_dwCount = 0; 
  m_crColour = cr;

  strcpy (m_szKeyword, pszKeyword);
 
  m_pPlugins = new CObList;
}

CKeywordListItem::~CKeywordListItem (void)
{
  POSITION stPos;

  for (stPos = m_pPlugins->GetHeadPosition (); stPos != NULL; ) 
  {
    delete (CKeywordPlugin*) m_pPlugins->GetNext (stPos);
  } 

  delete m_pPlugins;
}

CKeywordPlugin* CKeywordListItem::FindPlugin (
  const char* pszShortName)
{
  POSITION stPos;
  CKeywordPlugin* pPlugin = NULL;

  for (stPos = m_pPlugins->GetHeadPosition (); stPos != NULL; ) 
  {
    pPlugin = (CKeywordPlugin*) m_pPlugins->GetNext (stPos);

    if (strcmp (pPlugin->GetShortName (), pszShortName) == 0)
    {
      return pPlugin;
    }
  }

  return NULL;
}

CKeywordPlugin* CKeywordListItem::FindPlugin (
  CPlugin* pPlugin)
{
  POSITION stPos;
  CKeywordPlugin* pKeywordPlugin = NULL;

  for (stPos = m_pPlugins->GetHeadPosition (); stPos != NULL; ) 
  {
    pKeywordPlugin = (CKeywordPlugin*) m_pPlugins->GetNext (stPos);

    if (pKeywordPlugin->m_pPlugin == pPlugin)
    {
      return pKeywordPlugin;
    }
  }

  return NULL;
}


CKeywordPlugin* CKeywordListItem::FindPlugin (
  long lIndex)
{
  POSITION stPos;

  if ((stPos = m_pPlugins->FindIndex (lIndex)) != NULL)
  {
      return (CKeywordPlugin*) m_pPlugins->GetAt (stPos);
  }

  return NULL;
}


CKeywordPlugin* CKeywordListItem::AddPlugin (
  const char* pszPlugin)
{
  POSITION stPos;

  CString* str = new CString (pszPlugin);

  stPos = m_pPlugins->AddTail ((CObject*) str);

  return (CKeywordPlugin*) m_pPlugins->GetAt (stPos);
}

CKeywordPlugin* CKeywordListItem::AddPlugin (
  CPlugin* pPlugin)
{
  CKeywordPlugin* pNewPlugin = NULL;

  if (!(pNewPlugin = FindPlugin (pPlugin->GetShortName())))
  {
    pNewPlugin = new CKeywordPlugin (pPlugin);

    m_pPlugins->AddTail ((CObject*) pNewPlugin);
  }

  return pNewPlugin;
}

BOOL CKeywordListItem::Active (void)
{
  return m_bActive;
}

COLORREF CKeywordListItem::Colour (void)
{
  return m_crColour;
}

const char* CKeywordListItem::Keyword (void)
{
  return &m_szKeyword[0];
}

void CKeywordListItem::SetActive (BOOL bActive)
{
  m_bActive = bActive;
}

void CKeywordListItem::SetColour (COLORREF cr)
{
  m_crColour = cr;
}

CKeywordList::CKeywordList (
  void)
{
  m_pItems = new CObList;
}

CKeywordList::~CKeywordList (
  void)
{
  POSITION stPos;

  for (stPos = m_pItems->GetHeadPosition (); stPos != NULL; ) 
  {
    delete (CKeywordListItem*) m_pItems->GetNext (stPos);
  }

  delete m_pItems;
}

CKeywordListItem* CKeywordList::FindKeyword (
  const char* pszName)
{
  POSITION stPos;
  CKeywordListItem* pKeyword = NULL;

  for (stPos = m_pItems->GetHeadPosition (); stPos != NULL; ) 
  {
    pKeyword = (CKeywordListItem*) m_pItems->GetNext (stPos);

    if (strcmp (pKeyword->Keyword (), pszName) == 0)
    {
      return pKeyword;
    }
  }

  return NULL;
}

CKeywordListItem* CKeywordList::FindKeyword (
  long lIndex)
{
  POSITION stPos;

  if ((stPos = m_pItems->FindIndex (lIndex)) != NULL)
  {
      return (CKeywordListItem*) m_pItems->GetAt (stPos);
  }

  return NULL;
}

CKeywordListItem* CKeywordList::AddKeyword (
  const char* pszKeyword)
{
  CKeywordListItem* pNewKeyword = NULL;
  CPlugin* pPlugin = NULL;
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CSettings* pSettings = theApp->GetSettings ();

  if (!(pNewKeyword = FindKeyword (pszKeyword)))
  {
    pNewKeyword = new CKeywordListItem (pszKeyword, TRUE);
    m_pItems->AddTail ((CObject*) pNewKeyword);

    // Add the plugins.
    int j = 0;

    while (pPlugin = pSettings->FindPlugin (j++))
    {
      // Add a reference to each plugin to the keywords.
      pNewKeyword->AddPlugin (pPlugin);
    }
  }

  return pNewKeyword;
}

CKeywordListItem* CKeywordList::AddKeyword (
  const char* pszKeyword,
  BOOL        bActive)
{
  CKeywordListItem* pNewKeyword = NULL;

  if (!(pNewKeyword = FindKeyword (pszKeyword)))
  {
    pNewKeyword = new CKeywordListItem (pszKeyword, bActive);
    m_pItems->AddTail ((CObject*) pNewKeyword);
  }

  return pNewKeyword;
}

CKeywordListItem* CKeywordList::AddKeyword (
  const char* pszKeyword,
  BOOL        bActive,
  COLORREF    cr)
{
  CKeywordListItem* pNewKeyword = NULL;

  if (!(pNewKeyword = FindKeyword (pszKeyword)))
  {
    pNewKeyword = new CKeywordListItem (pszKeyword, bActive, cr);
    m_pItems->AddTail ((CObject*) pNewKeyword);
  }

  return pNewKeyword;
}

CKeywordListItem* CKeywordList::AddKeyword (
  CKeywordListItem* pKeyword)
{
  m_pItems->AddTail ((CObject*) pKeyword);

  return pKeyword;
}

BOOL CKeywordList::DeleteKeyword (
  long lIndex)
{
  POSITION stPos;
  CKeywordListItem* pKeyword = NULL;

  if ((stPos = m_pItems->FindIndex (lIndex)) != NULL)
  {
    pKeyword = (CKeywordListItem*) m_pItems->GetAt (stPos);

    m_pItems->RemoveAt (stPos);

    delete (CKeywordListItem*) pKeyword;

    return TRUE;
  }

  return FALSE;
}


long CKeywordList::GetNumKeywords (
  void)
{
  return m_pItems->GetCount ();
}


/////////////////////////////////////////////////////////////////////////////
// Public Functions
//

// Public function to load keyword file.
BOOL CKeywordList::LoadFile (
  CPluginList* pPlugins)
{
  CPlugin*  pPlugin = NULL;
  CKeywordPlugin*  pKeywordPlugin = NULL;
  CKeywordListItem* pKeyword = NULL;
  char* pszItem = NULL;
  FILE* fp = NULL;
  char szLine[KEYWORD_NAME_SIZE + 1] = "";
  char szAppPath[_MAX_PATH] = "";
  char szDrive[_MAX_DRIVE] = "";
  char szPath[_MAX_PATH] = "";
  char szConfigFile[_MAX_PATH] = "";
  char szText[1024] = "";

  CTailApp* theApp = (CTailApp*) AfxGetApp ();

  long lNumKeywords = 0;

  // PP: Change this to be a parameter to function.
  GetAppPath (szConfigFile);  
  strcat (szConfigFile, KEYWORD_FILE);

  fp = fopen (szConfigFile, "r");

  if (fp == NULL)
  {
    LogMessage ("Could not open keyword file '%s'", szConfigFile);
    return FALSE;
  }

  while (1)
  {
    fgets (szLine, sizeof (szLine) - 1, fp);

    if (feof (fp))
    {
      break;
    }

    lNumKeywords++;
  }

  if (lNumKeywords)
  {
    int i = 0;

    LogMessage ("Found %ld keywords in '%s'", lNumKeywords, szConfigFile);

//    ppszList = (char**) malloc ((lNumKeywords + 1) * sizeof (char*));

//    dwNumKeywords = lNumKeywords;
//    pstKeywordList = (KEYWORD_LIST*) malloc (lNumKeywords * sizeof (KEYWORD_LIST));

//    memset (pstKeywordList, 0, (lNumKeywords * sizeof (KEYWORD_LIST)));

    fseek (fp, 0, SEEK_SET);

    while (1)
    {
      fgets (szLine, sizeof (szLine) - 1, fp);

      if (szLine[strlen (szLine) - 1] == '\n')
      {
        szLine[strlen (szLine) - 1] = '\0';
      }

//      ppszList[i] = NULL;

      if (feof (fp))
      {
        break;
      }

//      ppszList[i] = (char*) malloc (sizeof (szLine));
//      *ppszList[i] = '\0';

//      strcpy (pstKeywordList[i].szKeyword, szLine);
//      pstKeywordList[i].bExclude = TRUE;
//      strcpy (ppszList[i++], szLine);

      pKeyword = AddKeyword (szLine);

      int j = 0;

      while (pPlugin = pPlugins->FindPlugin (j++))
      {
        // Add a reference to each plugin to the keywords.
        pKeywordPlugin = pKeyword->AddPlugin (pPlugin);
      }
    }
  }

  fclose (fp);

  sprintf (szText, "The keyword file '%s' has now been imported.\n"
                   "The file can safely be deleted.\n", 
                   szConfigFile);

  ::AfxMessageBox (szText);

  return TRUE;
}

BOOL CKeywordList::Load (
  CPluginList* pPlugins)
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CPlugin*  pPlugin = NULL;
  CKeywordPlugin*  pKeywordPlugin = NULL;
  CKeywordListItem* pKeyword = NULL;
  COLORREF cr;
  long  lNumKeys = 0;
  DWORD dwSize = 0;
  DWORD dwType = 0;
  HKEY  hKey = 0;
  HKEY  hSubKey = 0;
  HKEY  hSubSubKey = 0;
  HKEY  hSubSubSubKey = 0;
  BOOL  bActive = FALSE;
  char  szKey[512] = "";
  char  szLine[KEYWORD_NAME_SIZE + 1] = "";
  int   j = 0;

  if ((ERROR_SUCCESS == RegOpenKey (HKEY_CURRENT_USER, "Software", &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, theApp->m_pszRegistryKey, &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, "Tail for Win32", &hKey))
  && (ERROR_SUCCESS == RegOpenKey (hKey, "Keywords", &hKey)))
  {
    dwSize = sizeof (long);

    if (ERROR_SUCCESS == RegQueryValueEx (hKey, "NumKeywords", NULL, &dwType, (LPBYTE) &lNumKeys, &dwSize))
    {
      LogMessage ("Debug: Registry thinks there are %ld keywords.", lNumKeys);

      for (int i = 0; i < lNumKeys; i++)
      {
        sprintf (szKey, "Keyword %ld", i);

        if (ERROR_SUCCESS == RegOpenKey (hKey, szKey, &hSubKey))
        {
          memset (&szLine[0], 0, sizeof (szLine));

          dwSize = sizeof (szLine);
          RegQueryValueEx (hSubKey, "Keyword", NULL, &dwType, (LPBYTE) &szLine[0], &dwSize);

          dwSize = sizeof (bActive);
          RegQueryValueEx (hSubKey, "Active", NULL, &dwType, (LPBYTE) &bActive, &dwSize);

          dwSize = sizeof (COLORREF);
          if (ERROR_SUCCESS == RegQueryValueEx (hSubKey, "Colour", NULL, &dwType, (LPBYTE) &cr, &dwSize))
          {
            pKeyword = AddKeyword (szLine, bActive, cr);
          }
          else
          {
            pKeyword = AddKeyword (szLine, bActive);
          }

          j = 0;

          // Loop over the plugins.
          while (pPlugin = pPlugins->FindPlugin (j++))
          {
            // Add a reference to each plugin to the keywords.
            pKeywordPlugin = pKeyword->AddPlugin (pPlugin);

            bActive = FALSE;

            // Now fetch the active/inactive status of each plugin from the registry.
            if ((ERROR_SUCCESS == RegOpenKey (hSubKey, "Plugins", &hSubSubKey))
            && (ERROR_SUCCESS == RegOpenKey (hSubSubKey, pPlugin->GetShortName(), &hSubSubSubKey)))
            {
              dwSize = sizeof (bActive);
              RegQueryValueEx (hSubSubSubKey, "Active", NULL, &dwType, (LPBYTE) &bActive, &dwSize);

            }
            
            pKeywordPlugin->SetActive (bActive);
          }
        }
      }
    }
  }
  else
  {
    LogMessage ("No keyword list registry entry ('%s').", "Keywords");
    return FALSE;
  }

  RegCloseKey (hKey);
  RegCloseKey (hSubKey);

  return TRUE;
}

BOOL CKeywordList::Save (
  void)
{
  CTailApp* theApp = (CTailApp*) AfxGetApp ();
  CKeywordListItem* pKeyword = NULL;
  CKeywordPlugin* pPlugin = NULL;
  COLORREF cr;
  long  lNumKeys = 0;
  DWORD dwSize = 0;
  DWORD dwType = 0;
  HKEY  hKey = 0;
  HKEY  hSubKey = 0;
  HKEY  hSubSubKey = 0;
  HKEY  hSubSubSubKey = 0;
  BOOL  bActive = FALSE;
  char  szKey[512] = "";
  char  szValue[512] = "";
  char  szLine[KEYWORD_NAME_SIZE + 1] = "";

  if ((ERROR_SUCCESS == RegOpenKey (HKEY_CURRENT_USER, "Software", &hKey))
   && (ERROR_SUCCESS == RegOpenKey (hKey, theApp->m_pszRegistryKey, &hKey))
   && (ERROR_SUCCESS == RegOpenKeyEx (hKey, "Tail for Win32", 0, KEY_ALL_ACCESS, &hKey)))
  {
    SHDeleteKey (hKey, "Keywords");
    
    if (ERROR_SUCCESS == RegCreateKey (hKey, "Keywords", &hKey))
    {
      lNumKeys = GetNumKeywords (); 

      RegSetValueEx (hKey, "NumKeyWords", NULL, REG_DWORD, (LPBYTE) &lNumKeys, sizeof (long));

      for (int i = 0; i < lNumKeys; i++)
      {
        pKeyword = FindKeyword(i);

        sprintf (szKey, "Keyword %ld", i);
        strcpy (szValue, pKeyword->Keyword());
        
        RegSetValueEx (hKey, szKey, NULL, REG_SZ, (LPBYTE) &szValue[0], sizeof (szValue));              

        if (ERROR_SUCCESS == RegCreateKey (hKey, szKey, &hSubKey))
        {
          memset (&szLine[0], 0, sizeof (szLine));

          strcpy (szLine, pKeyword->Keyword());
          dwSize = sizeof (szLine);

          RegSetValueEx (hSubKey, "Keyword", NULL, REG_SZ, (LPBYTE) &szLine[0], dwSize);

          bActive = pKeyword->Active();
          dwSize = sizeof (bActive);
          RegSetValueEx (hSubKey, "Active", NULL, REG_DWORD, (LPBYTE) &bActive, dwSize);

          cr = pKeyword->Colour();
          dwSize = sizeof (cr);
          RegSetValueEx (hSubKey, "Colour", NULL, REG_DWORD, (LPBYTE) &cr, dwSize);

          if (ERROR_SUCCESS == RegCreateKey (hSubKey, "Plugins", &hSubSubKey))
          {
            int j = 0;

            while (pPlugin = pKeyword->FindPlugin (j))
            {
              sprintf (szKey, "Plugin %ld", j);
              strcpy (szValue, pPlugin->GetShortName());
              
              RegSetValueEx (hSubSubKey, szKey, NULL, REG_SZ, (LPBYTE) &szValue[0], sizeof (szValue));              

              strcpy (szKey, pPlugin->GetShortName());

              if (ERROR_SUCCESS == RegCreateKey (hSubSubKey, szKey, &hSubSubSubKey))
              {
                bActive = pPlugin->Active();
                dwSize = sizeof (bActive);

                RegSetValueEx (hSubSubSubKey, "Active", NULL, REG_DWORD, (LPBYTE) &bActive, dwSize);
              }

              j++;
            }

            strcpy (szKey, "NumPlugins");
            sprintf (szValue, "%ld", j);
            
            RegSetValueEx (hSubSubKey, szKey, NULL, REG_SZ, (LPBYTE) &szValue[0], sizeof (szValue));              
          }
        }
      }   
    }
    
    RegCloseKey (hKey);
    RegCloseKey (hSubKey);
  } 

  return TRUE;
}