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

static BOOL bDebug = FALSE;

char* GetAppPath (
  char* pszPath,
  size_t uiBufSize)
{
  char szDrive[_MAX_DRIVE] = "";
  char szPath[_MAX_PATH] = "";
  char szAppPath[_MAX_PATH] = "";

  GetModuleFileName (AfxGetApp ()->m_hInstance, szAppPath, _MAX_PATH);

  _splitpath_s (szAppPath, szDrive, sizeof (szDrive), szPath, sizeof (szPath), NULL, 0, NULL, 0);
  _makepath_s (pszPath, uiBufSize, szDrive, szPath, NULL, NULL);

  return pszPath;
}

BOOL DebugFilePresent (
  void)
{
  FILE* fp = NULL;

  char szPath[_MAX_PATH] = "";

  GetAppPath (szPath, sizeof (szPath));

  strcat_s (szPath, sizeof (szPath), LOGFILE);

  if (!fopen_s (&fp, szPath, "r"))
  {
    fclose (fp);

    bDebug = TRUE;

    return TRUE;
  }

  return FALSE;
}


void GetTimeStamp (
  time_t* pstTime,
  char*   pszBuf,
  size_t uiBufSize)
{
	struct tm newtime;

	if (localtime_s(&newtime, pstTime))
	{
		sprintf_s (pszBuf, uiBufSize, "<ERROR>");

	}
	else
	{
		sprintf_s (pszBuf, uiBufSize, "%02d/%02d/%04d %02d:%02d:%02d",
			newtime.tm_mday,
			newtime.tm_mon,
			newtime.tm_year + 1900,
			newtime.tm_hour,
			newtime.tm_min,
			newtime.tm_sec);
	}
}

void GetTimeStamp (
  char*  pszBuf,
  size_t uiBufSize)
{
	struct tm newtime;
	time_t aclock;

	time (&aclock);                

	if (localtime_s (&newtime, &aclock))
	{
		sprintf_s (pszBuf, uiBufSize,
			"<ERROR>");
	}
	else
	{
		sprintf_s (pszBuf, uiBufSize,
			"%02d:%02d:%02d",
			newtime.tm_hour,
			newtime.tm_min,
			newtime.tm_sec);
	}
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
	struct tm newtime;
	time_t aclock;
  char szPath[_MAX_PATH] = "";

  GetAppPath (szPath, sizeof (szPath));

  strcat_s (szPath, sizeof (szPath), LOGFILE);

  va_start (ap, pszMessage);
  vsprintf_s (szOut, sizeof (szOut), pszMessage, ap);
  va_end (ap);

  // Only write to it if the file already exists.
  if (bDebug)
  {
    // Now open the file for writing.
    if (fopen_s (&fp, szPath, "a"))
    {
      return;
    }

	  time (&aclock);                

	  if (localtime_s(&newtime, &aclock))
	  {
		  sprintf_s(szLine, sizeof(szLine),
				  "[ERROR] (%08x) ",
				  GetCurrentThreadId());
	  }
	  else
	  {
		  sprintf_s(szLine, sizeof(szLine),
			  "[%04d%02d%02d %02d:%02d:%02d] (%08x) ",
			  newtime.tm_year + 1900,
			  newtime.tm_mon + 1,
			  newtime.tm_mday,
			  newtime.tm_hour,
			  newtime.tm_min,
			  newtime.tm_sec,
			  GetCurrentThreadId());
	  }

    strcat_s (szLine, sizeof (szLine), szOut);

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
  vsprintf_s (szOut, sizeof (szOut), pszMessage, ap);
  va_end (ap);


//  char c = ((char*) lpMsgBuf)[strlen ((char*) lpMsgBuf) - 1];
//  int len = strlen ((char*) lpMsgBuf);

  char* p = ((char*) lpMsgBuf) + strlen ((char*) lpMsgBuf) - 2;

  if (*p == '\r')
  {
    *p = '\0';
  }

  sprintf_s (szLine, sizeof (szLine), "Debug: %s [%s]", szOut, (char*) lpMsgBuf); 

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
  vsprintf_s (szOut, sizeof (szOut), pszMessage, ap);
  va_end (ap);


//  char c = ((char*) lpMsgBuf)[strlen ((char*) lpMsgBuf) - 1];
//  int len = strlen ((char*) lpMsgBuf);

  char* p = ((char*) lpMsgBuf) + strlen ((char*) lpMsgBuf) - 2;

  if (*p == '\r')
  {
    *p = '\0';
  }

  sprintf_s (szLine, sizeof (szLine), "Debug: %s [%s]", szOut, (char*) lpMsgBuf); 

  // Free the buffer.
  LocalFree (lpMsgBuf);

  LogMessage (szLine);
}


BOOL FormatTime (
  char* pszBuffer,
  long  lSize)
{
	struct tm newtime;
	time_t aclock;
	BOOL rtn = FALSE;
	errno_t err;

	time (&aclock);                

	err = localtime_s (&newtime, &aclock);

	if (!err)
	{
		_snprintf_s(pszBuffer, lSize, lSize, "%02d/%02d/%04d %02d:%02d:%02d",
			newtime.tm_mday,
			newtime.tm_mon + 1,
			newtime.tm_year + 1900,
			newtime.tm_hour,
			newtime.tm_min,
			newtime.tm_sec);

		rtn = TRUE;
	}

	return rtn;
}