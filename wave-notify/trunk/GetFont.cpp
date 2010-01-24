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
#include "settings.h"

#define GENERIC_UNICODE_FONT		L"Microsoft Sans Serif"
#define WINE_GENERIC_UNICODE_FONT	L"DejaVu Sans"

static HFONT GetGenericUnicodeFont();
static int CALLBACK FontExistsProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam);

HFONT GetMessageBoxFont(BOOL fReload)
{
	static HFONT hFont = NULL;

	if (fReload && hFont != NULL)
	{
		DeleteObject(hFont);

		hFont = NULL;
	}

	if (hFont == NULL)
	{
		if (GetWindowsVersion() <= WV_WINDOWS_XP)
		{
			BOOL fValue;

			if (CSettings(FALSE).GetUseGenericUnicodeFont(fValue) && fValue)
			{
				hFont = GetGenericUnicodeFont();
			}
		}

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
				hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			}
		}
	}

	CHECK_HANDLE(hFont);

	return hFont;
}

HFONT CreateFontIndirectEx(HFONT hFont, LONG lWeight, BOOL fItalic, BOOL fUnderline)
{
	ASSERT(hFont != NULL);

	LOGFONT lf;

	GetObject(hFont, sizeof(LOGFONT), &lf);

	lf.lfWeight = lWeight;
	lf.lfItalic = fItalic;
	lf.lfUnderline = fUnderline;

	return CreateFontIndirect(&lf);
}

static HFONT GetGenericUnicodeFont()
{
	LPCWSTR szFont = RunningWine() ? WINE_GENERIC_UNICODE_FONT : GENERIC_UNICODE_FONT;

	if (FontExists(szFont))
	{
		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		LOGFONT lf;

		GetObject(hFont, sizeof(LOGFONT), &lf);

		DeleteObject(hFont);

		wcscpy(lf.lfFaceName, szFont);

		return CreateFontIndirect(&lf);
	}

	return NULL;
}

BOOL FontExists(wstring szFontName)
{
	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfCharSet = DEFAULT_CHARSET;
	wcscpy(lf.lfFaceName, szFontName.c_str());

	HDC hDC = GetDC(NULL);

	BOOL fExists = FALSE;

	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)FontExistsProc, (LPARAM)&fExists, 0);

	ReleaseDC(NULL, hDC);

	return fExists;
}

static int CALLBACK FontExistsProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
	ASSERT(lParam != 0);

	*((LPBOOL)lParam) = TRUE;

	return 0;
}
