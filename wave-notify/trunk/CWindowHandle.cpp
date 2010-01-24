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

wstring CWindowHandle::GetDlgItemText(INT nDlgItem) const
{
	HWND hWnd = GetDlgItem(nDlgItem);

	INT nLength = GetWindowTextLength(hWnd);

	if (nLength > 0)
	{
		LPWSTR szBuffer = (LPWSTR)malloc((nLength + 1) * sizeof(WCHAR));

		::GetWindowText(hWnd, szBuffer, nLength + 1);

		wstring szResult(szBuffer);

		free(szBuffer);

		return szResult;
	}
	else
	{
		return L"";
	}
}

wstring CWindowHandle::GetWindowText() const
{
	INT nLength = GetWindowTextLength(GetHandle());

	if (nLength > 0)
	{
		LPWSTR szBuffer = (LPWSTR)malloc((nLength + 1) * sizeof(WCHAR));

		::GetWindowText(GetHandle(), szBuffer, nLength + 1);

		wstring szResult(szBuffer);

		free(szBuffer);

		return szResult;
	}
	else
	{
		return L"";
	}
}
