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
#include "flyouts.h"
#include "aboutdialog.h"
#include "optionssheet.h"
#include "settings.h"
#include "wave.h"
#include "popups.h"
#include "logindialog.h"
#include "layout.h"

CAppWindow::CAppWindow() : CWindow(L"GoogleWaveNotifier")
{
	m_hPopupMenus = NULL;
	m_nWorkingCount = 0;
	m_lpView = NULL;
	m_fQuitting = FALSE;
	m_fWorking = FALSE;
	m_fManualUpdateCheck = FALSE;
	m_fReceivedFirstContactUpdates = FALSE;
	m_lpAvatarRequest = NULL;
	m_fClientSuspended = FALSE;
	m_fClientLocked = FALSE;
	
	m_lpTimers = new CTimerCollection(this);

	m_lpMonitor = new CCurlMonitor(this);

	m_lpSession = new CWaveSession(this);
	m_lpSession->AddProgressTarget(this);

	CVersion::Instance()->SetTargetWindow(this);

	m_lpWorkingTimer = new CTimer(TIMER_WORKING_INTERVAL);

	m_lpWorkingTimer->Tick += AddressOf<CAppWindow>(this, &CAppWindow::UpdateWorkingIcon);

	m_lpVersionTimer = new CTimer(TIMER_VERSION_INTERVAL_INITIAL);

	m_lpVersionTimer->Tick += AddressOf<CAppWindow>(this, &CAppWindow::CheckForUpdates);

	// TODO: Deserialize the last reported here.

	m_lpReportedView = new CWaveCollection();

	m_lpNotifyIcon = new CNotifyIcon(
		this,
		ID_NOTIFYICON,
		L"Google Wave Notifier",
		CNotifierApp::Instance()->GetNotifyIconGray());
}

CAppWindow::~CAppWindow()
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	m_lpSession->RemoveProgressTarget(this);

	delete m_lpSession;

	delete m_lpMonitor;

	if (m_lpView != NULL)
	{
		delete m_lpView;
	}
	
	// TODO: Serialize the last reported here.

	delete m_lpReportedView;

	delete m_lpNotifyIcon;

	delete m_lpWorkingTimer;
	delete m_lpVersionTimer;

	delete m_lpTimers;

	CVersion::Instance()->SetTargetWindow(NULL);

	PostQuitMessage(0);
}

ATOM CAppWindow::CreateClass(LPWNDCLASSEX lpWndClass)
{
	ASSERT(lpWndClass != NULL);

	lpWndClass->hIcon = CNotifierApp::Instance()->GetMainIcon();
	lpWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
	lpWndClass->hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	return CWindow::CreateClass(lpWndClass);
}

HWND CAppWindow::CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu)
{
	return CWindow::CreateHandle(
		dwExStyle,
		L"GoogleNotifier",
		WS_OVERLAPPEDWINDOW,
		x, y, cx, cy,
		lpParentWindow,
		hMenu);
}

LRESULT CAppWindow::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_CREATE:
		return OnCreate();

	case WM_NOTIFTICON:
		return OnNotifyIcon((UINT)lParam, (UINT)wParam);

	case WM_COMMAND:
		return OnCommand(LOWORD(wParam));

	case WM_WAVE_CONNECTION_STATE:
		return OnWaveConnectionState((WAVE_CONNECTION_STATE)wParam, lParam);

	case WM_TIMER:
		m_lpTimers->Process((UINT_PTR)wParam);
		return 0;

	case WM_DESTROY:
		m_lpNotifyIcon->Destroy();
		return 0;

	case WM_CURL_RESPONSE:
		return OnCurlResponse((CURL_RESPONSE)wParam, lParam);

	case WM_VERSION_STATE:
		return OnVersionState((VERSION_STATE)wParam);

	case WM_CLOSE:
		return OnClose();

	case WM_WTSSESSION_CHANGE:
		return OnWTSSessionChange(wParam);

	case WM_POWERBROADCAST:
		return OnPowerBroadcast(wParam);

	default:
		if (uMessage == CNotifierApp::Instance()->GetWmTaskbarCreated())
		{
			m_lpNotifyIcon->Recreate();
			return 0;
		}
		break;
	}

	return CWindow::WndProc(uMessage, wParam, lParam);
}

LRESULT CAppWindow::OnCreate()
{
	m_lpNotifyIcon->Create();

	if (!LoginFromRegistry())
	{
		PromptForCredentials();
	}

	Compat_WTSRegisterSessionNotification(GetHandle(), NOTIFY_FOR_THIS_SESSION);
	
	m_lpVersionTimer->SetRunning(TRUE);

	CheckApplicationUpdated();

	return 0;
}

LRESULT CAppWindow::OnClose()
{
	m_fQuitting = TRUE;

	CVersion::Instance()->CancelRequests();

	Compat_WTSUnRegisterSessionNotification(GetHandle());

	switch (m_lpSession->GetState())
	{
	case WSS_ONLINE:
	case WSS_RECONNECTING:
	case WSS_CONNECTING:
		SignOut(FALSE);
		break;

	case WSS_DISCONNECTING:
		// Already signing out, let it go and destroy from there.
		break;

	case WSS_OFFLINE:
		// Already offline, destroy the window
		DestroyWindow(GetHandle());
		break;
	}
	
	return 0;
}

LRESULT CAppWindow::OnWTSSessionChange(WPARAM wParam)
{
	switch (wParam)
	{
	case WTS_CONSOLE_CONNECT:
	case WTS_REMOTE_CONNECT:
	case WTS_SESSION_UNLOCK:
		ClientConnected(CR_LOCK);
		break;

	case WTS_CONSOLE_DISCONNECT:
	case WTS_REMOTE_DISCONNECT:
	case WTS_SESSION_LOCK:
		ClientDisconnected(CR_LOCK);
		break;
	}

	return 0;
}

LRESULT CAppWindow::OnPowerBroadcast(WPARAM wParam)
{
	switch (wParam)
	{
	case PBT_APMSUSPEND:
		ClientDisconnected(CR_SUSPEND);
		break;

	case PBT_APMRESUMESUSPEND:
	case PBT_APMRESUMEAUTOMATIC:
		ClientConnected(CR_SUSPEND);
		break;
	}

	return TRUE;
}

LRESULT CAppWindow::OnNotifyIcon(UINT uMessage, UINT uID)
{
	switch (uMessage)
	{
	case WM_LBUTTONUP:
		if (!ActivateOpenDialog())
		{
			if (m_lpSession->GetState() == WSS_ONLINE)
			{
				ShowFlyout();
			}
		}
		break;

	case WM_RBUTTONUP:
		if (!ActivateOpenDialog())
		{
			ShowContextMenu();
		}
		break;
	}

	return 0;
}

void CAppWindow::ShowFlyout()
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	if (m_lpView != NULL)
	{
		CUnreadWaveCollection * lpUnreads = CUnreadWaveCollection::CreateUnreadWaves(NULL, m_lpView->GetWaves());

		(new CUnreadWavesFlyout(lpUnreads))->Create();
	}
}

void CAppWindow::ShowContextMenu()
{
	if (!AllowContextMenu())
	{
		return;
	}

	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	if (m_hPopupMenus != NULL)
	{
		DestroyMenu(m_hPopupMenus);
	}

	m_hPopupMenus = LoadMenu(CApp::Instance()->GetInstance(), MAKEINTRESOURCE(IDR_POPUP_MENUS));

	HMENU hSubMenu = GetSubMenu(m_hPopupMenus, 0);

	if (m_lpSession->GetState() == WSS_ONLINE || m_lpSession->GetState() == WSS_RECONNECTING)
	{
		DeleteMenu(hSubMenu, ID_TRAYICON_LOGIN, MF_BYCOMMAND);
	}
	else
	{
		DeleteMenu(hSubMenu, ID_TRAYICON_SIGNOUT, MF_BYCOMMAND);

		EnableMenuItem(hSubMenu, ID_TRAYICON_INBOX, MF_GRAYED | MF_BYCOMMAND);

		EnableMenuItem(hSubMenu, ID_TRAYICON_CHECKWAVESNOW, MF_GRAYED | MF_BYCOMMAND);
	}

	if (CVersion::Instance()->GetState() != VS_NONE)
	{
		EnableMenuItem(hSubMenu, ID_TRAYICON_CHECKFORUPDATESNOW, MF_GRAYED | MF_BYCOMMAND);
	}

	POINT p;

	GetCursorPos(&p);

	SetForegroundWindow(GetHandle());

	TrackPopupMenuEx(
		hSubMenu,
		TPM_VERTICAL | TPM_RIGHTALIGN,
		p.x,
		p.y,
		GetHandle(),
		NULL);

	PostMessage(WM_NULL);
}

LRESULT CAppWindow::OnCommand(WORD wID)
{
	switch (wID)
	{
	case ID_TRAYICON_EXIT:
		SendMessage(WM_CLOSE);
		break;

	case ID_TRAYICON_CHECKFORUPDATESNOW:
		m_fManualUpdateCheck = TRUE;

		CheckForUpdates();
		break;

	case ID_TRAYICON_INBOX:
		OpenInbox();
		break;

	case ID_TRAYICON_LOGIN:
		PromptForCredentials();
		break;

	case ID_TRAYICON_SIGNOUT:
		SignOut(TRUE);
		break;

	case ID_TRAYICON_HELP:
		DisplayHelp();
		break;

	case ID_TRAYICON_ABOUT:
		(new CAboutDialog())->Create(DT_ABOUT, this);
		break;

	case ID_TRAYICON_OPTIONS:
		(new COptionsSheet())->Create(DT_OPTIONS, this);
		break;

	case ID_TRAYICON_CHECKWAVESNOW:
		CheckWavesNow();
		break;
	}

	return 0;
}

void CAppWindow::CheckForUpdates()
{
	m_lpVersionTimer->SetRunning(FALSE);

	CVersion::Instance()->CheckVersion();
}

void CAppWindow::OpenInbox()
{
	CNotifierApp::Instance()->OpenUrl(m_lpSession->GetInboxUrl());
}

void CAppWindow::SignOut(BOOL fManual)
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	m_lpSession->SignOut();

	if (fManual)
	{
		CSettings(TRUE).DeleteGooglePassword();
	}
}

BOOL CAppWindow::ActivateOpenDialog()
{
	CWindowHandle * lpWindow = NULL;

	if (CModelessDialogs::ContainsType(DT_ABOUT))
	{
		lpWindow = CModelessDialogs::GetDialog(DT_ABOUT);
	}
	else if (CModelessDialogs::ContainsType(DT_LOGIN))
	{
		lpWindow = CModelessDialogs::GetDialog(DT_LOGIN);
	}
	else if (CModelessPropertySheets::ContainsType(DT_OPTIONS))
	{
		lpWindow = CModelessPropertySheets::GetSheet(DT_OPTIONS);
	}

	if (lpWindow == NULL)
	{
		return FALSE;
	}
	else
	{
		SetForegroundWindow(lpWindow->GetHandle());

		return TRUE;
	}
}

LRESULT CAppWindow::OnWaveConnectionState(WAVE_CONNECTION_STATE nState, LPARAM lParam)
{
	CHECK_ENUM(nState, WCS_MAX);

	switch (nState)
	{
	case WCS_RECEIVED:
		ProcessResponse((CWaveResponse *)lParam);
		return 0;

	default:
		return OnLoginStateChanged(nState, (WAVE_LOGIN_ERROR)lParam);
	}
}

LRESULT CAppWindow::OnLoginStateChanged(WAVE_CONNECTION_STATE nStatus, WAVE_LOGIN_ERROR nError)
{
	CHECK_ENUM(nStatus, WCS_MAX);
	CHECK_ENUM(nError, WLE_MAX);

	if (!AllowContextMenu())
	{
		EndMenu();
	}

	switch (nStatus)
	{
	case WCS_BEGIN_LOGON:
	case WCS_BEGIN_SIGNOUT:
		StartWorking();
		break;

	case WCS_LOGGED_ON:
		StopWorking();
		ProcessLoggedOn();
		break;

	case WCS_RECONNECTING:
		StartWorking();
		ProcessReconnecting();
		break;

	case WCS_CONNECTED:
		StopWorking();
		ProcessConnected();
		break;

	case WCS_FAILED:
		StopWorking();
		break;

	case WCS_SIGNED_OUT:
		StopWorking();
		ProcessSignedOut();
		break;
	}

	return 0;
}

void CAppWindow::ProcessLoggedOn()
{
	m_lpNotifyIcon->SetIcon(CNotifierApp::Instance()->GetNotifyIcon());
}

void CAppWindow::ProcessSignedOut()
{
	m_lpNotifyIcon->SetIcon(CNotifierApp::Instance()->GetNotifyIconGray());

	if (m_fQuitting)
	{
		DestroyWindow(GetHandle());
	}
}

void CAppWindow::ProcessResponse(CWaveResponse * lpResponse)
{
	if (lpResponse != NULL)
	{
		if (lpResponse->GetType() == WMT_GET_CONTACT_DETAILS)
		{
			// We have received requested contact details. Update our internal map
			// of what contacts we have received.

			const TWaveContactMap & vContacts =
				((CWaveResponseGetContactDetails *)lpResponse)->GetContacts()->GetContacts();

			for (TWaveContactMapConstIter iter = vContacts.begin(); iter != vContacts.end(); iter++)
			{
				TStringBoolMapIter pos = m_vRequestedContacts.find(iter->first);

				if (pos != m_vRequestedContacts.end())
				{
					m_vRequestedContacts.erase(pos);
				}
			}
		}
		else if (lpResponse->GetType() == WMT_CONTACT_UPDATES)
		{
			if (m_fReceivedFirstContactUpdates)
			{
				if (CNotifierApp::Instance()->GetNotificationWhenOnline())
				{
					ReportContactUpdates(
						((CWaveResponseContactUpdates *)lpResponse)->GetStatuses());
				}
			}
			else
			{
				m_fReceivedFirstContactUpdates = TRUE;
			}
		}

		if (m_lpView != NULL)
		{
			// Integrate the response into the current view.

			m_lpView->ProcessResponse(lpResponse);

			// And display the new popups.

			DisplayWavePopups(FALSE);
		}

		if (lpResponse->GetType() == WMT_GET_CONTACT_DETAILS)
		{
			// Signal all running popups that we have received new
			// contact details and that they may want to repaint with
			// these contact details.

			if (CPopupWindow::Instance() != NULL)
			{
				TPopupVector vPopups;
				
				CPopupWindow::Instance()->GetPopups(vPopups);

				for (TPopupVectorIter iter = vPopups.begin(); iter != vPopups.end(); iter++)
				{
					if (((CPopupBase *)*iter)->GetType() == PT_WAVE)
					{
						CUnreadWavePopup * lpPopup = (CUnreadWavePopup *)*iter;

						lpPopup->ContactsUpdated(m_lpView->GetContacts());
					}
				}
			}

			// TODO: Signal the flyout
		}

		SeedAvatars();

		delete lpResponse;
	}
}

void CAppWindow::DisplayWavePopups(BOOL fManual)
{
	ASSERT(m_lpView != NULL);

	// Create a changelog of the current view and the last
	// reported view.

	CUnreadWaveCollection * lpUnreads = CUnreadWaveCollection::CreateUnreadWaves(
		m_lpReportedView, m_lpView->GetWaves());

	// Synchronize the changelog with the queued popups.

	SynchronisePopups(lpUnreads, fManual);

	// Remove all waves from the last reported that does
	// not appear in the current view.

	TruncateLastReported();

	// Update the notify icon.

	BOOL fHaveUnread = FALSE;

	const TWaveMap & vWaves = m_lpView->GetWaves()->GetWaves();

	for (TWaveMapConstIter iter = vWaves.begin(); iter != vWaves.end(); iter++)
	{
		if (iter->second->GetUnreadMessages() > 0)
		{
			fHaveUnread = TRUE;
			break;
		}
	}

	m_lpNotifyIcon->SetIcon(
		fHaveUnread ?
		CNotifierApp::Instance()->GetNotifyIconUnread() :
		CNotifierApp::Instance()->GetNotifyIcon()
	);

	delete lpUnreads;
}

void CAppWindow::TruncateLastReported()
{
	ASSERT(m_lpView != NULL);

	const TWaveMap & vReported = m_lpReportedView->GetWaves();
	const TWaveMap & vCurrent = m_lpView->GetWaves()->GetWaves();
	TStringVector vRemove;

	for (TWaveMapConstIter iter = vReported.begin(); iter != vReported.end(); iter++)
	{
		if (vCurrent.find(iter->first) == vCurrent.end())
		{
			vRemove.push_back(iter->first);
		}
	}

	m_lpReportedView->RemoveWaves(vRemove);
}

void CAppWindow::SynchronisePopups(CUnreadWaveCollection * lpUnreads, BOOL fManual)
{
	ASSERT(lpUnreads != NULL);

	BOOL fQueuedNewWaves = FALSE;
	TPopupVector vMustCancel;
	TStringVector vSeen;

	if (CPopupWindow::Instance() != NULL)
	{
		TPopupVector vPopups;
		
		CPopupWindow::Instance()->GetPopups(vPopups);

		for (TPopupVectorIter iter = vPopups.begin(); iter != vPopups.end(); iter++)
		{
			if (((CPopupBase *)(*iter))->GetType() == PT_WAVE)
			{
				CUnreadWavePopup * lpPopup = (CUnreadWavePopup *)*iter;
				wstring szPopupWaveId = lpPopup->GetUnread()->GetID();
				CUnreadWave * lpNewUnreadWave = lpUnreads->GetChange(szPopupWaveId);

				if (lpNewUnreadWave == NULL)
				{
					// Cancel the popup if it isn't changed anymore.

					vMustCancel.push_back(lpPopup);
				}
				else
				{
					// Else, update with the current changed status.

					lpPopup->UpdateUnread(lpNewUnreadWave);

					vSeen.push_back(szPopupWaveId);
				}
			}
		}
	}

	// Cancel all popups.

	for (TPopupVectorIter iter = vMustCancel.begin(); iter != vMustCancel.end(); iter++)
	{
		CPopup * lpPopup = *iter;

		if (!lpPopup->IsDisplaying())
		{
			lpPopup->Cancel();
		}
	}

	// Add all new popups.

	TUnreadWaveVector vUnreads = lpUnreads->GetChanges();

	// Waves can only be reported if has been reported within the
	// timeout period.

	CDateTime dtRereportLimit(CDateTime::Now() - CTimeSpan::FromMilliseconds((DOUBLE)TIMER_REREPORT_TIMEOUT));

	for (TUnreadWaveVectorIter iter1 = vUnreads.begin(); iter1 != vUnreads.end(); iter1++)
	{
		// If we haven't seen this before ...

		wstring szId((*iter1)->GetID());

		if (find(vSeen.begin(), vSeen.end(), szId) == vSeen.end())
		{
			// ... create a new popup

			TStringDateTimeMapIter pos = m_vReportedTimes.find(szId);

			// Verify whether this popup hasn't been reported for
			// the required time.

			if (pos == m_vReportedTimes.end() || pos->second < dtRereportLimit)
			{
				CUnreadWavePopup * lpPopup = new CUnreadWavePopup(*iter1);

				lpPopup->Show();

				if (pos != m_vReportedTimes.end())
				{
					m_vReportedTimes.erase(pos);
				}

				// We've queued a new popup, so make a noise.

				fQueuedNewWaves = TRUE;
			}
			else
			{
				// Silence the wave by reporting it as reported.

				HaveReportedWave(szId);

				// Delete the unread wave because we're going
				// to detach everything lateron.

				delete *iter1;
			}
		}
	}

	// Detach all unread objects because the popups have taken
	// them over

	lpUnreads->DetachAll();

	// Re-index the popups to correctly display the index and count number
	// shown of the popups.

	if (CPopupWindow::Instance() != NULL)
	{
		TPopupVector vPopups;
		
		CPopupWindow::Instance()->GetPopups(vPopups);

		UINT uCount = 0;

		for (TPopupVectorIter iter = vPopups.begin(); iter != vPopups.end(); iter++)
		{
			if (((CPopupBase *)(*iter))->GetType() == PT_WAVE)
			{
				uCount++;
			}
		}

		UINT uIndex = 1;

		for (TPopupVectorIter iter1 = vPopups.begin(); iter1 != vPopups.end(); iter1++)
		{
			if (((CPopupBase *)(*iter1))->GetType() == PT_WAVE)
			{
				CUnreadWavePopup * lpPopup = (CUnreadWavePopup *)*iter1;

				lpPopup->SetCountIndex(uIndex, uCount);

				uIndex++;
			}
		}
	}

	if (fManual)
	{
		// When the check was queued manually, we will report it when
		// there were no unread waves to report.

		if (!fQueuedNewWaves)
		{
			(new CMessagePopup(L"No unread Waves."))->Show();
		}
	}
	else
	{
		// When the check was not done manually, we will play a sound
		// when there were unread waves to report.

		if (fQueuedNewWaves && CNotifierApp::Instance()->GetPlaySoundOnNewWave())
		{
			PlaySound(
				MAKEINTRESOURCE(IDR_NEWWAVE),
				CNotifierApp::Instance()->GetInstance(),
				SND_ASYNC | SND_NOSTOP | SND_NOWAIT | SND_RESOURCE);
		}
	}
}

void CAppWindow::ProcessReconnecting()
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	if (m_lpView != NULL)
	{
		delete m_lpView;

		m_lpView = NULL;
	}
}

void CAppWindow::UpdateWorkingIcon()
{
	HICON hIcon;

	switch (m_nWorkingCount % 3)
	{
	case 0:
		hIcon = CNotifierApp::Instance()->GetNotifyIconGray1();
		break;

	case 1:
		hIcon = CNotifierApp::Instance()->GetNotifyIconGray2();
		break;

	default:
		hIcon = CNotifierApp::Instance()->GetNotifyIconGray3();
		break;
	}

	m_lpNotifyIcon->SetIcon(hIcon);

	m_nWorkingCount++;
}

void CAppWindow::ProcessConnected()
{
	if (m_lpView != NULL)
	{
		delete m_lpView;
	}

	m_lpView = new CWaveView();

	m_lpSession->QueueRequest(new CWaveRequestGetAllContacts());

	CWaveRequestGetContactDetails * lpRequest = new CWaveRequestGetContactDetails();

	wstring szEmailAddress(m_lpSession->GetEmailAddress());

	lpRequest->AddEmailAddress(szEmailAddress);

	m_lpSession->QueueRequest(lpRequest);

	m_lpSession->QueueRequest(new CWaveRequestStartListening(L"in:inbox"));

	m_fReceivedFirstContactUpdates = FALSE;

	m_lpSession->QueueRequest(new CWaveRequestContactUpdates());

	m_lpSession->FlushRequestQueue();
}

void CAppWindow::HaveReportedWave(wstring szWaveID)
{
	CHECK_NOT_EMPTY(szWaveID);

	ASSERT(m_lpView != NULL);

	TWaveMap vWaves = m_lpView->GetWaves()->GetWaves();

	TWaveMapIter pos = vWaves.find(szWaveID);

	if (pos != vWaves.end())
	{
		CWave * lpWave = new CWave(*pos->second);

		m_lpReportedView->AddWave(lpWave);
	}

	if (m_vReportedTimes.find(szWaveID) == m_vReportedTimes.end())
	{
		m_vReportedTimes[szWaveID] = CDateTime::Now();
	}
}


CWaveContact * CAppWindow::GetWaveContact(wstring szEmailAddress)
{
	if (m_lpView == NULL)
	{
		return NULL;
	}

	CWaveContact * lpContact = m_lpView->GetContacts()->GetContact(szEmailAddress);

	if (lpContact == NULL && m_vRequestedContacts.find(szEmailAddress) == m_vRequestedContacts.end())
	{
		CWaveRequestGetContactDetails * lpRequest = new CWaveRequestGetContactDetails();

		lpRequest->AddEmailAddress(szEmailAddress);

		m_lpSession->QueueRequest(lpRequest);

		m_lpSession->FlushRequestQueue();

		m_vRequestedContacts[szEmailAddress] = TRUE;
	}

	return lpContact;
}

void CAppWindow::CheckWavesNow()
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	delete m_lpReportedView;

	m_lpReportedView = new CWaveCollection();

	m_vReportedTimes.clear();

	DisplayWavePopups(TRUE);
}

void CAppWindow::CheckApplicationUpdated()
{
	wstring szVersion;

	if (CSettings(FALSE).GetInstalledVersion(szVersion))
	{
		wstring szNewVersion(CVersion::GetAppVersion());

		if (szVersion != szNewVersion)
		{
			CMessagePopup * lpPopup = new CMessagePopup(
				L"Google Wave Notifier has been updated. Click here to read about new features."
			);

			lpPopup->SetUrl(CHANGELOG_LINK);
			lpPopup->SetDuration(8000);

			lpPopup->Show();

			CSettings(TRUE).SetInstalledVersion(szNewVersion);
		}
	}
}

void CAppWindow::DisplayHelp()
{
	wstringstream szUrl;

	szUrl << MANUAL_LINK << L"?version=" << UrlEncode(CVersion::GetAppVersion());

	LPCWSTR szLanguageCode = GetLanguageCode();

	if (szLanguageCode != NULL)
	{
		szUrl << L"&lang=" << szLanguageCode;
	}
	
	CNotifierApp::Instance()->OpenUrl(szUrl.str());
}

void CAppWindow::Login(wstring szUsername, wstring szPassword)
{
	CHECK_NOT_EMPTY(szUsername);
	CHECK_NOT_EMPTY(szPassword);

	m_lpSession->Login(szUsername, szPassword);
}

BOOL CAppWindow::LoginFromRegistry()
{
	CSettings vSettings(FALSE);

	wstring szUsername;
	wstring szPassword;

	if (
		vSettings.GetGoogleUsername(szUsername) &&
		vSettings.GetGooglePassword(szPassword)
	) {
		if (!szUsername.empty() && !szPassword.empty())
		{
			Login(szUsername, szPassword);

			return TRUE;
		}
	}

	return FALSE;
}

void CAppWindow::PromptForCredentials()
{
	(new CLoginDialog(this))->Create(DT_LOGIN, CNotifierApp::Instance()->GetAppWindow());
}

void CAppWindow::ProcessUnreadWavesNotifyIcon(INT nUnreadWaves)
{
	m_lpNotifyIcon->SetIcon(
		nUnreadWaves > 0 ?
		CNotifierApp::Instance()->GetNotifyIconUnread() :
		CNotifierApp::Instance()->GetNotifyIcon()
	);
}

void CAppWindow::QueueRequest(CCurl * lpRequest)
{
	m_lpMonitor->QueueRequest(lpRequest);
}

void CAppWindow::CancelRequest(CCurl * lpRequest)
{
	m_lpMonitor->CancelRequest(lpRequest);
}

LRESULT CAppWindow::OnCurlResponse(CURL_RESPONSE nState, LPARAM lParam)
{
	CHECK_ENUM(nState, CR_MAX);

	switch (nState)
	{
	case CR_DATA_RECEIVED:
		return ProcessCurlDataReceived((LPCURL_DATA_RECEIVED)lParam);

	case CR_COMPLETED:
		return ProcessCurlCompleted((CCurl *)lParam);

	default:
		LOG1("Unknown CURL_RESPONSE state %d", nState);
		return 0;
	}
}

LRESULT CAppWindow::ProcessCurlDataReceived(LPCURL_DATA_RECEIVED lpParam)
{
	ASSERT(lpParam != NULL);

	return lpParam->lpReader->Read(lpParam->lpData, lpParam->cbData);
}

LRESULT CAppWindow::ProcessCurlCompleted(CCurl * lpCurl)
{
	ASSERT(lpCurl != NULL);

	if (m_lpAvatarRequest != NULL && m_lpAvatarRequest == lpCurl)
	{
		ProcessAvatarResponse();
	}
	else if (
		!m_lpSession->ProcessCurlResponse(lpCurl) &&
		!CVersion::Instance()->ProcessCurlResponse(lpCurl)
	) {
		//LOG("Could not process curl response");

		CCurl::Destroy(lpCurl);
	}

	return 0;
}

void CAppWindow::StartWorking()
{
	if (!m_fWorking)
	{
		m_nWorkingCount = 0;
		m_fWorking = TRUE;

		UpdateWorkingIcon();

		m_lpWorkingTimer->SetRunning(TRUE);
	}
}

void CAppWindow::StopWorking()
{
	if (m_fWorking)
	{
		m_fWorking = FALSE;

		m_lpWorkingTimer->SetRunning(FALSE);

		m_lpNotifyIcon->SetIcon(
			m_lpSession->GetState() == WSS_ONLINE ?
			CNotifierApp::Instance()->GetNotifyIcon() :
			CNotifierApp::Instance()->GetNotifyIconGray()
		);
	}
}

BOOL CAppWindow::AllowContextMenu()
{
	return !(m_lpSession->GetState() == WSS_CONNECTING || m_lpSession->GetState() == WSS_DISCONNECTING);
}

LRESULT CAppWindow::OnVersionState(VERSION_STATE nState)
{
	CHECK_ENUM(nState, VS_MAX);

	switch (nState)
	{
	case VS_DOWNLOADING:
		if (m_fManualUpdateCheck)
		{
			(new CMessagePopup(L"Downloading the latest version of Google Wave Notifier."))->Show();

			m_fManualUpdateCheck = FALSE;
		}
		break;

	case VS_NONE:
		if (m_fManualUpdateCheck)
		{
			(new CMessagePopup(L"Your version of Google Wave Notifier is up to date."))->Show();

			m_fManualUpdateCheck = FALSE;
		}

		m_lpVersionTimer->SetInterval(TIMER_VERSION_INTERVAL);
		m_lpVersionTimer->SetRunning(TRUE);
		break;

	case VS_AVAILABLE:
		SendMessage(WM_CLOSE);
		break;
	}

	return 0;
}

void CAppWindow::ReportContactUpdates(CWaveContactStatusCollection * lpStatuses)
{
	ASSERT(lpStatuses != NULL);

	wstring szSelf(m_lpSession->GetEmailAddress());

	const TWaveContactStatusMap & vStatuses = lpStatuses->GetStatuses();

	for (TWaveContactStatusMapConstIter iter = vStatuses.begin(); iter != vStatuses.end(); iter++)
	{
		CWaveContact * lpContact = GetWaveContact(iter->first);

		if (
			lpContact != NULL &&
			lpContact->GetEmailAddress() != szSelf &&
			lpContact->GetOnline() != iter->second->GetOnline()
		) {
			ReportContactOnline(lpContact, iter->second->GetOnline());
		}
	}
}

void CAppWindow::ReportContactOnline(CWaveContact * lpContact, BOOL fOnline)
{
	ASSERT(lpContact != NULL);

	CContactOnlinePopup * lpFound = NULL;

	if (CPopupWindow::Instance() != NULL)
	{
		TPopupVector vPopups;
		
		CPopupWindow::Instance()->GetPopups(vPopups);

		for (TPopupVectorIter iter = vPopups.begin(); iter != vPopups.end(); iter++)
		{
			if (((CPopupBase *)(*iter))->GetType() == PT_CONTACT_ONLINE)
			{
				CContactOnlinePopup * lpPopup = (CContactOnlinePopup *)*iter;

				if (lpPopup->GetContact()->GetEmailAddress() == lpContact->GetEmailAddress())
				{
					lpFound = lpPopup;
					break;
				}
			}
		}
	}

	// When there is an online popup of the to be reported contact, we are
	// not going to queue a new one.

	if (lpFound != NULL)
	{
		// When there is an online popup of the same contact and the
		// reported status is not equal to the to be reported status,
		// the existing popup is cancelled.

		if (lpFound->GetOnline() != fOnline)
		{
			lpFound->Cancel();
		}
	}
	else
	{
		(new CContactOnlinePopup(lpContact, fOnline))->Show();
	}
}

void CAppWindow::SeedAvatars()
{
	// The response processor of this request will automatically
	// queue the next avatar request for us.

	if (m_lpAvatarRequest != NULL)
	{
		return;
	}

	ASSERT(m_lpView != NULL);

	const TWaveContactMap & vContacts = m_lpView->GetContacts()->GetContacts();
	CWaveContact * lpContact = NULL;

	for (TWaveContactMapConstIter iter = vContacts.begin(); iter != vContacts.end(); iter++)
	{
		if (!iter->second->GetRequestedAvatar())
		{
			lpContact = iter->second;
			break;
		}
	}

	if (lpContact == NULL)
	{
		return;
	}

	m_szRequestingAvatar = lpContact->GetEmailAddress();

	wstring szAvatarUrl(lpContact->GetAbsoluteAvatarUrl());

	m_lpAvatarRequest = new CCurl(szAvatarUrl, this);

	m_lpAvatarRequest->SetUserAgent(USERAGENT);
	m_lpAvatarRequest->SetTimeout(WEB_TIMEOUT_LONG);
	m_lpAvatarRequest->SetIgnoreSSLErrors(TRUE);
	m_lpAvatarRequest->SetReader(new CCurlBinaryReader());
	m_lpAvatarRequest->SetCookies(m_lpSession->GetCookies());

	CNotifierApp::Instance()->QueueRequest(m_lpAvatarRequest);
}

void CAppWindow::ProcessAvatarResponse()
{
	ASSERT(m_lpAvatarRequest != NULL);

	CCurlBinaryReader * lpReader = (CCurlBinaryReader *)m_lpAvatarRequest->GetReader();

	CWaveContact * lpContact = GetWaveContact(m_szRequestingAvatar);

	if (lpContact != NULL)
	{
		lpContact->SetRequestedAvatar(TRUE);

		if (m_lpAvatarRequest->GetResult() == CURLE_OK && m_lpAvatarRequest->GetStatus() == 200)
		{
			SIZE szSize = { PL_CO_ICON_SIZE, PL_CO_ICON_SIZE };

			wstring szContentType(m_lpAvatarRequest->GetHeader(L"Content-Type"));

			if (!szContentType.empty())
			{
				lpContact->SetAvatar(
					CAvatar::Create(lpReader->GetData(), szSize, szContentType)
				);
			}
		}
		else
		{
			LOG2("Could not download avatar %S (%d)",
				m_lpAvatarRequest->GetUrl().c_str(),
				(int)m_lpAvatarRequest->GetStatus());
		}
	}

	delete lpReader;
	delete m_lpAvatarRequest;

	m_lpAvatarRequest = NULL;
	m_szRequestingAvatar = L"";

	SeedAvatars();
}

void CAppWindow::ClientDisconnected(CONNECT_REASON nReason)
{
	CHECK_ENUM(nReason, CR_MAX);

	switch (nReason)
	{
	case CR_SUSPEND:
		m_fClientSuspended = TRUE;
		break;

	case CR_LOCK:
		m_fClientLocked = TRUE;
		break;
	}

	if (CNotifierApp::Instance()->GetConnected())
	{
		CNotifierApp::Instance()->SetConnected(FALSE);
	}
}

void CAppWindow::ClientConnected(CONNECT_REASON nReason)
{
	CHECK_ENUM(nReason, CR_MAX);

	switch (nReason)
	{
	case CR_SUSPEND:
		if (m_fClientSuspended)
		{
			m_lpSession->ForceReconnect();
			m_fClientSuspended = FALSE;
		}
		break;

	case CR_LOCK:
		m_fClientLocked = FALSE;
		break;
	}

	if (!m_fClientSuspended && !m_fClientLocked)
	{
		if (!CNotifierApp::Instance()->GetConnected())
		{
			CNotifierApp::Instance()->SetConnected(TRUE);
		}
	}
}
