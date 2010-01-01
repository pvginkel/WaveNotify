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

#ifndef _INC_REGISTRY
#define _INC_REGISTRY

#pragma once

class CRegKey
{
private:
	HKEY m_hKey;

private:
	CRegKey(HKEY hKey);

public:
	virtual ~CRegKey();

	static CRegKey * CreateKey(HKEY hBaseKey, wstring szSubKey);
	static CRegKey * OpenKey(HKEY hBaseKey, wstring szSubKey);

	operator HKEY() { return (HKEY)m_hKey; }

	BOOL GetValue(wstring szName, wstring & szResult) const;
	BOOL SetValue(wstring szName, wstring szValue);
	BOOL GetValue(wstring szName, DWORD & dwResult) const;
	BOOL SetValue(wstring szName, DWORD dwValue);
	BOOL GetValue(wstring szName, BOOL & fResult) const;
	BOOL SetValue(wstring szName, BOOL fResult);
	BOOL DeleteValue(wstring szName);
};

#endif // _INC_REGISTRY
