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

#ifndef _INC_NOTIFIERAPP
#define _INC_NOTIFIERAPP

#pragma once

#include "version.h"
#include "wave.h"

typedef map<wstring, CDateTime> TStringDateTimeMap;
typedef TStringDateTimeMap::iterator TStringDateTimeMapIter;
typedef TStringDateTimeMap::const_iterator TStringDateTimeMapConstIter;

class CAppWindow : public CWindow
{
private:
	HMENU m_hPopupMenus;
	CNotifyIcon * m_lpNotifyIcon;
	BOOL m_fWorking;
	INT m_nWorkingCount;
	CWaveView * m_lpView;
	CWaveCollection * m_lpReportedView;
	TStringDateTimeMap m_vReportedTimes;
	TStringBoolMap m_vRequestedContacts;
	CWaveSession * m_lpSession;
	CCurlMonitor * m_lpMonitor;
	BOOL m_fQuitting;
	BOOL m_fManualUpdateCheck;
	CTimerCollection * m_lpTimers;
	CTimer * m_lpWorkingTimer;
	CTimer * m_lpVersionTimer;
	BOOL m_fReceivedFirstContactUpdates;
	CCurl * m_lpAvatarRequest;
	wstring m_szRequestingAvatar;
	BOOL m_fClientSuspended;
	BOOL m_fClientLocked;

public:
	CAppWindow();
	~CAppWindow();

	CNotifyIcon * GetNotifyIcon() const { return m_lpNotifyIcon; }
	void HaveReportedWave(wstring szWaveID);
	CWaveContact * GetWaveContact(wstring szEmailAddress);
	void DisplayHelp();
	void QueueRequest(CCurl * lpRequest);
	void CancelRequest(CCurl * lpRequest);
	void Login(wstring szUsername, wstring szPassword);
	BOOL LoginFromRegistry();
	void SignOut(BOOL fManual);
	CWaveSession * GetSession() const { return m_lpSession; }

protected:
	ATOM CreateClass(LPWNDCLASSEX lpWndClass);
	HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu);
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnCreate();
	LRESULT OnNotifyIcon(UINT uMessage, UINT uID);
	LRESULT OnCommand(WORD wID);
	LRESULT OnWaveConnectionState(WAVE_CONNECTION_STATE nStatus, LPARAM lParam);
	LRESULT OnCurlResponse(CURL_RESPONSE nState, LPARAM lParam);
	LRESULT OnLoginStateChanged(WAVE_CONNECTION_STATE nStatus, WAVE_LOGIN_ERROR nError);
	LRESULT OnVersionState(VERSION_STATE nState);
	LRESULT OnClose();
	LRESULT OnWTSSessionChange(WPARAM wParam);
	LRESULT OnPowerBroadcast(WPARAM wParam);
	LRESULT OnEndSession(BOOL fClose, LPARAM lParameter);

	void ShowFlyout();
	void ShowContextMenu();
	void CheckForUpdates();
	void OpenInbox();
	BOOL ActivateOpenDialog();
	void ProcessResponse(CWaveResponse * lpResponse);
	void ProcessReconnecting();
	void SynchronisePopups(CUnreadWaveCollection * lpUnreads, BOOL fManual);
	void TruncateLastReported();
	void DisplayWavePopups(BOOL fManual);
	void CheckWavesNow();
	void UpdateWorkingIcon();
	void CheckApplicationUpdated();
	void PromptForCredentials();
	void ProcessUnreadWavesNotifyIcon(INT nUnreadWaves);
	void ProcessLoggedOn();
	void ProcessSignedOut();
	void StartWorking();
	void StopWorking();
	BOOL AllowContextMenu();
	void ReportContactUpdates(CWaveContactStatusCollection * lpStatuses);
	void SeedAvatars();
	void ProcessAvatarResponse();
	void ClientConnected(CONNECT_REASON nReason);
	void ClientDisconnected(CONNECT_REASON nReason);
	void ReportContactOnline(CWaveContact * lpContact, BOOL fOnline);
	LRESULT ProcessCurlDataReceived(LPCURL_DATA_RECEIVED lpParam);
	LRESULT ProcessCurlCompleted(CCurl * lpCurl);

private:
	friend class CWorkingTimer;
};

class CNotifierApp : public CApp
{
private:
	HICON m_hNotifyIcon;
	HICON m_hNotifyIconGray;
	HICON m_hNotifyIconGray1;
	HICON m_hNotifyIconGray2;
	HICON m_hNotifyIconGray3;
	HICON m_hNotifyIconUnread;
	UINT m_uWmTaskbarCreated;
	HICON m_hMainIcon;
	HICON m_hMainIconSmall;
	HCURSOR m_hCursorArrow;
	HCURSOR m_hCursorHand;
	CAppWindow * m_lpWindow;
	wstring m_szShortcutTargetPath;
	BOOL m_fPlaySoundOnNewWave;
	wstring m_szBrowser;
	BOOL m_fNotificationWhenOnline;
	BOOL m_fConnected;

public:
	CNotifierApp(HINSTANCE hInstance, wstring szCmdLine);
	~CNotifierApp();

	INT Execute();
	HICON GetNotifyIcon() const { return m_hNotifyIcon; }
	HICON GetNotifyIconGray() const { return m_hNotifyIconGray; }
	HICON GetNotifyIconGray1() const { return m_hNotifyIconGray1; }
	HICON GetNotifyIconGray2() const { return m_hNotifyIconGray2; }
	HICON GetNotifyIconGray3() const { return m_hNotifyIconGray3; }
	HICON GetNotifyIconUnread() const { return m_hNotifyIconUnread; }
	HICON GetMainIcon() const { return m_hMainIcon; }
	HICON GetMainIconSmall() const { return m_hMainIconSmall; }
	UINT GetWmTaskbarCreated() const { return m_uWmTaskbarCreated; }
	HCURSOR GetCursorArrow() const { return m_hCursorArrow; }
	HCURSOR GetCursorHand() const { return m_hCursorHand; }
	CWaveSession * GetSession() const { return GetAppWindow()->GetSession(); }
	wstring GetShortcutTargetPath() const { return m_szShortcutTargetPath; }
	CAppWindow * GetAppWindow() const {
		ASSERT(m_lpWindow != NULL);
		return m_lpWindow;
	}
	BOOL Initialise();
	CWaveContact * GetWaveContact(wstring szEmailAddress) const {
		return GetAppWindow()->GetWaveContact(szEmailAddress);
	}
	void SetStartWithWindows(BOOL fValue);
	void SetPlaySoundOnNewWave(BOOL fValue) { m_fPlaySoundOnNewWave = fValue; }
	BOOL GetPlaySoundOnNewWave() const { return m_fPlaySoundOnNewWave; }
	void SetNotificationWhenOnline(BOOL fValue) { m_fNotificationWhenOnline = fValue; }
	BOOL GetNotificationWhenOnline() const { return m_fNotificationWhenOnline; }
	wstring GetBrowser() const { return m_szBrowser; }
	void SetBrowser(wstring szBrowser) {
		m_szBrowser = szBrowser;
	}
	void SyncProxySettings();
	void DetectStartWithWindowsSetting();
	void OpenUrl(wstring szUrl);
	void QueueRequest(CCurl * lpRequest);
	void CancelRequest(CCurl * lpRequest);
	BOOL GetConnected() const { return m_fConnected; }
	void SetConnected(BOOL fValue) { m_fConnected = fValue; }

	static CNotifierApp * Instance() { return (CNotifierApp *)CApp::Instance(); }
	static void Restart();

protected:
	int PerformMessageBox(HWND hWnd, wstring szText, wstring szCaption, UINT uType);

private:
	BOOL DetectShortcut(const wstring & szModulePath, const wstring & szFilename);
	void CreateShortcut();
};

#endif // _INC_NOTIFIERAPP
