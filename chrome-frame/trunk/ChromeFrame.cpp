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

extern "C" LPCHROMEFRAME chromeframe_create(HWND parent, LPRECT rect, LPCWSTR url)
{
	if (parent == NULL || rect == NULL || url == NULL)
	{
		return NULL;
	}

	return (LPCHROMEFRAME)new CChromeFrame(parent, rect, url);
}

extern "C" BOOL chromeframe_set_url_callback(LPCHROMEFRAME handle, URLCALLBACK callback)
{
	if (handle == NULL || callback == NULL)
	{
		return FALSE;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	lpControl->SetUrlCallback(callback);

	return TRUE;
}

extern "C" BOOL chromeframe_set_navigated_callback(LPCHROMEFRAME handle, NAVIGATEDCALLBACK callback)
{
	if (handle == NULL || callback == NULL)
	{
		return FALSE;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	lpControl->SetNavigatedCallback(callback);

	return TRUE;
}

extern "C" BOOL chromeframe_add_extension(LPCHROMEFRAME handle, LPCWSTR extension_path)
{
	if (handle == NULL || extension_path == NULL)
	{
		return FALSE;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	lpControl->AddExtension(extension_path);

	return TRUE;
}

extern "C" HWND chromeframe_create_handle(LPCHROMEFRAME handle)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	return lpControl->Create();
}

extern "C" BOOL chromeframe_destroy(LPCHROMEFRAME handle)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	//DestroyWindow(lpControl->GetHandle());

	delete lpControl;

	return TRUE;
}

extern "C" HWND chromeframe_get_handle(LPCHROMEFRAME handle)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	return lpControl->GetHandle();
}

extern "C" BOOL chromeframe_available()
{
	HKEY hKey;
	BOOL fResult = FALSE;

	HRESULT hr = RegOpenKey(HKEY_CLASSES_ROOT, CHROME_FRAME_PROGID, &hKey);

	if (hr == ERROR_SUCCESS)
	{
		fResult = TRUE;

		RegCloseKey(hKey);
	}

	return fResult;
}

extern "C" BOOL chromeframe_set_param(LPCHROMEFRAME handle, LPVOID param)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	lpControl->SetParam(param);

	return TRUE;
}

extern "C" LPVOID chromeframe_get_param(LPCHROMEFRAME handle)
{
	if (handle == NULL)
	{
		return NULL;
	}

	CChromeFrame * lpControl = (CChromeFrame*)handle;

	return lpControl->GetParam();
}
