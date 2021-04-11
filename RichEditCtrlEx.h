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
 * $Id: RichEditCtrlEx.h,v 1.1 2003/11/11 07:31:45 paulperkins Exp $
 * 
 */

#if !defined(AFX_RICHEDITCTRLEX_H__1EDE4055_9595_4CA5_AE3E_2AAC99DC7F51__INCLUDED_)
#define AFX_RICHEDITCTRLEX_H__1EDE4055_9595_4CA5_AE3E_2AAC99DC7F51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichEditCtrlEx.h : header file
//

class _AFX_RICHEDITEX_STATE
{
	public:

			 _AFX_RICHEDITEX_STATE() ;
	virtual ~_AFX_RICHEDITEX_STATE() ;

	public:

	HINSTANCE m_hInstRichEdit20 ;
} ;


/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window

class CRichEditCtrlEx : public CRichEditCtrl
{
// Construction
public:
	CRichEditCtrlEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditCtrlEx)
	public:
  virtual BOOL Create( DWORD dwStyle, const RECT& rcRect, CWnd* ipParentWnd, UINT nID ) ;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRichEditCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRichEditCtrlEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITCTRLEX_H__1EDE4055_9595_4CA5_AE3E_2AAC99DC7F51__INCLUDED_)
