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

static void GetTaskbarLocationForWine(LPTASKBARLOCATION lpLocation);
static void GetTaskbarLocationForWindows(LPTASKBARLOCATION lpLocation);

LPSTR dupwcstombs(LPCWSTR szSource)
{
	size_t uLength;
	char * szTarget;
	errno_t uError;

	uError = wcstombs_s(&uLength, NULL, 0, szSource, 0);

	ASSERT(uError == 0);
	ASSERT(uLength != -1);

	szTarget = (LPSTR)ALLOC(uLength + 1);

	uError = wcstombs_s(&uLength, szTarget, uLength + 1, szSource, uLength + 1);

	ASSERT(uError == 0);

	return szTarget;
}

LPWSTR dupmbstowcs(LPCSTR szSource)
{
	size_t uLength;
	LPWSTR szTarget;
	errno_t uError;

	uError = mbstowcs_s(&uLength, NULL, 0, szSource, 0);

	ASSERT(uError == 0);
	ASSERT(uLength != -1);

	szTarget = (LPWSTR)ALLOC((uLength + 1) * sizeof(WCHAR));

	uError = mbstowcs_s(&uLength, szTarget, uLength + 1, szSource, uLength + 1);

	ASSERT(uError == 0);

	return szTarget;
}

LPWSTR duputf8towcs(LPCSTR szSource)
{
	return duputf8towcsn(szSource, strlen(szSource));
}

LPWSTR duputf8towcsn(LPCSTR szSource, size_t uMaxLength)
{
	size_t uLength;
	LPWSTR szTarget;

	uLength = MultiByteToWideChar(CP_UTF8, 0, szSource, uMaxLength, NULL, 0);

	ASSERT(uLength != -1);

	szTarget = (LPWSTR)ALLOC((uLength + 1) * sizeof(WCHAR));

	MultiByteToWideChar(CP_UTF8, 0, szSource, uMaxLength, szTarget, uLength + 1);

	szTarget[uLength] = L'\0';

	return szTarget;
}

#ifdef _DEBUG

void * dbg_safe_malloc(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)
{
	void * _Result;

	_Result = _malloc_dbg(_Size, _BlockType, _Filename, _LineNumber);

	ASSERT(_Result != NULL);

	return _Result;
}

void * dbg_safe_malloc_zero(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)
{
	void * _Result;

	_Result = dbg_safe_malloc(_Size, _BlockType, _Filename, _LineNumber);

	ZeroMemory(_Result, _Size);

	return _Result;
}

void * dbg_safe_realloc(void * _Memory, size_t _NewSize, int _BlockType, const char * _Filename, int _LineNumber)
{
	void * _Result;

	_Result = _realloc_dbg(_Memory, _NewSize, _BlockType, _Filename, _LineNumber);

	ASSERT(_Result != NULL);

	return _Result;
}

void dbg_safe_free(void * _Memory, int _BlockType)
{
	// No extra checks needed here (we let the hash check functions
	// worry about NULL pointers).

	_free_dbg(_Memory, _BlockType);
}

#else // !_DEBUG

void * safe_malloc(size_t _Size)
{
	void * _Result;

	_Result = malloc(_Size);

	ASSERT(_Result != NULL);

	return _Result;
}

void * safe_malloc_zero(size_t _Size)
{
	void * _Result;

	_Result = safe_malloc(_Size);

	ZeroMemory(_Result, _Size);

	return _Result;
}

void * safe_realloc(void * _Memory, size_t _NewSize)
{
	void * _Result;

	_Result = realloc(_Memory, _NewSize);

	ASSERT(_Result != NULL);

	return _Result;
}

void safe_free(void * _Memory)
{
	free(_Memory);
}

#endif // _DEBUG

HFONT GetMessageBoxFont()
{
	static HFONT hFont = NULL;

	if (hFont == NULL)
	{
		NONCLIENTMETRICS ncm;
		BOOL fResult;

		memset(&ncm, 0, sizeof(NONCLIENTMETRICS));

		ncm.cbSize = sizeof(NONCLIENTMETRICS);

		fResult = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);

		if (fResult)
		{
			hFont = CreateFontIndirect(&ncm.lfMessageFont);
		}
		else
		{
			hFont = GetStockFont(DEFAULT_GUI_FONT);
		}
	}

	return hFont;
}

HFONT GetBoldFont(HFONT hFont)
{
	LOGFONT lf;

	GetObject(hFont, sizeof(LOGFONT), &lf);

	lf.lfWeight = FW_BOLD;

	return CreateFontIndirect(&lf);
}

void GetTaskbarLocation(LPTASKBARLOCATION lpLocation)
{
	POINT pBar;

	memset(lpLocation, 0, sizeof(TASKBARLOCATION));

	if (RunningWine())
	{
		GetTaskbarLocationForWine(lpLocation);
	}
	else
	{
		GetTaskbarLocationForWindows(lpLocation);
	}

	pBar.x = lpLocation->rTaskbar.left + (lpLocation->rTaskbar.right - lpLocation->rTaskbar.left) / 2;
	pBar.y = lpLocation->rTaskbar.top + (lpLocation->rTaskbar.bottom - lpLocation->rTaskbar.top) / 2;

	if (pBar.x > lpLocation->rWorkArea.right)
	{
		lpLocation->uEdge = TLE_RIGHT;
	}
	else if (pBar.x < lpLocation->rWorkArea.left)
	{
		lpLocation->uEdge = TLE_LEFT;
	}
	else if (pBar.y > lpLocation->rWorkArea.bottom)
	{
		lpLocation->uEdge = TLE_BOTTOM;
	}
	else if (pBar.y < lpLocation->rWorkArea.top)
	{
		lpLocation->uEdge = TLE_TOP;
	}
	else
	{
		lpLocation->uEdge = TLE_BOTTOM;
	}
}

BOOL GetTaskbarPopupLocation(SIZE sSize, SIZE sOffset, LPRECT lpLocation)
{
	TASKBARLOCATION tl;

	GetTaskbarLocation(&tl);

	*lpLocation = tl.rWorkArea;

	switch (tl.uEdge)
	{
	case TLE_LEFT:
		lpLocation->left += sOffset.cx;
		lpLocation->right = lpLocation->left + sSize.cx;
		lpLocation->bottom -= sOffset.cy;
		lpLocation->top = lpLocation->bottom - sSize.cy;
		break;

	case TLE_RIGHT:
		lpLocation->right -= sOffset.cx;
		lpLocation->left = lpLocation->right - sSize.cx;
		lpLocation->bottom -= sOffset.cy;
		lpLocation->top = lpLocation->bottom - sSize.cy;
		break;

	case TLE_TOP:
		lpLocation->right -= sOffset.cx;
		lpLocation->left = lpLocation->right - sSize.cx;
		lpLocation->top  += sOffset.cy;
		lpLocation->bottom = lpLocation->top + sSize.cy;
		break;

	case TLE_BOTTOM:
		lpLocation->right -= sOffset.cx;
		lpLocation->left = lpLocation->right - sSize.cx;
		lpLocation->bottom -= sOffset.cy;
		lpLocation->top = lpLocation->bottom - sSize.cy;
		break;

	default:
		memset(lpLocation, 0, sizeof(RECT));
		return FALSE;
	}

	return TRUE;
}

void SnapTaskbarPopupLocation(LPRECT lpLocation, POINT p)
{
	TASKBARLOCATION tl;
	RECT r;
	SIZE s;

	GetTaskbarLocation(&tl);

	if (!PtInRect(&tl.rTaskbar, p))
	{
		return;
	}

	r = *lpLocation;
	s.cx = lpLocation->right - lpLocation->left;
	s.cy = lpLocation->bottom - lpLocation->top;

	if (tl.uEdge == TLE_TOP || tl.uEdge == TLE_BOTTOM)
	{
		r.left = p.x - s.cx / 2;
		r.right = r.left + s.cx;

		if (p.x > tl.rWorkArea.left + (tl.rWorkArea.right - tl.rWorkArea.left) / 2)
		{
			if (r.right > lpLocation->right)
			{
				return;
			}
		}
		else
		{
			if (r.left < lpLocation->left)
			{
				return;
			}
		}
	}
	else
	{
		r.top = p.y - s.cy / 2;
		r.bottom = r.top + s.cy;

		if (p.y > tl.rWorkArea.top + (tl.rWorkArea.bottom - tl.rWorkArea.top) / 2)
		{
			if (r.bottom > lpLocation->bottom)
			{
				return;
			}
		}
		else
		{
			if (r.top < lpLocation->top)
			{
				return;
			}
		}
	}
	
	*lpLocation = r;
}

BOOL RunningWine()
{
	static BOOL fInitialised = FALSE;
	static BOOL fRunningWine;

	if (!fInitialised)
	{
		HMODULE hModule;

		fRunningWine = FALSE;

		hModule = LoadLibrary(L"ntdll.dll");

		if (hModule != NULL)
		{
			fRunningWine =
				GetProcAddress(hModule, "wine_get_version") != NULL ||
				GetProcAddress(hModule, "wine_nt_to_unix_file_name") != NULL;

			FreeLibrary(hModule);
		}

		fInitialised = TRUE;
	}

	return fRunningWine;
}

BOOL RunningRemoteDesktop()
{
	return (GetSystemMetrics(SM_REMOTESESSION) & 1) != 0;
}

BOOL DegradeVisualPerformance()
{
#if defined(_DEBUG) && defined(FORCE_ANIMATIONS)
	return FALSE;
#else
	return RunningWine() || RunningRemoteDesktop();
#endif
}

void OpenUrl(LPWSTR szUrl)
{
	if (!RunningWine())
	{
		ShellExecute(NULL, L"open", szUrl, NULL, NULL, SW_SHOW);
	}
	else
	{
		WCHAR szPath[MAX_PATH];
		LPWSTR szCommandLine;
		size_t nLength;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, szPath);

		nLength = wcslen(szPath);

		wcscpy_s(szPath + nLength, _ARRAYSIZE(szPath) - nLength, L"\\winebrowser.exe");

		nLength = wcslen(szPath) + wcslen(szUrl) + 6;
		szCommandLine = (LPWSTR)ALLOC(nLength * sizeof(WCHAR));

		StringCbPrintf(szCommandLine, nLength * 2, L"\"%s\" \"%s\"", szPath, szUrl);

		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));

		si.cb = sizeof(PROCESS_INFORMATION); 

		CreateProcess(
			szPath,
			szCommandLine,
			NULL,
			NULL,
			FALSE,
			CREATE_DEFAULT_ERROR_MODE,
			NULL,
			NULL,
			&si,
			&pi);

		FREE(szCommandLine);
	}
}

static void GetTaskbarLocationForWine(LPTASKBARLOCATION lpLocation)
{
	HMONITOR hMonitor;
	MONITORINFOEX miex;
	POINT p;

	p.x = 0;
	p.y = 0;

	hMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);

	memset(&miex, 0, sizeof(MONITORINFOEX));

	miex.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfo(hMonitor, (LPMONITORINFO)&miex);

	if (
		miex.rcMonitor.left == miex.rcWork.left &&
		miex.rcMonitor.top == miex.rcWork.top &&
		miex.rcMonitor.right == miex.rcWork.right &&
		miex.rcMonitor.bottom == miex.rcWork.bottom
	) {
		lpLocation->rTaskbar = miex.rcMonitor;
		lpLocation->rWorkArea = miex.rcMonitor;

		lpLocation->rTaskbar.bottom = lpLocation->rTaskbar.top + 25;
		lpLocation->rWorkArea.top = lpLocation->rTaskbar.bottom;
	}
	else
	{
		lpLocation->rWorkArea = miex.rcWork;

		// Detect where the taskbar is located

		lpLocation->rTaskbar = miex.rcMonitor;

		if (miex.rcWork.top > miex.rcMonitor.top)
		{
			lpLocation->rTaskbar.bottom = miex.rcWork.top;
		}
		else if (miex.rcWork.bottom < miex.rcMonitor.bottom)
		{
			lpLocation->rTaskbar.top = miex.rcWork.bottom;
		}
		else if (miex.rcWork.left > miex.rcMonitor.left)
		{
			lpLocation->rTaskbar.right = miex.rcWork.left;
		}
		else // if (miex.rcWork.right < miex.rcMonitor.right)
		{
			lpLocation->rTaskbar.left = miex.rcWork.right;
		}
	}
}

static void GetTaskbarLocationForWindows(LPTASKBARLOCATION lpLocation)
{
	APPBARDATA abd;
	HMONITOR hMonitor;
	MONITORINFOEX miex;
	POINT p;

	memset(&abd, 0, sizeof(APPBARDATA));

	abd.cbSize = sizeof(APPBARDATA);

	SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
	
	lpLocation->rTaskbar = abd.rc;

	p.x = abd.rc.left;
	p.y = abd.rc.top;

	hMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);

	memset(&miex, 0, sizeof(MONITORINFOEX));

	miex.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfo(hMonitor, (LPMONITORINFO)&miex);

	lpLocation->rWorkArea = miex.rcWork;
}
