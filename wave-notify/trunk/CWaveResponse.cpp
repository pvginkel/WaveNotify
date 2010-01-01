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

CWaveResponse * CWaveResponse::Parse(Json::Value & vRoot)
{
	BOOL fHadType = FALSE;
	BOOL fHadParameter = FALSE;

	WAVE_MESSAGE_TYPE nType = WMT_UNKNOWN;
	wstring szRequestID(L"");
	BOOL fFinal = TRUE;

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		if (iter.key() == L"t")
		{
			fHadType = TRUE;
			nType = (WAVE_MESSAGE_TYPE)(*iter).asInt();
		}
		else if (iter.key() == L"r")
		{
			szRequestID = (*iter).asString();
		}
		else if (iter.key() == L"p")
		{
			fHadParameter = TRUE;
		}
		else if (iter.key() == L"f")
		{
			fFinal = (*iter).asBool();
		}
	}

	CWaveResponse * lpResponse = NULL;

	if (fHadType && fHadParameter)
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
		lpResponse->AssignJson(vRoot[L"p"]);
	}

	return lpResponse;
}

void CWaveResponse::ReadStringArray(TStringVector & vStrings, Json::Value & vRoot)
{
	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		vStrings.push_back((*iter).asString());
	}
}
