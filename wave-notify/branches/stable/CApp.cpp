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

CApp * CApp::m_lpInstance = NULL;

CApp::CApp(HINSTANCE hInstance, wstring szCmdLine)
{
	ASSERT(hInstance != NULL && m_lpInstance == NULL);

	m_lpInstance = this;
	m_hInstance = hInstance;
	m_szCmdLine = szCmdLine;
}

CApp::~CApp()
{
	m_lpInstance = NULL;
}

BOOL CApp::Initialise()
{
	return TRUE;
}

INT CApp::Execute()
{
	return MessageLoop();
}

INT CApp::MessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (
			!CModelessDialogs::IsDialogMessage(&msg) &&
			!CModelessPropertySheets::IsDialogMessage(&msg)
		) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

INT CApp::DoEvents()
{
	// TODO: This is not fully tested; there may be problems with this code.

	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (
			!CModelessDialogs::IsDialogMessage(&msg) &&
			!CModelessPropertySheets::IsDialogMessage(&msg)
		) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}
