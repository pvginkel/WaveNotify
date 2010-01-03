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

unzFile unzOpenW(wstring szFile)
{
	unzFile lpResult;

	lpResult = unzOpen(ConvertToMultiByte(szFile).c_str());

	return lpResult;
}

wstring unzGetCurrentFileNameW(unzFile lpZip)
{
	CHAR szFilenameA[MAX_PATH];
	INT nResult;

	nResult = unzGetCurrentFileInfo(
		lpZip,
		NULL,
		szFilenameA, _ARRAYSIZE(szFilenameA),
		NULL, 0,
		NULL, 0);

	if (nResult == UNZ_OK)
	{
		return ConvertToWideChar(szFilenameA);
	}
	else
	{
		return L"";
	}
}
