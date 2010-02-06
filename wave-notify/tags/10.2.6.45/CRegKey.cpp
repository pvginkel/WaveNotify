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

CRegKey::CRegKey(HKEY hKey)
{
	ASSERT(hKey != NULL);

	m_hKey = hKey;
}

CRegKey::~CRegKey()
{
	RegCloseKey(m_hKey);
}

CRegKey * CRegKey::CreateKey(HKEY hBaseKey, wstring szSubKey)
{
	ASSERT(hBaseKey != NULL && !szSubKey.empty());

	HKEY hKey;

	if (RegCreateKeyEx(hBaseKey, szSubKey.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS)
	{
		return NULL;
	}

	return new CRegKey(hKey);
}

CRegKey * CRegKey::OpenKey(HKEY hBaseKey, wstring szSubKey)
{
	ASSERT(hBaseKey != NULL && !szSubKey.empty());

	HKEY hKey;
	
	if (RegOpenKeyEx(hBaseKey, szSubKey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return NULL;
	}

	return new CRegKey(hKey);
}

BOOL CRegKey::GetValue(wstring szName, wstring & szResult) const
{
	DWORD dwType;
	DWORD dwSize;

	if (RegQueryValueEx(m_hKey, szName.c_str(), NULL, &dwType, NULL, &dwSize) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (dwType != REG_SZ && dwType != REG_EXPAND_SZ && dwType != REG_MULTI_SZ)
	{
		return FALSE;
	}

	LPWSTR szBuffer = (LPWSTR)malloc(dwSize);

	if (RegQueryValueEx(m_hKey, szName.c_str(), NULL, &dwType, (LPBYTE)szBuffer, &dwSize) == ERROR_SUCCESS)
	{
		szResult = szBuffer;
	}

	free(szBuffer);

	return TRUE;
}

BOOL CRegKey::SetValue(wstring szName, wstring szValue)
{
	DWORD dwType = szValue.find(L'\n') == wstring::npos ? REG_SZ : REG_MULTI_SZ;

	return RegSetValueEx(m_hKey, szName.c_str(), 0, dwType, (LPBYTE)szValue.c_str(), sizeof(WCHAR) * (szValue.size() + 1)) == ERROR_SUCCESS;
}

BOOL CRegKey::GetValue(wstring szName, DWORD & dwResult) const
{
	DWORD dwType;
	DWORD dwSize;

	dwSize = 4;

	return
		RegQueryValueEx(m_hKey, szName.c_str(), NULL, &dwType, (LPBYTE)&dwResult, &dwSize) == ERROR_SUCCESS &&
		dwType == REG_DWORD;
}

BOOL CRegKey::SetValue(wstring szName, DWORD dwValue)
{
	return RegSetValueEx(m_hKey, szName.c_str(), 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS;
}

BOOL CRegKey::DeleteValue(wstring szName)
{
	ASSERT(!szName.empty());

	return RegDeleteValue(m_hKey, szName.c_str()) == ERROR_SUCCESS;
}

BOOL CRegKey::GetValue(wstring szName, BOOL & fResult) const
{
	DWORD dwTmp;

	if (GetValue(szName, dwTmp))
	{
		fResult = dwTmp != 0;
		return TRUE;
	}

	return FALSE;
}

BOOL CRegKey::SetValue(wstring szName, BOOL fResult)
{
	return SetValue(szName, (DWORD)(fResult ? 1 : 0));
}

BOOL CRegKey::GetSubKeys(TStringVector & vKeys) const
{
	DWORD dwLength = 0;

	LONG lStatus = RegQueryInfoKey(
		m_hKey, NULL, NULL, NULL, NULL, &dwLength, NULL, NULL, NULL, NULL, NULL, NULL
	);
	
	if (lStatus != ERROR_SUCCESS)
	{
		return FALSE;
	}

	LPWSTR szBuffer = (LPWSTR)malloc((dwLength + 1) * sizeof(WCHAR));
	DWORD dwIndex = 0;
	BOOL fSuccess = FALSE;

	vKeys.clear();

	for (;;)
	{
		lStatus = RegEnumKey(m_hKey, dwIndex, szBuffer, dwLength + 1);

		switch (lStatus)
		{
		case ERROR_SUCCESS:
			vKeys.push_back(szBuffer);
			break;

		case ERROR_NO_MORE_ITEMS:
			fSuccess = TRUE;

			// Fall through
		default:
			goto __end;
		}

		dwIndex++;
	}

__end:
	free(szBuffer);

	return fSuccess;
}
