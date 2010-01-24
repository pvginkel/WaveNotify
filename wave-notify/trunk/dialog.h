/*
 * This file is part of Google Wave Notifier.
 *
 * Google Wave Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Google Wave Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INC_DIALOG
#define _INC_DIALOG

#pragma once

class CDialog;

class CModelessDialogs
{
public:
	typedef map<INT, CDialog *> THandlesMap;
	typedef THandlesMap::iterator THandlesMapIter;
	typedef THandlesMap::const_iterator THandlesMapConstIter;

private:
	static THandlesMap m_vDialogs;

public:
	static BOOL IsDialogMessage(LPMSG lpMsg);
	static void RegisterDialog(INT nType, CDialog * lpDialog);
	static void UnregisterDialog(INT nType);
	static void UnregisterDialog(CDialog * lpDialog);
	static BOOL ContainsType(INT nType);
	static CDialog * GetDialog(INT nType) { return m_vDialogs[nType]; }
};

class CDialog : public CWindowHandle
{
private:
	INT m_nResource;
	BOOL m_fDisposing;

protected:
	CDialog(INT nResource);
	virtual ~CDialog();

public:
	INT_PTR ShowDialog(CWindowHandle * lpParentWindow = NULL);
	void Create(INT nType, CWindowHandle * lpParentWindow = NULL);

protected:
	virtual INT_PTR DialogProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	virtual void BeforeShow() { }

private:
	static INT_PTR CALLBACK DialogProcCallback(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};

#endif // _INC_DIALOG
