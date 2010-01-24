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

#define PROPERTY_INSTANCE	L"WAVENOTIFY_WINDOW_INSTANCE"

CWindow::CWindow(wstring szClassName) :
	m_nRef(1),
	m_szClassName(szClassName),
	m_fClassRegistered(FALSE),
	m_hCursor(NULL),
	m_fHandleCreated(FALSE)
{
}

CWindow::~CWindow()
{
	::RemoveProp(GetHandle(), PROPERTY_INSTANCE);

	SetHandle(NULL);
}

ATOM CWindow::CreateClass(LPWNDCLASSEX lpWndClass)
{
	ASSERT(lpWndClass != NULL);

	// These cannot be overridden

	lpWndClass->cbSize = sizeof(WNDCLASSEX);
	lpWndClass->lpszClassName = m_szClassName.c_str();
	lpWndClass->lpfnWndProc = CWindow::WndProcCallback;
	lpWndClass->hInstance = CApp::Instance()->GetInstance();

	return RegisterClassEx(lpWndClass);
}

wstring CWindow::GetClassName()
{
	if (!m_fClassRegistered)
	{
		WNDCLASSEX wcex;

		memset(&wcex, 0, sizeof(WNDCLASSEX));

		wcex.cbSize = sizeof(WNDCLASSEX);

		m_fClassRegistered = GetClassInfoEx(CApp::Instance()->GetInstance(), m_szClassName.c_str(), &wcex);
	}

	if (!m_fClassRegistered)
	{
		WNDCLASSEX wcex;

		memset(&wcex, 0, sizeof(WNDCLASSEX));

		ATOM nResult = CreateClass(&wcex);

		ASSERT(nResult != 0);

		m_fClassRegistered = TRUE;
	}

	return m_szClassName;
}

HWND CWindow::CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu)
{
	return CreateWindowEx(
		dwExStyle,
		GetClassName().c_str(),
		szWindowName.c_str(),
		dwStyle,
		x, y, cx, cy,
		lpParentWindow == NULL ? NULL : lpParentWindow->GetHandle(),
		hMenu,
		CApp::Instance()->GetInstance(),
		this);
}

LRESULT CWindow::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	if (m_hCursor != NULL && uMessage == WM_SETCURSOR)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return DefWindowProc(GetHandle(), uMessage, wParam, lParam);
}

BOOL CWindow::Create()
{
	ASSERT(!m_fHandleCreated);

	m_fHandleCreated = TRUE;

	CreateHandle(0, L"", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL);

	return GetHandle() != NULL && IsWindow();
}

LRESULT CALLBACK CWindow::WndProcCallback(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CWindow * lpWindow;

	if (uMessage == WM_CREATE)
	{
		LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;

		ASSERT(lpCreateStruct != NULL && lpCreateStruct->lpCreateParams != NULL);
		
		::SetProp(hWnd, PROPERTY_INSTANCE, (HANDLE)lpCreateStruct->lpCreateParams);

		lpWindow = (CWindow *)lpCreateStruct->lpCreateParams;

		lpWindow->SetHandle(hWnd);
	}
	else
	{
		lpWindow = (CWindow *)::GetProp(hWnd, PROPERTY_INSTANCE);
	}

	if (lpWindow != NULL)
	{
		lpWindow->AddRef();

		lpWindow->PreviewWndProc(uMessage, wParam, lParam);

		LRESULT lResult = lpWindow->WndProc(uMessage, wParam, lParam);

		if (uMessage == WM_DESTROY)
		{
			lpWindow->Release();
		}

		lpWindow->Release();

		return lResult;
	}
	else
	{
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
}
