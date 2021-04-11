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
 * $Id: Workspace.h,v 1.4 2003/11/21 14:02:29 paulperkins Exp $
 * 
 */


#ifndef __TAIL_WORKSPACE__
#define __TAIL_WORKSPACE__

class CTailDoc;
class CTailView;

class CWorkspaceItem : public CObject
{
protected:
  DECLARE_DYNAMIC(CWorkspaceItem)

public:
  CWorkspaceItem (void);
  ~CWorkspaceItem (void);

  void Close (void);
  void Activate (void);
  const char* GetFilename (void);

  CTailView* m_pView;
  CTailDoc*  m_pDoc;
  char       m_szTailFile[_MAX_FNAME];
  HTREEITEM  m_hti;
};

/////////////////////////////////////////////////////////////////////////////
// CWorkspace 
//
// This class abstracts a collection of files to be monitored.
//
class CWorkspace : public CObject
{
protected:
  DECLARE_DYNAMIC(CWorkspace)

public:
  CWorkspace (void);
  ~CWorkspace (void);

  CWorkspaceItem* AddItem (CWorkspaceItem* pItem);
  CWorkspaceItem* AddDocument (CTailDoc* pDoc);
  CWorkspaceItem* FindItem (CWorkspaceItem* pItem);
  CWorkspaceItem* FindDocument (CTailDoc* pDoc);
  CWorkspaceItem* FindView (CTailView* pView);

  BOOL RemoveItem (CWorkspaceItem* pItem);
  void Activate (void);


  CObList* m_pItems;

  HTREEITEM m_hti;

  char  m_szName[1024];
};

#endif // #ifndef __TAIL_WORKSPACE__
