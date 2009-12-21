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

BOOL CCurlAnsiStringReader::Read(LPBYTE lpData, DWORD cbData)
{
	BOOL fResult = FALSE;

	// Converts using ISO-8859-1 codepage.

	DWORD dwSize = MultiByteToWideChar(28591, 0, (LPCSTR)lpData, cbData, NULL, 0);
	
	if (dwSize > 0)
	{
		if (dwSize + 1 > m_cbBuffer)
		{
			m_cbBuffer = dwSize + 1;

			if (m_szBuffer == NULL)
			{
				m_szBuffer = (LPWSTR)malloc(sizeof(WCHAR) * m_cbBuffer);
			}
			else
			{
				m_szBuffer = (LPWSTR)realloc(m_szBuffer, sizeof(WCHAR) * m_cbBuffer);
			}
		}

		dwSize = MultiByteToWideChar(28591, 0, (LPCSTR)lpData, cbData, m_szBuffer, m_cbBuffer);

		if (dwSize > 0)
		{
			m_szBuffer[dwSize] = L'\0';

			m_szResult << m_szBuffer;

			fResult = TRUE;
		}
	}

	return fResult;
};
