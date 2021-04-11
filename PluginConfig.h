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
 * $Id: PluginConfig.h,v 1.3 2003/11/11 09:45:09 paulperkins Exp $
 * 
 */

#if !defined(AFX_PLUGINCONFIG_H__D00338FB_8725_4F08_BD41_7CFBE8032376__INCLUDED_)
#define AFX_PLUGINCONFIG_H__D00338FB_8725_4F08_BD41_7CFBE8032376__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PluginConfig.h : header file
//

#include "Plugin.h"

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPluginConfig dialog

class CPluginConfig : public CDialog
{
// Construction
public:
	CPluginConfig(CWnd* pParent = NULL);   // standard constructor
  CPluginList* m_pPlugins;


// Dialog Data
	//{{AFX_DATA(CPluginConfig)
	enum { IDD = IDD_PLUGINS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPluginConfig)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnConfigure();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINCONFIG_H__D00338FB_8725_4F08_BD41_7CFBE8032376__INCLUDED_)
