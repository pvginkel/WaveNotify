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

CWaveContactStatus::CWaveContactStatus() :
	m_fOnline(FALSE)
{
}

CWaveContactStatus::~CWaveContactStatus()
{
}

CWaveContactStatus * CWaveContactStatus::CreateFromJson(Json::Value & vRoot)
{
	CWaveContactStatus * lpResult = new CWaveContactStatus();

// (( scope ))
{
	if (!vRoot.isObject())
	{
		goto __failure;
	}

	Json::Value & vItem2 = vRoot[L"2"];
	Json::Value & vStatus = vRoot[L"3"];

	if (
		!vItem2.isObject() ||
		!vStatus.isObject()
	) {
		goto __failure;
	}

	Json::Value & vEmailAddress = vItem2[L"1"];
	Json::Value & vOnline = vStatus[L"2"];
	Json::Value & vStatusMessage = vStatus[L"3"];

	if (
		!vEmailAddress.isString() ||
		!vOnline.isBool() ||
		!vStatusMessage.isString()
	) {
		goto __failure;
	}

	lpResult->m_szEmailAddress = vEmailAddress.asString();
	
	lpResult->m_fOnline = vOnline.asBool();
	lpResult->m_szStatusMessage = vStatusMessage.asString();

	return lpResult;
}

__failure:
	LOG("Could not parse json");

	delete lpResult;

	return NULL;
}
