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
#include "bootstrap.h"

INT CBootstrap::Execute(INT nCmdShow)
{
	if (SwitchToRunning())
	{
		return 0;
	}

	if (!InitializePath())
	{
		return 1;
	}

	if (!DetectModules())
	{
		return 2;
	}

	// We require a wave-notify module.

	TStringStringMapIter pos = m_vModules.find(L"wave-notify");

	if (pos == m_vModules.end())
	{
		return 3;
	}

	// We require the wave-notify.exe executable to exist in the root
	// of the wave-notify module.

	wstring szExecutable(m_szPath + pos->first + L"-" + pos->second + L"\\wave-notify.exe");

	DWORD dwAttr = GetFileAttributes(szExecutable.c_str());

	if (dwAttr == INVALID_FILE_ATTRIBUTES)
	{
		return 4;
	}

	UpdateEnvironment();

	if (!BootApplication(szExecutable, nCmdShow))
	{
		return 5;
	}

	return 0;
}

BOOL CBootstrap::SwitchToRunning()
{
	// Is the application already running?

	HANDLE hMutex = CreateMutex(NULL, FALSE, GLOBAL_MUTEX);

	if (hMutex != NULL && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);

		return FALSE;
	}

	// Find the main window of the notifier.

	HWND hWnd = FindWindow(WAVENOTIFY_WND_CLASS, NULL);

	if (hWnd != NULL)
	{
		SetForegroundWindow(hWnd);
	}

	// We do not differentiate our state based on whether we have found the
	// window because there is nothing to do. When the mutex exists,
	// the executable won't boot anyway so we can quit now.

	return TRUE;
}

BOOL CBootstrap::InitializePath()
{
	// We operate relative to the path the bootstrapper is located in,
	// unregarding of the current work directory of this instance.

	WCHAR szPath[MAX_PATH];

	DWORD dwResult = GetModuleFileName(NULL, szPath, _ARRAYSIZE(szPath));

	if (dwResult == 0 || GetLastError() != ERROR_SUCCESS)
	{
		return FALSE;
	}

	LPWSTR szPos = wcsrchr(szPath, L'\\');

	if (szPos == NULL)
	{
		return FALSE;
	}

	szPos[1] = L'\0';

	DWORD dwAttr = GetFileAttributes(szPath);

	if (dwAttr == INVALID_FILE_ATTRIBUTES || !(dwAttr | FILE_ATTRIBUTE_DIRECTORY))
	{
		return FALSE;
	}

	m_szPath = szPath;

	return TRUE;
}

BOOL CBootstrap::DetectModules()
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	wstring szSearch(m_szPath + L"*");

	hFind = FindFirstFile(szSearch.c_str(), &wfd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0)
			{
				continue;
			}

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				AddModulePath(wfd.cFileName);
			}
		}
		while (FindNextFile(hFind, &wfd));

		FindClose(hFind);
	}

	return TRUE;
}

void CBootstrap::AddModulePath(wstring szFilename)
{
	// Only add directories that look like a module (code and version
	// separated by a dash).

	DWORD nPos = szFilename.find_last_of(L'-');

	if (nPos != wstring::npos)
	{
		wstring szModule(szFilename.substr(0, nPos));
		wstring szVersion(szFilename.substr(nPos + 1));

		if (IsVersion(szVersion))
		{
			TStringStringMapIter pos = m_vModules.find(szModule);

			if (pos == m_vModules.end() || CompareVersion(pos->second, szVersion) > 0)
			{
				m_vModules[szModule] = szVersion;
			}
		}
	}
}

void CBootstrap::UpdateEnvironment()
{
	UpdatePath();
	RegisterModules();
}

void CBootstrap::UpdatePath()
{
	wstringstream szPath;

	// Put the wave-notify module in front.

	TStringStringMapIter pos = m_vModules.find(L"wave-notify");

	szPath << m_szPath << pos->first << L"-" << pos->second << L"\\;";

	// Put all other modules into the path.

	for (TStringStringMapIter iter = m_vModules.begin(); iter != m_vModules.end(); iter++)
	{
		if (iter->first != L"wave-notify")
		{
			szPath << m_szPath << iter->first << L"-" << iter->second << L"\\;";
		}
	}

	// Append the current path.

	DWORD dwSize = GetEnvironmentVariable(L"PATH", NULL, 0);

	if (dwSize != 0)
	{
		LPWSTR szEnv = (LPWSTR)malloc(sizeof(WCHAR) * (dwSize + 1));

		GetEnvironmentVariable(L"PATH", szEnv, dwSize + 1);

		szPath << szEnv;

		free(szEnv);
	}

	// And put back the path.

	SetEnvironmentVariable(L"PATH", szPath.str().c_str());
}

void CBootstrap::RegisterModules()
{
	// Create a list of available modules for the wave-notify.exe application.

	wstringstream szModules;

	TStringStringMapIter pos = m_vModules.find(L"wave-notify");

	szModules << pos->first + L"-" + pos->second;

	for (TStringStringMapIter iter = m_vModules.begin(); iter != m_vModules.end(); iter++)
	{
		if (iter->first != L"wave-notify")
		{
			szModules << L";" << iter->first << L"-" << iter->second;
		}
	}

	SetEnvironmentVariable(L"WAVENOTIFY_MODULES", szModules.str().c_str());
}

BOOL CBootstrap::BootApplication(wstring szExecutable, INT nCmdShow)
{
	// Detect all required variables.

	TStringStringMapIter pos = m_vModules.find(L"wave-notify");

	wstring szWorkDir(m_szPath + pos->first + L"-" + pos->second + L"\\");

	wstring szCommandLine = L"\"" + szExecutable + L"\"";

	LPWSTR szMutableCommandLine = _wcsdup(szCommandLine.c_str());

	// Start the application.

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = nCmdShow;

	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	BOOL fResult = CreateProcess(
		NULL,
		szMutableCommandLine,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		szWorkDir.c_str(),
		&si,
		&pi);

	if (fResult)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	free(szMutableCommandLine);

	return fResult;
}

BOOL CBootstrap::IsVersion(wstring szVersion)
{
	BOOL fInNumber = FALSE;
	BOOL fFirst = TRUE;

	for (wstring::iterator iter = szVersion.begin(); iter != szVersion.end(); iter++)
	{
		if (fFirst)
		{
			// Versions must start with a digit.

			if (!(iswdigit(*iter)))
			{
				return FALSE;
			}

			fFirst = FALSE;
			fInNumber = TRUE;
		}
		else
		{
			if (iswdigit(*iter))
			{
				// If we're switching from a dot to a digit, we're
				// starting a new group. Otherwise, we're just
				// reading more digits.

				if (!fInNumber)
				{
					fInNumber = TRUE;
				}
			}
			else if (*iter == L'.')
			{
				if (fInNumber)
				{
					// Switching from digits to a dit.

					fInNumber = FALSE;
				}
				else
				{
					// If we're already reading a dot, there
					// are two dots in a row and this is
					// not a legal version.

					return FALSE;
				}
			}
			else
			{
				// Illegal character.
				return FALSE;
			}
		}
	}

	// A correct version ends in a digit.

	return fInNumber;
}

INT CBootstrap::CompareVersion(wstring szLeft, wstring szRight)
{
	if (szLeft == szRight)
	{
		return 0;
	}

	DWORD dwLeftOffset = 0;
	DWORD dwRightOffset = 0;

	for (;;)
	{
		DWORD dwEnd = szLeft.find(L'.', dwLeftOffset);

		if (dwEnd == wstring::npos)
		{
			dwEnd = szLeft.size();
		}

		if (dwLeftOffset > dwEnd)
		{
			dwLeftOffset = dwEnd;
		}

		wstring szLeftPart = szLeft.substr(dwLeftOffset, dwEnd - dwLeftOffset);

		dwEnd = szRight.find(L'.', dwRightOffset);
		dwLeftOffset = dwEnd + 1;

		if (dwEnd == wstring::npos)
		{
			dwEnd = szRight.size();
		}

		if (dwRightOffset > dwEnd)
		{
			dwRightOffset = dwEnd;
		}

		wstring szRightPart = szRight.substr(dwRightOffset, dwEnd - dwRightOffset);
		dwRightOffset = dwEnd + 1;

		if (szLeftPart.empty() && szRightPart.empty())
		{
			return 0;
		}
		else if (szLeftPart.empty())
		{
			return 1;
		}
		else if (szRightPart.empty())
		{
			return -1;
		}
		else if (!iswdigit(szLeftPart) || !iswdigit(szRightPart))
		{
			// Version numbers be checked for validity before being
			// compared. Don't know what to do when we did not get a number.

			return 0;
		}

		INT nLeftPart = _wtol(szLeftPart.c_str());
		INT nRightPart = _wtol(szRightPart.c_str());

		if (nLeftPart != nRightPart)
		{
			return nRightPart - nLeftPart;
		}
	}

	return 0;
}
