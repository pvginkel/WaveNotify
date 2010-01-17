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
