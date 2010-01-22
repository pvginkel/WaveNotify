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

CFlyout::CFlyout() : CWindow(L"Flyout")
{
	m_nHeight = 300;
	m_nWidth = 200;
	GetCursorPos(&m_ptSnap);
	m_fMouseOver = FALSE;
}

ATOM CFlyout::CreateClass(LPWNDCLASSEX lpWndClass)
{
	lpWndClass->style = CS_HREDRAW | CS_VREDRAW;
	lpWndClass->hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	lpWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);

	return CWindow::CreateClass(lpWndClass);
}

HWND CFlyout::CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu)
{
	HWND hWnd = CWindow::CreateHandle(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE,
		L"",
		WS_POPUP | WS_CLIPCHILDREN | WS_THICKFRAME,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL);

	// Compensate for frame

	RECT rc;
	RECT rcClient;

	GetWindowRect(GetHandle(), &rc);
	GetClientRect(GetHandle(), &rcClient);
	
	SIZE sOffset = { 17, 8 };

	SIZE sFlyoutSize = {
		(rc.right - rc.left) - (rcClient.right - rcClient.left) + GetWidth(),
		(rc.bottom - rc.top) - (rcClient.bottom - rcClient.top) + GetHeight()
	};

	RECT rcFlyout;

	GetTaskbarPopupLocation(sFlyoutSize, sOffset, &rcFlyout);
	SnapTaskbarPopupLocation(&rcFlyout, m_ptSnap);

	SetWindowPos(
		GetHandle(),
		NULL, 
		rcFlyout.left,
		rcFlyout.top,
		sFlyoutSize.cx,
		sFlyoutSize.cy,
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOZORDER | SWP_SHOWWINDOW);

	UpdateWindow(GetHandle());
	SetForegroundWindow(GetHandle());

	return hWnd;
}

void CFlyout::PreviewWndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_MOUSEMOVE:
		OnMouseOver();
		break;

	case WM_MOUSELEAVE:
		m_fMouseOver = FALSE;
		break;
	}

	CWindow::PreviewWndProc(uMessage, wParam, lParam);
}

LRESULT CFlyout::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_KILLFOCUS:
		DestroyWindow(GetHandle());
		break;

	case WM_NCHITTEST:
		return HTCLIENT;
	}

	return CWindow::WndProc(uMessage, wParam, lParam);
}

void CFlyout::OnMouseOver()
{
	if (!m_fMouseOver)
	{
		TRACKMOUSEEVENT tme;

		memset(&tme, 0, sizeof(TRACKMOUSEEVENT));

		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetHandle();

		TrackMouseEvent(&tme);

		m_fMouseOver = TRUE;
	}
}
