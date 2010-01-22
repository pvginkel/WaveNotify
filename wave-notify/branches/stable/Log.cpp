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

static CHAR g_szAppVersion[64];

void Log_WriteA(LPCSTR szFile, DWORD dwLine, LPCSTR szFormat, ...);

void __declspec(noreturn) Log_AssertFailA(LPCSTR szFile, DWORD dwLine, LPCSTR szCond)
{
	Log_WriteA(szFile, dwLine, "ASSERTION FAILED: %s", szCond);

	ExitProcess(-1);
}

void Log_WriteA(LPCSTR szFile, DWORD dwLine, LPCSTR szFormat, ...)
{
	DWORD dwLastError = GetLastError();

	va_list argptr;

	va_start(argptr, szFormat);

	CHAR szBuffer[512];

	StringCbVPrintfA(szBuffer, _ARRAYSIZE(szBuffer), szFormat, argptr);

	szBuffer[_ARRAYSIZE(szBuffer) - 1] = '\0';

	va_end(argptr);

	/* Write the file line */

	CHAR szBuffer2[512];

	SYSTEMTIME st;

	GetSystemTime(&st);

	StringCbPrintfA(szBuffer2, _ARRAYSIZE(szBuffer2), "%s(%u) - %d - %s - %d-%d-%d %d:%02d:%02d\r\n%s\r\n",
		szFile, dwLine,
		dwLastError, ( g_szAppVersion == NULL ? "(unknown)" : g_szAppVersion ),
		(int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond,
		szBuffer);

	strcpy(szBuffer2 + (_ARRAYSIZE(szBuffer2) - 3), "\r\n");

	Log_Append(L"log.txt", szBuffer2);
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

void Log_SetAppVersion(LPCSTR szAppVersion)
{
	strncpy(g_szAppVersion, szAppVersion, _ARRAYSIZE(g_szAppVersion) - 1);
	g_szAppVersion[_ARRAYSIZE(g_szAppVersion) - 1] = '\0';
}
