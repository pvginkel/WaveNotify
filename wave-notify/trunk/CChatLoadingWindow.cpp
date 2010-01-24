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

#include "stdafx.h"
#include "include.h"
#include "chatwindow.h"

ATOM CChatLoadingWindow::CreateClass(LPWNDCLASSEX lpWndClass)
{
	ASSERT(lpWndClass != NULL);

	lpWndClass->style = CS_HREDRAW | CS_VREDRAW;
	lpWndClass->hCursor = CNotifierApp::Instance()->GetCursorArrow();

	return CWindow::CreateClass(lpWndClass);
}

HWND CChatLoadingWindow::CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu)
{
	return CWindow::CreateHandle(
		0,
		L"Loading...",
		WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0,
		0,
		150,
		19,
		lpParentWindow,
		hMenu);
}

LRESULT CChatLoadingWindow::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_SIZE: {
		SIZE szSize = { LOWORD(lParam), HIWORD(lParam) };
		return OnSize(szSize);
	}
	case WM_PAINT:
		return OnPaint();

	default:
		return CWindow::WndProc(uMessage, wParam, lParam);
	}
}

LRESULT CChatLoadingWindow::OnPaint()
{
	CPaintDC dc(this);

	CThemeScheme * lpScheme = CThemeScheme::GetScheme(ThemeSchemeActive);

	ASSERT(lpScheme != NULL);

	RECT rc;

	GetClientRect(&rc);

	dc.FillRect(&rc, lpScheme->GetBrush(ThemeColorLoadingBackground));

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(ThemeColorLoadingBorder));

	INT nWidth = rc.right - 1;
	INT nHeight = rc.bottom - 1;

	dc.MoveToEx(0, 0);
	dc.LineTo(nWidth - 3, 0);
	dc.LineTo(nWidth - 3, 1);
	dc.LineTo(nWidth - 1, 1);
	dc.LineTo(nWidth - 1, 3);
	dc.LineTo(nWidth, 3);
	dc.LineTo(nWidth, nHeight);

	COLORREF crTextColor = dc.SetTextColor(lpScheme->GetColor(ThemeColorLoadingText));
	COLORREF crBkColor = dc.SetBkColor(lpScheme->GetColor(ThemeColorLoadingBackground));

	dc.SelectObject(hOriginal);

	hOriginal = dc.SelectFont(CThemeScheme::GetFont());

	InflateRect(&rc, -3, -3);

	dc.DrawText(GetWindowText(), &rc, DT_NOPREFIX | DT_SINGLELINE);

	dc.SelectObject(hOriginal);
	dc.SetTextColor(crTextColor);
	dc.SetBkColor(crBkColor);

	return 0;
}

LRESULT CChatLoadingWindow::OnSize(SIZE szSize)
{
	INT nWidth = szSize.cx;
	INT nHeight = szSize.cy;

	POINT vPoints[] = {
		{ 0, 0 },
		{ nWidth - 3, 0 },
		{ nWidth - 3, 1 },
		{ nWidth - 1, 1 },
		{ nWidth - 1, 3 },
		{ nWidth, 3 },
		{ nWidth, nHeight },
		{ 0, nHeight + 1 },
		{ 0, 0 }
	};

	SetWindowRgn(
		CreatePolygonRgn(vPoints, _ARRAYSIZE(vPoints), ALTERNATE),
		TRUE
	);

	return 0;
}
