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

CWaveContactCollection::CWaveContactCollection(Json::Value & vRoot)
{
	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		// Detect whether we're reading the "all contacts" list or the "get contact
		// details" list.

		CWaveContact * lpContact;

		if ((*iter)[L"8"].type() == Json::stringValue)
		{
			lpContact = new CWaveContact(*iter);
		}
		else
		{
			lpContact = new CWaveContact((*iter)[L"6"]);
		}

		m_vContacts[lpContact->GetEmailAddress()] = lpContact;
	}
}

CWaveContactCollection::~CWaveContactCollection()
{
	for (TWaveContactMapIter iter = m_vContacts.begin(); iter != m_vContacts.end(); iter++)
	{
		delete iter->second;
	}
}

void CWaveContactCollection::Merge(CWaveContactCollection * lpContacts)
{
	for (TWaveContactMapIter iter = lpContacts->m_vContacts.begin(); iter != lpContacts->m_vContacts.end(); iter++)
	{
		// Remove the existing item when it exists.

		TWaveContactMapIter pos = m_vContacts.find(iter->first);

		if (pos != m_vContacts.end())
		{
			delete pos->second;

			m_vContacts.erase(pos);
		}

		// Add the new or updated item to our collection.

		m_vContacts[iter->second->GetEmailAddress()] = iter->second;
	}

	// We've transferred the enitre contents of the other collection to our collection;
	// clear out the other collection.

	lpContacts->m_vContacts.clear();
}
