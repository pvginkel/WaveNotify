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

#define TIMER_LOGIN_SUCCESS	1

static BOOL CALLBACK CLoginDialog_DisableWindowsEnum(HWND hWnd, LPARAM lParam);

CLoginDialog::CLoginDialog() : CDialog(IDD_LOGIN)
{
	m_hStateOffline = (HICON)LoadImage(
		CApp::Instance()->GetInstance(),
		MAKEINTRESOURCE(IDI_STATE_OFFLINE),
		IMAGE_ICON, 16, 16, 0);

	m_hStateUnknown = (HICON)LoadImage(
		CApp::Instance()->GetInstance(),
		MAKEINTRESOURCE(IDI_STATE_UNKNOWN),
		IMAGE_ICON, 16, 16, 0);

	m_hStateOnline = (HICON)LoadImage(
		CApp::Instance()->GetInstance(),
		MAKEINTRESOURCE(IDI_STATE_ONLINE),
		IMAGE_ICON, 16, 16, 0);

	m_lpLoginThread = NULL;
}

CLoginDialog::~CLoginDialog()
{
	DestroyIcon(m_hStateOffline);
	DestroyIcon(m_hStateUnknown);
	DestroyIcon(m_hStateOnline);

	if (m_lpLoginThread != NULL)
	{
		delete m_lpLoginThread;
	}
}

INT_PTR CLoginDialog::DialogProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_INITDIALOG:
		return OnInitDialog();

	case WM_COMMAND:
		return OnCommand(LOWORD(wParam), HIWORD(wParam));

	case WM_LOGIN_STATUS:
		return OnLoginStateChanged((LOGIN_STATUS)wParam, (WAVE_LOGIN_ERROR)lParam);

	case WM_TIMER:
		return OnTimer(wParam);

	default:
		return CDialog::DialogProc(uMessage, wParam, lParam);
	}
}

INT_PTR CLoginDialog::OnInitDialog()
{
	SendMessage(WM_SETICON, ICON_BIG, (LPARAM)CNotifierApp::Instance()->GetMainIcon());
	SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)CNotifierApp::Instance()->GetMainIconSmall());

	SetStateIcon(m_hStateUnknown);

	CSettings settings(FALSE);

	BOOL fHasUsername = FALSE;
	BOOL fHasPassword = FALSE;

	wstring szValue;

	if (settings.GetGoogleUsername(szValue))
	{
		SetDlgItemText(IDC_LOGIN_USERNAME, szValue);

		fHasUsername = !szValue.empty();
	}

	if (settings.GetGooglePassword(szValue))
	{
		SetDlgItemText(IDC_LOGIN_PASSWORD, szValue);

		fHasPassword = !szValue.empty();
	}

	BOOL fValue;

	if (settings.GetRememberPassword(fValue))
	{
		SetDlgItemChecked(IDC_LOGIN_REMEMBERPASSWORD, fValue);
	}

	if (fHasUsername)
	{
		if (fHasPassword)
		{
			SetFocus(GetDlgItem(IDOK));
		}
		else
		{
			SetFocus(GetDlgItem(IDC_LOGIN_PASSWORD));
		}
	}
	else
	{
		SetFocus(GetDlgItem(IDC_LOGIN_USERNAME));
	}

	UpdateEnabled();

	return FALSE;
}

INT_PTR CLoginDialog::OnCommand(WORD wCommand, WORD wNotifyMessage)
{
	switch (wCommand)
	{
	case IDOK:
		return ProcessLogin();

	case IDCANCEL:
		DestroyWindow(GetHandle());
		return (INT_PTR)TRUE;

	case IDC_LOGIN_USERNAME:
	case IDC_LOGIN_PASSWORD:
		switch (wNotifyMessage)
		{
		case EN_CHANGE:
			return UpdateEnabled();
		}
		return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}

INT_PTR CLoginDialog::ProcessLogin()
{
	CNotifierApp::Instance()->SignOut();

	CWaveSession * lpSession = new CWaveSession(
		GetDlgItemText(IDC_LOGIN_USERNAME),
		GetDlgItemText(IDC_LOGIN_PASSWORD)
	);

	SetStateIcon(m_hStateUnknown);
	SetDlgItemText(IDC_LOGIN_STATE, L"Signing in");

	EnableControls(FALSE);

	m_lpLoginThread = new CLoginThread(lpSession, this);

	return TRUE;
}

void CLoginDialog::EnableControls(BOOL fEnabled)
{
	if (fEnabled)
	{
		for (THwndVectorIter iter = m_vDisabledWindows.begin(); iter != m_vDisabledWindows.end(); iter++)
		{
			EnableWindow(*iter, TRUE);
		}

		m_vDisabledWindows.empty();
	}
	else
	{
		EnumChildWindows(GetHandle(), CLoginDialog_DisableWindowsEnum, (LPARAM)&m_vDisabledWindows);

	}

	EnableWindow(GetHandle(), fEnabled);
}

INT_PTR CLoginDialog::OnLoginStateChanged(LOGIN_STATUS nState, WAVE_LOGIN_ERROR nLoginError)
{
	switch (nState)
	{
	case LIS_GOT_KEY:
		SetStateIcon(m_hStateUnknown);
		SetDlgItemText(IDC_LOGIN_STATE, L"Signing in");
		break;

	case LIS_GOT_COOKIE:
		SetStateIcon(m_hStateUnknown);
		SetDlgItemText(IDC_LOGIN_STATE, L"Authenticating");
		break;

	case LIS_DONE:
		SetStateIcon(m_hStateOnline);
		SetDlgItemText(IDC_LOGIN_STATE, L"Online");
		SetTimer(GetHandle(), TIMER_LOGIN_SUCCESS, 320, NULL);
		break;

	case LIS_FAILED:
		SetStateIcon(m_hStateUnknown);
		SetDlgItemText(IDC_LOGIN_STATE, L"Offline");
		ProcessLoginFailure(nLoginError);
		break;
	}

	return TRUE;
}

void CLoginDialog::ProcessLoginSuccess()
{
	UpdateRegistry();

	CNotifierApp::Instance()->SetWaveSession(m_lpLoginThread->GetSession());

	delete m_lpLoginThread;

	m_lpLoginThread = NULL;

	DestroyWindow(GetHandle());
}

void CLoginDialog::ProcessLoginFailure(WAVE_LOGIN_ERROR nLoginError)
{
	EnableControls(TRUE);

	LPCWSTR szMessage;

	switch (nLoginError)
	{
	case WLE_NETWORK:
		szMessage = L"Could not connect to http://www.googlewave.com/.";
		break;

	case WLE_AUTHENTICATE:
		szMessage = L"Incorrect username or password.";
		break;

	default:
		szMessage = L"Unknown error.";
		break;
	}

	MessageBox(
		GetHandle(),
		szMessage,
		L"Google Wave Notifier",
		MB_ICONERROR | MB_OK);

	delete m_lpLoginThread->GetSession();

	delete m_lpLoginThread;

	m_lpLoginThread = NULL;
}

INT_PTR CLoginDialog::UpdateEnabled()
{
	BOOL fEnabled =
		!GetDlgItemText(IDC_LOGIN_USERNAME).empty() &&
		!GetDlgItemText(IDC_LOGIN_PASSWORD).empty();

	EnableWindow(GetDlgItem(IDOK), fEnabled);

	SendMessage(DM_SETDEFID, IDOK);

	return TRUE;
}

void CLoginDialog::UpdateRegistry()
{
	CSettings settings(TRUE);

	settings.SetGoogleUsername(GetDlgItemText(IDC_LOGIN_USERNAME));

	BOOL fRemember = GetDlgItemChecked(IDC_LOGIN_REMEMBERPASSWORD);

	settings.SetRememberPassword(fRemember);

	if (fRemember)
	{
		settings.SetGooglePassword(GetDlgItemText(IDC_LOGIN_PASSWORD));
	}
	else
	{
		settings.DeleteGooglePassword();
	}
}

void CLoginDialog::SetStateIcon(HICON hIcon)
{
	SendDlgItemMessage(IDC_LOGIN_STATEICON, STM_SETICON, (WPARAM)hIcon);
}

INT_PTR CLoginDialog::OnTimer(WPARAM nTimerId)
{
	switch (nTimerId)
	{
	case TIMER_LOGIN_SUCCESS:
		ProcessLoginSuccess();
		break;
	}

	return TRUE;
}

static BOOL CALLBACK CLoginDialog_DisableWindowsEnum(HWND hWnd, LPARAM lParam)
{
	THwndVector * lpDisabledWindows = (THwndVector *)lParam;

	if (IsWindowEnabled(hWnd))
	{
		lpDisabledWindows->push_back(hWnd);
		EnableWindow(hWnd, FALSE);
	}

	return TRUE;
}
