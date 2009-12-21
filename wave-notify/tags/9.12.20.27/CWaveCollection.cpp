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

CWaveCollection::CWaveCollection(Json::Value & vRoot)
{
	for (Json::Value::iterator iter = vRoot.begin(); iter != vRoot.end(); iter++)
	{
		CWave * lpWave = new CWave(*iter);

		m_vWaves[lpWave->GetID()] = lpWave;
	}
}

CWaveCollection::~CWaveCollection()
{
	for (TWaveMapIter iter = m_vWaves.begin(); iter != m_vWaves.end(); iter++)
	{
		delete iter->second;
	}
}

void CWaveCollection::Merge(CWaveCollection * lpWaves)
{
	for (TWaveMapIter iter = lpWaves->m_vWaves.begin(); iter != lpWaves->m_vWaves.end(); iter++)
	{
		// Remove the existing item when it exists.

		TWaveMapIter pos = m_vWaves.find(iter->first);

		if (pos != m_vWaves.end())
		{
			delete pos->second;

			m_vWaves.erase(pos);
		}

		// Add the new or updated item to our collection.

		m_vWaves[iter->second->GetID()] = iter->second;
	}

	// We've transferred the enitre contents of the other collection to our collection;
	// clear out the other collection.

	lpWaves->m_vWaves.clear();
}

void CWaveCollection::RemoveWaves(const TStringVector & vRemovedWaves)
{
	for (TStringVectorConstIter iter = vRemovedWaves.begin(); iter != vRemovedWaves.end(); iter++)
	{
		TWaveMapIter pos = m_vWaves.find(*iter);

		if (pos != m_vWaves.end())
		{
			delete pos->second;

			m_vWaves.erase(pos);
		}
	}
}

void CWaveCollection::AddWave(CWave * lpWave)
{
	TWaveMapIter pos = m_vWaves.find(lpWave->GetID());

	if (pos != m_vWaves.end())
	{
		delete pos->second;

		m_vWaves.erase(pos);
	}

	m_vWaves[lpWave->GetID()] = lpWave;
}
