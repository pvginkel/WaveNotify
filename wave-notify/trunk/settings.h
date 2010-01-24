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

#ifndef _INC_SETTINGS
#define _INC_SETTINGS

#pragma once

#define SETTINGS_VALUE(_Type, _Name)					\
	BOOL Get##_Name(_Type & _Value) const				\
	{ return GetValue(Reg##_Name, _Value); }			\
	BOOL Set##_Name(_Type _Value)					\
	{ return SetValue(Reg##_Name, _Value); }			\
	BOOL Set##_Name(_Type _Value, BOOL & fChanged)			\
	{ return SetValue(Reg##_Name, _Value, fChanged); }		\
	BOOL Delete##_Name()						\
	{ return DeleteValue(Reg##_Name); }				\

#define SETTINGS_ENCRYPTED_VALUE(_Type, _Name)				\
	BOOL Get##_Name(_Type & _Value) const				\
	{ return GetEncryptedValue(Reg##_Name, _Value); }		\
	BOOL Set##_Name(_Type _Value)					\
	{ return SetEncryptedValue(Reg##_Name, _Value); }		\
	BOOL Set##_Name(_Type _Value, BOOL & fChanged)			\
	{ return SetEncryptedValue(Reg##_Name, _Value, fChanged); }	\
	BOOL Delete##_Name()						\
	{ return DeleteValue(Reg##_Name); }				\

class CSettings
{
private:
	static const wstring RegBaseKey;
	static const wstring RegGoogleUsername;
	static const wstring RegGooglePassword;
	static const wstring RegRememberPassword;
	static const wstring RegStartWithWindows;
	static const wstring RegCollectStatistics;
	static const wstring RegStatisticsCookie;
	static const wstring RegProxyHaveSettings;
	static const wstring RegProxyHost;
	static const wstring RegProxyPort;
	static const wstring RegProxyAuthenticated;
	static const wstring RegProxyUsername;
	static const wstring RegProxyPassword;
	static const wstring RegInstalledVersion;
	static const wstring RegAttemptedVersion;
	static const wstring RegUseGenericUnicodeFont;
	static const wstring RegPlaySoundOnNewWave;
	static const wstring RegBrowser;
	static const wstring RegNotificationWhenOnline;
	static const wstring RegApplicationRunning;

	CRegKey * m_lpKey;

public:
	CSettings(BOOL fWritable) {
		m_lpKey = fWritable ?
			CRegKey::CreateKey(HKEY_CURRENT_USER, RegBaseKey) :
			CRegKey::OpenKey(HKEY_CURRENT_USER, RegBaseKey);
	}
	virtual ~CSettings() {
		if (m_lpKey != NULL) {
			delete m_lpKey;
		}
	}

	SETTINGS_VALUE(wstring, GoogleUsername);
	SETTINGS_ENCRYPTED_VALUE(wstring, GooglePassword);
	SETTINGS_VALUE(BOOL, RememberPassword);
	SETTINGS_VALUE(BOOL, StartWithWindows);
	SETTINGS_VALUE(BOOL, CollectStatistics);
	SETTINGS_VALUE(wstring, StatisticsCookie);
	SETTINGS_VALUE(BOOL, ProxyHaveSettings);
	SETTINGS_VALUE(wstring, ProxyHost);
	SETTINGS_VALUE(DWORD, ProxyPort);
	SETTINGS_VALUE(BOOL, ProxyAuthenticated);
	SETTINGS_VALUE(wstring, ProxyUsername);
	SETTINGS_ENCRYPTED_VALUE(wstring, ProxyPassword);
	SETTINGS_VALUE(wstring, InstalledVersion);
	SETTINGS_VALUE(wstring, AttemptedVersion);
	SETTINGS_VALUE(BOOL, UseGenericUnicodeFont);
	SETTINGS_VALUE(BOOL, PlaySoundOnNewWave);
	SETTINGS_VALUE(wstring, Browser);
	SETTINGS_VALUE(BOOL, NotificationWhenOnline);
	SETTINGS_VALUE(BOOL, ApplicationRunning);

	BOOL GetValue(wstring szName, wstring & szValue) const {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		return m_lpKey->GetValue(szName, szValue);
	}
	BOOL SetValue(wstring szName, wstring szValue) {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		return m_lpKey->SetValue(szName, szValue);
	}
	BOOL SetValue(wstring szName, wstring szValue, BOOL & fChanged) {
		ASSERT(!szName.empty());

		fChanged = TRUE;
		wstring szTmp;
		if (GetValue(szName, szTmp)) {
			fChanged = szValue != szTmp;
		}
		return SetValue(szName, szValue);
	}
	BOOL GetEncryptedValue(wstring szName, wstring & szValue) const {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		wstring szTmp;
		if (m_lpKey->GetValue(szName, szTmp)) {
			return DecryptString(szTmp, szValue);
		}
		return FALSE;
	}
	BOOL SetEncryptedValue(wstring szName, wstring szValue) {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		wstring szTmp;
		if (EncryptString(szValue, szTmp)) {
			return m_lpKey->SetValue(szName, szTmp);
		}
		return FALSE;
	}
	BOOL SetEncryptedValue(wstring szName, wstring szValue, BOOL & fChanged) {
		ASSERT(!szName.empty());

		fChanged = TRUE;
		wstring szTmp;
		if (GetEncryptedValue(szName, szTmp)) {
			fChanged = szValue != szTmp;
		}
		return SetEncryptedValue(szName, szValue);
	}
	BOOL GetValue(wstring szName, DWORD & dwValue) const {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		return m_lpKey->GetValue(szName, dwValue);
	}
	BOOL SetValue(wstring szName, DWORD dwValue) {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		return m_lpKey->SetValue(szName, dwValue);
	}
	BOOL SetValue(wstring szName, DWORD dwValue, BOOL & fChanged) {
		ASSERT(!szName.empty());

		fChanged = TRUE;
		DWORD dwTmp;
		if (GetValue(szName, dwTmp)) {
			fChanged = dwValue != dwTmp;
		}
		return SetValue(szName, dwValue);
	}
	BOOL GetValue(wstring szName, BOOL & fValue) const {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		return m_lpKey->GetValue(szName, fValue);
	}
	BOOL SetValue(wstring szName, BOOL fValue) {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		return m_lpKey->SetValue(szName, fValue);
	}
	BOOL SetValue(wstring szName, BOOL fValue, BOOL & fChanged) {
		ASSERT(!szName.empty());

		fChanged = TRUE;
		BOOL fTmp;
		if (GetValue(szName, fTmp)) {
			fChanged = fValue != fTmp;
		}
		return SetValue(szName, fValue);
	}
	BOOL DeleteValue(wstring szName) {
		ASSERT(!szName.empty());

		if (m_lpKey == NULL) return FALSE;
		return m_lpKey->DeleteValue(szName);
	}
};

#endif // _INC_SETTINGS
