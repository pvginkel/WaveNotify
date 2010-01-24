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
	BOOL m_fUpdatesLocked;
	LONG_PTR m_nStoredStyle;

public:
	CWindowHandle(HWND hWnd = NULL) {
		m_hWnd = hWnd;
		m_fUpdatesLocked = FALSE;
		m_nStoredStyle = 0;
	}
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
	BOOL ShowWindow(INT nCmdShow) const {
		return ::ShowWindow(GetHandle(), nCmdShow);
	}
	LONG_PTR GetWindowLong(INT nIndex) const {
		return ::GetWindowLongPtr(GetHandle(), nIndex);
	}
	LONG_PTR SetWindowLong(INT nIndex, LONG_PTR nValue) const {
		return ::SetWindowLongPtr(GetHandle(), nIndex, nValue);
	}
	BOOL SetProp(LPCWSTR szString, HANDLE hData) const {
		return ::SetProp(GetHandle(), szString, hData);
	}
	HANDLE GetProp(LPCWSTR szString) const {
		return ::GetProp(GetHandle(), szString);
	}
	HANDLE RemoveProp(LPCWSTR szString) const {
		return ::RemoveProp(GetHandle(), szString);
	}
	BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) const {
		return ::SetWindowPos(GetHandle(), hWndInsertAfter, X, Y, cx, cy, uFlags);
	}
	INT SetWindowRgn(HRGN hRgn, BOOL bRedraw) const {
		return ::SetWindowRgn(GetHandle(), hRgn, bRedraw);
	}
	BOOL InvalidateRect(CONST RECT *lpRect, BOOL bErase = FALSE) const {
		return ::InvalidateRect(GetHandle(), lpRect, bErase);
	}
	BOOL UpdateWindow() const {
		return ::UpdateWindow(GetHandle());
	}
	BOOL GetWindowRect(LPRECT lpRect) const {
		return ::GetWindowRect(GetHandle(), lpRect);
	}
	BOOL ScreenToClient(LPPOINT lpPoint) const {
		return ::ScreenToClient(GetHandle(), lpPoint);
	}
	BOOL GetClientRect(LPRECT lpRect) const {
		return ::GetClientRect(GetHandle(), lpRect);
	}
	ULONG_PTR GetClassLong(int nIndex) const {
		return ::GetClassLongPtr(GetHandle(), nIndex);
	}
	BOOL IsZoomed() const {
		return !!::IsZoomed(GetHandle());
	}
	BOOL IsForegroundWindow() const {
		return GetHandle() == GetForegroundWindow();
	}
	BOOL IsActiveWindow() const {
		return GetHandle() == GetActiveWindow();
	}
	BOOL IsFocusedWindow() const {
		return GetHandle() == GetFocus();
	}
	BOOL HasMinimizeButton() const {
		return !!(GetWindowLong(GWL_STYLE) & WS_MINIMIZEBOX);
	}
	BOOL HasMaximizeButton() const {
		return !!(GetWindowLong(GWL_STYLE) & WS_MAXIMIZEBOX);
	}
	BOOL HasCloseButton() const {
		return !!(GetWindowLong(GWL_STYLE) & WS_SYSMENU);
	}
	wstring GetWindowText() const;
	BOOL ClientToScreen(LPPOINT lpPoint) const {
		return ::ClientToScreen(GetHandle(), lpPoint);
	}
	void ExecuteSystemMenuCommand(INT nCommand) const {
		SendMessage(WM_SYSCOMMAND, (WPARAM)nCommand);
	}
	void LockUpdates() {
		m_fUpdatesLocked = TRUE;
		m_nStoredStyle = GetWindowLong(GWL_STYLE);
		SetWindowLong(GWL_STYLE, m_nStoredStyle & ~WS_VISIBLE);
	}
	void UnlockUpdates() {
		m_fUpdatesLocked = FALSE;
		SetWindowLong(GWL_STYLE, m_nStoredStyle);
	}
	BOOL IsIconic() const {
		return ::IsIconic(GetHandle());
	}
	BOOL GetUpdatesLocked() const { return m_fUpdatesLocked; }
	BOOL SetForegroundWindow() const {
		return ::SetForegroundWindow(GetHandle());
	}
	BOOL IsWindow() const { return ::IsWindow(m_hWnd); }
	BOOL RedrawWindow(CONST RECT * lprcUpdate, HRGN hrgnUpdate, UINT uFlags) const {
		return ::RedrawWindow(GetHandle(), lprcUpdate, hrgnUpdate, uFlags);
	}

protected:
	void SetHandle(HWND hWnd) { m_hWnd = hWnd; }
};

class CScopedRedrawLock
{
private:
	CWindowHandle * m_lpWindow;

public:
	CScopedRedrawLock(CWindowHandle * lpWindow) : m_lpWindow(lpWindow) {
		m_lpWindow->LockUpdates();
	}
	virtual ~CScopedRedrawLock() {
		m_lpWindow->UnlockUpdates();
	}
};

#endif // _INC_WINDOWHANDLE
