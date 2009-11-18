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

#ifndef _INC_TRAYICON
#define _INC_TRAYICON

#pragma once

#define WM_NOTIFTICON	(WM_USER + 1)

typedef struct tagTRAYICON
{
	UINT uID;
	HWND hWnd;
	WCHAR szTooltip[128];
	HICON hIcon;
} NOTIFYICON, * LPNOTIFYICON;

LPNOTIFYICON NotifyIcon_Create(HWND hWnd, UINT uID, HICON hIcon, LPWSTR szTooltip);
void NotifyIcon_SetIcon(LPNOTIFYICON lpNotifyIcon, HICON hIcon);
void NotifyIcon_SetTooltip(LPNOTIFYICON lpNotifyIcon, LPWSTR szTooltip);
void NotifyIcon_Destroy(LPNOTIFYICON lpNotifyIcon);
void NotifyIcon_Recreate(LPNOTIFYICON lpNotifyIcon);

#endif // _INC_TRAYICON
