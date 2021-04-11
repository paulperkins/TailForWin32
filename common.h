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
 * $Id: common.h,v 1.4 2003/11/21 13:26:35 paulperkins Exp $
 * 
 */

#ifndef __TAIL_COMMON_H__
#define __TAIL_COMMON_H__

#define DIE_EVENT_NAME      "TailDieEvent"
#define RELOAD_EVENT_NAME   "TailReloadEvent"

#define DEFAULT_BACK_COLOR      (RGB (128, 128, 128))

#define MAX_KEYWORD_LEN (256)
#define TIMESTAMP_LEN   20

typedef struct _KEYWORDLIST
{
  char  szKeyword[MAX_KEYWORD_LEN + 1];
  char  szTimestamp[TIMESTAMP_LEN + 1];
  DWORD dwMatches;
  BOOL  bExclude;
} KEYWORD_LIST;

#endif // #ifndef __TAIL_COMMON_H__
