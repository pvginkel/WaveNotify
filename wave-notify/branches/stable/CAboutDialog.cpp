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
#include "resource.h"
#include "notifierapp.h"
#include "aboutdialog.h"
#include "layout.h"

INT_PTR CAboutDialog::DialogProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_INITDIALOG:
		return OnInitDialog();

	case WM_COMMAND:
		return OnCommand(LOWORD(wParam), HIWORD(wParam));

	default:
		return CDialog::DialogProc(uMessage, wParam, lParam);
	}
}

INT_PTR CAboutDialog::OnInitDialog()
{
	SendMessage(WM_SETICON, ICON_BIG, (LPARAM)CNotifierApp::Instance()->GetMainIcon());
	SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)CNotifierApp::Instance()->GetMainIconSmall());

	static HICON hIcon = (HICON)LoadImage(
		CApp::Instance()->GetInstance(),
		MAKEINTRESOURCE(IDI_MAINICON),
		IMAGE_ICON, PL_ICON_SIZE, PL_ICON_SIZE, 0);

	static HFONT hBoldFont = CreateFontIndirectEx(
		(HFONT)SendDlgItemMessage(IDC_ABOUT_TITLE, WM_GETFONT),
		FW_BOLD, FALSE, FALSE);

	TStringStringMap vVersions;

	ExplodeVersions(vVersions, ConvertToWideChar(curl_version()));

	SendDlgItemMessage(IDC_ABOUT_ICON, STM_SETICON, (WPARAM)hIcon);

	SetDlgItemText(
		IDC_ABOUT_VERSION,
		Format(
			GetDlgItemText(IDC_ABOUT_VERSION),
			CVersion::GetAppVersion().c_str()
		)
	);

	SetDlgItemText(
		IDC_ABOUT_CURL_VERSION,
		Format(
			GetDlgItemText(IDC_ABOUT_CURL_VERSION),
			vVersions[L"libcurl"].c_str()
		)
	);

	SetDlgItemText(
		IDC_ABOUT_ZLIB_VERSION,
		Format(
			GetDlgItemText(IDC_ABOUT_ZLIB_VERSION),
			vVersions[L"zlib"].c_str()
		)
	);

	SetDlgItemText(
		IDC_ABOUT_OPENSSL_VERSION,
		Format(
			GetDlgItemText(IDC_ABOUT_OPENSSL_VERSION),
			vVersions[L"OpenSSL"].c_str()
		)
	);

	SetDlgItemText(
		IDC_ABOUT_GD_VERSION,
		Format(
			GetDlgItemText(IDC_ABOUT_GD_VERSION),
			ConvertToWideChar(GD_VERSION_STRING).c_str()
		)
	);

	SubclassStaticForLink(
		GetDlgItem(IDC_ABOUT_HOMEPAGE),
		HOMEPAGE_LINK
	);
	SubclassStaticForLink(GetDlgItem(IDC_ABOUT_LICENSE_LINK));

	SendDlgItemMessage(IDC_ABOUT_TITLE, WM_SETFONT, (WPARAM)hBoldFont);

	return TRUE;
}

INT_PTR CAboutDialog::OnCommand(WORD wCommand, WORD wNotifyMessage)
{
	switch (wCommand)
	{
	case IDOK:
	case IDCANCEL:
		DestroyWindow(GetHandle());
		break;
	}

	return TRUE;
}

void CAboutDialog::ExplodeVersions(TStringStringMap & vVersions, wstring szVersion)
{
	ASSERT(!szVersion.empty());

	UINT uOffset = 0;

	for (;;)
	{
		UINT uEnd = szVersion.find(L' ', uOffset);

		if (uEnd == wstring::npos)
		{
			uEnd = szVersion.size();
		}

		wstring szPart = szVersion.substr(uOffset, uEnd - uOffset);

		UINT uSeparator = szPart.find(L'/');

		ASSERT(uSeparator != szPart.npos);

		vVersions[szPart.substr(0, uSeparator)] = szPart.substr(uSeparator + 1);

		if (uEnd == szVersion.size())
		{
			break;
		}

		uOffset = uEnd + 1;
	}
}
