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

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL (WINAPI * WTSRegisterSessionNotification_t)(HWND, DWORD);
typedef BOOL (WINAPI * WTSUnRegisterSessionNotification_t)(HWND);

BOOL WINAPI Compat_WTSRegisterSessionNotification(HWND hWnd, DWORD dwFlags)
{
	ASSERT(hWnd != NULL);

	static BOOL fLoaded = FALSE;
	static WTSRegisterSessionNotification_t lpProc = NULL;

	if (!fLoaded)
	{
		fLoaded = TRUE;

		HMODULE hModule = LoadLibrary(L"wtsapi32.dll");

		if (hModule != NULL)
		{
			lpProc = (WTSRegisterSessionNotification_t)GetProcAddress(
				hModule, "WTSRegisterSessionNotification");
		}
	}

	if (lpProc == NULL)
	{
		return FALSE;
	}
	else
	{
		return lpProc(hWnd, dwFlags);
	}
}

BOOL WINAPI Compat_WTSUnRegisterSessionNotification(HWND hWnd)
{
	ASSERT(hWnd != NULL);

	static BOOL fLoaded = FALSE;
	static WTSUnRegisterSessionNotification_t lpProc = NULL;

	if (!fLoaded)
	{
		fLoaded = TRUE;

		HMODULE hModule = LoadLibrary(L"wtsapi32.dll");

		if (hModule != NULL)
		{
			lpProc = (WTSUnRegisterSessionNotification_t)GetProcAddress(
				hModule, "WTSUnRegisterSessionNotification");
		}
	}

	if (lpProc == NULL)
	{
		return FALSE;
	}
	else
	{
		return lpProc(hWnd);
	}
}

#ifdef __cplusplus
};
#endif
