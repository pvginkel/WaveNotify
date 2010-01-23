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

CNotifyIcon::CNotifyIcon(CWindow * lpWindow, UINT uId, wstring szTooltip, HICON hIcon)
{
	ASSERT(lpWindow != NULL);

	m_lpWindow = lpWindow;
	m_uId = uId;
	m_szTooltip = szTooltip;
	m_hIcon = hIcon;
}

CNotifyIcon::~CNotifyIcon()
{
	Destroy();
}

void CNotifyIcon::Create()
{
	NOTIFYICONDATA nid;

	PopulateData(&nid);

	BOOL fResult = Shell_NotifyIcon(NIM_ADD, &nid);

	CHECK(fResult);
}

void CNotifyIcon::Destroy()
{
	NOTIFYICONDATA nid;

	PopulateData(&nid);

	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void CNotifyIcon::Recreate()
{
	Create();
}

void CNotifyIcon::Update()
{
	NOTIFYICONDATA nid;

	PopulateData(&nid);

	BOOL fResult = Shell_NotifyIcon(NIM_MODIFY, &nid);

	CHECK(fResult);
}

void CNotifyIcon::PopulateData(PNOTIFYICONDATA lpnid)
{
	ASSERT(lpnid != NULL);

	memset(lpnid, 0, sizeof(NOTIFYICONDATA));

	lpnid->cbSize = sizeof(NOTIFYICONDATA);
	lpnid->hWnd = m_lpWindow->GetHandle();
	lpnid->uID = m_uId;
	lpnid->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	lpnid->hIcon = m_hIcon;
	lpnid->uCallbackMessage = WM_NOTIFTICON;
	wcsncpy(lpnid->szTip, m_szTooltip.c_str(), _ARRAYSIZE(lpnid->szTip));
	lpnid->szTip[_ARRAYSIZE(lpnid->szTip) - 1] = L'\0';
	lpnid->uVersion = NOTIFYICON_VERSION;
}
