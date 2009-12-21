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

void OpenUrl(wstring szUrl)
{
	if (!RunningWine())
	{
		ShellExecute(NULL, L"open", szUrl.c_str(), NULL, NULL, SW_SHOW);
	}
	else
	{
		WCHAR szSystemPath[MAX_PATH];

		SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, szSystemPath);

		wstring szPath(szSystemPath);
		
		szPath += L"\\winebrowser.exe";

		wstring szCommandLine(Format(L"\"%s\" \"%s\"", szPath.c_str(), szUrl.c_str()));

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));

		si.cb = sizeof(PROCESS_INFORMATION);

		LPWSTR szMutableCommandLine = (LPWSTR)malloc(sizeof(WCHAR) * (szCommandLine.size() + 1));

		wcscpy(szMutableCommandLine, szCommandLine.c_str());

		CreateProcess(
			szPath.c_str(),
			szMutableCommandLine,
			NULL,
			NULL,
			FALSE,
			CREATE_DEFAULT_ERROR_MODE,
			NULL,
			NULL,
			&si,
			&pi);

		free(szMutableCommandLine);
	}
}
