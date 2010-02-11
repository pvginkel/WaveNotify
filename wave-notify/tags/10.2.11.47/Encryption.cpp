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

BOOL EncryptString(wstring szValue, wstring & szEncrypted)
{
	DATA_BLOB vInput;
	DATA_BLOB vOutput;

	vInput.pbData = (LPBYTE)szValue.c_str();
	vInput.cbData = (szValue.length() + 1) * sizeof(WCHAR);

	BOOL fResult = CryptProtectData(
		&vInput,
		L"Google Wave Notifier",
		NULL,
		NULL,
		NULL,
		0,
		&vOutput);

	if (!fResult)
	{
		return FALSE;
	}

	TByteVector vEncoded(vOutput.cbData);

	memcpy(_VECTOR_DATA(vEncoded), vOutput.pbData, vOutput.cbData);

	LocalFree(vOutput.pbData);

	szEncrypted = Base64Encode(vEncoded);

	return TRUE;
}

BOOL DecryptString(wstring szValue, wstring & szDecrypted)
{
	TByteVector vEncoded;
	
	Base64Decode(szValue, vEncoded);

	DATA_BLOB vInput;
	DATA_BLOB vOutput;

	vInput.cbData = vEncoded.size();
	vInput.pbData = _VECTOR_DATA(vEncoded);

	BOOL fResult = CryptUnprotectData(
		&vInput,
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		&vOutput);

	if (!fResult)
	{
		return FALSE;
	}

	szDecrypted = (LPCWSTR)vOutput.pbData;

	LocalFree(vOutput.pbData);

	return TRUE;
}
