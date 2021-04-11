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
 * $Id: utils.h,v 1.5 2003/11/14 12:26:33 paulperkins Exp $
 * 
 */

#ifndef __TAIL_UTIL_H__
#define __TAIL_UTIL_H__

BOOL DebugFilePresent (
  void);

void GetTimeStamp (
  char* pszBuf);

void GetTimeStamp (
  time_t* pstTime,
  char*   pszBuf);

void LogMessage (
  char* pszMessage,
  ...);

void LogDebugMessage (
  char* pszMessage,
  ...);

void LogDebugMessage (
  DWORD dwErrorCode,
  char* pszMessage,
  ...);

char* GetAppPath (
  char* pszPath);

BOOL FormatTime (
  char* pszBuffer,
  long  lSize);


#endif // #ifdef __TAIL_UTIL_H__