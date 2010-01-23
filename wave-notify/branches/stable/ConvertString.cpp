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

const wstring ConvertToWideChar(const string szString, int nCodePage)
{
	return ConvertToWideChar(szString.c_str(), szString.length(), nCodePage);
}

const wstring ConvertToWideChar(const TByteVector * szData, int nCodePage)
{
	return ConvertToWideChar((LPCSTR)_VECTOR_DATA(*szData), szData->size(), nCodePage);
}

const wstring ConvertToWideChar(LPCSTR szString, size_t nLength, int nCodePage)
{
	size_t nNewLength = MultiByteToWideChar(
		nCodePage < 0 ? 0 : nCodePage,
		0,
		szString,
		nLength,
		NULL,
		0);
	
	wchar_t * szBuffer = new wchar_t[nNewLength + 1];

	MultiByteToWideChar(
		nCodePage < 0 ? 0 : nCodePage,
		0,
		szString,
		nLength,
		szBuffer,
		nNewLength + 1);

	szBuffer[nNewLength] = L'\0';

	wstring szResult(szBuffer);

	delete szBuffer;

	return szResult;
}

const string ConvertToMultiByte(const wstring szString, int nCodePage)
{
	return ConvertToMultiByte(szString.c_str(), szString.length(), nCodePage);
}

const string ConvertToMultiByte(LPCWSTR szString, size_t nLength, int nCodePage)
{
	size_t nNewLength = WideCharToMultiByte(
		nCodePage < 0 ? 0 : nCodePage,
		0,
		szString,
		nLength,
		NULL,
		0,
		NULL,
		NULL);

	char * szBuffer = new char[nNewLength + 1];

	WideCharToMultiByte(
		nCodePage < 0 ? 0 : nCodePage,
		0,
		szString,
		nLength,
		szBuffer,
		nNewLength + 1,
		NULL,
		NULL);

	szBuffer[nNewLength] = '\0';

	string szResult(szBuffer);

	delete szBuffer;

	return szResult;
}
