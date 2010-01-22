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
#include "wave.h"

BOOL CWaveReader::Read(LPBYTE lpData, DWORD cbData)
{
	m_szBuffer << m_vConverter.Parse(lpData, cbData);

	return PumpResponseBuffer();
}

BOOL CWaveReader::PumpResponseBuffer()
{
	BOOL fSuccess;

	while (PumpMessage(fSuccess) && fSuccess)
		;

	return fSuccess;
}

BOOL CWaveReader::PumpMessage(BOOL & fSuccess)
{
	wstring szReceived = m_szBuffer.str();

	// Does the current content has a newline?

	DWORD dwPos = szReceived.find(L'\n');

	if (dwPos == wstring::npos)
	{
		// No full packet in yet.

		fSuccess = TRUE;
		return FALSE;
	}

	wstring szLength = szReceived.substr(0, dwPos);

	if (szLength.length() == 0 || !iswdigit(szLength))
	{
		// The first line was not a number; we cannot read a package.

		fSuccess = FALSE;
		return FALSE;
	}

	DWORD dwLength = _wtol(szLength.c_str());

	if (szReceived.length() < dwPos + 1 + dwLength)
	{
		// No full packet in yet.

		fSuccess = TRUE;
		return FALSE;
	}

	// We have received a full package; parse to a response and submit.

	fSuccess = m_lpSession->ParseChannelResponse(szReceived.substr(dwPos + 1, dwLength));

	m_szBuffer.str(L"");

	if (!fSuccess)
	{
		return FALSE;
	}

	if (szReceived.length() > dwPos + 1 + dwLength)
	{
		m_szBuffer << szReceived.substr(dwPos + 1 + dwLength);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
