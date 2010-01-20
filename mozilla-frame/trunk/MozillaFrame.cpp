#include "stdafx.h"
#include "include.h"

extern "C" LPMOZILLAFRAME mozillaframe_create(HWND parent, LPRECT rect)
{
	CMozillaFrame * lpFrame = new CMozillaFrame();

	BOOL fSuccess = lpFrame->Create(parent, rect);

	if (fSuccess)
	{
		return (LPMOZILLAFRAME)lpFrame;
	}
	else
	{
		delete lpFrame;

		return NULL;
	}
}

extern "C" BOOL mozillaframe_destroy(LPMOZILLAFRAME handle)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	CMozillaFrame * lpFrame = (CMozillaFrame *)handle;

	DestroyWindow(lpFrame->GetHandle());
	lpFrame->Destroy();

	delete lpFrame;

	return TRUE;
}

extern "C" HWND mozillaframe_get_handle(LPMOZILLAFRAME handle)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	return ((CMozillaFrame *)handle)->GetHandle();
}

extern "C" LPVOID mozillaframe_get_param(LPMOZILLAFRAME handle)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	return ((CMozillaFrame *)handle)->GetParam();
}

extern "C" BOOL mozillaframe_set_param(LPMOZILLAFRAME handle, LPVOID param)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	((CMozillaFrame *)handle)->SetParam(param);

	return TRUE;
}

extern "C" BOOL mozillaframe_set_before_navigate_callback(LPMOZILLAFRAME handle, MFBEFORENAVIGATE callback)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	((CMozillaFrame *)handle)->SetBeforeNavigateCallback(callback);

	return TRUE;
}

extern "C" BOOL mozillaframe_navigate(LPMOZILLAFRAME handle, LPCWSTR url)
{
	if (handle == NULL)
	{
		return FALSE;
	}

	((CMozillaFrame *)handle)->NavigateTo(url);

	return TRUE;
}
