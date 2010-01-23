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

CWave::CWave() :
	m_uMessages(0),
	m_uUnreadMessages(0)
{
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

CWave * CWave::CreateFromJson(Json::Value & vRoot)
{
	CWave * lpResult = new CWave();

	if (!vRoot.isObject())
	{
		goto __failure;
	}

	Json::Value & vID = vRoot[L"1"];
	Json::Value & vEmailAddress = vRoot[L"4"];
	Json::Value & vMessages = vRoot[L"6"];
	Json::Value & vUnreadMessages = vRoot[L"7"];
	Json::Value & vItem9 = vRoot[L"9"];

	if (
		!vID.isString() ||
		!vEmailAddress.isString() ||
		!vMessages.isIntegral() ||
		!vItem9.isObject()
	) {
		goto __failure;
	}

	Json::Value & vSubject = vItem9[L"1"];

	if (!vSubject.isString())
	{
		goto __failure;
	}

	lpResult->m_szID = vID.asString();
	lpResult->m_szEmailAddress = vEmailAddress.asString();
	lpResult->m_uMessages = vMessages.asUInt();
	lpResult->m_uUnreadMessages = vUnreadMessages.asUInt();
	lpResult->m_szSubject = vSubject.asString();
	
	if (!CWave::CreateDateTime(vRoot[L"8"], lpResult->m_dtTime))
	{
		goto __failure;
	}

	if (
		!lpResult->AddContacts(vRoot[L"5"]) ||
		!lpResult->AddMessages(vRoot[L"10"])
	) {
		goto __failure;
	}

	return lpResult;

__failure:
	LOG("Could not parse json");

	delete lpResult;

	return NULL;
}

BOOL CWave::AddContacts(Json::Value & vRoot)
{
	if (!vRoot.isArray())
	{
		return FALSE;
	}

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		if (!(*iter).isString())
		{
			return FALSE;
		}

		m_vContacts.push_back((*iter).asString());
	}

	return TRUE;
}

BOOL CWave::AddMessages(Json::Value & vRoot)
{
	if (!vRoot.isArray())
	{
		return FALSE;
	}

	UINT uOrder = 0;

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		CWaveMessage * lpMessage = CWaveMessage::CreateFromJson(*iter, uOrder);

		if (lpMessage == NULL)
		{
			return FALSE;
		}

		lpMessage->ResolveContact(this);

		m_vMessages.push_back(lpMessage);

		uOrder++;
	}

	return TRUE;
}
