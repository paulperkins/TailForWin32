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
 * $Id: Eventlist.cpp,v 1.3 2003/11/11 09:45:09 paulperkins Exp $
 * 
 */

#include "stdafx.h"

#include "Eventlist.h"


DWORD CTailEmailEvent::Action (
  const char* pszContext1,
  const char* pszContext2)
{
  
  return 0;
}


CEventList::~CEventList (
  void)
{
  POSITION stPos;

  for (stPos = pItems->GetHeadPosition (); stPos != NULL; ) 
  {
    delete (CTailEvent*) pItems->GetNext (stPos);
  }

  delete pItems;
}

CTailEvent* CEventList::FindEvent (
  const char* pszName)
{
  POSITION stPos;
  CTailEvent* pEvent = NULL;

  for (stPos = pItems->GetHeadPosition (); stPos != NULL; ) 
  {
    pEvent = (CTailEvent*) pItems->GetNext (stPos);

    if (strcmp (pEvent->szName, pszName) == 0)
    {
      return pEvent;
    }
  }

  return NULL;
}

void CEventList::AddEvent (
  CTailEvent* pEvent)
{
  pItems->AddHead ((CObject*) pEvent);
}
