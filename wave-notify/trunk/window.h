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

#ifndef _INC_WINDOW
#define _INC_WINDOW

#pragma once

class CWindow :
	public CWindowHandle,
	public CRefCounted
{
private:
	wstring m_szClassName;
	BOOL m_fClassRegistered;
	HCURSOR m_hCursor;
	BOOL m_fHandleCreated;

protected:
	CWindow(wstring szClassName);
	virtual ~CWindow();

protected:
	virtual ATOM CreateClass(LPWNDCLASSEX lpWndClass);
	virtual HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu);
	virtual LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	virtual void PreviewWndProc(UINT uMessage, WPARAM wParam, LPARAM lParam) { }

public:
	virtual BOOL Create();
	wstring GetClassName();
	BOOL IsHandleCreated() const { return m_fHandleCreated; }
	void SetCursor(HCURSOR hCursor) {
		m_hCursor = hCursor;
		if (hCursor != NULL) ::SetCursor(m_hCursor);
	}

private:
	static LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};

#endif // _INC_WINDOW
