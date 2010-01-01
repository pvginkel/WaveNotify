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

#ifndef _INC_FLYOUT
#define _INC_FLYOUT

#pragma once

class CFlyout : public CWindow
{
private:
	INT m_nHeight;
	INT m_nWidth;
	POINT m_ptSnap;
	BOOL m_fMouseOver;

protected:
	CFlyout();

	ATOM CreateClass(LPWNDCLASSEX lpWndClass);
	HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu);
	void PreviewWndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

public:
	INT GetHeight() const { return m_nHeight; }
	void SetHeight(INT nHeight) { m_nHeight = nHeight; }
	INT GetWidth() const { return m_nWidth; }
	void SetWidth(INT nWidth) { m_nWidth = nWidth; }
	POINT GetSnapPoint() const { return m_ptSnap; }
	void SetSnapPoint(POINT pt) { m_ptSnap = pt; }
	void OnMouseOver();
};

#endif //_INC_FLYOUT
