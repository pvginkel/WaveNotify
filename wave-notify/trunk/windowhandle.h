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
		CHECK_HANDLE_OR_NULL(hWnd);
		
		m_hWnd = hWnd;
		m_fUpdatesLocked = FALSE;
		m_nStoredStyle = 0;
	}
	virtual ~CWindowHandle() { }

	HWND GetHandle() const { return m_hWnd; }

	BOOL IsWindow() const { return ::IsWindow(GetHandle()); }
	LRESULT SendMessage(UINT uMessage, WPARAM wParam = 0, LPARAM lParam = 0) const {
		CHECK(IsWindow());

		return ::SendMessage(m_hWnd, uMessage, wParam, lParam);
	}
	LRESULT PostMessage(UINT uMessage, WPARAM wParam = 0, LPARAM lParam = 0) const {
		CHECK(IsWindow());

		return ::PostMessage(m_hWnd, uMessage, wParam, lParam);
	}
	LRESULT SendDlgItemMessage(INT nDlgItem, UINT uMessage, WPARAM wParam = 0, LPARAM lParam = 0) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		return ::SendDlgItemMessage(GetHandle(), nDlgItem, uMessage, wParam, lParam);
	}
	wstring GetDlgItemText(INT nDlgItem) const;
	void SetDlgItemText(INT nDlgItem, wstring szValue) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		::SetDlgItemText(GetHandle(), nDlgItem, szValue.c_str());
	}
	INT GetDlgItemInt(INT nDlgItem, BOOL fSigned = TRUE) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		return ::GetDlgItemInt(GetHandle(), nDlgItem, NULL, fSigned);
	}
	INT GetDlgItemInt(INT nDlgItem, BOOL fSigned, BOOL & fSuccess) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		return ::GetDlgItemInt(GetHandle(), nDlgItem, &fSuccess, fSigned);
	}
	void SetDlgItemInt(INT nDlgItem, INT nValue) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		::SetDlgItemInt(GetHandle(), nDlgItem, nValue, TRUE);
	}
	void SetDlgItemInt(INT nDlgItem, DWORD dwValue) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		INT nResult = ::SetDlgItemInt(GetHandle(), nDlgItem, dwValue, FALSE);

		CHECK_NE_0(nResult);
	}
	BOOL GetDlgItemChecked(INT nDlgItem) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		return SendDlgItemMessage(nDlgItem, BM_GETCHECK) == BST_CHECKED;
	}
	void SetDlgItemChecked(INT nDlgItem, BOOL fChecked) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		SendDlgItemMessage(nDlgItem, BM_SETCHECK, fChecked ? BST_CHECKED : BST_UNCHECKED);
	}
	HWND GetDlgItem(INT nDlgItem) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		return ::GetDlgItem(GetHandle(), nDlgItem);
	}
	void SetDlgItemEnabled(INT nDlgItem, BOOL fEnabled) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		::EnableWindow(GetDlgItem(nDlgItem), fEnabled);
	}
	BOOL GetDlgItemEnabled(INT nDlgItem) const {
		CHECK(IsWindow());
		CHECK_GT_0(nDlgItem);

		return ::IsWindowEnabled(GetDlgItem(nDlgItem));
	}
	void SetMessageResult(INT_PTR lResult) const {
		CHECK(IsWindow());

		::SetWindowLongPtr(GetHandle(), DWL_MSGRESULT, lResult);
	}
	BOOL ShowWindow(INT nCmdShow) const {
		CHECK(IsWindow());

		return ::ShowWindow(GetHandle(), nCmdShow);
	}
	LONG_PTR GetWindowLong(INT nIndex) const {
		CHECK(IsWindow());

		return ::GetWindowLongPtr(GetHandle(), nIndex);
	}
	LONG_PTR SetWindowLong(INT nIndex, LONG_PTR nValue) const {
		CHECK(IsWindow());

		return ::SetWindowLongPtr(GetHandle(), nIndex, nValue);
	}
	BOOL SetProp(LPCWSTR szString, HANDLE hData) const {
		ASSERT(szString != NULL);
		CHECK(IsWindow());

		return ::SetProp(GetHandle(), szString, hData);
	}
	HANDLE GetProp(LPCWSTR szString) const {
		ASSERT(szString != NULL);
		CHECK(IsWindow());

		return ::GetProp(GetHandle(), szString);
	}
	HANDLE RemoveProp(LPCWSTR szString) const {
		ASSERT(szString != NULL);
		CHECK(IsWindow());

		return ::RemoveProp(GetHandle(), szString);
	}
	BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) const {
		CHECK(IsWindow());

		return ::SetWindowPos(GetHandle(), hWndInsertAfter, X, Y, cx, cy, uFlags);
	}
	INT SetWindowRgn(HRGN hRgn, BOOL bRedraw) const {
		CHECK(IsWindow());

		return ::SetWindowRgn(GetHandle(), hRgn, bRedraw);
	}
	BOOL InvalidateRect(CONST RECT *lpRect, BOOL bErase = FALSE) const {
		CHECK(IsWindow());

		return ::InvalidateRect(GetHandle(), lpRect, bErase);
	}
	BOOL UpdateWindow() const {
		CHECK(IsWindow());

		return ::UpdateWindow(GetHandle());
	}
	BOOL GetWindowRect(LPRECT lpRect) const {
		CHECK(IsWindow());

		return ::GetWindowRect(GetHandle(), lpRect);
	}
	BOOL ScreenToClient(LPPOINT lpPoint) const {
		CHECK(IsWindow());

		return ::ScreenToClient(GetHandle(), lpPoint);
	}
	BOOL GetClientRect(LPRECT lpRect) const {
		CHECK(IsWindow());

		return ::GetClientRect(GetHandle(), lpRect);
	}
	ULONG_PTR GetClassLong(int nIndex) const {
		CHECK(IsWindow());

		return ::GetClassLongPtr(GetHandle(), nIndex);
	}
	BOOL IsZoomed() const {
		CHECK(IsWindow());

		return !!::IsZoomed(GetHandle());
	}
	BOOL IsForegroundWindow() const {
		CHECK(IsWindow());

		return GetHandle() == GetForegroundWindow();
	}
	BOOL IsActiveWindow() const {
		CHECK(IsWindow());

		return GetHandle() == GetActiveWindow();
	}
	BOOL IsFocusedWindow() const {
		CHECK(IsWindow());

		return GetHandle() == GetFocus();
	}
	BOOL HasMinimizeButton() const {
		CHECK(IsWindow());

		return !!(GetWindowLong(GWL_STYLE) & WS_MINIMIZEBOX);
	}
	BOOL HasMaximizeButton() const {
		CHECK(IsWindow());

		return !!(GetWindowLong(GWL_STYLE) & WS_MAXIMIZEBOX);
	}
	BOOL HasCloseButton() const {
		CHECK(IsWindow());

		return !!(GetWindowLong(GWL_STYLE) & WS_SYSMENU);
	}
	wstring GetWindowText() const;
	BOOL ClientToScreen(LPPOINT lpPoint) const {
		CHECK(IsWindow());

		return ::ClientToScreen(GetHandle(), lpPoint);
	}
	void ExecuteSystemMenuCommand(INT nCommand) const {
		CHECK(IsWindow());

		SendMessage(WM_SYSCOMMAND, (WPARAM)nCommand);
	}
	void LockUpdates() {
		CHECK(IsWindow());
		CHECK(!m_fUpdatesLocked);

		m_fUpdatesLocked = TRUE;
		m_nStoredStyle = GetWindowLong(GWL_STYLE);
		SetWindowLong(GWL_STYLE, m_nStoredStyle & ~WS_VISIBLE);
	}
	void UnlockUpdates() {
		CHECK(IsWindow());
		CHECK(m_fUpdatesLocked);

		m_fUpdatesLocked = FALSE;
		SetWindowLong(GWL_STYLE, m_nStoredStyle);
	}
	BOOL IsIconic() const {
		CHECK(IsWindow());

		return ::IsIconic(GetHandle());
	}
	BOOL GetUpdatesLocked() const { return m_fUpdatesLocked; }
	BOOL SetForegroundWindow() const {
		CHECK(IsWindow());

		return ::SetForegroundWindow(GetHandle());
	}
	BOOL RedrawWindow(CONST RECT * lprcUpdate, HRGN hrgnUpdate, UINT uFlags) const {
		CHECK(IsWindow());

		return ::RedrawWindow(GetHandle(), lprcUpdate, hrgnUpdate, uFlags);
	}

protected:
	void SetHandle(HWND hWnd) {
		CHECK_HANDLE_OR_NULL(hWnd);

		m_hWnd = hWnd;
	}
};

class CScopedRedrawLock
{
private:
	CWindowHandle * m_lpWindow;

public:
	CScopedRedrawLock(CWindowHandle * lpWindow) : m_lpWindow(lpWindow) {
		ASSERT(lpWindow != NULL);
		m_lpWindow->LockUpdates();
	}
	virtual ~CScopedRedrawLock() {
		m_lpWindow->UnlockUpdates();
	}
};

#endif // _INC_WINDOWHANDLE
