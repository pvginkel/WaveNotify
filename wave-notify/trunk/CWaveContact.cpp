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

CWaveContact::CWaveContact(Json::Value & vRoot)
{
	m_szEmailAddress = vRoot[L"8"].asString();
	m_szName = vRoot[L"1"].type() == Json::nullValue ? m_szEmailAddress : vRoot[L"1"].asString();
	m_szAvatarUrl = vRoot[L"2"].asString();
	m_fIsSelf = m_szEmailAddress == CNotifierApp::Instance()->GetSession()->GetEmailAddress();
	m_fRequestedAvatar = m_szAvatarUrl.empty();
	m_lpAvatar = NULL;

	if (!m_fIsSelf)
	{
		m_szDisplayName = m_szName;
	}

	Json::Value & vNames = vRoot[L"7"];

	TStringBoolMap vNamesRead;

	for (Json::Value::iterator iter = vNames.begin(); iter != vNames.end(); iter++)
	{
		CWaveName * lpName = new CWaveName(*iter);

		if (vNamesRead.find(lpName->GetName()) == vNamesRead.end())
		{
			m_vNames.push_back(lpName);

			vNamesRead[lpName->GetName()] = TRUE;

			if (m_fIsSelf && m_szDisplayName.empty() && lpName->GetType() == WNT_SELF)
			{
				m_szDisplayName = lpName->GetName();
			}
		}
		else
		{
			delete lpName;
		}
	}

	if (m_szDisplayName.empty())
	{
		m_szDisplayName = m_szEmailAddress;
	}

	m_szDisplayName[0] = towupper(m_szDisplayName[0]);
}

CWaveContact::~CWaveContact()
{
	for (TWaveNameVectorIter iter = m_vNames.begin(); iter != m_vNames.end(); iter++)
	{
		delete *iter;
	}

	if (m_lpAvatar != NULL)
	{
		delete m_lpAvatar;
	}
}

void CWaveContact::Merge(CWaveContactStatus * lpStatus)
{
	m_fOnline = lpStatus->GetOnline();
	m_szStatusMessage = lpStatus->GetStatusMessage();
}

void CWaveContact::Merge(CWaveContact * lpContact)
{
	m_szEmailAddress = lpContact->m_szEmailAddress;
	m_szName = lpContact->m_szName;
	m_szDisplayName = lpContact->m_szDisplayName;
	m_szAvatarUrl = lpContact->m_szAvatarUrl;
	m_fIsSelf = lpContact->m_fIsSelf;
	m_szStatusMessage = lpContact->m_szStatusMessage;
	m_fOnline = lpContact->m_fOnline;
	m_fRequestedAvatar = lpContact->m_fRequestedAvatar;

	for (TWaveNameVectorIter iter = m_vNames.begin(); iter != m_vNames.end(); iter++)
	{
		delete *iter;
	}

	for (TWaveNameVectorIter iter1 = lpContact->m_vNames.begin(); iter1 != lpContact->m_vNames.end(); iter1++)
	{
		m_vNames.push_back(*iter1);
	}

	lpContact->m_vNames.clear();

	if (lpContact->m_lpAvatar != NULL)
	{
		if (m_lpAvatar != NULL)
		{
			delete m_lpAvatar;
		}

		m_lpAvatar = lpContact->m_lpAvatar;

		lpContact->m_lpAvatar = NULL;
	}
}

wstring CWaveContact::GetAbsoluteAvatarUrl() const
{
	if (m_szAvatarUrl.empty())
	{
		return L"";
	}

	if (
		m_szAvatarUrl.find(L"http://") != 0 &&
		m_szAvatarUrl.find(L"https://") != 0
	) {
		if (m_szAvatarUrl.find(L"/photos/public") == 0)
		{
			return WAVE_URL_AVATAR_PUBLIC_PREFIX + m_szAvatarUrl;
		}
		else
		{
			return WAVE_URL_AVATAR_PRIVATE_PREFIX + m_szAvatarUrl;
		}
	}
	else
	{
		return m_szAvatarUrl;
	}
}
