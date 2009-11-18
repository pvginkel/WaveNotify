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

#define URL_VERSION_RSS		L"http://sourceforge.net/api/file/index/project-id/285382/rss"

#define VERSION_LENGTH		40

typedef enum
{
	STATE_NEED_DIGIT,
	STATE_CAN_DOT
} MATCH_STATE;

static void Version_GetLatestVersion(LPWSTR szResponse, LPWSTR szLatestVersion);
static INT Version_Compare(LPWSTR szLeft, LPWSTR szRight);
static BOOL Version_GetCurrentVersion(LPWSTR szVersion);

BOOL Version_IsNewAvailable()
{
	LPREQUEST lpRequest;
	LPRESPONSE lpResponse;
	WCHAR szCurrentVersion[VERSION_LENGTH + 1];
	WCHAR szLatestVersion[VERSION_LENGTH + 1];
	LPWSTR szResponse;

	lpRequest = Curl_Create(URL_VERSION_RSS, USERAGENT);

	Curl_SetTimeout(lpRequest, CURL_TIMEOUT);

	lpResponse = Curl_Execute(lpRequest);

	if (lpResponse == NULL)
	{
		Curl_DestroyRequest(lpRequest);
		return FALSE;
	}

	szResponse = duputf8towcsn((LPCSTR)lpResponse->lpData->lpData, lpResponse->lpData->dwSize);

	Version_GetLatestVersion(szResponse, szLatestVersion);

	FREE(szResponse);

	Curl_DestroyRequest(lpRequest);

	Version_GetCurrentVersion(szCurrentVersion);

	return Version_Compare(szCurrentVersion, szLatestVersion) > 0;
}

static void Version_GetLatestVersion(LPWSTR szResponse, LPWSTR szLatestVersion)
{
	WCHAR szVersion[VERSION_LENGTH + 1];
	LPWSTR szOffset;
	LPWSTR szVersionBegin;
	LPWSTR szVersionEnd;
	INT nDots = 0;
	MATCH_STATE nState;

	wcscpy_s(szLatestVersion, VERSION_LENGTH + 1, L"");

	szOffset = szResponse;
	szVersionBegin = NULL;
	szVersionEnd = NULL;

	// We are getting all -[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+

	while (*szOffset != L'\0')
	{
		if (szVersionBegin == NULL)
		{
			if (*szOffset == L'-')
			{
				szVersionBegin = szOffset + 1;
				szVersionEnd = NULL;
				nDots = 0;
				nState = STATE_NEED_DIGIT;
			}
		}
		else
		{
			switch (nState)
			{
			case STATE_NEED_DIGIT:
				if (!iswdigit(*szOffset))
				{
					szVersionBegin = NULL;
				}
				nState = STATE_CAN_DOT;
				break;

			case STATE_CAN_DOT:
				if (iswdigit(*szOffset))
				{
					// this is OK
				}
				else
				{
					if (nDots == 3)
					{
						szVersionEnd = szOffset;
					}
					else
					{
						if (*szOffset == L'.')
						{
							nState = STATE_NEED_DIGIT;
							nDots++;
						}
						else
						{
							szVersionBegin = NULL;
						}
					}
				}
				break;
			}
		}

		if (szVersionBegin != NULL && szVersionEnd != NULL)
		{
			if (szVersionEnd - szVersionBegin <= VERSION_LENGTH)
			{
				wcsncpy_s(szVersion, VERSION_LENGTH + 1, szVersionBegin, szVersionEnd - szVersionBegin);
				szVersion[szVersionEnd - szVersionBegin] = L'\0';

				if (Version_Compare(szLatestVersion, szVersion) > 0)
				{
					wcscpy_s(szLatestVersion, VERSION_LENGTH + 1, szVersion);
				}
			}

			szVersionBegin = NULL;
		}

		szOffset++;
	}
}

static INT Version_Compare(LPWSTR szLeft, LPWSTR szRight)
{
	INT nLeftPart;
	INT nRightPart;

	// Are the versions the same?

	if (wcscmp(szLeft, szRight) == 0)
	{
		return 0;
	}

	// Is one of the versions empty?

	if (wcslen(szLeft) == 0)
	{
		return 1;
	}
	else if (wcslen(szRight) == 0)
	{
		return -1;
	}

	// Compare per part
	
	for (;;)
	{
		// wtol only matches numbers and signs; so it matches
		// until the dot or end of string; this is safe

		nLeftPart = _wtol(szLeft);
		nRightPart = _wtol(szRight);

		if (nLeftPart != nRightPart)
		{
			return nRightPart - nLeftPart;
		}

		// Jump to the next part

		szLeft = wcschr(szLeft, L'.');
		szRight = wcschr(szRight, L'.');

		// The can't both be NULL otherwise the strings would match and
		// we handled that already

		if (szLeft == NULL)
		{
			return 1;
		}
		else if (szRight == NULL)
		{
			return -1;
		}

		// Jump over the dots
		
		szLeft++;
		szRight++;
	}
}

static BOOL Version_GetCurrentVersion(LPWSTR szVersion)
{
	WCHAR szFilename[MAX_PATH + 1];
	DWORD dwSize;
	DWORD dwHandle;
	LPVOID lpData = NULL;
	VS_FIXEDFILEINFO * lpffi;
	UINT uLen;
	BOOL fResult = FALSE;

	if (GetModuleFileName(g_hInstance, szFilename, _ARRAYSIZE(szFilename)) == 0)
	{
		goto __end;
	}

	dwHandle = 0;
	dwSize = GetFileVersionInfoSize(szFilename, &dwHandle);

	if (dwSize == 0)
	{
		goto __end;
	}

	lpData = ALLOC(dwSize);

	if (!GetFileVersionInfo(szFilename, dwHandle, dwSize, lpData))
	{
		goto __end;
	}

	if (!VerQueryValue(lpData, L"\\", (LPVOID *)&lpffi, &uLen))
	{
		goto __end;
	}

	StringCbPrintf(szVersion, VERSION_LENGTH * sizeof(WCHAR), L"%d.%d.%d.%d",
		(INT)HIWORD(lpffi->dwProductVersionMS),
		(INT)LOWORD(lpffi->dwProductVersionMS),
		(INT)HIWORD(lpffi->dwProductVersionLS),
		(INT)LOWORD(lpffi->dwProductVersionLS));

__end:
	if (lpData != NULL)
	{
		FREE(lpData);
	}

	return fResult;
}
