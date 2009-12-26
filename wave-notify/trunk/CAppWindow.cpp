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

CAppWindow::CAppWindow() : CWindow(L"GoogleWaveNotifier")
{
	m_hPopupMenus = NULL;
	m_fReconnecting = FALSE;
	m_nReconnectingCount = 0;
	m_lpView = NULL;

	// TODO: Deserialize the last reported here.

	m_lpReportedView = new CWaveCollection();
}

CAppWindow::~CAppWindow()
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	CNotifierApp::Instance()->SignOut();

	if (m_lpView != NULL)
	{
		delete m_lpView;
	}
	
	// TODO: Serialize the last reported here.

	delete m_lpReportedView;

	delete m_lpNotifyIcon;

	PostQuitMessage(0);
}

ATOM CAppWindow::CreateClass(LPWNDCLASSEX lpWndClass)
{
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

	case WM_LISTENER_STATUS:
		return OnListenerStatus((LISTENER_THREAD_STATUS)wParam, (CWaveResponse *)lParam);

	case WM_TIMER:
		return OnTimer(wParam);

	case WM_DESTROY:
		m_lpNotifyIcon->Destroy();
		return 0;
	}

	return CWindow::WndProc(uMessage, wParam, lParam);
}

LRESULT CAppWindow::OnCreate()
{
	m_lpNotifyIcon = new CNotifyIcon(
		this,
		ID_NOTIFYICON,
		L"Google Wave Notifier",
		CNotifierApp::Instance()->GetNotifyIconGray());

	CNotifierApp::Instance()->Login();
	
	SetTimer(GetHandle(), TIMER_VERSION, TIMER_VERSION_INTERVAL, NULL);

	CheckApplicationUpdated();

	return 0;
}

LRESULT CAppWindow::OnNotifyIcon(UINT uMessage, UINT uID)
{
	switch (uMessage)
	{
	case WM_LBUTTONUP:
		if (!ActivateOpenDialog())
		{
			if (CNotifierApp::Instance()->IsLoggedIn())
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
	if (m_hPopupMenus != NULL)
	{
		DestroyMenu(m_hPopupMenus);
	}

	m_hPopupMenus = LoadMenu(CApp::Instance()->GetInstance(), MAKEINTRESOURCE(IDR_POPUP_MENUS));

	HMENU hSubMenu = GetSubMenu(m_hPopupMenus, 0);

	if (CNotifierApp::Instance()->IsLoggedIn())
	{
		DeleteMenu(hSubMenu, ID_TRAYICON_LOGIN, MF_BYCOMMAND);
	}
	else
	{
		DeleteMenu(hSubMenu, ID_TRAYICON_SIGNOUT, MF_BYCOMMAND);

		EnableMenuItem(hSubMenu, ID_TRAYICON_INBOX, MF_GRAYED | MF_BYCOMMAND);

		EnableMenuItem(hSubMenu, ID_TRAYICON_CHECKWAVESNOW, MF_GRAYED | MF_BYCOMMAND);
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
		CheckForUpdates(TRUE);
		break;

	case ID_TRAYICON_INBOX:
		OpenInbox();
		break;

	case ID_TRAYICON_LOGIN:
		Login();
		break;

	case ID_TRAYICON_SIGNOUT:
		SignOut();
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

void CAppWindow::CheckForUpdates(BOOL fManual)
{
	if (CVersion::NewVersionAvailable())
	{
		KillTimer(GetHandle(), TIMER_VERSION);

		SendMessage(WM_CLOSE);
	}
	else
	{
		if (fManual)
		{
			(new CMessagePopup(L"Your version of Google Wave Notifier is up to date."))->Show();
		}
	}
}

void CAppWindow::OpenInbox()
{
	OpenUrl(CNotifierApp::Instance()->GetSession()->GetInboxUrl());
}

void CAppWindow::Login()
{
	if (m_fReconnecting)
	{
		StopReconnecting();

		CNotifierApp::Instance()->GetSession()->StopListener();
	}

	CNotifierApp::Instance()->Login();
}

void CAppWindow::SignOut()
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	CSettings(TRUE).DeleteGooglePassword();

	CNotifierApp::Instance()->SignOut();
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

LRESULT CAppWindow::OnListenerStatus(LISTENER_THREAD_STATUS nStatus, CWaveResponse * lpResponse)
{
	switch (nStatus)
	{
	case LTS_RECONNECTING:
		ProcessReconnecting();
		break;

	case LTS_CONNECTED:
		ProcessConnected();
		break;

	case LTS_RECEIVED:
		ProcessResponse(lpResponse);
		break;
	}

	return 0;
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

		if (m_lpView != NULL)
		{
			// Integrate the response into the current view.

			m_lpView->ProcessResponse(lpResponse);

			// And display the new popups.

			DisplayWavePopups();
		}

		if (lpResponse->GetType() == WMT_GET_CONTACT_DETAILS)
		{
			// Signal all running popups that we have received new
			// contact details and that they may want to repaint with
			// these contact details.

			if (CPopupWindow::Instance() != NULL)
			{
				TPopupVector & vPopups = CPopupWindow::Instance()->GetPopups();

				for (TPopupVectorIter iter = vPopups.begin(); iter != vPopups.end(); iter++)
				{
					if (((CPopupBase *)*iter)->GetType() == PT_WAVE)
					{
						CUnreadWavePopup * lpPopup = (CUnreadWavePopup *)*iter;

						lpPopup->ContactsUpdated(m_lpView->GetContacts());
					}
				}
			}
		}

		delete lpResponse;
	}
}

void CAppWindow::DisplayWavePopups()
{
	// Create a changelog of the current view and the last
	// reported view.

	CUnreadWaveCollection * lpUnreads = CUnreadWaveCollection::CreateUnreadWaves(
		m_lpReportedView, m_lpView->GetWaves());

	// Synchronize the changelog with the queued popups.

	SynchronisePopups(lpUnreads);

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

void CAppWindow::SynchronisePopups(CUnreadWaveCollection * lpUnreads)
{
	BOOL fQueuedNewWaves = FALSE;
	TPopupVector vMustCancel;
	TStringVector vSeen;

	if (CPopupWindow::Instance() != NULL)
	{
		TPopupVector vPopups(CPopupWindow::Instance()->GetPopups());

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

	CDateTime dtRereportLimit(CDateTime::Now() - CTimeSpan::FromMilliseconds((DOUBLE)REREPORT_TIMEOUT));

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
		TPopupVector vPopups(CPopupWindow::Instance()->GetPopups());

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

	// Make a sound (when applicable).

	if (fQueuedNewWaves && CNotifierApp::Instance()->GetPlaySoundOnNewWave())
	{
		PlaySound(
			MAKEINTRESOURCE(IDR_NEWWAVE),
			CNotifierApp::Instance()->GetInstance(),
			SND_ASYNC | SND_NOSTOP | SND_NOWAIT | SND_RESOURCE);
	}
}

void CAppWindow::ProcessReconnecting()
{
	if (CPopupWindow::Instance() != NULL)
	{
		CPopupWindow::Instance()->CancelAll();
	}

	m_fReconnecting = TRUE;
	m_nReconnectingCount = 0;

	UpdateReconnectingIcon();

	SetTimer(GetHandle(), TIMER_RECONNECTING, TIMER_RECONNECTING_INTERVAL, NULL);

	if (m_lpView != NULL)
	{
		delete m_lpView;

		m_lpView = NULL;
	}
}

void CAppWindow::UpdateReconnectingIcon()
{
	HICON hIcon;

	switch (m_nReconnectingCount % 3)
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

	m_nReconnectingCount++;
}

void CAppWindow::ProcessConnected()
{
	StopReconnecting();

	if (m_lpView != NULL)
	{
		delete m_lpView;
	}

	m_lpView = new CWaveView();

	TWaveRequestVector vRequests;

	CWaveSession * lpSession = CNotifierApp::Instance()->GetSession();

	vRequests.push_back(new CWaveRequestGetAllContacts());

	CWaveRequestGetContactDetails * lpRequest = new CWaveRequestGetContactDetails();

	lpRequest->AddEmailAddress(lpSession->GetEmailAddress());

	vRequests.push_back(lpRequest);

	vRequests.push_back(new CWaveRequestStartListening(L"in:inbox"));

	lpSession->PostRequests(vRequests);
}

void CAppWindow::StopReconnecting()
{
	KillTimer(GetHandle(), TIMER_RECONNECTING);

	m_lpNotifyIcon->SetIcon(
		CNotifierApp::Instance()->IsLoggedIn() ?
		CNotifierApp::Instance()->GetNotifyIcon() :
		CNotifierApp::Instance()->GetNotifyIconGray()
	);

	m_fReconnecting = FALSE;
}

void CAppWindow::HaveReportedWave(wstring szWaveID)
{
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
	CWaveContact * lpContact = m_lpView->GetContacts()->GetContact(szEmailAddress);

	if (lpContact == NULL && m_vRequestedContacts.find(szEmailAddress) == m_vRequestedContacts.end())
	{
		CWaveRequestGetContactDetails * lpRequest = new CWaveRequestGetContactDetails();

		lpRequest->AddEmailAddress(szEmailAddress);

		TWaveRequestVector vRequests;

		vRequests.push_back(lpRequest);

		CNotifierApp::Instance()->GetSession()->PostRequests(vRequests);

		m_vRequestedContacts[szEmailAddress] = TRUE;
	}

	return lpContact;
}

LRESULT CAppWindow::OnTimer(WPARAM nTimerID)
{
	switch (nTimerID)
	{
	case TIMER_VERSION:
		CheckForUpdates(FALSE);
		break;

	case TIMER_RECONNECTING:
		UpdateReconnectingIcon();
		break;
	}

	return 0;
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

	DisplayWavePopups();

	if (CPopupWindow::Instance() == NULL || CPopupWindow::Instance()->GetPopups().size() == 0)
	{
		(new CMessagePopup(L"No unread Waves."))->Show();
	}
}

void CAppWindow::CheckApplicationUpdated()
{
	wstring szVersion;

	if (CSettings(FALSE).GetInstalledVersion(szVersion))
	{
		wstring szNewVersion(CVersion::GetAppVersion());

		if (szVersion != szNewVersion)
		{
			CMessagePopup * lpPopup = new CMessagePopup(L"Google Wave Notifier has been updated. Click here to read about new features.");

			lpPopup->SetUrl(L"http://wave-notify.sourceforge.net/changelog.php");
			lpPopup->SetDuration(8000);

			lpPopup->Show();

			CSettings(TRUE).SetInstalledVersion(szNewVersion);
		}
	}
}

void CAppWindow::DisplayHelp()
{
	wstring szPath(GetDirname(GetModuleFileNameEx()) + L"\\" + CHM_FILENAME);

	ShellExecute(NULL, L"open", szPath.c_str(), NULL, NULL, SW_SHOW);
}
