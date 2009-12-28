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

#ifndef _INC_LOGINDIALOG
#define _INC_LOGINDIALOG

#pragma once

class CLoginThread;

class CLoginDialog : public CDialog
{
private:
	THwndVector m_vDisabledWindows;
	HICON m_hStateOffline;
	HICON m_hStateUnknown;
	HICON m_hStateOnline;
	CLoginThread * m_lpLoginThread;

public:
	CLoginDialog();
	virtual ~CLoginDialog();

protected:
	INT_PTR DialogProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	INT_PTR OnInitDialog();
	INT_PTR OnCommand(WORD wCommand, WORD wNotifyMessage);
	INT_PTR ProcessLogin();
	INT_PTR UpdateEnabled();
	void UpdateRegistry();
	void SetStateIcon(HICON hIcon);
	INT_PTR OnLoginStateChanged(LOGIN_STATUS nState, WAVE_LOGIN_ERROR nLoginError);
	void ProcessLoginSuccess();
	void ProcessLoginFailure(WAVE_LOGIN_ERROR nLoginError);
	INT_PTR OnTimer(WPARAM nTimerId);
	void EnableControls(BOOL fEnabled);
};

class CLoginThread : public CThread
{
private:
	CWaveSession * m_lpSession;
	CWindowHandle * m_lpTargetWindow;

public:
	CLoginThread(CWaveSession * lpSession, CWindowHandle * lpTargetWindow) {
		m_lpSession = lpSession;
		m_lpTargetWindow = lpTargetWindow;
	}

	CWaveSession * GetSession() const { return m_lpSession; }

protected:
	DWORD ThreadProc();
};

#endif // _INC_LOGINDIALOG
