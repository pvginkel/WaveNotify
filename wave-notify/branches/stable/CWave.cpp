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

CWave::CWave(Json::Value & vRoot)
{
	m_szID = vRoot[L"1"].asString();
	m_szEmailAddress = vRoot[L"4"].asString();
	m_uMessages = vRoot[L"6"].asUInt();
	m_uUnreadMessages = vRoot[L"7"].asUInt();
	m_szSubject = vRoot[L"9"][L"1"].asString();
	m_dtTime = CWave::CreateDateTime(vRoot[L"8"]);

	AddContacts(vRoot[L"5"]);
	AddMessages(vRoot[L"10"]);
}

CWave::CWave(const CWave & _Other)
{
	m_szID = _Other.m_szID;
	m_uMessages = _Other.m_uMessages;
	m_uUnreadMessages = _Other.m_uUnreadMessages;
	m_szSubject = _Other.m_szSubject;
	m_szEmailAddress = _Other.m_szEmailAddress;
	m_dtTime = _Other.m_dtTime;

	for (TStringVectorConstIter iter = _Other.m_vContacts.begin(); iter != _Other.m_vContacts.end(); iter++)
	{
		m_vContacts.push_back(*iter);
	}

	for (TWaveMessageVectorConstIter iter1 = _Other.m_vMessages.begin(); iter1 != _Other.m_vMessages.end(); iter1++)
	{
		m_vMessages.push_back(new CWaveMessage(**iter1));
	}
}

CWave::~CWave()
{
	for (TWaveMessageVectorIter iter = m_vMessages.begin(); iter != m_vMessages.end(); iter++)
	{
		delete *iter;
	}
}

void CWave::AddContacts(Json::Value & vRoot)
{
	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		m_vContacts.push_back((*iter).asString());
	}
}

void CWave::AddMessages(Json::Value & vRoot)
{
	UINT uOrder = 0;

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		CWaveMessage * lpMessage = new CWaveMessage(*iter, uOrder);

		lpMessage->ResolveContact(this);

		m_vMessages.push_back(lpMessage);

		uOrder++;
	}
}
