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

typedef enum
{
	VR_NONE,
	VR_VERSION,
	VR_DOWNLOAD,
	VT_MAX
} VERSION_REQUESTING;

typedef enum
{
	VS_NONE,
	VS_CHECKING,
	VS_DOWNLOADING,
	VS_AVAILABLE,
	VS_MAX
} VERSION_STATE;

class CVersion
{
private:
	static CVersion * m_lpInstance;

	CWindowHandle * m_lpTargetWindow;
	wstring m_szLink;
	wstring m_szVersion;
	CCurl * m_lpRequest;
	VERSION_REQUESTING m_nRequesting;
	VERSION_STATE m_nState;

public:
	CVersion();
	virtual ~CVersion();

	BOOL CheckVersion();
	BOOL PerformUpdate();
	BOOL ProcessCurlResponse(CURL_RESPONSE nState, CCurl * lpCurl);
	void SetTargetWindow(CWindowHandle * lpTargetWindow) {
		m_lpTargetWindow = lpTargetWindow;
	}
	CWindowHandle * GetTargetWindow() const { return m_lpTargetWindow; }
	VERSION_STATE GetState() const { return m_nState; }
	void CancelRequests();

	static CVersion * Instance() {
		ASSERT(m_lpInstance != NULL);

		return m_lpInstance;
	}
	static wstring GetAppVersion();

private:
	wstring GetRequestUrl();
	BOOL ParseNewVersionResponse(const wstring & szResponse);
	BOOL DownloadUpdate(wstring szBasePath);
	BOOL PrepareInstall(wstring szPath);
	BOOL ExtractUpdate(wstring szUpdateFilename);
	BOOL ExtractFile(unzFile lpZip, wstring szUpdatePath, wstring szFilename);
	BOOL ValidateUpdate();
	BOOL GetLogDump(wstringstream & szLogDump);
	BOOL ReadLogToEnd(HANDLE hFile, wstringstream & szLogDump);
	wstring GetNewVersionLink() const { return m_szLink; }
	void PostVersionRequest();
	void PostDownloadRequest();
	void ProcessVersionResponse();
	void ProcessDownloadResponse();

	wstring GetBasePath() const {
		return GetDirname(GetModuleFileNameEx()) + L"\\";
	}
	void SetState(VERSION_STATE nState) {
		CHECK_ENUM(nState, VS_MAX);

		m_nState = nState;
		m_lpTargetWindow->PostMessage(WM_VERSION_STATE, m_nState);
	}
};

#endif // _INC_VERSION
