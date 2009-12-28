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

////////////////////////////////////////////////////////////////////////////////
// COptionsGeneralPage /////////////////////////////////////////////////////////

class COptionsGeneralPage : public CPropertySheetPage
{
private:
	TStringVector m_vBrowsers;

public:
	COptionsGeneralPage();

protected:
	BOOL OnFocus();
	BOOL OnApply();

private:
	void LoadBrowserList(wstring szBrowser);
};

////////////////////////////////////////////////////////////////////////////////
// COptionsProxyPage ///////////////////////////////////////////////////////////

class COptionsProxyPage : public CPropertySheetPage
{
private:
	BOOL m_fSuspendSettings;
public:
	COptionsProxyPage();

protected:
	BOOL OnFocus();
	BOOL OnApply();
	BOOL OnCommand(WORD wCommand, WORD wNotifyMessage);

private:
	void CorrectSettings();
};

////////////////////////////////////////////////////////////////////////////////
// COptionsAdvancedPage ////////////////////////////////////////////////////////

class COptionsAdvancedPage : public CPropertySheetPage
{
public:
	COptionsAdvancedPage();

protected:
	BOOL OnFocus();
	BOOL OnApply();
};

////////////////////////////////////////////////////////////////////////////////
// COptionsSheet ///////////////////////////////////////////////////////////////

COptionsSheet::COptionsSheet()
{
	SetFlags(PSH_NOCONTEXTHELP);
	SetIcon(CNotifierApp::Instance()->GetMainIcon());
	SetCaption(L"Google Wave Notifier Options");

	AddPage(new COptionsGeneralPage());
	AddPage(new COptionsProxyPage());
	AddPage(new COptionsAdvancedPage());
}

void COptionsSheet::OnCreated()
{
	SendMessage(WM_SETICON, ICON_BIG, (LPARAM)CNotifierApp::Instance()->GetMainIcon());
	SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)CNotifierApp::Instance()->GetMainIconSmall());
}

////////////////////////////////////////////////////////////////////////////////
// COptionsGeneralPage /////////////////////////////////////////////////////////

COptionsGeneralPage::COptionsGeneralPage()
{
	SetTitle(L"General");
	SetResource(IDD_OPTIONS_GENERAL);

	CBrowser::GetAvailableBrowsers(m_vBrowsers);
}

BOOL COptionsGeneralPage::OnFocus()
{
	CSettings vSettings(FALSE);
	BOOL fValue;

	// Synchronize the start with windows setting.

	CNotifierApp::Instance()->DetectStartWithWindowsSetting();

	// Load all settings.

	if (vSettings.GetStartWithWindows(fValue))
	{
		SetDlgItemChecked(IDC_OPTIONS_STARTWITHWINDOWS, fValue);
	}

	if (vSettings.GetPlaySoundOnNewWave(fValue))
	{
		SetDlgItemChecked(IDC_OPTIONS_PLAYSOUNDONNEWWAVE, fValue);
	}

	wstring szBrowser;

	if (!vSettings.GetBrowser(szBrowser))
	{
		szBrowser = CBrowser::BrowserDefault;
	}

	LoadBrowserList(szBrowser);

	return FALSE;
}

void COptionsGeneralPage::LoadBrowserList(wstring szBrowser)
{
	CWindowHandle vBrowser(GetDlgItem(IDC_OPTIONS_BROWSER));

	vBrowser.SendMessage(CB_RESETCONTENT);

	INT nNewSel = 0;

	for (TStringVectorIter iter = m_vBrowsers.begin(); iter != m_vBrowsers.end(); iter++)
	{
		wstring szBrowserTitle(CBrowser::GetBrowserDescription(*iter));

		INT nIndex = vBrowser.SendMessage(CB_ADDSTRING, 0, (LPARAM)szBrowserTitle.c_str());

		if (*iter == szBrowser)
		{
			nNewSel = nIndex;
		}
	}

	vBrowser.SendMessage(CB_SETCURSEL, nNewSel);
}

BOOL COptionsGeneralPage::OnApply()
{
	CSettings vSettings(TRUE);
	BOOL fValue;
	BOOL fChanged;

	fValue = GetDlgItemChecked(IDC_OPTIONS_STARTWITHWINDOWS);

	if (vSettings.SetStartWithWindows(fValue, fChanged) && fChanged)
	{
		CNotifierApp::Instance()->SetStartWithWindows(fValue);
	}

	fValue = GetDlgItemChecked(IDC_OPTIONS_PLAYSOUNDONNEWWAVE);

	vSettings.SetPlaySoundOnNewWave(fValue);

	CNotifierApp::Instance()->SetPlaySoundOnNewWave(fValue);

	INT nSelectedBrowser = SendDlgItemMessage(IDC_OPTIONS_BROWSER, CB_GETCURSEL);

	if (nSelectedBrowser >= 0 && nSelectedBrowser < m_vBrowsers.size())
	{
		wstring szBrowser(m_vBrowsers[nSelectedBrowser]);

		vSettings.SetBrowser(szBrowser);

		CNotifierApp::Instance()->SetBrowser(szBrowser);
	}

	SetStateValid();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// COptionsProxyPage ///////////////////////////////////////////////////////////

COptionsProxyPage::COptionsProxyPage()
{
	SetTitle(L"Proxy");
	SetResource(IDD_OPTIONS_PROXY);

	m_fSuspendSettings = FALSE;
}

BOOL COptionsProxyPage::OnFocus()
{
	CSettings vSettings(FALSE);

	BOOL fHaveSettings;
	wstring szHost;
	DWORD dwPort;
	BOOL fAuthenticated;
	wstring szUsername;
	wstring szPassword;

	// Load all settings.

	BOOL fSuccess =
		vSettings.GetProxyHaveSettings(fHaveSettings) &&
		vSettings.GetProxyHost(szHost) &&
		vSettings.GetProxyPort(dwPort) &&
		vSettings.GetProxyAuthenticated(fAuthenticated) &&
		vSettings.GetProxyUsername(szUsername) &&
		vSettings.GetProxyPassword(szPassword);

	// If we were not able to retrieve the vSettings, force to default.

	m_fSuspendSettings = TRUE;

	if (!fSuccess)
	{
		// Setting the HaveSettings to FALSE forces the rest of the vSettings
		// to empty.

		SetDlgItemChecked(IDC_OPTIONS_DETECTPROXY, TRUE);
	}
	else
	{
		SetDlgItemChecked(fHaveSettings ? IDC_OPTIONS_USEPROXY : IDC_OPTIONS_DETECTPROXY, TRUE);
		SetDlgItemText(IDC_OPTIONS_PROXYHOST, szHost);
		SetDlgItemInt(IDC_OPTIONS_PROXYPORT, dwPort);
		SetDlgItemChecked(IDC_OPTIONS_PROXYAUTHENTICATE, fAuthenticated);
		SetDlgItemText(IDC_OPTIONS_PROXYUSERNAME, szUsername);
		SetDlgItemText(IDC_OPTIONS_PROXYPASSWORD, szPassword);
	}

	m_fSuspendSettings = FALSE;
	
	CorrectSettings();

	return FALSE;
}

BOOL COptionsProxyPage::OnApply()
{
	CSettings vSettings(TRUE);

	vSettings.SetProxyHaveSettings(GetDlgItemChecked(IDC_OPTIONS_USEPROXY));
	vSettings.SetProxyHost(GetDlgItemText(IDC_OPTIONS_PROXYHOST));
	vSettings.SetProxyPort(GetDlgItemInt(IDC_OPTIONS_PROXYPORT, FALSE));
	vSettings.SetProxyAuthenticated(GetDlgItemChecked(IDC_OPTIONS_PROXYAUTHENTICATE));
	vSettings.SetProxyUsername(GetDlgItemText(IDC_OPTIONS_PROXYUSERNAME));
	vSettings.SetProxyPassword(GetDlgItemText(IDC_OPTIONS_PROXYPASSWORD));

	CNotifierApp::Instance()->SyncProxySettings();

	SetStateValid();

	return TRUE;
}

void COptionsProxyPage::CorrectSettings()
{
	BOOL fHaveSettings = GetDlgItemChecked(IDC_OPTIONS_USEPROXY);
	BOOL fAuthenticated = FALSE;

	if (fHaveSettings)
	{
		fAuthenticated = GetDlgItemChecked(IDC_OPTIONS_PROXYAUTHENTICATE);
	}

	SetDlgItemEnabled(IDC_OPTIONS_PROXYHOST, fHaveSettings);
	SetDlgItemEnabled(IDC_OPTIONS_PROXYPORT, fHaveSettings);
	SetDlgItemEnabled(IDC_OPTIONS_PROXYAUTHENTICATE, fHaveSettings);
	SetDlgItemEnabled(IDC_OPTIONS_PROXYUSERNAME, fHaveSettings && fAuthenticated);
	SetDlgItemEnabled(IDC_OPTIONS_PROXYPASSWORD, fHaveSettings && fAuthenticated);
}

BOOL COptionsProxyPage::OnCommand(WORD wCommand, WORD wNotifyMessage)
{
	switch (wCommand)
	{
	case IDC_OPTIONS_USEPROXY:
	case IDC_OPTIONS_DETECTPROXY:
	case IDC_OPTIONS_PROXYAUTHENTICATE:
		switch (wNotifyMessage)
		{
		case BST_UNCHECKED:
		case BST_CHECKED:
			CorrectSettings();
		}
		break;

	case IDC_OPTIONS_PROXYUSERNAME:
	case IDC_OPTIONS_PROXYPASSWORD:
	case IDC_OPTIONS_PROXYHOST:
	case IDC_OPTIONS_PROXYPORT:
		switch (wNotifyMessage)
		{
		case EN_CHANGE:
			CorrectSettings();
		}
		break;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// COptionsProxyPage ///////////////////////////////////////////////////////////

COptionsAdvancedPage::COptionsAdvancedPage()
{
	SetTitle(L"Advanced");
	SetResource(IDD_OPTIONS_ADVANCED);
}

BOOL COptionsAdvancedPage::OnFocus()
{
	CSettings vSettings(FALSE);
	BOOL fValue;

	if (!vSettings.GetUseGenericUnicodeFont(fValue))
	{
		fValue = TRUE;
	}

	if (GetWindowsVersion() > WV_WINDOWS_XP)
	{
		EnableWindow(GetDlgItem(IDC_OPTIONS_USEGENERICUNICODEFONT), FALSE);
		EnableWindow(GetDlgItem(IDC_OPTIONS_USEGENERICUNICODEFONT_HELP), FALSE);

		fValue = FALSE;
	}

	SetDlgItemChecked(IDC_OPTIONS_USEGENERICUNICODEFONT, fValue);

	if (vSettings.GetCollectStatistics(fValue))
	{
		SetDlgItemChecked(IDC_OPTIONS_STATISTICS, fValue);
	}

	return FALSE;
}

BOOL COptionsAdvancedPage::OnApply()
{
	CSettings vSettings(TRUE);
	BOOL fValue;
	BOOL fChanged;

	fValue = GetDlgItemChecked(IDC_OPTIONS_USEGENERICUNICODEFONT);

	if (vSettings.SetUseGenericUnicodeFont(fValue, fChanged) && fChanged)
	{
		GetMessageBoxFont(TRUE);
	}

	fValue = GetDlgItemChecked(IDC_OPTIONS_STATISTICS);

	vSettings.SetCollectStatistics(fValue);

	SetStateValid();

	return TRUE;
}
