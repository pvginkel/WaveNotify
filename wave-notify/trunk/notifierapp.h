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

#define TIMER_VERSION			1
#define	TIMER_RECONNECTING		2

#define TIMER_QUERY_INTERVAL		(2 * 60 * 1000)		// 2 minutes
#define TIMER_VERSION_INTERVAL		(60 * 60 * 1000)	// 1 hour
#define TIMER_RECONNECTING_INTERVAL	900

#define REREPORT_TIMEOUT		(3 * 60 * 1000)		// 3 minutes

typedef map<wstring, CDateTime> TStringDateTimeMap;
typedef TStringDateTimeMap::iterator TStringDateTimeMapIter;
typedef TStringDateTimeMap::const_iterator TStringDateTimeMapConstIter;

class CAppWindow : public CWindow
{
private:
	HMENU m_hPopupMenus;
	CNotifyIcon * m_lpNotifyIcon;
	BOOL m_fReconnecting;
	INT m_nReconnectingCount;
	CWaveView * m_lpView;
	CWaveCollection * m_lpReportedView;
	TStringDateTimeMap m_vReportedTimes;
	TStringBoolMap m_vRequestedContacts;

public:
	CAppWindow();
	~CAppWindow();

	CNotifyIcon * GetNotifyIcon() const { return m_lpNotifyIcon; }
	void HaveReportedWave(wstring szWaveID);
	CWaveContact * GetWaveContact(wstring szEmailAddress);
	void DisplayHelp();

protected:
	ATOM CreateClass(LPWNDCLASSEX lpWndClass);
	HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu);
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnCreate();
	LRESULT OnNotifyIcon(UINT uMessage, UINT uID);
	LRESULT OnCommand(WORD wID);
	LRESULT OnListenerStatus(LISTENER_THREAD_STATUS nStatus, CWaveResponse * lpResponse);
	LRESULT OnTimer(WPARAM nTimerID);

	void ShowFlyout();
	void ShowContextMenu();
	void CheckForUpdates(BOOL fManual);
	void OpenInbox();
	void Login();
	void SignOut();
	BOOL ActivateOpenDialog();
	void ProcessResponse(CWaveResponse * lpResponse);
	void ProcessReconnecting();
	void ProcessConnected();
	void SynchronisePopups(CUnreadWaveCollection * lpUnreads);
	void TruncateLastReported();
	void DisplayWavePopups();
	void CheckWavesNow();
	void UpdateReconnectingIcon();
	void StopReconnecting();
	void CheckApplicationUpdated();
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
	CWaveSession * m_lpSession;
	CCurlCache * m_lpCurlCache;
	wstring m_szShortcutTargetPath;
	BOOL m_fPlaySoundOnNewWave;
	wstring m_szBrowser;

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
	CWaveSession * GetSession() const { return m_lpSession; }
	wstring GetShortcutTargetPath() const { return m_szShortcutTargetPath; }
	void SetWaveSession(CWaveSession * lpSession) {
		if (m_lpSession != NULL) delete m_lpSession;
		m_lpSession = lpSession;
		ProcessLoggedIn();
	}
	void Login();
	void SignOut();
	CAppWindow * GetAppWindow() const { return m_lpWindow; }
	BOOL IsLoggedIn() const { return m_lpSession != NULL && m_lpSession->IsLoggedIn(); }
	BOOL Initialise();
	CCurlCache * GetCurlCache() const { return m_lpCurlCache; }
	CWaveContact * GetWaveContact(wstring szEmailAddress) const { return m_lpWindow->GetWaveContact(szEmailAddress); }
	void SetStartWithWindows(BOOL fValue);
	void SetPlaySoundOnNewWave(BOOL fValue) { m_fPlaySoundOnNewWave = fValue; }
	BOOL GetPlaySoundOnNewWave() { return m_fPlaySoundOnNewWave; }
	wstring GetBrowser() { return m_szBrowser; }
	void SetBrowser(wstring szBrowser) { m_szBrowser = szBrowser; }
	void SyncProxySettings();
	void DetectStartWithWindowsSetting();
	void OpenUrl(wstring szUrl);

	static CNotifierApp * Instance() { return (CNotifierApp *)CApp::Instance(); }
	static void Restart();

private:
	CWaveSession * CreateWaveSessionFromRegistry();
	void PromptForCredentials();
	void ProcessUnreadWavesNotifyIcon(INT nUnreadWaves);
	void ProcessLoggedIn();
	BOOL DetectShortcut(const wstring & szModulePath, const wstring & szFilename);
	void CreateShortcut();
};

#endif // _INC_NOTIFIERAPP
