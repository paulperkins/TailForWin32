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
 * $Id: EventLogThread.cpp,v 1.7 2003/11/12 09:25:25 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include <io.h>
#include <stdio.h>

#include "utils.h"

#include "eventlogthread.h"

// Function to call on update.
// From TailView.cpp.
extern BOOL InsertText (
  CTailView*      pView,
  char*           pszText,
  BOOL            bDoColouring,
  long*           plMatchCount,
  BOOL*           pbMatch);

void GetEventLogType (
  unsigned short uEventType,
  char* pszType)
{
  switch (uEventType)
  {
  case EVENTLOG_SUCCESS:
    sprintf (pszType, "EVENTLOG_SUCCESS");
    break;

  case EVENTLOG_ERROR_TYPE:
    sprintf (pszType, "EVENTLOG_ERROR_TYPE");

    break;
  
  case EVENTLOG_WARNING_TYPE:
    sprintf (pszType, "EVENTLOG_WARNING_TYPE");

    break;
  
  case EVENTLOG_INFORMATION_TYPE:
    sprintf (pszType, "EVENTLOG_INFORMATION_TYPE");

    break;
  
  case EVENTLOG_AUDIT_SUCCESS:
    sprintf (pszType, "EVENTLOG_AUDIT_SUCCESS");

    break;
  
  case EVENTLOG_AUDIT_FAILURE:
    sprintf (pszType, "EVENTLOG_AUDIT_FAILURE");

    break;

  default:
    sprintf (pszType, "UNKNOWN");

    break;


  }
}

UINT EventLogThread (
  LPVOID pParams)
{
  EVENT_LOG_THREAD_PARAMS* parms;
  HANDLE    fch = NULL;
  HANDLE    hEventLog = NULL;
  HANDLE    haEvents[2];
  HANDLE    hDie;
  DWORD     dwFileSize = 0;
  DWORD     dwNewFileSize = 0;
  DWORD     dwOldFileSize = 0;
  DWORD     dwRtn;
  HWND      hwndEdit = NULL;
  char      szNewPath[_MAX_PATH] = "";
  char      szEvent[_MAX_PATH] = "";
  BOOL      bFirst = TRUE;
  BOOL      bIgnoreHotStartup = FALSE;
  BOOL      bUpdated = FALSE;
  BOOL      bDebug = FALSE;
  BOOL      bPaused = FALSE;
  int       nTimeout = 0;
  char*     pszNewLines = NULL;
  CTailView* pView = NULL;                        
  BOOL      bRtn = FALSE;
  BOOL      bFileGone = FALSE;
  UINT      uiFlags = 0;                          // Type of drive containing file.
	EVENTLOGRECORD* pBuffer = NULL;                 // Buffer for changed lines.
	DWORD     dwBytesRead = 0;                      // Number of bytes read from file.
	BOOL      bReadOK = FALSE;                      // Read succeeded.
  DWORD     dwBufferSize = 0;                     // Size of buffer to allocate for changed segment.
  DWORD     dwBytesNeeded = 0;                    // Size of buffer needed for next record.
  long      lMatches = 0;                         // Number of matches found against keyword list.
  BOOL      bMatch = FALSE;                       // A match against list was found in this scan.
  DWORD     dwMaxInitFileSize = 100 * 1024;       // Size of file before considering only getting last n bytes.
  DWORD     dwInitFileSize = dwMaxInitFileSize;   // Size of chunk to bite off end of large file at startup.
  BOOL      bLastChunk = FALSE;                   // Only bite off last chunk of file.
  char      szComputerName[1024] = "";
  char      szUNCComputerName[1024] = "";
  DWORD     dwCNBufferSize = 0;
  DWORD     dwOldestRecord = 0;
  DWORD     dwCurrentRecord = 1;                  // Event log records seem to be '1' based.
  DWORD     dwRecordsToRead = 0;

  if (!pParams)
  {
    ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), "PANIC: EventLogThread: No document!", "Tail for Win32", MB_OK | MB_ICONSTOP);

    return 1;
  }

  parms = (EVENT_LOG_THREAD_PARAMS*) pParams;

//  strcpy (gszFileName, parms->szFileName);

  pView = parms->pView;

  bIgnoreHotStartup = parms->bIgnoreHotStartup;

  // Open the event log.
//  hEventLog = OpenEventLog ("", parms->szFileName);

  dwCNBufferSize = sizeof (szComputerName);

  GetComputerName (szComputerName, &dwCNBufferSize);

  sprintf (szUNCComputerName, "\\\\%s", szComputerName);

  LogMessage ("Computer name is %s", szUNCComputerName);

  hEventLog = OpenEventLog (szUNCComputerName, "Application");

  if (hEventLog == NULL)
  {
    ::MessageBox (hwndEdit, "Could not open Event Log", "Tail for Win32", MB_OK | MB_ICONSTOP);

    LogDebugMessage ("Could not open Event Log '%s'.",
                parms->szFileName);                  
    
    return 0;
  }

  // Create the change event.
  // PP: Need to change the name of the event.
  fch = CreateEvent (NULL, TRUE, FALSE, parms->szEvent); 

  if (fch == NULL)
  {
    ::MessageBox (hwndEdit, "Could not create wait event.", "Tail for Win32", MB_OK | MB_ICONSTOP);

    LogDebugMessage ("Could not CreateEvent '%s'.",
                parms->szEvent);                  
    
    return 0;
  }

  if (!NotifyChangeEventLog (hEventLog, fch))
  {
    ::MessageBox (hwndEdit, "Could not NotifyChangeEventLog.", "Tail for Win32", MB_OK | MB_ICONSTOP);

    LogDebugMessage ("Could not NotifyChangeEventLog.");                  

    return 0;
  }

  if (!(hDie = OpenEvent (EVENT_ALL_ACCESS, FALSE, parms->szDieEvent)))
  {
    hDie = CreateEvent (NULL, TRUE, FALSE, parms->szDieEvent); 
  }

  if (hDie == NULL)
  {
    LogDebugMessage ("CreateEvent (hDie) failed");

    return 0;
  }
  else
  {
    LogMessage ("Debug: CreateEvent (hDie) succeeded");
  }

  bDebug = parms->bDebug;
  bPaused = parms->bPaused;
  nTimeout = parms->nTimeout;
  hwndEdit = parms->hEdit;
//  strcpy (szFileName, parms->szFileName);

  // Set up the wait event array.
  haEvents[0] = hDie;
  haEvents[1] = fch;

  // The main loop.
  // Keep going 'til we get told to go away.
  while (1)
  {  
    if (!bFirst)
    {
			LogMessage ("Debug: Waiting for event log  change...");

      // Wait for the *directory* change or die event to occur.
      dwRtn = WaitForMultipleObjects (2, haEvents, FALSE, INFINITE);  

      if (dwRtn == WAIT_FAILED)
      {
        LogDebugMessage ("WaitForMultipleObjects failed - returned %ld", dwRtn);
      }
      else
      {
				if (dwRtn == WAIT_OBJECT_0)
				{
	        LogDebugMessage ("WaitForMultipleObjects triggered from die event", dwRtn);
				}
				else if (dwRtn == (WAIT_OBJECT_0 + 1))
				{
	        LogMessage ("Debug: WaitForMultipleObjects triggered from file wait event", dwRtn);
		
//              MessageBeep (MB_ICONINFORMATION);
				}
      }

      switch (dwRtn)
      {
        case WAIT_TIMEOUT:
          // Wait got bored, start waiting again.
          if (bDebug)
          {
            LogMessage ("Debug: Wait timed out.");
          }

          continue;

        case WAIT_OBJECT_0:
          // We're dead.      
          LogMessage ("Debug: Received kill event.");

/*          bRtn = FindCloseChangeNotification (fch);

          if (bDebug)
          {          
            if (!bRtn)
            {
              LogDebugMessage ("FindCloseChangeNotification failed - returned %ld", bRtn);
            }
            else
            {
              LogDebugMessage ("FindCloseChangeNotification returned %ld", bRtn);
            }
          } */

          bRtn = CloseHandle (hDie);

          if (bDebug)
          {
            if (!bRtn)
            {
              LogDebugMessage ("CloseHandle (hDie) failed - returned %ld", bRtn);
            }
            else
            {
              LogDebugMessage ("CloseHandle (hDie) succeeded - returned %ld", bRtn);
            }
          }

          return 0;

        case WAIT_ABANDONED:    
        case WAIT_ABANDONED + 1:    
          ::MessageBox (hwndEdit, "Wait abandoned!", "Tail for Win32", MB_OK | MB_ICONSTOP);

          LogMessage ("Wait abandoned!");

//          FindCloseChangeNotification (fch);

          CloseHandle (hDie);

          return 1;

        default:
          LogMessage ("Debug: Received event log change event.");

          break;
      }

      if (dwRtn != (WAIT_OBJECT_0 + 1))
      {
        LogDebugMessage ("WaitForMultipleObjects returned %ld. In a pickle.", dwRtn);
        ::MessageBox (hwndEdit, "WaitForMultipleObjects in a pickle!!", "Tail for Win32", MB_OK | MB_ICONSTOP);

//          FindCloseChangeNotification (fch);

        CloseHandle (hDie);

        return 1;
      }
    } /* End if (!bFirst) */

    // Open the file we're interested in to see if it has changed size.
    GetNumberOfEventLogRecords (hEventLog, &dwNewFileSize);

    // Save away the old file size.
    dwOldFileSize = dwFileSize;

    // If the file has changed size, update the view.
    if (dwNewFileSize != dwOldFileSize)
    {
      GetOldestEventLogRecord (hEventLog, &dwOldestRecord);

      dwRecordsToRead = dwNewFileSize - dwOldFileSize;

      LogMessage ("Debug: Event log has changed size (was %ld, now %ld)", dwOldFileSize, dwNewFileSize);

      dwFileSize = dwNewFileSize;

      dwBufferSize = 1024;

      LogMessage ("Allocating %ld bytes for buffer", dwBufferSize);      

      pBuffer = (EVENTLOGRECORD*) malloc (dwBufferSize);

      while (dwRecordsToRead > 0)
      {
        // Get the data from the event log. 
        // Try with a small buffer first.
        dwBytesRead = 1;
        dwBytesNeeded = 1;
  			
        memset ((void*)pBuffer, 0, dwBufferSize);

        if (0 == ReadEventLog (hEventLog, 
                               EVENTLOG_SEEK_READ | EVENTLOG_FORWARDS_READ,
                               dwCurrentRecord,
                               pBuffer,
                               dwBufferSize,
                               &dwBytesRead,
                               &dwBytesNeeded))
        {
          DWORD dwError = 0;
        
          free ((void*) pBuffer);

          dwError = GetLastError ();

          // Get the number of bytes needed.
          if (ERROR_INSUFFICIENT_BUFFER == dwError)
          {
            dwBufferSize = dwBytesNeeded;

  			    LogMessage ("Reallocating %ld bytes for buffer for record %ld.", dwBufferSize, dwCurrentRecord);      

            pBuffer = (EVENTLOGRECORD*) malloc (dwBufferSize);

            memset ((void*)pBuffer, 0, dwBufferSize);

            if (0 == ReadEventLog (hEventLog, 
                       EVENTLOG_SEEK_READ | EVENTLOG_FORWARDS_READ,
                       dwCurrentRecord,
                       pBuffer,
                       dwBufferSize,
                       &dwBytesRead,
                       &dwBytesNeeded))
            {
              ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), "Cannot read log record even after expanding buffer!.", 
                            "Tail for Win32", MB_OK | MB_ICONERROR);

              LogDebugMessage (dwError, "Could not read log record %ld even after expanding buffer!", dwCurrentRecord);

              return 0;
            }

          }
          else
          {
            ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), "Cannot read log record.", 
                          "Tail for Win32", MB_OK | MB_ICONERROR);

            LogDebugMessage (dwError, "Could not read log record %ld.", dwCurrentRecord);

            return 0;
          }
        }

        if (dwNewFileSize < dwOldFileSize)
        {
          // Event log has been truncated.
          ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), "Event Log has been truncated. Not yet supported!.", 
                        "Tail for Win32", MB_OK | MB_ICONINFORMATION);
  			  LogMessage ("Debug: Event Log has been truncated - not yet supported!!");      
        
          return 0;
        }

      char szNewEntry[10 * 1024] = "";
      char szTimestamp[1024] = "";
      char szType[1024] = "";
//      char szStrings[10 * 1024] = "";
      char szStrings[128] = "";
      char szSource[1024] = "";
      char* pszData = 0;

/*      if (pBuffer->DataLength > 0)
      {
        pszData = (char*) malloc (pBuffer->DataLength + 1);

        memset ((void*)pszData, 0, pBuffer->DataLength + 1);
  
        strncpy (pszData, (LPSTR)((LPBYTE)pBuffer + pBuffer->DataOffset), pBuffer->DataLength);
      } */

      GetTimeStamp ((time_t*)&pBuffer->TimeWritten, szTimestamp);
      GetEventLogType (pBuffer->EventType, szType);

      strcpy (szSource, (LPSTR)((LPBYTE)pBuffer + sizeof (EVENTLOGRECORD)));

      if (pBuffer->NumStrings > 0)
      {
        strncpy (szStrings, (LPSTR) ((LPBYTE)pBuffer + pBuffer->StringOffset), sizeof (szStrings) - 1);

        szStrings[sizeof (szStrings) - 1] = '\0';
      }

      sprintf (szNewEntry, "%s %s %s %ld [%s]\n", 
                szTimestamp, szType, szSource, pBuffer->EventID, szStrings);

/*      if (pBuffer->DataLength > 0)
      {
        free ((void*)pszData);
      } */

      pszNewLines = &szNewEntry[0];

      // Update the window, doing syntax highlighting, setting counters if matches found.
      bUpdated = InsertText (pView, pszNewLines, !(bFirst && bIgnoreHotStartup), &lMatches, &bMatch);

      dwRecordsToRead--;
      dwCurrentRecord++;
    }

    free ((void*) pBuffer); 

		bFirst = FALSE;
  } // Go back and wait for another change.

// Close the notification handle.
//    bRtn = FindCloseChangeNotification (fch);
  }

  CloseHandle (hDie);

  return 0;
}

