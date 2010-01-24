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

#define PROPERTY_INSTANCE	L"WAVENOTIFY_PROPERTYSHEET_INSTANCE"

CPropertySheetPage::CPropertySheetPage()
{
	m_dwFlags = 0;
	m_uResource = 0;
	m_szTitle = L"";
}

INT_PTR CPropertySheetPage::DialogProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_NOTIFY:
		ASSERT(lParam != 0);

		switch (((LPNMHDR)lParam)->code)
		{
		case PSN_SETACTIVE:
			return OnFocus();

		case PSN_APPLY:
			return OnApply();

		case PSN_HELP:
			return OnHelp();

		case PSN_KILLACTIVE:
			return OnLeave();

		case PSN_QUERYCANCEL:
			return OnQueryCancel();

		case PSN_QUERYINITIALFOCUS:
			return OnQueryInitialFocus();

		case PSN_RESET:
			return OnReset();
		}
		break;

	case WM_COMMAND:
		return OnCommand(LOWORD(wParam), HIWORD(wParam));
	}

	return FALSE;
}

INT_PTR CALLBACK CPropertySheetPage::DialogProcCallback(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CPropertySheetPage * lpPage;

	if (uMessage == WM_INITDIALOG)
	{
		LPPROPSHEETPAGE lpPropSheetPage = (LPPROPSHEETPAGE)lParam;

		ASSERT(lpPropSheetPage != NULL);

		lpPage = (CPropertySheetPage *)lpPropSheetPage->lParam;

		ASSERT(lpPage != NULL);

		::SetProp(hWnd, PROPERTY_INSTANCE, (HANDLE)lpPropSheetPage->lParam);

		lpPage->SetHandle(hWnd);
	}
	else
	{
		lpPage = (CPropertySheetPage *)::GetProp(hWnd, PROPERTY_INSTANCE);
	}

	if (lpPage != NULL)
	{
		INT_PTR nResult = lpPage->DialogProc(uMessage, wParam, lParam);

		if (uMessage == WM_DESTROY)
		{
			::RemoveProp(hWnd, PROPERTY_INSTANCE);
		}

		return nResult;
	}
	else
	{
		return FALSE;
	}
}

void CPropertySheetPage::InitializeStructure(LPPROPSHEETPAGE lpPage)
{
	ASSERT(lpPage != NULL);

	memset(lpPage, 0, sizeof(PROPSHEETPAGE));

	lpPage->dwSize = sizeof(PROPSHEETPAGE);
	lpPage->dwFlags = m_dwFlags | PSP_USETITLE;
	lpPage->hInstance = CApp::Instance()->GetInstance();
	lpPage->pszTemplate = MAKEINTRESOURCE(m_uResource);
	lpPage->pfnDlgProc = CPropertySheetPage::DialogProcCallback;
	lpPage->lParam = (LPARAM)this;
	lpPage->pszTitle = m_szTitle.c_str();
}
