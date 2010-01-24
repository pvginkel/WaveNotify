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

CWaveMessage::CWaveMessage() :
	m_uContactId(0),
	m_uOrder(0)
{
}

CWaveMessage::CWaveMessage(const CWaveMessage & _Other)
{
	m_szText = _Other.m_szText;
	m_szEmailAddress = _Other.m_szEmailAddress;
	m_uContactId = _Other.m_uContactId;
	m_uOrder = _Other.m_uOrder;
}

CWaveMessage * CWaveMessage::CreateFromJson(Json::Value & vRoot, UINT uOrder)
{
	CWaveMessage * lpResult = new CWaveMessage();

	// (( scope ))
	{
		if (!vRoot.isObject())
		{
			goto __failure;
		}

		Json::Value & vText = vRoot[L"1"];
		Json::Value & vContactId = vRoot[L"6"];

		if (
			!vText.isString() ||
			!vContactId.isIntegral()
		) {
			goto __failure;
		}

		lpResult->m_szText = vText.asString();
		lpResult->m_uContactId = vContactId.asUInt();
		lpResult->m_uOrder = uOrder;

		return lpResult;
	}

__failure:
	LOG("Could not parse json");

	delete lpResult;

	return NULL;
}

void CWaveMessage::ResolveContact(CWave * lpWave)
{
	ASSERT(lpWave != NULL);

	const TStringVector & vContacts = lpWave->GetContacts();

	if (m_uContactId < vContacts.size())
	{
		m_szEmailAddress = vContacts[m_uContactId];
	}
}
