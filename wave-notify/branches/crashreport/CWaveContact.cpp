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
#include "wave.h"
#include "notifierapp.h"

CWaveContact::CWaveContact() :
	m_fIsSelf(FALSE),
	m_fOnline(FALSE),
	m_fRequestedAvatar(FALSE),
	m_lpAvatar(NULL)
{
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

CWaveContact * CWaveContact::CreateFromJson(Json::Value & vRoot)
{
	CWaveContact * lpResult = new CWaveContact();

// (( scope ))
{
	if (!vRoot.isObject())
	{
		goto __failure;
	}

	Json::Value & vEmailAddress = vRoot[L"8"];
	Json::Value & vName = vRoot[L"1"];
	Json::Value & vAvatarUrl = vRoot[L"2"];
	Json::Value & vNames = vRoot[L"7"];

	if (
		!vEmailAddress.isString() ||
		!(vName.isNull() || vName.isString()) ||
		!(vAvatarUrl.isNull() || vAvatarUrl.isString()) ||
		!vNames.isArray()
	) {
		goto __failure;
	}

	lpResult->m_szEmailAddress = vEmailAddress.asString();
	lpResult->m_szName = vName.type() == Json::nullValue ? lpResult->m_szEmailAddress : vName.asString();
	lpResult->m_szAvatarUrl = vAvatarUrl.asString();
	lpResult->m_fIsSelf =
		(lpResult->m_szEmailAddress == CNotifierApp::Instance()->GetSession()->GetEmailAddress());
	lpResult->m_fRequestedAvatar = lpResult->m_szAvatarUrl.empty();
	lpResult->m_lpAvatar = NULL;

	if (!lpResult->m_fIsSelf)
	{
		lpResult->m_szDisplayName = lpResult->m_szName;
	}

	TStringBoolMap vNamesRead;

	for (Json::Value::iterator iter = vNames.begin(); iter != vNames.end(); iter++)
	{
		CWaveName * lpName = CWaveName::CreateFromJson(*iter);

		if (lpName == NULL)
		{
			goto __failure;
		}

		if (vNamesRead.find(lpName->GetName()) == vNamesRead.end())
		{
			lpResult->m_vNames.push_back(lpName);

			vNamesRead[lpName->GetName()] = TRUE;

			if (
				lpResult->m_fIsSelf &&
				lpResult->m_szDisplayName.empty() &&
				lpName->GetType() == WNT_SELF)
			{
				lpResult->m_szDisplayName = lpName->GetName();
			}
		}
		else
		{
			delete lpName;
		}
	}

	if (lpResult->m_szDisplayName.empty())
	{
		lpResult->m_szDisplayName = lpResult->m_szEmailAddress;
	}

	lpResult->m_szDisplayName[0] = towupper(lpResult->m_szDisplayName[0]);

	return lpResult;
}

__failure:
	LOG("Could not parse json");

	delete lpResult;

	return NULL;
}

void CWaveContact::Merge(CWaveContactStatus * lpStatus)
{
	ASSERT(lpStatus != NULL);

	m_fOnline = lpStatus->GetOnline();
	m_szStatusMessage = lpStatus->GetStatusMessage();
}

void CWaveContact::Merge(CWaveContact * lpContact)
{
	ASSERT(lpContact != NULL);

	m_szEmailAddress = lpContact->m_szEmailAddress;
	m_szName = lpContact->m_szName;
	m_szDisplayName = lpContact->m_szDisplayName;
	m_szAvatarUrl = lpContact->m_szAvatarUrl;
	m_fIsSelf = lpContact->m_fIsSelf;
	m_szStatusMessage = lpContact->m_szStatusMessage;
	m_fOnline = lpContact->m_fOnline;

	for (TWaveNameVectorIter iter = m_vNames.begin(); iter != m_vNames.end(); iter++)
	{
		delete *iter;
	}

	m_vNames.clear();

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
		m_fRequestedAvatar = lpContact->m_fRequestedAvatar;

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
