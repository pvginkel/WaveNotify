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

#ifndef _INC_VERSION
#define _INC_VERSION

#pragma once

class CVersion
{
private:
	static wstring m_szLink;
	static BOOL m_fNewVersionAvailable;
	static wstring m_szVersion;

public:
	static BOOL NewVersionAvailable();

	static BOOL GetNewVersionAvailable() { return m_fNewVersionAvailable; }
	static wstring GetNewVersionLink() { return m_szLink; }
	static wstring GetAppVersion();
	static BOOL PerformUpdate();

private:
	static wstring GetRequestUrl();
	static BOOL ParseNewVersionResponse(const wstring & szResponse);
	static BOOL DownloadUpdate(wstring szBasePath);
	static BOOL ExtractUpdate(wstring szBasePath);
	static BOOL InitiateInstall(wstring szBasePath);
	static BOOL ExtractFile(unzFile lpZip, wstring szUpdatePath, wstring szFilename);
	static BOOL ValidateUpdate(wstring szBasePath);
	static BOOL GetLogDump(wstringstream & szLogDump);
	static BOOL ReadLogToEnd(HANDLE hFile, wstringstream & szLogDump);
};

#endif // _INC_VERSION
