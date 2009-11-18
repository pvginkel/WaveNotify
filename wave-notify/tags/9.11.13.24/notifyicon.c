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

static void NotifyIcon_PopulateData(PNOTIFYICONDATA lpnid, LPNOTIFYICON lpNotifyIcon, HICON hIcon, LPWSTR szTooltip);

LPNOTIFYICON NotifyIcon_Create(HWND hWnd, UINT uID, HICON hIcon, LPWSTR szTooltip)
{
	LPNOTIFYICON lpNotifyIcon = ALLOCA(NOTIFYICON);
	NOTIFYICONDATA nid;

	lpNotifyIcon->hWnd = hWnd;
	lpNotifyIcon->uID = uID;
	lpNotifyIcon->hIcon = hIcon;

	if (szTooltip == NULL)
	{
		lpNotifyIcon->szTooltip[0] = L'\0';
	}
	else
	{
		wcsncpy_s(lpNotifyIcon->szTooltip, _ARRAYSIZE(lpNotifyIcon->szTooltip), szTooltip, _ARRAYSIZE(lpNotifyIcon->szTooltip));
		lpNotifyIcon->szTooltip[_ARRAYSIZE(lpNotifyIcon->szTooltip) - 1] = L'\0';
	}

	NotifyIcon_PopulateData(&nid, lpNotifyIcon, hIcon, szTooltip);

	Shell_NotifyIcon(NIM_ADD, &nid);

	return lpNotifyIcon;
}

void NotifyIcon_SetIcon(LPNOTIFYICON lpNotifyIcon, HICON hIcon)
{
	NOTIFYICONDATA nid;

	lpNotifyIcon->hIcon = hIcon;

	NotifyIcon_PopulateData(&nid, lpNotifyIcon, hIcon, NULL);

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void NotifyIcon_SetTooltip(LPNOTIFYICON lpNotifyIcon, LPWSTR szTooltip)
{
	NOTIFYICONDATA nid;

	if (szTooltip == NULL)
	{
		lpNotifyIcon->szTooltip[0] = L'\0';
	}
	else
	{
		wcsncpy_s(lpNotifyIcon->szTooltip, _ARRAYSIZE(lpNotifyIcon->szTooltip), szTooltip, _ARRAYSIZE(lpNotifyIcon->szTooltip));
		lpNotifyIcon->szTooltip[_ARRAYSIZE(lpNotifyIcon->szTooltip) - 1] = L'\0';
	}

	NotifyIcon_PopulateData(&nid, lpNotifyIcon, NULL, szTooltip);

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void NotifyIcon_Destroy(LPNOTIFYICON lpNotifyIcon)
{
	NOTIFYICONDATA nid;

	NotifyIcon_PopulateData(&nid, lpNotifyIcon, NULL, NULL);

	Shell_NotifyIcon(NIM_DELETE, &nid);

	FREE(lpNotifyIcon);
}

static void NotifyIcon_PopulateData(PNOTIFYICONDATA lpnid, LPNOTIFYICON lpNotifyIcon, HICON hIcon, LPWSTR szTooltip)
{
	memset(lpnid, 0, sizeof(NOTIFYICONDATA));

	lpnid->cbSize = sizeof(NOTIFYICONDATA);
	lpnid->hWnd = lpNotifyIcon->hWnd;
	lpnid->uID = lpNotifyIcon->uID;
	lpnid->uFlags = NIF_MESSAGE;

	if (hIcon != NULL)
	{
		lpnid->uFlags |= NIF_ICON;
		lpnid->hIcon = hIcon;
	}

	lpnid->uCallbackMessage = WM_NOTIFTICON;

	if (szTooltip != NULL)
	{
		lpnid->uFlags |= NIF_TIP;
		wcsncpy_s(lpnid->szTip, _ARRAYSIZE(lpnid->szTip), szTooltip, _ARRAYSIZE(lpnid->szTip));
		lpnid->szTip[_ARRAYSIZE(lpnid->szTip) - 1] = L'\0';
	}

	lpnid->uVersion = NOTIFYICON_VERSION;
}

void NotifyIcon_Recreate(LPNOTIFYICON lpNotifyIcon)
{
	LPNOTIFYICON lpNewNotifyIcon;
	LPWSTR szTooltip;

	if (wcslen(lpNotifyIcon->szTooltip) == 0)
	{
		szTooltip = NULL;
	}
	else
	{
		szTooltip = lpNotifyIcon->szTooltip;
	}

	lpNewNotifyIcon = NotifyIcon_Create(lpNotifyIcon->hWnd, lpNotifyIcon->uID, lpNotifyIcon->hIcon, szTooltip);

	memcpy(lpNotifyIcon, lpNewNotifyIcon, sizeof(NOTIFYICON));
	
	FREE(lpNewNotifyIcon);
}
