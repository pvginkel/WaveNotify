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

#ifndef _INC_WINDOWHANDLE
#define _INC_WINDOWHANDLE

#pragma once

class CWindowHandle;

typedef vector<CWindowHandle *> TWindowHandleVector;
typedef TWindowHandleVector::iterator TWindowHandleVectorIter;
typedef TWindowHandleVector::const_iterator TWindowHandleVectorConstIter;

class CWindowHandle
{
private:
	HWND m_hWnd;

public:
	CWindowHandle() { m_hWnd = NULL; }
	CWindowHandle(HWND hWnd) { m_hWnd = hWnd; }
	virtual ~CWindowHandle() { }

	HWND GetHandle() const { return m_hWnd; }

	LRESULT SendMessage(UINT uMessage, WPARAM wParam = 0, LPARAM lParam = 0) const {
		return ::SendMessage(m_hWnd, uMessage, wParam, lParam);
	}
	LRESULT PostMessage(UINT uMessage, WPARAM wParam = 0, LPARAM lParam = 0) const {
		return ::PostMessage(m_hWnd, uMessage, wParam, lParam);
	}
	LRESULT SendDlgItemMessage(INT nDlgItem, UINT uMessage, WPARAM wParam = 0, LPARAM lParam = 0) const {
		return ::SendDlgItemMessage(GetHandle(), nDlgItem, uMessage, wParam, lParam);
	}
	wstring GetDlgItemText(INT nDlgItem) const;
	void SetDlgItemText(INT nDlgItem, wstring szValue) const { ::SetDlgItemText(GetHandle(), nDlgItem, szValue.c_str()); }
	INT GetDlgItemInt(INT nDlgItem, BOOL fSigned = TRUE) const {
		return ::GetDlgItemInt(GetHandle(), nDlgItem, NULL, fSigned);
	}
	INT GetDlgItemInt(INT nDlgItem, BOOL fSigned, BOOL & fSuccess) const {
		return ::GetDlgItemInt(GetHandle(), nDlgItem, &fSuccess, fSigned);
	}
	void SetDlgItemInt(INT nDlgItem, INT nValue) const {
		::SetDlgItemInt(GetHandle(), nDlgItem, nValue, TRUE);
	}
	void SetDlgItemInt(INT nDlgItem, DWORD dwValue) const {
		::SetDlgItemInt(GetHandle(), nDlgItem, dwValue, FALSE);
	}
	BOOL GetDlgItemChecked(INT nDlgItem) const {
		return SendDlgItemMessage(nDlgItem, BM_GETCHECK) == BST_CHECKED;
	}
	void SetDlgItemChecked(INT nDlgItem, BOOL fChecked) const {
		SendDlgItemMessage(nDlgItem, BM_SETCHECK, fChecked ? BST_CHECKED : BST_UNCHECKED);
	}
	HWND GetDlgItem(INT nDlgItem) const {
		return ::GetDlgItem(GetHandle(), nDlgItem);
	}
	void SetDlgItemEnabled(INT nDlgItem, BOOL fEnabled) const {
		::EnableWindow(GetDlgItem(nDlgItem), fEnabled);
	}
	BOOL GetDlgItemEnabled(INT nDlgItem) const {
		return ::IsWindowEnabled(GetDlgItem(nDlgItem));
	}
	void SetMessageResult(INT_PTR lResult) const {
		::SetWindowLongPtr(GetHandle(), DWL_MSGRESULT, lResult);
	}

protected:
	void SetHandle(HWND hWnd) { m_hWnd = hWnd; }
};

#endif // _INC_WINDOWHANDLE
