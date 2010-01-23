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

CWaveContactCollection::~CWaveContactCollection()
{
	for (TWaveContactMapIter iter = m_vContacts.begin(); iter != m_vContacts.end(); iter++)
	{
		delete iter->second;
	}
}

CWaveContactCollection * CWaveContactCollection::CreateFromJson(Json::Value & vRoot)
{
	CWaveContactCollection * lpResult = new CWaveContactCollection();

	if (!vRoot.isArray())
	{
		goto __failure;
	}

	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		if (!(*iter).isObject())
		{
			goto __failure;
		}

		// Detect whether we're reading the "all contacts" list or the "get contact
		// details" list.

		CWaveContact * lpContact;

		if ((*iter)[L"8"].isString())
		{
			lpContact = CWaveContact::CreateFromJson(*iter);
		}
		else
		{
			lpContact = CWaveContact::CreateFromJson((*iter)[L"6"]);
		}

		lpResult->m_vContacts[lpContact->GetEmailAddress()] = lpContact;
	}

	return lpResult;

__failure:
	LOG("Could not parse json");

	delete lpResult;

	return NULL;
}

void CWaveContactCollection::Merge(CWaveContactCollection * lpContacts)
{
	ASSERT(lpContacts != NULL);

	for (TWaveContactMapIter iter = lpContacts->m_vContacts.begin(); iter != lpContacts->m_vContacts.end(); iter++)
	{
		TWaveContactMapIter pos = m_vContacts.find(iter->first);

		if (pos != m_vContacts.end())
		{
			pos->second->Merge(iter->second);

			delete iter->second;
		}
		else
		{
			m_vContacts[iter->second->GetEmailAddress()] = iter->second;
		}
	}

	// We've transferred the enitre contents of the other collection to our collection;
	// clear out the other collection.

	lpContacts->m_vContacts.clear();
}

void CWaveContactCollection::Merge(CWaveContactStatusCollection * lpStatuses)
{
	ASSERT(lpStatuses != NULL);

	const TWaveContactStatusMap & vStatuses = lpStatuses->GetStatuses();

	for (TWaveContactStatusMapConstIter iter = vStatuses.begin(); iter != vStatuses.end(); iter++)
	{
		TWaveContactMapIter pos = m_vContacts.find(iter->first);

		if (pos != m_vContacts.end())
		{
			pos->second->Merge(iter->second);
		}
	}
}
