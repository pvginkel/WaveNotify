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
#include "notifierapp.h"

static LRESULT CALLBACK SubclassStaticForLink_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void SubclassStaticForLink_Paint(HWND hWnd);

class CUrlControlData
{
private:
	wstring m_szUrl;
	WNDPROC m_lpOldProc;

public:
	CUrlControlData(wstring szUrl, WNDPROC lpOldProc)
	{
		m_szUrl = szUrl;
		m_lpOldProc = lpOldProc;
	}

	wstring GetUrl() { return m_szUrl; }
	WNDPROC GetOldWndProc() { return m_lpOldProc; }
};

void SubclassStaticForLink(HWND hWnd, wstring szUrl)
{
	if (szUrl.empty())
	{
		INT nLength = GetWindowTextLength(hWnd) + 1;

		LPWSTR szBuffer = (LPWSTR)malloc(sizeof(WCHAR) * nLength);

		GetWindowText(hWnd, szBuffer, nLength);

		szUrl = szBuffer;

		free(szBuffer);
	}

	WNDPROC lpOldProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)SubclassStaticForLink_WndProc);

	CUrlControlData * lpData = new CUrlControlData(szUrl, lpOldProc);

	SetWindowLongPtr(hWnd, GWLP_USERDATA, (INT_PTR)lpData);
}

static LRESULT CALLBACK SubclassStaticForLink_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	
	CUrlControlData * lpData = (CUrlControlData *)GetWindowLong(hWnd, GWLP_USERDATA);
	WNDPROC lpOldWndProc = lpData->GetOldWndProc();

	switch(message)
	{
	case WM_NCDESTROY:
		SetWindowLong(hWnd, GWL_WNDPROC, (INT_PTR)lpOldWndProc);
		delete lpData;
		break;

	case WM_SETCURSOR:
		SetCursor(hCursor);
		return 1;

	case WM_PAINT:
		SubclassStaticForLink_Paint(hWnd);
		return 0;

	case WM_LBUTTONUP:
		CNotifierApp::Instance()->OpenUrl(lpData->GetUrl());
		PostMessage(GetParent(hWnd), WM_LINK_CLICKED, 0, (LPARAM)hWnd);
		return 0;

	case WM_NCHITTEST:
    		return HTCLIENT;
	}

	return CallWindowProc(lpOldWndProc, hWnd, message, wParam, lParam);
}

static void SubclassStaticForLink_Paint(HWND hWnd)
{
	PAINTSTRUCT ps;
	
	//
	// Determine the required characteristics for the text of the static control.
	//

	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	DWORD dwDrawStyle = DT_SINGLELINE;

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

	//
	// Create a new appropriate font.
	//

	static HFONT hFont = NULL;

	if (hFont == NULL)
	{
		hFont = CreateFontIndirectEx(
			(HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0),
			FW_NORMAL,
			FALSE,
			TRUE);
	}

	//
	// Draw the label.
	//

	CDC dc(BeginPaint(hWnd, &ps));

	dc.SetTextColor(RGB(0, 102, 204));
	dc.SetBkColor(GetSysColor(COLOR_3DFACE));

	RECT rc;

	GetClientRect(hWnd, &rc);

	INT nLength = GetWindowTextLength(hWnd) + 1;

	LPWSTR szBuffer = (LPWSTR)malloc(nLength * sizeof(WCHAR));

	GetWindowText(hWnd, szBuffer, nLength);

	HGDIOBJ hOriginal = dc.SelectObject(hFont);

	dc.DrawText(szBuffer, &rc, dwDrawStyle);

	free(szBuffer);

	dc.SelectObject(hOriginal);

	EndPaint(hWnd, &ps);
}
