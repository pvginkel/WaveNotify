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

BOOL RunningRemoteDesktop()
{
	return (GetSystemMetrics(SM_REMOTESESSION) & 1) != 0;
}

BOOL DegradeVisualPerformance()
{
#if defined(_DEBUG) && defined(FORCE_ANIMATIONS)
	return FALSE;
#else
	return RunningWine() || RunningRemoteDesktop();
#endif
}

bool isword(char _Ch)
{
	return (_Ch == '_' || isalnum(_Ch));
}

bool isword(wchar_t _Ch)
{
	return (_Ch == L'_' || isalnum(_Ch));
}

bool isalnum(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isalnum(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isalpha(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isalpha(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iscntrl(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iscntrl(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isdigit(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isdigit(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isgraph(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isgraph(*iter))
		{
			return false;
		}
	}

	return true;
}

bool islower(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!islower(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isprint(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isprint(*iter))
		{
			return false;
		}
	}

	return true;
}

bool ispunct(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!ispunct(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isspace(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isspace(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isupper(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isupper(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isword(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isword(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isxdigit(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!isxdigit(*iter))
		{
			return false;
		}
	}

	return true;
}

wstring tolower(wstring _String)
{
	wstring _Result;
	
	_Result.reserve(_String.length());

	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		_Result += tolower(*iter);
	}

	return _Result;
}

wstring touppoer(wstring _String)
{
	wstring _Result;
	
	_Result.reserve(_String.length());

	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		_Result += toupper(*iter);
	}

	return _Result;
}

INT Rand(INT nMin, INT nMax)
{
	static BOOL fInitialised = FALSE;

	if (!fInitialised)
	{
		fInitialised = TRUE;

		srand(GetTickCount());
	}

	INT nRand = 0;
	INT nRandMax = 0;
	INT nCount = INT(ceil(DOUBLE(nMax - nMin) / DOUBLE(RAND_MAX)));

	if (nCount < 1)
	{
		nCount = 1;
	}

	for (INT i = 0; i < nCount; i++)
	{
		nRand += rand();
		nRandMax += RAND_MAX;
	}

	INT nResult = nMin + INT((DOUBLE(nRand) / DOUBLE(nRandMax)) * DOUBLE(nMax - nMin + 1));

	// Force result inside nMin/nMax because of DOUBLE rounding errors

	if (nResult < nMin)
	{
		return nMin;
	}
	else if (nResult > nMax)
	{
		return nMax;
	}
	else
	{
		return nResult;
	}
}

BOOL ParseStringMap(const wstring & szInput, TStringStringMap & vMap)
{
	INT nOffset = 0;

	for (;;)
	{
		INT nEqualOffset = szInput.find(L'=', nOffset);

		if (nEqualOffset == wstring::npos)
		{
			break;
		}

		INT nEnd = szInput.find(L'\n', nOffset);

		if (nEnd == wstring::npos)
		{
			nEnd = szInput.length();
		}

		// Validate that there is no newline in the key. If there were,
		// the input probably isn't a valid map.

		if (nEnd < nEqualOffset)
		{
			return FALSE;
		}

		wstring szKey = Trim(szInput.substr(nOffset, nEqualOffset - nOffset));
		wstring szValue = Trim(szInput.substr(nEqualOffset + 1, nEnd - (nEqualOffset + 1)));

		vMap[szKey] = szValue;

		if (nEnd == szInput.length())
		{
			break;
		}

		nOffset = nEnd + 1;
	}

	return TRUE;
}

BOOL RemoveDirectory(wstring szPath, BOOL fRecurse)
{
	if (GetFileAttributes(szPath.c_str()) != FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	if (!fRecurse)
	{
		return RemoveDirectory(szPath.c_str());
	}

	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	BOOL fSuccess = FALSE;

	hFind = FindFirstFile((szPath + L"\\*").c_str(), &wfd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	do
	{
		if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0)
		{
			continue;
		}

		wstring szFullPath(szPath + L"\\" + wfd.cFileName);

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!RemoveDirectory(szFullPath, TRUE))
			{
				goto __end;
			}
		}
		else
		{
			if (!DeleteFile(szFullPath.c_str()))
			{
				goto __end;
			}
		}
	}
	while (FindNextFile(hFind, &wfd) != 0);

	fSuccess = GetLastError() == ERROR_NO_MORE_FILES;

__end:
	FindClose(hFind);

	return fSuccess;
}

wstring GetModuleFileNameEx()
{
	WCHAR szPath[MAX_PATH];

	if (::GetModuleFileName(NULL, szPath, _ARRAYSIZE(szPath)) == 0)
	{
		return L"";
	}

	szPath[_ARRAYSIZE(szPath) - 1] = L'\0';

	return wstring(szPath);
}

wstring GetDirname(wstring szPath)
{
	UINT uEnd = szPath.size();

	while (uEnd > 0 && szPath[uEnd - 1] == L'\\')
	{
		uEnd--;
	}

	if (uEnd == 0)
	{
		return L".";
	}

	UINT uPos = szPath.find_last_of(L'\\');

	if (uPos == wstring::npos || uPos == 0)
	{
		return L".";
	}
	else
	{
		return szPath.substr(0, uPos);
	}
}

wstring GetBasename(wstring szPath)
{
	UINT nPos = szPath.find_last_of(L'\\');

	if (nPos == wstring::npos)
	{
		return szPath;
	}

	return szPath.substr(nPos + 1);
}

wstring GetLongPathName(wstring szPath)
{
	DWORD cbBuffer = GetLongPathName(szPath.c_str(), NULL, 0);

	if (cbBuffer == 0)
	{
		return L"";
	}

	LPWSTR szBuffer = (LPWSTR)malloc(sizeof(WCHAR) * (cbBuffer + 1));

	wstring szResult;

	if (GetLongPathName(szPath.c_str(), szBuffer, cbBuffer + 1) > 0)
	{
		szResult = szBuffer;
	}

	free(szBuffer);

	return szResult;
}

wstring ExpandEnvironmentStrings(wstring szPath)
{
	DWORD cbBuffer = ExpandEnvironmentStrings(szPath.c_str(), NULL, 0);

	if (cbBuffer == 0)
	{
		return L"";
	}

	LPWSTR szBuffer = (LPWSTR)malloc(sizeof(WCHAR) * (cbBuffer + 1));

	wstring szResult;

	if (ExpandEnvironmentStrings(szPath.c_str(), szBuffer, cbBuffer + 1) > 0)
	{
		szResult = szBuffer;
	}

	free(szBuffer);

	return szResult;
}
