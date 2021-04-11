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
 * $Id: utils.cpp,v 1.8 2003/11/14 12:26:33 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include "string.h"

#define LOGFILE       "tail.log"

static bDebug = FALSE;

char* GetAppPath (
  char* pszPath)
{
  char szDrive[_MAX_DRIVE] = "";
  char szPath[_MAX_PATH] = "";
  char szAppPath[_MAX_PATH] = "";

  GetModuleFileName (AfxGetApp ()->m_hInstance, szAppPath, _MAX_PATH);

  _splitpath (szAppPath, szDrive, szPath, NULL, NULL);
  _makepath (pszPath, szDrive, szPath, NULL, NULL);

  return pszPath;
}

BOOL DebugFilePresent (
  void)
{
  FILE* fp = NULL;

  char szPath[_MAX_PATH] = "";

  GetAppPath (szPath);

  strcat (szPath, LOGFILE);

  if (fp = fopen (szPath, "r"))
  {
    fclose (fp);

    bDebug = TRUE;

    return TRUE;
  }

  return FALSE;
}

void GetTimeStamp (
  time_t* pstTime,
  char*   pszBuf)
{
	struct tm *newtime;

	newtime = localtime (pstTime); 

	sprintf (pszBuf, "%02d/%02d/%04d %02d:%02d:%02d", 
        newtime->tm_mday,
        newtime->tm_mon,
        newtime->tm_year + 1900,
				newtime->tm_hour,
				newtime->tm_min,
				newtime->tm_sec);
}

void GetTimeStamp (
  char* pszBuf)
{
	struct tm *newtime;
	time_t aclock;

  time (&aclock);                

	newtime = localtime (&aclock); 

	sprintf (pszBuf, "%02d:%02d:%02d", 
				newtime->tm_hour,
				newtime->tm_min,
				newtime->tm_sec);
}

/******************************************************************************
* LogMessage
*
*/
void LogMessage (
  char* pszMessage,
  ...)
{
  FILE* fp = NULL;
  char szOut[1024] = "";
  char szLine[2048] = "";
  va_list ap;
	struct tm *newtime;
	time_t aclock;
  char szPath[_MAX_PATH] = "";

  GetAppPath (szPath);

  strcat (szPath, LOGFILE);

  va_start (ap, pszMessage);
  vsprintf (szOut, pszMessage, ap);
  va_end (ap);

  // Only write to it if the file already exists.
  if (bDebug)
  {
    // Now open the file for writing.
    fp = fopen (szPath, "a");

    if (!fp)
    {
      return;
    }

	  time (&aclock);                

	  newtime = localtime (&aclock); 

	  sprintf (szLine, "[%04d%02d%02d %02d:%02d:%02d] (%08x) ", 
				  newtime->tm_year + 1900, 
				  newtime->tm_mon + 1,
				  newtime->tm_mday,
				  newtime->tm_hour,
				  newtime->tm_min,
				  newtime->tm_sec,
          GetCurrentThreadId ());

    strcat (szLine, szOut);

    fprintf (fp, "%s\n", szLine);

    fclose (fp);  
  }
}

void LogDebugMessage (
  char* pszMessage,
  ...)
{
  va_list ap;
  char szOut[1024] = "";
  char szLine[4096] = "";
  DWORD dwError = 0;
  LPVOID lpMsgBuf = NULL;

  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,    
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
                  (LPTSTR) &lpMsgBuf,    
                  0,    
                  NULL ); // Display the string.
  
  va_start (ap, pszMessage);
  vsprintf (szOut, pszMessage, ap);
  va_end (ap);


//  char c = ((char*) lpMsgBuf)[strlen ((char*) lpMsgBuf) - 1];
//  int len = strlen ((char*) lpMsgBuf);

  char* p = ((char*) lpMsgBuf) + strlen ((char*) lpMsgBuf) - 2;

  if (*p == '\r')
  {
    *p = '\0';
  }

  sprintf (szLine, "Debug: %s [%s]", szOut, lpMsgBuf); 

  // Free the buffer.
  LocalFree (lpMsgBuf);

  LogMessage (szLine);
}

void LogDebugMessage (
  DWORD dwErrorCode,
  char* pszMessage,
  ...)
{
  va_list ap;
  char szOut[1024] = "";
  char szLine[4096] = "";
  DWORD dwError = 0;
  LPVOID lpMsgBuf = NULL;

  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,    
                  NULL,
                  dwErrorCode,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
                  (LPTSTR) &lpMsgBuf,    
                  0,    
                  NULL ); // Display the string.
  
  va_start (ap, pszMessage);
  vsprintf (szOut, pszMessage, ap);
  va_end (ap);


//  char c = ((char*) lpMsgBuf)[strlen ((char*) lpMsgBuf) - 1];
//  int len = strlen ((char*) lpMsgBuf);

  char* p = ((char*) lpMsgBuf) + strlen ((char*) lpMsgBuf) - 2;

  if (*p == '\r')
  {
    *p = '\0';
  }

  sprintf (szLine, "Debug: %s [%s]", szOut, lpMsgBuf); 

  // Free the buffer.
  LocalFree (lpMsgBuf);

  LogMessage (szLine);
}


BOOL FormatTime (
  char* pszBuffer,
  long  lSize)
{
	struct tm *newtime;
	time_t aclock;

	time (&aclock);                

	newtime = localtime (&aclock); 

	_snprintf (pszBuffer, lSize, "%02d/%02d/%04d %02d:%02d:%02d", 
				newtime->tm_mday,
				newtime->tm_mon + 1,
				newtime->tm_year + 1900, 
				newtime->tm_hour,
				newtime->tm_min,
				newtime->tm_sec);

  return TRUE;
}