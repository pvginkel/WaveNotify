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

#define FLYOUT_CLASS L"Flyout"

static void Flyout_Initialize();
static LRESULT CALLBACK Flyout_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void Flyout_WmCreate(HWND hWnd, LPCREATESTRUCT lpcs);

static ATOM g_uClass = 0;
static HWND g_hWnd = NULL;

LPFLYOUT Flyout_Create()
{
	LPFLYOUT lpFlyout = ALLOCA(FLYOUT);

	memset(lpFlyout, 0, sizeof(FLYOUT));

	lpFlyout->nHeight = 300;
	lpFlyout->nWidth = 200;

	return lpFlyout;
}

void Flyout_Show(LPFLYOUT lpFlyout, POINT p)
{
	RECT r;
	RECT rClient;
	SIZE sFlyoutSize;
	SIZE sOffset;
	RECT rFlyout;

	Flyout_Initialize();

	if (g_hWnd != NULL)
	{
		DestroyWindow(g_hWnd);
		g_hWnd = NULL;
	}

	sFlyoutSize.cx = lpFlyout->nWidth;
	sFlyoutSize.cy = lpFlyout->nHeight;

	sOffset.cx = 17;
	sOffset.cy = 8;

	GetTaskbarPopupLocation(sFlyoutSize, sOffset, &rFlyout);

	g_hWnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE,
		(LPWSTR)g_uClass,
		L"",
		WS_POPUP | WS_CLIPCHILDREN | WS_THICKFRAME,
		rFlyout.left,
		rFlyout.top,
		lpFlyout->nWidth,
		lpFlyout->nHeight,
		NULL,
		NULL,
		g_hInstance,
		lpFlyout);

	// Compensate for frame

	GetWindowRect(g_hWnd, &r);
	GetClientRect(g_hWnd, &rClient);

	sFlyoutSize.cx = (r.right - r.left) - (rClient.right - rClient.left) + lpFlyout->nWidth;
	sFlyoutSize.cy = (r.bottom - r.top) - (rClient.bottom - rClient.top) + lpFlyout->nHeight;

	GetTaskbarPopupLocation(sFlyoutSize, sOffset, &rFlyout);
	SnapTaskbarPopupLocation(&rFlyout, p);

	SetWindowPos(
		g_hWnd,
		NULL, 
		rFlyout.left,
		rFlyout.top,
		sFlyoutSize.cx,
		sFlyoutSize.cy,
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOZORDER | SWP_SHOWWINDOW);

	UpdateWindow(g_hWnd);

	SetForegroundWindow(g_hWnd);
}

static void Flyout_Initialize()
{
	if (g_uClass == 0)
	{
		WNDCLASSEX wcex;

		memset(&wcex, 0, sizeof(WNDCLASSEX));

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Flyout_WndProc;
		wcex.hInstance = g_hInstance;
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszClassName = FLYOUT_CLASS;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

		g_uClass = RegisterClassEx(&wcex);
	}
}

static LRESULT CALLBACK Flyout_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPFLYOUT lpFlyout = (LPFLYOUT)GetWindowLongPtr(hWnd, GWL_USERDATA);
	LRESULT lResult;

	if (lpFlyout != NULL)
	{
		lResult = lpFlyout->lpfnWndProc(hWnd, message, wParam, lParam);
	}
	else
	{
		lResult = 0;
	}

	switch (message)
	{
	case WM_CREATE:
		Flyout_WmCreate(hWnd, (LPCREATESTRUCT)lParam);
		return 0;

	case WM_KILLFOCUS:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		FREE(lpFlyout);
		SetWindowLongPtr(hWnd, GWL_USERDATA, 0);
		return 0;

	case WM_NCHITTEST:
		return HTCLIENT;
	}

	if (lResult == 0)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	else
	{
		return lResult;
	}
}

static void Flyout_WmCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	LPFLYOUT lpFlyout = (LPFLYOUT)lpcs->lpCreateParams;

	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)lpFlyout);

	lpFlyout->lpfnWndProc(hWnd, WM_CREATE, 0, 0);
}

INT_PTR Flyout_GetTag(HWND hWnd)
{
	LPFLYOUT lpFlyout = (LPFLYOUT)GetWindowLongPtr(hWnd, GWL_USERDATA);

	return lpFlyout->nTag;
}
