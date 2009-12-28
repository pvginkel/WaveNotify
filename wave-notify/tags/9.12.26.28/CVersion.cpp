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

#define UPDATE_FILENAME		L"update.zip"
#define UPDATE_PATH		L"update-tmp"
#define UPDATE_EXEC		L"update.exe"

BOOL CVersion::m_fNewVersionAvailable = FALSE;

// Buffer is allocated here to circumvent the memory leak detection.

wstring CVersion::m_szLink(140, L' ');
wstring CVersion::m_szVersion(140, L' ');

BOOL CVersion::NewVersionAvailable()
{
	CCurl vRequest(GetRequestUrl());
	CCurlAnsiStringReader vReader;

	vRequest.SetTimeout(CURL_TIMEOUT);
	vRequest.SetIgnoreSSLErrors(TRUE);
	vRequest.SetReader(&vReader);

	if (vRequest.Execute())
	{
		return ParseNewVersionResponse(vReader.GetString());
	}
	else
	{
		return FALSE;
	}
}

wstring CVersion::GetAppVersion()
{
	wstring szVersion = L"";

	wstring szFilename(GetModuleFileNameEx());

	DWORD dwHandle = 0;
	DWORD dwSize = GetFileVersionInfoSize(szFilename.c_str(), &dwHandle);
	LPVOID lpData = NULL;

	if (dwSize == 0)
	{
		goto __end;
	}

	lpData = malloc(dwSize);

	if (!GetFileVersionInfo(szFilename.c_str(), dwHandle, dwSize, lpData))
	{
		goto __end;
	}

	UINT uLen;
	VS_FIXEDFILEINFO * lpffi;

	if (!VerQueryValue(lpData, L"\\", (LPVOID *)&lpffi, &uLen))
	{
		goto __end;
	}

	szVersion = Format(
		L"%d.%d.%d.%d",
		(INT)HIWORD(lpffi->dwProductVersionMS),
		(INT)LOWORD(lpffi->dwProductVersionMS),
		(INT)HIWORD(lpffi->dwProductVersionLS),
		(INT)LOWORD(lpffi->dwProductVersionLS));

__end:
	if (lpData != NULL)
	{
		free(lpData);
	}

	return szVersion;
}

wstring CVersion::GetRequestUrl()
{
	wstringstream szUrl;

	szUrl << VERSION_LINK << L"?version=" << UrlEncode(GetAppVersion());

	CSettings vSettings(FALSE);
	BOOL fCollectStatistics;

	if (vSettings.GetCollectStatistics(fCollectStatistics) && fCollectStatistics)
	{
		wstring szClientVersion;

		if (RunningWine())
		{
			szClientVersion = GetWineVersion();

			if (szClientVersion.empty())
			{
				szClientVersion = L"Wine";
			}
			else
			{
				szClientVersion = L"Wine " + szClientVersion;
			}
		}
		else
		{
			OSVERSIONINFO osi;

			memset(&osi, 0, sizeof(OSVERSIONINFO));

			osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

			if (GetVersionEx(&osi))
			{
				szClientVersion = Format(L"Windows %d.%d.%d",
					osi.dwMajorVersion, osi.dwMinorVersion, osi.dwBuildNumber);

				if (wcslen(osi.szCSDVersion) > 0)
				{
					szClientVersion += L" ";
					szClientVersion += osi.szCSDVersion;
				}
			}
			else
			{
				szClientVersion = L"Windows";
			}
		}

		szUrl << L"&client=" << UrlEncode(szClientVersion);

		wstring szCookie;

		if (vSettings.GetStatisticsCookie(szCookie) && !szCookie.empty())
		{
			szUrl << L"&cookie=" << UrlEncode(szCookie);
		}
	}

	return szUrl.str();
}

BOOL CVersion::ParseNewVersionResponse(const wstring & szResponse)
{
	TStringStringMap vMap;
	wstring szAttemptedVersion;

	if (!ParseStringMap(szResponse, vMap))
	{
		return FALSE;
	}

	if (vMap.empty())
	{
		return FALSE;
	}

	TStringStringMapIter pos = vMap.find(L"Error");

	if (pos != vMap.end())
	{
		LOG1("Problem while checking for new version: %S", pos->second.c_str());
		return FALSE;
	}

	pos = vMap.find(L"Cookie");

	if (pos != vMap.end())
	{
		CSettings vSettings(TRUE);

		vSettings.SetStatisticsCookie(pos->second);
	}

	pos = vMap.find(L"Link");

	if (pos == vMap.end())
	{
		goto __up_to_date;
	}


	m_szLink = pos->second;

	pos = vMap.find(L"Version");

	if (pos == vMap.end())
	{
		goto __up_to_date;
	}

	m_szVersion = pos->second;

	if (
		CSettings(FALSE).GetAttemptedVersion(szAttemptedVersion) &&
		m_szVersion == szAttemptedVersion
	) {
		goto __up_to_date;
	}

	m_fNewVersionAvailable = TRUE;

	return TRUE;

__up_to_date:
	m_szLink = L"";
	m_szVersion = L"";

	m_fNewVersionAvailable = FALSE;

	return FALSE;
}

BOOL CVersion::PerformUpdate()
{
	wstring szBasePath(GetDirname(GetModuleFileNameEx()) + L"\\");

	if (szBasePath.empty())
	{
		LOG("Could not get module name");

		return FALSE;
	}

	if (!DownloadUpdate(szBasePath))
	{
		return FALSE;
	}

	CSettings(TRUE).SetAttemptedVersion(m_szVersion);

	if (!ExtractUpdate(szBasePath))
	{
		return FALSE;
	}

	if (!ValidateUpdate(szBasePath))
	{
		return FALSE;
	}

	return InitiateInstall(szBasePath);
}

BOOL CVersion::DownloadUpdate(wstring szBasePath)
{
	wstring szPath = szBasePath + UPDATE_FILENAME;

	DeleteFile(szPath.c_str());

	HANDLE hFile = CreateFile(szPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		LOG1("Could not create file %S", szPath.c_str());

		return FALSE;
	}

	CCurl vRequest(m_szLink);
	CCurlFileReader vReader(hFile);

	vRequest.SetTimeout(CURL_TIMEOUT);
	vRequest.SetIgnoreSSLErrors(TRUE);
	vRequest.SetReader(&vReader);
	vRequest.SetAutoRedirect(TRUE);

	BOOL fResult = vRequest.Execute();

	CloseHandle(hFile);

	if (!fResult)
	{
		DeleteFile(szPath.c_str());
	}

	return fResult;
}

BOOL CVersion::ExtractUpdate(wstring szBasePath)
{
	// Ensure that an empty update directory exists.

	wstring szUpdatePath(szBasePath + UPDATE_PATH + L"\\");

	RemoveDirectory(szUpdatePath, TRUE);

	CreateDirectory(szUpdatePath.c_str(), NULL);

	// Extract the zip file.

	wstring szUpdateFilename(szBasePath + UPDATE_FILENAME);

	unzFile lpZip = unzOpenW(szUpdateFilename);

	if (lpZip == NULL)
	{
		LOG1("Could not open zip file %S", szUpdateFilename.c_str());
		return FALSE;
	}

	INT nResult = unzGoToFirstFile(lpZip);

	BOOL fSuccess = FALSE;

	while (nResult != UNZ_END_OF_LIST_OF_FILE)
	{
		if (nResult != UNZ_OK)
		{
			goto __end;
		}

		wstring szFilename(unzGetCurrentFileNameW(lpZip));

		if (szFilename.empty())
		{
			goto __end;
		}

		if (!ExtractFile(lpZip, szUpdatePath, szFilename))
		{
			goto __end;
		}

		nResult = unzGoToNextFile(lpZip);
	}

	fSuccess = TRUE;

__end:
	unzClose(lpZip);

	if (!fSuccess)
	{
		RemoveDirectory(szUpdatePath, TRUE);
	}

	DeleteFile(szUpdateFilename.c_str());

	return fSuccess;
}

BOOL CVersion::InitiateInstall(wstring szBasePath)
{
	wstring szUpdatePath(szBasePath + UPDATE_PATH + L"\\");
	wstring szExecPathSource(szUpdatePath + UPDATE_EXEC);
	wstring szExecPath(szBasePath + UPDATE_EXEC);

	// Move the update executable to the base path.

	DeleteFile(szExecPath.c_str());

	if (!MoveFileEx(szExecPathSource.c_str(), szExecPath.c_str(), MOVEFILE_COPY_ALLOWED))
	{
		return FALSE;
	}

	// Now we start the update executable. As a parameter, we send our process ID,
	// the executable to execute after restart and the path where the update
	// has been extracted to as parameters.

	wstring szModuleFilename(GetBasename(GetModuleFileNameEx()));

	if (szModuleFilename.empty())
	{
		return FALSE;
	}

	wstring szCommandLine(Format(
		L"\"%s\" /pid %u /path \"%s\" /exec \"%s\"",
		szExecPath.c_str(), GetCurrentProcessId(), UPDATE_PATH, szModuleFilename.c_str()));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(PROCESS_INFORMATION);

	LPWSTR szMutableCommandLine = (LPWSTR)malloc(sizeof(WCHAR) * (szCommandLine.size() + 1));

	wcscpy(szMutableCommandLine, szCommandLine.c_str());

	BOOL fSuccess = CreateProcess(
		szExecPath.c_str(),
		szMutableCommandLine,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		szBasePath.c_str(),
		&si,
		&pi);

	free(szMutableCommandLine);

	return fSuccess;
}

BOOL CVersion::ExtractFile(unzFile lpZip, wstring szUpdatePath, wstring szFilename)
{
	if (unzOpenCurrentFile(lpZip) != UNZ_OK)
	{
		return FALSE;
	}

	wstring szFullPath(szUpdatePath + L"\\" + szFilename);

	HANDLE hFile = CreateFile(szFullPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	LPBYTE lpBuffer = (LPBYTE)malloc(FILECOPY_BUFFER_SIZE);

	BOOL fSuccess = FALSE;

	for (;;)
	{
		INT nResult = unzReadCurrentFile(lpZip, lpBuffer, FILECOPY_BUFFER_SIZE);

		if (nResult == 0)
		{
			break;
		}

		if (nResult < 0)
		{
			goto __end;
		}

		DWORD dwWritten;

		if (!WriteFile(hFile, lpBuffer, (DWORD)nResult, &dwWritten, NULL) || dwWritten != (DWORD)nResult)
		{
			goto __end;
		}
	}

	fSuccess = TRUE;

__end:
	unzCloseCurrentFile(lpZip);

	CloseHandle(hFile);

	free(lpBuffer);

	return fSuccess;
}

BOOL CVersion::ValidateUpdate(wstring szBasePath)
{
	wstring szUpdatePath(szBasePath + UPDATE_PATH + L"\\");
	wstring szExecPath(szUpdatePath + UPDATE_EXEC);

	// For the update to be correct, we need an update.exe.

	BOOL fValid = GetFileAttributes(szExecPath.c_str()) != INVALID_FILE_ATTRIBUTES;

	if (!fValid)
	{
		RemoveDirectory(szUpdatePath, TRUE);
	}

	return fValid;
}
