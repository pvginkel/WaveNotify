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

wstring Format(wstring szFormat, ...)
{
	INT nLength = 400;
	LPWSTR szBuffer = (LPWSTR)malloc(sizeof(WCHAR) * nLength);
	va_list argptr;

	va_start(argptr, szFormat);

	for (;;)
	{
		HRESULT hr = StringCchVPrintf(szBuffer, nLength - 1, szFormat.c_str(), argptr);

		if (SUCCEEDED(hr))
		{
			break;
		}
		else if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
		{
			nLength *= 2;
			szBuffer = (LPWSTR)realloc(szBuffer, nLength * sizeof(WCHAR));
		}
		else
		{
			*szBuffer = L'\0';
			break;
		}
	}

	va_end(argptr);

	wstring szResult(szBuffer);

	free(szBuffer);

	return szResult;
}
