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
#include "resource.h"
#include "notifierapp.h"
#include "layout.h"
#include "settings.h"
#include "browser.h"

CNotifierApp::CNotifierApp(HINSTANCE hInstance, wstring szCmdLine)
	: CApp(hInstance, szCmdLine)
{
	m_fConnected = TRUE;
	m_hNotifyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON));
	m_hNotifyIconGray = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON_GRAY));
	m_hNotifyIconGray1 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON_GRAY_1));
	m_hNotifyIconGray2 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON_GRAY_2));
	m_hNotifyIconGray3 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON_GRAY_3));
	m_hNotifyIconUnread = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON_UNREAD));
	m_hMainIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	m_hMainIconSmall = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON_SMALL));

	m_uWmTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

	m_hCursorArrow = LoadCursor(NULL, IDC_ARROW);
	m_hCursorHand = LoadCursor(NULL, IDC_HAND);

	m_lpWindow = new CAppWindow();

	SyncProxySettings();

	CSettings vSettings(FALSE);

	if (!vSettings.GetPlaySoundOnNewWave(m_fPlaySoundOnNewWave))
	{
		m_fPlaySoundOnNewWave = TRUE;
	}

	if (!vSettings.GetNotificationWhenOnline(m_fNotificationWhenOnline))
	{
		m_fNotificationWhenOnline = TRUE;
	}

	if (!vSettings.GetBrowser(m_szBrowser))
	{
		m_szBrowser = CBrowser::BrowserDefault;
	}
}

CNotifierApp::~CNotifierApp()
{
	DestroyIcon(m_hNotifyIcon);
	DestroyIcon(m_hNotifyIconGray);
	DestroyIcon(m_hNotifyIconGray1);
	DestroyIcon(m_hNotifyIconGray2);
	DestroyIcon(m_hNotifyIconGray3);
	DestroyIcon(m_hNotifyIconUnread);
	DestroyIcon(m_hMainIcon);
	DestroyIcon(m_hMainIconSmall);

	DestroyCursor(m_hCursorArrow);
	DestroyCursor(m_hCursorHand);

	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	CCurl::SetProxySettings(NULL);
}

BOOL CNotifierApp::Initialise()
{
	return TRUE;
}

INT CNotifierApp::Execute()
{
	GetAppWindow()->Create();

	return CApp::Execute();
}

void CNotifierApp::SetStartWithWindows(BOOL fValue)
{
	if (fValue)
	{
		CreateShortcut();
	}
	else
	{
		if (!m_szShortcutTargetPath.empty())
		{
			DeleteFile(m_szShortcutTargetPath.c_str());
		}
	}
}

void CNotifierApp::SyncProxySettings()
{
	CSettings vSettings(FALSE);
	CCurlProxySettings * lpProxySettings = NULL;

	BOOL fHaveSettings;
	wstring szHost;
	DWORD dwPort;
	BOOL fAuthenticated;
	wstring szUsername;
	wstring szPassword;

	BOOL fSuccess =
		vSettings.GetProxyHaveSettings(fHaveSettings) &&
		vSettings.GetProxyHost(szHost) &&
		vSettings.GetProxyPort(dwPort) &&
		vSettings.GetProxyAuthenticated(fAuthenticated) &&
		vSettings.GetProxyUsername(szUsername) &&
		vSettings.GetProxyPassword(szPassword);

	if (fSuccess && fHaveSettings)
	{
		if (fAuthenticated)
		{
			lpProxySettings = new CCurlProxySettings(szHost, dwPort, szUsername, szPassword);
		}
		else
		{
			lpProxySettings = new CCurlProxySettings(szHost, dwPort);
		}
	}

	CCurl::SetProxySettings(lpProxySettings);
}

void CNotifierApp::Restart()
{
	wstring szPath(GetModuleFileNameEx());

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(PROCESS_INFORMATION);

	LPWSTR szMutableCommandLine = (LPWSTR)malloc(sizeof(WCHAR) * (szPath.size() + 1));

	wcscpy(szMutableCommandLine, szPath.c_str());

	CreateProcess(
		szPath.c_str(),
		szMutableCommandLine,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		NULL,
		&si,
		&pi);

	free(szMutableCommandLine);
}

void CNotifierApp::DetectStartWithWindowsSetting()
{
	m_szShortcutTargetPath = L"";

	WCHAR szPath[MAX_PATH];
	wstring szModulePath(GetLongPathName(GetModuleFileNameEx()));

	if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_STARTUP , FALSE))
	{
		return;
	}

	wstring szFindPath(szPath);

	szFindPath += L"\\*.lnk";

	WIN32_FIND_DATA wfd;

	HANDLE hFind = FindFirstFile(szFindPath.c_str(), &wfd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}

			wstring szFilename(szPath);

			szFilename += L"\\";
			szFilename += wfd.cFileName;

			if (DetectShortcut(szModulePath, szFilename))
			{
				m_szShortcutTargetPath = szFilename;
				break;
			}
		}
		while (FindNextFile(hFind, &wfd) != 0);
	}

	FindClose(hFind);

	CSettings(TRUE).SetStartWithWindows(!m_szShortcutTargetPath.empty());
}

BOOL CNotifierApp::DetectShortcut(const wstring & szModulePath, const wstring & szFilename)
{
	HRESULT hr;
	IShellLink * lpShellLink = NULL;
	IPersistFile * lpPersistFile = NULL;
	BOOL fSuccess = FALSE;
	wstring szLongTargetPath;

	hr = CoCreateInstance(
		CLSID_ShellLink,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IShellLink,
		reinterpret_cast<void**>(&lpShellLink)
	);

	if (!SUCCEEDED(hr))
	{
		goto __end;
	}

	ASSERT(lpShellLink != NULL);

	hr = lpShellLink->QueryInterface(
		IID_IPersistFile,
		reinterpret_cast<void**>(&lpPersistFile)
	);

	if (!SUCCEEDED(hr))
	{
		goto __end;
	}

	ASSERT(lpPersistFile != NULL);

	hr = lpPersistFile->Load(szFilename.c_str(), STGM_READ | STGM_SHARE_DENY_NONE);

	if (!SUCCEEDED(hr))
	{
		goto __end;
	}

	WCHAR szTargetPath[MAX_PATH];
	WIN32_FIND_DATA wfd;

	hr = lpShellLink->GetPath(szTargetPath, MAX_PATH, &wfd, 0);

	if (!SUCCEEDED(hr))
	{
		goto __end;
	}

	szLongTargetPath = ExpandEnvironmentStrings(GetLongPathName(szTargetPath));

	fSuccess = _wcsicmp(szModulePath.c_str(), szLongTargetPath.c_str()) == 0;

__end:
	if (lpPersistFile != NULL)
	{
		lpPersistFile->Release();
	}

	if (lpShellLink != NULL)
	{
		lpShellLink->Release();
	}

	return fSuccess;
}

void CNotifierApp::CreateShortcut()
{
	HRESULT hr;
	IShellLink * lpShellLink = NULL;
	IPersistFile * lpPersistFile = NULL;
	WCHAR szStartupPath[MAX_PATH];
	wstring szModuleFilename(GetModuleFileNameEx());
	wstring szModulePath(GetDirname(szModuleFilename));

	if (!SHGetSpecialFolderPath(NULL, szStartupPath, CSIDL_STARTUP , FALSE))
	{
		return;
	}

	wstring szTargetPath(szStartupPath);

	szTargetPath += L"\\Google Wave Notifier.lnk";

	hr = CoCreateInstance(
		CLSID_ShellLink,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IShellLink,
		reinterpret_cast<void**>(&lpShellLink)
	);

	if (!SUCCEEDED(hr))
	{
		goto __end;
	}

	ASSERT(lpShellLink != NULL);

	lpShellLink->SetIconLocation(szModuleFilename.c_str(), 0);
	lpShellLink->SetPath(szModuleFilename.c_str());
	lpShellLink->SetWorkingDirectory(szModulePath.c_str());

	hr = lpShellLink->QueryInterface(
		IID_IPersistFile,
		reinterpret_cast<void**>(&lpPersistFile)
	);

	if (!SUCCEEDED(hr))
	{
		goto __end;
	}

	ASSERT(lpPersistFile != NULL);

	// hr not checked because we can't do anything about it.

	lpPersistFile->Save(szTargetPath.c_str(), FALSE);

__end:
	if (lpPersistFile != NULL)
	{
		lpPersistFile->Release();
	}

	if (lpShellLink != NULL)
	{
		lpShellLink->Release();
	}
}

void CNotifierApp::OpenUrl(wstring szUrl)
{
	CBrowser::OpenUrl(m_szBrowser, szUrl);
}

void CNotifierApp::QueueRequest(CCurl * lpRequest)
{
	GetAppWindow()->QueueRequest(lpRequest);
}

void CNotifierApp::CancelRequest(CCurl * lpRequest)
{
	GetAppWindow()->CancelRequest(lpRequest);
}
