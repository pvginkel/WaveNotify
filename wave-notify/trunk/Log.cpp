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

void Log_WriteA(LPCSTR szFile, DWORD dwLine, LPCSTR szFormat, ...);

void __declspec(noreturn) Log_AssertFailA(LPCSTR szFile, DWORD dwLine, LPCSTR szCond)
{
	Log_WriteA(szFile, dwLine, "ASSERTION FAILED: %s", szCond);

	ExitProcess(-1);
}

void Log_WriteA(LPCSTR szFile, DWORD dwLine, LPCSTR szFormat, ...)
{
	CHAR szBuffer[512];
	CHAR szBuffer2[512];
	DWORD dwLastError;
	LPSTR szMsgBuf;
	LPSTR szTail;
	va_list argptr;

	dwLastError = GetLastError();

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&szMsgBuf,
		0, NULL);

	va_start(argptr, szFormat);

	StringCbVPrintfA(szBuffer, _ARRAYSIZE(szBuffer), szFormat, argptr);

	szBuffer[_ARRAYSIZE(szBuffer) - 1] = '\0';

	va_end(argptr);

	/* Trim the whitespaces from the end */

	szTail = strrchr(szMsgBuf, '\0');

	szTail--;

	while (szTail > szMsgBuf)
	{
		if (isspace(*szTail) || *szTail == '.')
		{
			*szTail-- = '\0';
		}
		else
		{
			break;
		}
	}

	/* Write the file line */

	StringCbPrintfA(szBuffer2, _ARRAYSIZE(szBuffer2), "%s(%u) - %s\r\n%s\r\n", szFile, dwLine, szMsgBuf, szBuffer);

	szBuffer2[_ARRAYSIZE(szBuffer2) - 1] = '\0';

	Log_Append(L"log.txt", szBuffer2);

	LocalFree(szMsgBuf);
}

void Log_Append(LPCWSTR szFileName, LPCSTR szLine)
{
	HANDLE hFile = CreateFile(
		szFileName,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(hFile, 0, 0, FILE_END);

		DWORD dwWritten;

		WriteFile(hFile, szLine, strlen(szLine) * sizeof(CHAR), &dwWritten, NULL);

		CloseHandle(hFile);
	}
}
