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
 * $Id: FileReaderThread.cpp,v 1.12 2003/11/21 10:27:31 paulperkins Exp $
 * 
 */

#include "stdafx.h"
#include <io.h>
#include <stdio.h>

#include "utils.h"

#include "filereaderthread.h"

#define   NO_HANDLER_THREAD       (1)                 // Don't spawn a separate thread.

#define   WAIT_OBJECT_DIE         (WAIT_OBJECT_0)
#define   WAIT_OBJECT_FILEUPDATE  (WAIT_OBJECT_0 + 1)
#define   WAIT_OBJECT_RELOAD      (WAIT_OBJECT_0 + 2)

#define   WAIT_OBJECT_POLL_RELOAD (WAIT_OBJECT_0 + 1)

#define   MAX_INIT_FILE_SIZE      (100 * 1024)

typedef struct _FILE_HANDLER_PARAMS
{
  CTailView* pView;
  DWORD*  pdwOldFileSize;
  DWORD*  pdwNewFileSize;
  char    szDieEvent[_MAX_FNAME];
  char    szGoEvent[_MAX_FNAME];
  BOOL*   pbFirst;
  BOOL*   pbIgnoreHotStartup;
  HWND    hwndEdit;
  char    szFileName[_MAX_FNAME];

  //char szDieEvent[_MAX_FNAME];
  char szReloadEvent[_MAX_FNAME];
  HWND hEdit;
  BOOL bUseTimeout;
  BOOL bIgnoreHotStartup;
  BOOL bDebug;
  int  nTimeout;
  BOOL bPaused;

} FILE_HANDLER_PARAMS;

// Function pointer for update function. To be called by thread
// when an update occurs. 
// The first param is to be used as a generic placeholder for
// any type of data.
typedef BOOL (*UPDATE_PROC) (DWORD, char*, BOOL, long*, BOOL*);

// Function to call on update.
// From TailView.cpp.
extern BOOL InsertText (
  CTailView*      pView,
  char*           pszText,
  BOOL            bDoColouring,
  long*           plMatchCount,
  BOOL*           pbMatch);

// Function to update status bar.
extern void UpdateStatusBar (
  CTailView* pView,
  char*      pszText);

extern void ReloadKeywords (
  CTailView* pView);

static DWORD GetFileSizeBytes (
  const char* pszFilename)
{
  HANDLE  hFile = NULL;
  DWORD   dwFileSize = 0;

  hFile = CreateFile (pszFilename, 
                      GENERIC_READ, 
                      FILE_SHARE_READ  | FILE_SHARE_WRITE, 
                      NULL, 
                      OPEN_EXISTING, 
                      FILE_ATTRIBUTE_NORMAL, 
                      NULL);

  dwFileSize = GetFileSize (hFile, NULL);

  CloseHandle (hFile);

  return dwFileSize;
}

static DWORD GetChangedLines (
  char*       pszNewLines,
  char**      ppszChangedLines,
  const DWORD dwOldFileSize,
  const DWORD dwNewFileSize,
  DWORD*      pdwType,
  BOOL        bFullRead)
{
  if (bFullRead)
  {
    // File is now smaller than it was -- assume the file was truncated or re-written.
    if (dwNewFileSize < dwOldFileSize)
    {
      *ppszChangedLines = pszNewLines;
      *pdwType = CHANGE_FULL;

      return 0;
    }

    *ppszChangedLines = pszNewLines + dwOldFileSize;
    *pdwType = CHANGE_PARTIAL;
  }
  else
  {
    *ppszChangedLines = pszNewLines;
    *pdwType = CHANGE_PARTIAL;
  }


  return 0;
}

BOOL FileHandlerThread (
  LPVOID pParams)
{
  HANDLE  hFile = 0;
  HANDLE  hDie = 0;
  HANDLE  hGo = 0;
  DWORD   dwRtn = 0;
  HANDLE  haEvents[2];
  BOOL    bRtn = FALSE;
  BOOL    bFileGone = FALSE;
  BOOL    bLastChunk = FALSE;
  DWORD   dwBufferSize = 0;
  DWORD   dwNewFileSize = 0;
  DWORD   dwOldFileSize = 0;
  DWORD   dwChangeType = 0;
  char*   pszBuffer = NULL;
  char*   pszNewLines = NULL;
  char*   pszNewLinesSkip = NULL;
  BOOL    bReadOK = FALSE;
  DWORD   dwMaxInitFileSize = MAX_INIT_FILE_SIZE; // Size of file before considering only getting last n bytes.
  DWORD   dwInitFileSize = dwMaxInitFileSize;     // Size of chunk to bite off end of large file at startup.
  DWORD   dwBytesRead = 0;
  BOOL    bMatch = FALSE;   // PP: I think this is not used any more - only used to pass to View functions.
  BOOL    bUpdated = FALSE; // PP: I think this is not used any more.
  long    lMatches = 0;     // PP: I think this is not used any more - only used to pass to View functions.

  FILE_HANDLER_PARAMS* parms = NULL;

  LogMessage ("Debug: Started handler thread.");

  parms = (FILE_HANDLER_PARAMS*) pParams;

  if (!(hDie = OpenEvent (EVENT_ALL_ACCESS, FALSE, parms->szDieEvent)))
  {
    hDie = CreateEvent (NULL, TRUE, FALSE, parms->szDieEvent); 
  }

  if (!(hGo = OpenEvent (EVENT_ALL_ACCESS, FALSE, parms->szGoEvent)))
  {
    hGo = CreateEvent (NULL, TRUE, FALSE, parms->szGoEvent); 
  }

  // Set up the wait event array.
  haEvents[0] = hDie;
  haEvents[1] = hGo;

  while (1)
  {
    dwRtn = WaitForMultipleObjects (2, haEvents, FALSE, INFINITE);  

    if (dwRtn == WAIT_FAILED)
    {
      LogDebugMessage ("WaitForMultipleObjects failed - returned %ld", dwRtn);
    }
    else if (dwRtn == WAIT_OBJECT_DIE)
		{
	    LogDebugMessage ("WaitForMultipleObjects triggered from die event", dwRtn);
		}
    else if (dwRtn == WAIT_TIMEOUT)
    {
	    LogMessage ("Debug: WaitForMultipleObjects triggered from polling interval timeout", dwRtn);
    }
		else if (dwRtn == (WAIT_OBJECT_FILEUPDATE))
		{
	    LogMessage ("Debug: WaitForMultipleObjects triggered from file wait event", dwRtn);			
		}

    switch (dwRtn)
    {
      case WAIT_TIMEOUT:
      // Wait got bored, start waiting again.
        LogMessage ("Debug: Wait timed out.");

        continue;

      case WAIT_OBJECT_DIE:
        // We're dead.      
        LogMessage ("Debug: Received kill event.");

        CloseHandle (hDie);
        CloseHandle (hGo);

        return 0;

      case WAIT_ABANDONED:    
      case WAIT_ABANDONED + 1:    
        ::AfxMessageBox ("Wait abandoned!");

        LogMessage ("Wait abandoned!");

        CloseHandle (hDie);
        CloseHandle (hGo);

        return 1;

      default:
        LogMessage ("Debug: Received change event (or polled).");

        break;
    }

    // The main loop.

    dwNewFileSize = *(parms->pdwNewFileSize);
    dwOldFileSize = *(parms->pdwOldFileSize);

    hFile = CreateFile (parms->szFileName, 
                        GENERIC_READ, 
                        FILE_SHARE_READ | FILE_SHARE_WRITE, 
                        NULL, 
                        OPEN_EXISTING, 
                        FILE_ATTRIBUTE_NORMAL, 
                        NULL);

    if ((hFile == INVALID_HANDLE_VALUE) && !bFileGone)
    {
      ::AfxMessageBox ("Hmmm... Looks like the file has been deleted. I'll wait.");

      bFileGone = TRUE;

      UpdateStatusBar (parms->pView, "Waiting for file...");

      continue;
    }

    if (bFileGone)
    {
      // We're now here, so the file must have come back.
      bFileGone = FALSE;
    }

    // Work out the size of the buffer needed to hold the changed lines.
    // if (bFirst && bIgnoreHotStartup) // Warlock, commented out, changed below
    if (*(parms->pbFirst)) // Warlock, still load when bIgnoreHotStartup is on
    {
      // See if the file is larger than the maximum size.
      // If so, only bite off the last few lines to save memory.
      // PP: This logic will need to change for cases where the file has 
      // been truncated and the initial contents are still huge.
      if (dwNewFileSize > dwMaxInitFileSize)
      {
        bLastChunk = TRUE;
        dwBufferSize = dwInitFileSize;
      }
      else
      {
        // Org.
        dwBufferSize = dwNewFileSize + 1;
      }
    }
    else  // Most times - just load the changed lines.
    {
      if (dwNewFileSize > dwOldFileSize)
      {
        dwBufferSize = (dwNewFileSize - dwOldFileSize) + 1;
      }
      else
      {
        // File has been truncated.
  			LogMessage ("Debug: File has been truncated.");      

        dwBufferSize = dwNewFileSize + 1;

//          dwOldFileSize = 0;
        dwChangeType = CHANGE_FULL;
      }
    }

		LogMessage ("Debug: Allocating %ld bytes for buffer", dwBufferSize);      
    
    pszBuffer = (char*) malloc (dwBufferSize);

    if (pszBuffer == NULL)
    {
      ::AfxMessageBox ("Could not allocate memory for file!");

      CloseHandle (hDie);
      CloseHandle (hGo);

      return 1;
    }

    memset ((void*) pszBuffer, 0, dwBufferSize);

    if (bLastChunk)
    {
      DWORD rtn;

    	LogMessage ("Debug: File is larger than %ld, only reading last %ld bytes.", dwMaxInitFileSize, dwBufferSize);      

      rtn = SetFilePointer (hFile, 1 - dwBufferSize, NULL, FILE_END);

      LogMessage ("Debug: File pointer now set to %ld", rtn);
    }
    else
    {
      if (*(parms->pbFirst) && (dwNewFileSize > dwMaxInitFileSize))
      {
        LogMessage ("Debug: File is larger than %ld, only reading last %ld bytes.", dwMaxInitFileSize, dwBufferSize);      

        SetFilePointer (hFile, dwBufferSize, NULL, FILE_END);
      }
      else
      {
        if (dwNewFileSize < dwOldFileSize)
        {
          SetFilePointer (hFile, 0, NULL, FILE_BEGIN);
        }
        else
        {
          // Org.
          SetFilePointer (hFile, dwOldFileSize, NULL, FILE_BEGIN);
        }
      }
    }

    bReadOK = ReadFile (hFile, (LPVOID) pszBuffer, dwBufferSize - 1, &dwBytesRead, NULL);

    CloseHandle (hFile);     	

    if (bReadOK)
    {
      if (bLastChunk)
      {
        GetChangedLines (pszBuffer, &pszNewLines, dwOldFileSize, dwBufferSize, &dwChangeType, FALSE);
        bLastChunk = FALSE;
        if (*(parms->pbFirst) && (dwNewFileSize > dwMaxInitFileSize)) // Warlock, neat: skip/blank the chopped line.
        {
          pszNewLinesSkip = strchr( pszNewLines, '\n' ); 
          LogMessage ("Debug: Skipping chopped line, jumping forward %ld Bytes.", pszNewLinesSkip - pszNewLines);
          pszNewLines = pszNewLinesSkip;
        }
      }
      else
      {
        if (dwNewFileSize < dwOldFileSize)
        {
          pszNewLines = pszBuffer;
        }
        else
        {
          // Org.
          GetChangedLines (pszBuffer, &pszNewLines, dwOldFileSize, dwNewFileSize, &dwChangeType, FALSE);
        }
      }

      // Clear the window down first.
      if (dwChangeType == CHANGE_FULL)
      {
        ::SetWindowText (parms->hwndEdit, "");
      }

      bMatch = FALSE;

      // Update the window, doing syntax highlighting, setting counters if matches found.
      bUpdated = InsertText (parms->pView, pszNewLines, !(*(parms->pbFirst) && *(parms->pbIgnoreHotStartup)), &lMatches, &bMatch);
    }

    free ((void*) pszBuffer); 


    // End main loop.

  } // while (1)

}


UINT ReaderThread (
  LPVOID pParams)
{
  READER_PARAMS* parms;
  HANDLE    fch = NULL;
  HANDLE    hFile = NULL;
  HANDLE    haEvents[3];
  HANDLE    hDie;
  HANDLE    hReload;
  DWORD     dwFileSize = 0;
  DWORD     dwNewFileSize = 0;
  DWORD     dwOldFileSize = 0;
  DWORD     dwRtn;
  HWND      hwndEdit = NULL;
  char      szDrive[_MAX_DRIVE] = "";
  char      szPath[_MAX_PATH] = "";
  char      szFileName[_MAX_FNAME] = "";
  char      szNewPath[_MAX_PATH] = "";
  char      szEvent[_MAX_PATH] = "";
  BOOL      bFirst = TRUE;
  BOOL      bUseTimeout = FALSE;
  BOOL      bIgnoreHotStartup = FALSE;
  BOOL      bUpdated = FALSE;
  BOOL      bDebug = FALSE;
  BOOL      bPaused = FALSE;
  int       nTimeout = 0;
  char*     pszNewLines = NULL;
  DWORD     dwChangeType = CHANGE_FULL;
  CTailView* pView = NULL;                        
  BOOL      bRtn = FALSE;
  BOOL      bFileGone = FALSE;
  UINT      uiFlags = 0;                          // Type of drive containing file.
  char*     pszBuffer = NULL;                     // Buffer for changed lines.
  DWORD     dwBytesRead = 0;                      // Number of bytes read from file.
  BOOL      bReadOK = FALSE;                      // Read succeeded.
  DWORD     dwBufferSize = 0;                     // Size of buffer to allocate for changed segment.
  long      lMatches = 0;                         // Number of matches found against keyword list.
  BOOL      bMatch = FALSE;                       // A match against list was found in this scan.
  DWORD     dwMaxInitFileSize = 100 * 1024;       // Size of file before considering only getting last n bytes.
  DWORD     dwInitFileSize = dwMaxInitFileSize;   // Size of chunk to bite off end of large file at startup.
  BOOL      bLastChunk = FALSE;                   // Only bite off last chunk of file.
  char*     pszNewLinesSkip;                      // Warlock, skip forward passed chopped line.

#ifndef NO_HANDLER_THREAD
  HANDLE    hHandlerGo = 0;
  HANDLE    hHandlerDie = 0;
  FILE_HANDLER_PARAMS stFileHandlerParams;
#endif

  LogMessage ("Debug: Started main thread.");

  if (!pParams)
  {
    ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), "PANIC: ReaderThread: No document!", "Tail for Win32", MB_OK | MB_ICONSTOP);

    return 1;
  }

  parms = (READER_PARAMS*) pParams;

//  strcpy (gszFileName, parms->szFileName);

// Get the path of the file, and monitor the directory it is in for changes. (!)
  _splitpath (parms->szFileName, szDrive, szPath, NULL, NULL);
  _makepath (szNewPath, szDrive, szPath, NULL, NULL);

  pView = parms->pView;

  bIgnoreHotStartup = parms->bIgnoreHotStartup;

  // Determine the type of the drive. If it is not local, then
  // we cannot use file change notifications.
  _splitpath (szNewPath, szDrive, NULL, NULL, NULL);

  uiFlags = GetDriveType (szDrive);

  // PP: Not sure if this is the full list that we should use to 
  // decide whether this directory/file can be monitored with change 
  // notifications.
  if ((uiFlags & DRIVE_REMOTE) || (uiFlags & DRIVE_UNKNOWN))
  {
    LogMessage ("File '%s' is not on a local drive. Using polling to determine file changes.", parms->szFileName);                  

    bUseTimeout = TRUE;
  }
  else
  {
    fch = FindFirstChangeNotification (szNewPath, FALSE, FILE_NOTIFY_CHANGE_SIZE);
//    fch = FindFirstChangeNotification (szNewPath, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);

    if (fch == INVALID_HANDLE_VALUE)
    {
      LogMessage ("Could not register file change notification on '%s'. "
                    "The file may not be on a local filesystem - using polling to "
                    "detect file changes.", parms->szFileName);                  

      bUseTimeout = FALSE;
    }
    else
    {
      LogMessage ("Using file change notification on '%s' to detect file changes. (fch = %08x)", parms->szFileName, fch);                  
    }
  }

  if (!(hDie = OpenEvent (EVENT_ALL_ACCESS, FALSE, parms->szDieEvent)))
  {
    hDie = CreateEvent (NULL, TRUE, FALSE, parms->szDieEvent); 
  }

  if (hDie == NULL)
  {
    LogDebugMessage ("CreateEvent (hDie) failed");
  }
  else
  {
    LogMessage ("Debug: CreateEvent (hDie) succeeded");
  }

  if (!(hReload = OpenEvent (EVENT_ALL_ACCESS, FALSE, parms->szReloadEvent)))
  {
    hReload = CreateEvent (NULL, TRUE, FALSE, parms->szReloadEvent); 
  }

  if (hReload == NULL)
  {
    LogDebugMessage ("CreateEvent (hReload) failed");
  }
  else
  {
    LogMessage ("Debug: CreateEvent (hReload) succeeded");
  }

#ifndef NO_HANDLER_THREAD

  // Create the handler thread.
  memset (&stFileHandlerParams, 0, sizeof (FILE_HANDLER_PARAMS));

  stFileHandlerParams.pView = pView;
  stFileHandlerParams.pdwOldFileSize = &dwOldFileSize;
  stFileHandlerParams.pdwNewFileSize = &dwNewFileSize;
  sprintf (stFileHandlerParams.szDieEvent, "HANDLER_%s_DIE", parms->szDieEvent);
  sprintf (stFileHandlerParams.szGoEvent, "HANDLER_%s_GO", parms->szDieEvent);
  stFileHandlerParams.pbFirst = &bFirst;
  stFileHandlerParams.pbIgnoreHotStartup = &bIgnoreHotStartup;
  stFileHandlerParams.hwndEdit = parms->hEdit;
  strcpy (stFileHandlerParams.szFileName, parms->szFileName);

  if (!AfxBeginThread ((AFX_THREADPROC) FileHandlerThread, (LPVOID) &stFileHandlerParams))
  {
    LogMessage ("Could not create handler thread.");
    ::AfxMessageBox ("Could not create handler thread.");

    return 1;
  }

#endif // #ifndef NO_HANDLER_THREAD

  bDebug = parms->bDebug;
  bPaused = parms->bPaused;
  nTimeout = parms->nTimeout;
  hwndEdit = parms->hEdit;
  strcpy (szFileName, parms->szFileName);

  // Set up the wait event array.
  haEvents[0] = hDie;
  haEvents[1] = fch;
  haEvents[2] = hReload;

  // The main loop.
  // Keep going 'til we get told to go away.
  while (1)
  {  
    if (!bFirst)
    {
      if (bUseTimeout)
      {
        // Use different array for non-local (polled) files.
        haEvents[0] = hDie;
        haEvents[1] = hReload;

        dwRtn = WaitForMultipleObjects (2, haEvents, FALSE, nTimeout);  

        if (dwRtn == WAIT_FAILED)
        {
          LogDebugMessage ("WaitForMultipleObjects failed - returned %ld", dwRtn);
        }
        else if (dwRtn == WAIT_OBJECT_DIE)
				{
	        LogDebugMessage ("WaitForMultipleObjects triggered from die event", dwRtn);
				}
        else if (dwRtn == WAIT_TIMEOUT)
        {
	        LogMessage ("Debug: WaitForMultipleObjects triggered from polling interval timeout", dwRtn);
        }
				else if (dwRtn == WAIT_OBJECT_POLL_RELOAD)
				{
	        LogMessage ("Debug: WaitForMultipleObjects triggered from keyword reload event", dwRtn);

          ReloadKeywords (pView);		
				}

        // We've been killed.
        if (dwRtn == WAIT_OBJECT_DIE)
        {
          CloseHandle (hDie);

#ifndef NO_HANDLER_THREAD

          // Kill the handler thread.
          HANDLE hHandlerDie;

          hHandlerDie = OpenEvent (EVENT_ALL_ACCESS, FALSE, stFileHandlerParams.szDieEvent);

          // Release the threads and then reset the event automatically.
          PulseEvent (hHandlerDie);

          CloseHandle (hHandlerDie);
#endif // #ifndef NO_HANDLER_THREAD

          return 0;
        }

        LogMessage ("Debug: WaitForSingleObject (polling) fired");

        dwRtn = WAIT_OBJECT_FILEUPDATE;
      }
      else
      {
				LogMessage ("Debug: Waiting for file change...");

        // Wait for the *directory* change or die event to occur.
        dwRtn = WaitForMultipleObjects (3, haEvents, FALSE, INFINITE);  

        if (dwRtn == WAIT_FAILED)
        {
          LogDebugMessage ("WaitForMultipleObjects failed - returned %ld", dwRtn);
        }
        else
        {
					if (dwRtn == WAIT_OBJECT_DIE)
					{
	          LogDebugMessage ("WaitForMultipleObjects triggered from die event", dwRtn);
					}
					else if (dwRtn == (WAIT_OBJECT_FILEUPDATE))
					{
	          LogMessage ("Debug: WaitForMultipleObjects triggered from file wait event", dwRtn);			
					}
					else if (dwRtn == (WAIT_OBJECT_RELOAD))
					{
	          LogMessage ("Debug: WaitForMultipleObjects triggered from keyword reload event", dwRtn);

            ReloadKeywords (pView);
					}
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

        case WAIT_OBJECT_DIE:
          // We're dead.      
          LogMessage ("Debug: Received kill event.");

          if (!bUseTimeout)
          {
            bRtn = FindCloseChangeNotification (fch);

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
            }
          }

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

#ifndef NO_HANDLER_THREAD

          hHandlerDie = OpenEvent (EVENT_ALL_ACCESS, FALSE, stFileHandlerParams.szDieEvent);

          // Release the threads and then reset the event automatically.
          PulseEvent (hHandlerDie);

          CloseHandle (hHandlerDie);

#endif // #ifndef NO_HANDLER_THREAD

          return 0;

        case WAIT_ABANDONED:    
        case WAIT_ABANDONED + 1:    
          ::MessageBox (hwndEdit, "Wait abandoned!", "Tail for Win32", MB_OK | MB_ICONSTOP);

          LogMessage ("Wait abandoned!");

          if (!bUseTimeout)
          {
            FindCloseChangeNotification (fch);
          }

          CloseHandle (hDie);

          return 1;

        default:
          LogMessage ("Debug: Received change event (or polled).");

          break;
      }

      if ((dwRtn != WAIT_OBJECT_FILEUPDATE) && (dwRtn != WAIT_OBJECT_RELOAD))
      {
        LogDebugMessage ("WaitForMultipleObjects returned %ld. In a pickle.", dwRtn);
        ::MessageBox (hwndEdit, "WaitForMultipleObjects in a pickle!!", "Tail for Win32", MB_OK | MB_ICONSTOP);

        if (!bUseTimeout)
        {
          FindCloseChangeNotification (fch);
        }

        CloseHandle (hDie);

        return 1;
      }
    } /* End if (!bFirst) */

    // Open the file we're interested in to see if it has changed size.
    dwNewFileSize = GetFileSizeBytes (szFileName);

    // Save away the old file size.
    dwOldFileSize = dwFileSize;

    // If the file has changed size, update the view.
    if (dwNewFileSize != dwOldFileSize)
    {
      // Save the new size.
      dwFileSize = dwNewFileSize;

      LogMessage ("Debug: File has changed size (was %ld, now %ld)", dwOldFileSize, dwNewFileSize);

#ifndef NO_HANDLER_THREAD
      hHandlerGo = OpenEvent (EVENT_ALL_ACCESS, FALSE, stFileHandlerParams.szGoEvent);

      // Release the threads and then reset the event automatically.
      PulseEvent (hHandlerGo);

      CloseHandle (hHandlerGo);
#endif

#ifdef NO_HANDLER_THREAD

      hFile = CreateFile (szFileName, 
                          GENERIC_READ, 
                          FILE_SHARE_READ | FILE_SHARE_WRITE, 
                          NULL, 
                          OPEN_EXISTING, 
                          FILE_ATTRIBUTE_NORMAL, 
                          NULL);

      if ((hFile == INVALID_HANDLE_VALUE) && !bFileGone)
      {
        ::AfxMessageBox ("Hmmm... Looks like the file has been deleted. I'll wait.");

        bFileGone = TRUE;

        UpdateStatusBar (pView, "Waiting for file...");

        continue;
      }

      if (bFileGone)
      {
        // We're now here, so the file must have come back.
        bFileGone = FALSE;
      }

      // Work out the size of the buffer needed to hold the changed lines.
      // if (bFirst && bIgnoreHotStartup) // Warlock, commented out, changed below
      if (bFirst) // Warlock, still load when bIgnoreHotStartup is on
      {
        // See if the file is larger than the maximum size.
        // If so, only bite off the last few lines to save memory.
        // PP: This logic will need to change for cases where the file has 
        // been truncated and the initial contents are still huge.
        if (dwNewFileSize > dwMaxInitFileSize)
        {
          bLastChunk = TRUE;
          dwBufferSize = dwInitFileSize;
        }
        else
        {
          // Org.
          dwBufferSize = dwNewFileSize + 1;
        }
      }
      else  // Most times - just load the changed lines.
      {
        if (dwNewFileSize > dwOldFileSize)
        {
          dwBufferSize = (dwNewFileSize - dwOldFileSize) + 1;
        }
        else
        {
          // File has been truncated.
  			  LogMessage ("Debug: File has been truncated.");      

          dwBufferSize = dwNewFileSize + 1;

//          dwOldFileSize = 0;
          dwChangeType = CHANGE_FULL;
        }
      }

			LogMessage ("Debug: Allocating %ld bytes for buffer", dwBufferSize);      
      
      pszBuffer = (char*) malloc (dwBufferSize);

      if (pszBuffer == NULL)
      {
        ::MessageBox (AfxGetMainWnd()->GetSafeHwnd(), "Could not allocate memory for file!", 
                      "Tail for Win32", MB_OK | MB_ICONSTOP);

        CloseHandle (hDie);
        if (!bUseTimeout)
        {
          bRtn = FindCloseChangeNotification (fch);
        }
        return 1;
      }

      memset ((void*) pszBuffer, 0, dwBufferSize);

      if (bLastChunk)
      {
        DWORD rtn;

    		LogMessage ("Debug: File is larger than %ld, only reading last %ld bytes.", dwMaxInitFileSize, dwBufferSize);      

        rtn = SetFilePointer (hFile, 1 - dwBufferSize, NULL, FILE_END);

        LogMessage ("Debug: File pointer now set to %ld", rtn);
      }
      else
      {
        if (bFirst && (dwNewFileSize > dwMaxInitFileSize))
        {
          LogMessage ("Debug: File is larger than %ld, only reading last %ld bytes.", dwMaxInitFileSize, dwBufferSize);      

          SetFilePointer (hFile, dwBufferSize, NULL, FILE_END);
        }
        else
        {
          if (dwNewFileSize < dwOldFileSize)
          {
            SetFilePointer (hFile, 0, NULL, FILE_BEGIN);
          }
          else
          {
            // Org.
            SetFilePointer (hFile, dwOldFileSize, NULL, FILE_BEGIN);
          }
        }
      }

      bReadOK = ReadFile (hFile, (LPVOID) pszBuffer, dwBufferSize - 1, &dwBytesRead, NULL);

      CloseHandle (hFile);     	

      if (bReadOK)
      {
        if (bLastChunk)
        {
          GetChangedLines (pszBuffer, &pszNewLines, dwOldFileSize, dwBufferSize, &dwChangeType, FALSE);
          bLastChunk = FALSE;
          if (bFirst && (dwNewFileSize > dwMaxInitFileSize)) // Warlock, neat: skip/blank the chopped line.
          {
            pszNewLinesSkip = strchr( pszNewLines, '\n' ); 
            LogMessage ("Debug: Skipping chopped line, jumping forward %ld Bytes.", pszNewLinesSkip - pszNewLines);
            pszNewLines = pszNewLinesSkip;
          }
        }
        else
        {
          if (dwNewFileSize < dwOldFileSize)
          {
            pszNewLines = pszBuffer;
          }
          else
          {
            // Org.
            GetChangedLines (pszBuffer, &pszNewLines, dwOldFileSize, dwNewFileSize, &dwChangeType, FALSE);
          }
        }

        // Clear the window down first.
        if (dwChangeType == CHANGE_FULL)
        {
          ::SetWindowText (hwndEdit, "");
        }

        bMatch = FALSE;

        // Update the window, doing syntax highlighting, setting counters if matches found.
        bUpdated = InsertText (pView, pszNewLines, !(bFirst && bIgnoreHotStartup), &lMatches, &bMatch);
      }

      free ((void*) pszBuffer); 

#endif // #ifdef NO_HANDLER_THREAD

    } // if (dwNewFileSize != dwOldFileSize)

    if (!bUseTimeout)
    {
			bRtn = FindNextChangeNotification (fch);

			if (!bRtn)
			{
				LogDebugMessage ("FindNextChangeNotification failed - returned %ld", bRtn);
			}
			else
			{
				LogMessage ("Debug: FindNextChangeNotification returned %ld", bRtn);
			}
		}

		bFirst = FALSE;
  } // Go back and wait for another change.

// Close the notification handle.
  if (!bUseTimeout)
  {
    bRtn = FindCloseChangeNotification (fch);

    if (bRtn)
    {
      LogMessage ("Final FindCloseChangeNotification returned %ld", bRtn);
    }
    else
    {
      LogDebugMessage ("Final FindCloseChangeNotification falied - returned %ld", bRtn);
    }
  }

  CloseHandle (hDie);

  return 0;
}
