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
 * $Id: Plugin.cpp,v 1.5 2003/11/25 08:15:13 paulperkins Exp $
 * 
 */


#include "stdafx.h"
#include "plugin.h"
#include "tail.h"

#include <io.h>
#include <stdio.h>


BOOL CPlugin::GetMethods ()
{
  TAILENTRY pEntry = NULL;
  char szKeyword[256] = "";
  int i = 0;

  pEntry = (TAILENTRY) GetProcAddress (hModule, PLUGIN_ENTRY_METHOD);

  if (pEntry == NULL)
  {
    LogMessage ("Could not get entry point for '%s'", szFilename);
    return FALSE;
  }

  pEntry (PLUGIN_GET_NAME, 0, szName);

  LogMessage ("Got name '%s' for '%s'", szName, szFilename);


  pEntry (PLUGIN_GET_DESCRIPTION, 0, szDescription);

  LogMessage ("Got description '%s' for '%s'", szDescription, szName);

  pEntry (PLUGIN_GET_SHORT_NAME, 0, szShortName);

  LogMessage ("Got short name '%s' for '%s'", szShortName, szName);

  pProcessFn = (TAILPROCESS) GetProcAddress (hModule, PLUGIN_PROCESS_METHOD);

  if (pProcessFn == NULL)
  {
    LogMessage ("Could not get process method entry point for '%s'", szName);
    return FALSE;
  }

  LogMessage ("Got process method entry point for '%s'", szName);


  pConfigureFn = (TAILCONFIGURE) GetProcAddress (hModule, PLUGIN_CONFIGURE_METHOD);

  if (pConfigureFn == NULL)
  {
    LogMessage ("Could not get configure method entry point for '%s'", szName);
  }
  else
  {
    LogMessage ("Got configure method entry point for '%s'", szName);
  }

  return TRUE;
}

BOOL CPlugin::Load (
  char* pszPlugin)
{
  HMODULE hMod = NULL;
//  CPlugin* pPlugin = NULL;

  LogMessage ("Attempting to load plugin '%s'", pszPlugin);

  hMod = LoadLibrary (pszPlugin);

  if (hMod == NULL)
  {
    LogMessage ("Could not load '%s'", pszPlugin);
    return FALSE;
  }
  
//  pPlugin = new CPlugin;

  hModule = hMod;
  strcpy (szFilename, pszPlugin);

  if (!GetMethods ())
  {
    FreeLibrary (hModule);
    return FALSE;
  }

  return TRUE;
}

void CPlugin::Configure (
  HWND hWnd)
{
  if (pConfigureFn)
  {
    pConfigureFn (hWnd);
  }
}

CPlugin* CPluginList::FindPlugin (
  const char* pszName)
{
  POSITION stPos;
  CPlugin* pPlugin = NULL;

  for (stPos = pItems->GetHeadPosition (); stPos != NULL; ) 
  {
    pPlugin = (CPlugin*) pItems->GetNext (stPos);

    if (pPlugin->GotKeyword (pszName))
    {
      return pPlugin;
    }
  }

  return NULL;
}


CPlugin* CPluginList::FindPlugin (
  long lIndex)
{
  POSITION stPos;
  CPlugin* pPlugin = NULL;

  if ((stPos = pItems->FindIndex (lIndex)) != NULL)
  {
      pPlugin = (CPlugin*) pItems->GetAt (stPos);
      return pPlugin;
  }

  return NULL;
}

BOOL CPlugin::GotKeyword (
  const char* pszKeyword)
{
  if (-1 != strKeywordList.Find (pszKeyword))
  {
    return TRUE;
  }

  return FALSE;
}

BOOL CPlugin::Action (
  const char* pszContext1, 
  const char* pszContext2,
  const char* pszContext3)
{
  if (pProcessFn)
  {
    pProcessFn (0, (char*) pszContext1, (char*) pszContext2, (char*) pszContext3);
  }

  return TRUE;
}

CPluginList::~CPluginList (
  void)
{
  POSITION stPos;

  for (stPos = pItems->GetHeadPosition (); stPos != NULL; ) 
  {
    delete (CPlugin*) pItems->GetNext (stPos);
  }

  delete pItems;
}

void CPluginList::AddPlugin (
  CPlugin* pPlugin)
{
  pItems->AddTail ((CObject*) pPlugin);
}

char* CPluginList::GetPluginFilename (
  long lIndex)
{
  POSITION stPos;
  CPlugin* pPlugin = NULL;

  if ((stPos = pItems->FindIndex (lIndex)) != NULL)
  {
      pPlugin = (CPlugin*) pItems->GetAt (stPos);
      return pPlugin->GetFilename ();
  }

  return NULL;
}

char* CPluginList::GetPluginName (
  long lIndex)
{
  POSITION stPos;
  CPlugin* pPlugin = NULL;

  if ((stPos = pItems->FindIndex (lIndex)) != NULL)
  {
      pPlugin = (CPlugin*) pItems->GetAt (stPos);
      return pPlugin->GetName ();
  }

  return NULL;
}

char* CPluginList::GetPluginShortName (
  long lIndex)
{
  POSITION stPos;
  CPlugin* pPlugin = NULL;

  if ((stPos = pItems->FindIndex (lIndex)) != NULL)
  {
      pPlugin = (CPlugin*) pItems->GetAt (stPos);
      return pPlugin->GetShortName ();
  }

  return NULL;
}

char* CPluginList::GetPluginDescription (
  long lIndex)
{
  POSITION stPos;
  CPlugin* pPlugin = NULL;

  if ((stPos = pItems->FindIndex (lIndex)) != NULL)
  {
      pPlugin = (CPlugin*) pItems->GetAt (stPos);
      return pPlugin->GetDescription ();
  }

  return NULL;
}

long CPluginList::GetNumPlugins (
  void)
{
  return pItems->GetCount ();
}

// Public function to load plugins.
BOOL CPluginList::Load (
  void)
{
  HINSTANCE hMod = NULL;
  char szAppPath[_MAX_PATH] = "";
  char szPlugin[_MAX_PATH] = "";
	struct _finddata_t file_info;
	char szFileSpec[_MAX_FNAME] = "";
	long hFind = 0;
  CPlugin* pPlugin = NULL;

  CTailApp* pApp = (CTailApp*) AfxGetApp ();

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
      AddPlugin (pPlugin);
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
        AddPlugin (pPlugin);
      }
		}
	}
	
	_findclose (hFind);

  return TRUE;
}

