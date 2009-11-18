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

#define WM_LINK_CLICKED (WM_USER + 1)

static INT_PTR CALLBACK NewVersionForm_DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static INT_PTR NewVersionForm_WmInitDialog(HWND hWnd);
static void NewVersionForm_UrlControlProcPaint(HWND hWnd);
static LRESULT CALLBACK NewVersionForm_UrlControlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void NewVersionForm_StaticToUrlControl(HWND hWnd, UINT uID, LPWSTR szUrl);
static INT_PTR NewVersionForm_WmCommand(HWND hWnd, WORD wCommand, WORD wNotifyMessage);

typedef struct tagURLCONTROL
{
    WCHAR szUrl[_MAX_PATH];
    WNDPROC lpOldProc;
} URLCONTROL, * LPURLCONTROL;

void NewVersionForm_ShowDialog(HWND hWnd)
{
	DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_NEW_VERSION), hWnd, NewVersionForm_DialogProc);
}

static INT_PTR CALLBACK NewVersionForm_DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return NewVersionForm_WmInitDialog(hWnd);

	case WM_COMMAND:
		return NewVersionForm_WmCommand(hWnd, LOWORD(wParam), HIWORD(wParam));

	case WM_LINK_CLICKED:
		EndDialog(hWnd, IDOK);
		return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

static INT_PTR NewVersionForm_WmInitDialog(HWND hWnd)
{
	static HICON hIcon = NULL;
	static HICON hIconSmall = NULL;
	static HFONT hLinkFont = NULL;

	if (hIcon == NULL)
	{
		hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	}
	if (hIconSmall == NULL)
	{
	 	hIconSmall = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAINICON_SMALL));
	}

	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);

	NewVersionForm_StaticToUrlControl(hWnd, IDC_DOWNLOAD_LATEST_VERSION, L"http://sourceforge.net/projects/wave-notify/");

	return (INT_PTR)TRUE;
}

static void NewVersionForm_StaticToUrlControl(HWND hWnd, UINT uID, LPWSTR szUrl)
{
	HWND hItemWnd = GetDlgItem(hWnd, uID);

	LPURLCONTROL lpData = ALLOCA(URLCONTROL);

	wcsncpy_s(lpData->szUrl, _ARRAYSIZE(lpData->szUrl), szUrl, _ARRAYSIZE(lpData->szUrl));

	lpData->szUrl[_ARRAYSIZE(lpData->szUrl) - 1] = L'\0';

	lpData->lpOldProc = (WNDPROC)SetWindowLong(hItemWnd, GWL_WNDPROC, (LONG)NewVersionForm_UrlControlProc);

	SetWindowLongPtr(hItemWnd, GWL_USERDATA, (INT_PTR)lpData);
}

static LRESULT CALLBACK NewVersionForm_UrlControlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPURLCONTROL lpData;
	static HCURSOR hCursor = NULL;
	WNDPROC lpOldProc;
	
	lpData = (LPURLCONTROL)GetWindowLong(hWnd, GWL_USERDATA);

	lpOldProc = lpData->lpOldProc;

	if (hCursor == NULL)
	{
		hCursor = LoadCursor(NULL, IDC_HAND);
	}

	switch(message)
	{
	case WM_NCDESTROY:
		FREE(lpData);
		break;

	case WM_SETCURSOR:
		SetCursor(hCursor);
		return 1;

	case WM_PAINT:
		NewVersionForm_UrlControlProcPaint(hWnd);
		return 0;

	case WM_LBUTTONUP:
		ShellExecute(NULL, L"OPEN", lpData->szUrl, NULL, NULL, SW_SHOW);

		PostMessage(GetParent(hWnd), WM_LINK_CLICKED, 0, 0);
		return 0;

	case WM_NCHITTEST:
    		return HTCLIENT;
	}

	return CallWindowProc(lpOldProc, hWnd, message, wParam, lParam);
}

static void NewVersionForm_UrlControlProcPaint(HWND hWnd)
{
	DWORD dwStyle   = GetWindowLong(hWnd, GWL_STYLE);
	DWORD dwDrawStyle;
	PAINTSTRUCT ps;
	static HFONT hFont = NULL;
	HGDIOBJ hOriginal;
	WCHAR szBuffer[REGVALUE_SIZE];
	RECT r;
	HDC hdc;
	
	BeginPaint(hWnd, &ps);

	hdc = ps.hdc;

	dwDrawStyle = DT_SINGLELINE;

	if(dwStyle & SS_CENTER)
	{
		dwDrawStyle |= DT_CENTER;
	}

	if(dwStyle & SS_RIGHT)
	{
		dwDrawStyle |= DT_RIGHT;
	}

	if(dwStyle & SS_CENTERIMAGE)
	{
		dwDrawStyle |= DT_VCENTER;
	}

	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkColor(hdc, GetSysColor(COLOR_3DFACE));

	if (hFont == NULL)
	{
		LOGFONT lf;

		GetObject(
			(HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0),
			sizeof(LOGFONT),
			&lf);

		lf.lfUnderline = TRUE;

		hFont = CreateFontIndirect(&lf);
	}

	hOriginal = SelectObject(hdc, hFont);

	GetClientRect(hWnd, &r);

	GetWindowText(hWnd, szBuffer, _ARRAYSIZE(szBuffer));
	DrawText(hdc, szBuffer, -1, &r, dwDrawStyle);

	SelectObject(hdc, hOriginal);

	EndPaint(hWnd, &ps);
}


static INT_PTR NewVersionForm_WmCommand(HWND hWnd, WORD wCommand, WORD wNotifyMessage)
{
	switch (wCommand)
	{
	case IDOK:
	case IDCANCEL:
		EndDialog(hWnd, wCommand);
		return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}
