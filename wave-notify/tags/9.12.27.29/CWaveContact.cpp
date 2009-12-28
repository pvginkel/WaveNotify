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
}
