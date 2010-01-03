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

CVersion * CVersion::m_lpInstance = NULL;

CVersion::CVersion()
{
	ASSERT(m_lpInstance == NULL);

	m_lpTargetWindow = NULL;
	m_nRequesting = VR_NONE;
	m_lpRequest = NULL;
	m_nState = VS_NONE;

	m_lpInstance = this;
}

CVersion::~CVersion()
{
	ASSERT(m_lpRequest == NULL);

	m_lpInstance = NULL;
}

BOOL CVersion::ProcessCurlResponse(CURL_RESPONSE nState, CCurl * lpCurl)
{
	// Do we understand the response?

	if (nState != CR_COMPLETED || lpCurl == NULL)
	{
		return FALSE;
	}

	// Check whether it's our Curl request.

	if (lpCurl == m_lpRequest)
	{
		switch (m_nRequesting)
		{
		case VR_VERSION:
			ProcessVersionResponse();
			break;

		case VR_DOWNLOAD:
			ProcessDownloadResponse();
			break;

		default:
			LOG1("Illegal requesting state %d", m_nRequesting);

			CCurl::Destroy(m_lpRequest);

			m_lpRequest = NULL;
			m_nRequesting = VR_NONE;

			SetState(VS_NONE);
			break;
		}

		return TRUE;
	}

	// This is not one of our request.

	return FALSE;
}

BOOL CVersion::CheckVersion()
{
	// The Window handle must have been attached to be able to perform Curl
	// requests.

	ASSERT(m_lpTargetWindow != NULL);

	if (GetState() == VS_NONE)
	{
		SetState(VS_CHECKING);

		PostVersionRequest();

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CVersion::PostVersionRequest()
{
	m_lpRequest = new CCurl(GetRequestUrl(), m_lpTargetWindow);

	m_lpRequest->SetTimeout(WEB_TIMEOUT_LONG);
	m_lpRequest->SetIgnoreSSLErrors(TRUE);
	m_lpRequest->SetReader(new CCurlAnsiStringReader());

#if !defined(_DEBUG) || defined(TEST_LOG_DUMP)
	wstringstream szLogDump;

	if (GetLogDump(szLogDump))
	{
		m_lpRequest->SetUrlEncodedPostData(szLogDump.str());
	}
#endif

	m_nRequesting = VR_VERSION;

	CNotifierApp::Instance()->QueueRequest(m_lpRequest);
}

void CVersion::PostDownloadRequest()
{
	wstring szPath(GetBasePath() + UPDATE_FILENAME);

	DeleteFile(szPath.c_str());

	HANDLE hFile = CreateFile(szPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		LOG1("Could not create file %S", szPath.c_str());

		return;
	}

	m_lpRequest = new CCurl(m_szLink, m_lpTargetWindow);

	m_lpRequest->SetTimeout(WEB_TIMEOUT_LONG);
	m_lpRequest->SetIgnoreSSLErrors(TRUE);
	m_lpRequest->SetReader(new CCurlFileReader(hFile));
	m_lpRequest->SetAutoRedirect(TRUE);

	m_nRequesting = VR_DOWNLOAD;

	CNotifierApp::Instance()->QueueRequest(m_lpRequest);
}

void CVersion::ProcessVersionResponse()
{
	CCurlAnsiStringReader * lpReader = (CCurlAnsiStringReader *)m_lpRequest->GetReader();

	BOOL fSuccess = FALSE;

	if (m_lpRequest->GetResult() == CURLE_OK)
	{
		fSuccess = ParseNewVersionResponse(lpReader->GetString());
	}
	
	delete lpReader;
	delete m_lpRequest;

	m_lpRequest = NULL;
	m_nRequesting = VR_NONE;

	if (fSuccess && !m_szLink.empty())
	{
		SetState(VS_DOWNLOADING);

		PostDownloadRequest();
	}
	else
	{
		SetState(VS_NONE);
	}
}

void CVersion::ProcessDownloadResponse()
{
	CCurlFileReader * lpReader = (CCurlFileReader *)m_lpRequest->GetReader();

	BOOL fSuccess = m_lpRequest->GetResult() == CURLE_OK;

	CloseHandle(lpReader->GetHandle());

	delete lpReader;
	delete m_lpRequest;

	m_lpRequest = NULL;
	m_nRequesting = VR_NONE;

	wstring szPath(GetBasePath() + UPDATE_FILENAME);

	if (fSuccess)
	{
		fSuccess = PrepareInstall(szPath);
	}

	DeleteFile(szPath.c_str());

	if (fSuccess)
	{
		SetState(VS_AVAILABLE);
	}
	else
	{
		SetState(VS_NONE);
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
	wstring szAttemptedVersion;

	if (vSettings.GetAttemptedVersion(szAttemptedVersion))
	{
		szUrl << L"&attempted=" << szAttemptedVersion;
	}

	// Cookie has been forced on to help debug update problems. Having
	// the cookie present always allows the server to make sure clients are
	// updating correctly.

	wstring szCookie;

	if (vSettings.GetStatisticsCookie(szCookie) && !szCookie.empty())
	{
		szUrl << L"&cookie=" << UrlEncode(szCookie);
	}

	BOOL fCollectStatistics;

	if (vSettings.GetCollectStatistics(fCollectStatistics) && fCollectStatistics)
	{
		LPCWSTR szLanguageCode = GetLanguageCode();

		if (szLanguageCode != NULL)
		{
			szUrl << L"&lang=" << szLanguageCode;
		}

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
	}

	return szUrl.str();
}

BOOL CVersion::ParseNewVersionResponse(const wstring & szResponse)
{
	TStringStringMap vMap;

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

	return TRUE;

__up_to_date:
	m_szVersion = L"";
	m_szLink = L"";

	return TRUE;
}

BOOL CVersion::PrepareInstall(wstring szPath)
{
	CSettings(TRUE).SetAttemptedVersion(m_szVersion);

	if (!ExtractUpdate(szPath))
	{
		return FALSE;
	}

	if (!ValidateUpdate())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CVersion::ExtractUpdate(wstring szUpdateFilename)
{
	wstring szBasePath(GetBasePath());

	// Ensure that an empty update directory exists.

	wstring szUpdatePath(szBasePath + UPDATE_PATH + L"\\");

	RemoveDirectory(szUpdatePath, TRUE);

	CreateDirectory(szUpdatePath.c_str(), NULL);

	// Extract the zip file.

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

	return fSuccess;
}

BOOL CVersion::PerformUpdate()
{
	wstring szBasePath(GetBasePath());
	wstring szUpdatePath(szBasePath + UPDATE_PATH + L"\\");
	wstring szExecPathSource(szUpdatePath + UPDATE_EXEC);
	wstring szExecPath(szBasePath + UPDATE_EXEC);

	// Move the update executable to the base path.

	DeleteFile(szExecPath.c_str());

	if (!MoveFile(szExecPathSource.c_str(), szExecPath.c_str()))
	{
		return FALSE;
	}

	// Now we start the update executable. As a parameter, we send our process ID,
	// the executable to execute after restart and the path where the update
	// has been extracted to as parameters.

	wstring szModuleFilename(GetBasename(GetModuleFileNameEx()));

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

BOOL CVersion::ValidateUpdate()
{
	wstring szUpdatePath(GetBasePath() + UPDATE_PATH + L"\\");
	wstring szExecPath(szUpdatePath + UPDATE_EXEC);

	// For the update to be correct, we need an update.exe.

	BOOL fValid = GetFileAttributes(szExecPath.c_str()) != INVALID_FILE_ATTRIBUTES;

	if (!fValid)
	{
		RemoveDirectory(szUpdatePath, TRUE);
	}

	return fValid;
}

BOOL CVersion::GetLogDump(wstringstream & szLogDump)
{
	szLogDump << L"logdump=";

	BOOL fResult = FALSE;

	HANDLE hFile = CreateFile(L"log.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize = GetFileSize(hFile, NULL);

		if (dwFileSize != INVALID_FILE_SIZE)
		{
			if (dwFileSize > 0)
			{
				DWORD dwOffset = 0;

				if (dwFileSize - dwOffset > MAX_LOG_DUMP)
				{
					dwOffset = dwFileSize - MAX_LOG_DUMP;
				}

				DWORD dwResult = SetFilePointer(hFile, (LONG)dwOffset, NULL, FILE_BEGIN);

				if (dwResult != INVALID_SET_FILE_POINTER)
				{
					fResult = ReadLogToEnd(hFile, szLogDump);
				}
			}
		}

		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		SetEndOfFile(hFile);

		CloseHandle(hFile);
	}

	return fResult;
}

BOOL CVersion::ReadLogToEnd(HANDLE hFile, wstringstream & szLogDump)
{
	LPSTR szBuffer = (LPSTR)malloc(FILECOPY_BUFFER_SIZE + 1);
	BOOL fResult = FALSE;

	for (;;)
	{
		DWORD dwRead;

		if (!ReadFile(hFile, szBuffer, FILECOPY_BUFFER_SIZE, &dwRead, NULL))
		{
			break;
		}

		if (dwRead == 0)
		{
			fResult = TRUE;
			break;
		}

		szBuffer[dwRead] = '\0';

		szLogDump << UrlEncode(ConvertToWideChar(szBuffer));
	}

	free(szBuffer);

	return fResult;
}

void CVersion::CancelRequests()
{
	if (m_lpRequest != NULL)
	{
		CNotifierApp::Instance()->CancelRequest(m_lpRequest);

		m_lpRequest = NULL;
		m_nRequesting = VR_NONE;
	}
}
