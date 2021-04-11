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
 * $Id: Plugin.h,v 1.4 2003/11/25 08:15:14 paulperkins Exp $
 * 
 */

#ifndef __TAIL_PLUGIN_H__
#define __TAIL_PLUGIN_H__


#define PLUGIN_ENTRY_METHOD   "TailEntry"
#define PLUGIN_PROCESS_METHOD "TailProcess"
#define PLUGIN_CONFIGURE_METHOD "TailConfigure"

#define PLUGIN_GET_NAME           (1)
#define PLUGIN_GET_DESCRIPTION    (2)
#define PLUGIN_GET_KEYWORD_COUNT  (3)
#define PLUGIN_GET_KEYWORD        (4)
#define PLUGIN_GET_FILE_EXT_COUNT (5)
#define PLUGIN_GET_FILE_EXT       (6)
#define PLUGIN_GET_SHORT_NAME     (7)


typedef int __declspec (dllimport) (*TAILENTRY) (int, int, char*);
typedef int __declspec (dllimport) (*TAILPROCESS) (int, char*, char*, char*);
typedef int __declspec (dllimport) (*TAILCONFIGURE) (HWND);


BOOL LoadPlugins (
  void);


class CPlugin
{
public:
  CPlugin ()
  {
    strcpy (szFilename, "");
    strcpy (szShortName, "");
    strcpy (szName, "");
    strcpy (szDescription, "");
    nNumKeywords = 0;
    hModule = 0;
    pProcessFn = NULL;
    pConfigureFn = NULL;
  }

  BOOL Action (const char* pszContext1, const char* pszContext2, const char* pszContext3);
  BOOL Load (char* pszPlugin);
  BOOL GotKeyword (const char* pszKeyword);
  void Configure (HWND hWnd);

  char* GetFilename ()
  {
    return szFilename;
  }

  char* GetName ()
  {
    return szName;
  }

  char* GetShortName ()
  {
    return szShortName;
  }

  char* GetDescription ()
  {
    return szDescription;
  }

private:
  BOOL GetMethods ();


  char    szFilename[_MAX_FNAME];
  char    szName[_MAX_FNAME];
  char    szShortName[_MAX_FNAME];
  char    szDescription[2048];
  int     nNumKeywords;
  CString strKeywordList;
  HMODULE hModule;

  TAILPROCESS pProcessFn;
  TAILCONFIGURE pConfigureFn;
};

class CPluginList
{
public:
  CPluginList ()
  {
    pItems = new CObList;
  }

  ~CPluginList ();

  BOOL Load (void);

  void AddPlugin (CPlugin* pPlugin);
  CPlugin* FindPlugin (const char* pszName);
  CPlugin* FindPlugin (long lIndex);
  long GetNumPlugins ();
  char* GetPluginFilename (long lIndex);
  char* GetPluginName (long lIndex);
  char* GetPluginShortName (long lIndex);
  char* GetPluginDescription (long lIndex);

private:
  CObList* pItems;

};


#endif // #ifndef __TAIL_PLUGIN_H__