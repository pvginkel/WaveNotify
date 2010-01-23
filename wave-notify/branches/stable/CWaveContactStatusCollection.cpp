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

CWaveContactStatusCollection::CWaveContactStatusCollection()
{
}

CWaveContactStatusCollection::~CWaveContactStatusCollection()
{
	for (TWaveContactStatusMapIter iter = m_vStatuses.begin(); iter != m_vStatuses.end(); iter++)
	{
		delete iter->second;
	}
}

CWaveContactStatusCollection * CWaveContactStatusCollection::CreateFromJson(Json::Value & vRoot)
{
	CWaveContactStatusCollection * lpResult = new CWaveContactStatusCollection();

	// An empty status collection?

	if (vRoot.isObject())
	{
		Json::Value & vItem1 = vRoot[L"1"];

		if (vItem1.isIntegral() && vItem1.asInt() == 0)
		{
			return lpResult;
		}
	}

	if (!vRoot.isArray())
	{
		goto __failure;
	}

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		CWaveContactStatus * lpStatus = CWaveContactStatus::CreateFromJson(*iter);

		if (lpStatus == NULL)
		{
			goto __failure;
		}

		lpResult->m_vStatuses[lpStatus->GetEmailAddress()] = lpStatus;
	}

	return lpResult;

__failure:
	LOG("Could not parse json");

	delete lpResult;

	return NULL;
}
