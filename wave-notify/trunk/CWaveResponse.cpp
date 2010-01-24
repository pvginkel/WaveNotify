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

CWaveResponse * CWaveResponse::Parse(Json::Value & vRoot)
{
	BOOL fHadType = FALSE;
	BOOL fHadPayload = FALSE;

	WAVE_MESSAGE_TYPE nType = WMT_UNKNOWN;
	wstring szRequestID(L"");
	BOOL fFinal = TRUE;

	if (!vRoot.isObject())
	{
		LOG("Could not parse json");

		return NULL;
	}

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		if (iter.key() == L"t")
		{
			if (!(*iter).isIntegral())
			{
				LOG("Could not parse json");

				return NULL;
			}

			fHadType = TRUE;
			nType = (WAVE_MESSAGE_TYPE)(*iter).asInt();
		}
		else if (iter.key() == L"r")
		{
			if (!(*iter).isString())
			{
				LOG("Could not parse json");

				return NULL;
			}

			szRequestID = (*iter).asString();
		}
		else if (iter.key() == L"p")
		{
			fHadPayload = TRUE;
		}
		else if (iter.key() == L"f")
		{
			if (!(*iter).isIntegral())
			{
				LOG("Could not parse json");

				return NULL;
			}

			fFinal = (*iter).asBool();
		}
	}

	CWaveResponse * lpResponse = NULL;

	if (fHadType && fHadPayload)
	{
		switch (nType)
		{
		case WMT_GET_ALL_CONTACTS:
			lpResponse = new CWaveResponseGetAllContacts();
			break;

		case WMT_START_LISTENING:
			lpResponse = new CWaveResponseStartListening();
			break;

		case WMT_GET_CONTACT_DETAILS:
			lpResponse = new CWaveResponseGetContactDetails();
			break;

		case WMT_STOP_LISTENING:
			lpResponse = new CWaveResponseStopListening();
			break;

		case WMT_CONTACT_UPDATES:
			lpResponse = new CWaveResponseContactUpdates();
			break;
		}
	}

	if (lpResponse != NULL)
	{
		lpResponse->m_szRequestID = szRequestID;
		lpResponse->m_fIsFinal = fFinal;
		
		if (!lpResponse->AssignJson(vRoot[L"p"]))
		{
			delete lpResponse;

			lpResponse = NULL;
		}
	}

	return lpResponse;
}

BOOL CWaveResponse::ReadStringArray(TStringVector & vStrings, Json::Value & vRoot)
{
	if (!vRoot.isArray())
	{
		LOG("Could not parse json");

		return FALSE;
	}

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		if (!(*iter).isString())
		{
			LOG("Could not parse json");

			return FALSE;
		}

		vStrings.push_back((*iter).asString());
	}

	return TRUE;
}
