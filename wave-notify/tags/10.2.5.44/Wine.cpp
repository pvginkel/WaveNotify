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

typedef const char * (* wine_get_version_t)();

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

wstring GetWineVersion()
{
	static wstring szWineVersion(L"");
	static BOOL fInitialised = FALSE;
	
	if (!fInitialised)
	{
		HMODULE hModule;

		hModule = LoadLibrary(L"ntdll.dll");

		if (hModule != NULL)
		{
			wine_get_version_t lpProc =
				(wine_get_version_t)GetProcAddress(hModule, "wine_get_version");

			szWineVersion = ConvertToWideChar(lpProc());

			FreeLibrary(hModule);
		}

		fInitialised = TRUE;
	}

	return szWineVersion;
}
