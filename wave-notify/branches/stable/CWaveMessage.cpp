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

CWaveMessage::CWaveMessage(Json::Value & vRoot, UINT uOrder)
{
	m_szText = vRoot[L"1"].asString();
	m_uContactId = vRoot[L"6"].asUInt();
	m_uOrder = uOrder;
}

CWaveMessage::CWaveMessage(const CWaveMessage & _Other)
{
	m_szText = _Other.m_szText;
	m_szEmailAddress = _Other.m_szEmailAddress;
	m_uContactId = _Other.m_uContactId;
	m_uOrder = _Other.m_uOrder;
}

void CWaveMessage::ResolveContact(CWave * lpWave)
{
	const TStringVector & vContacts = lpWave->GetContacts();

	if (m_uContactId < vContacts.size())
	{
		m_szEmailAddress = vContacts[m_uContactId];
	}
}
