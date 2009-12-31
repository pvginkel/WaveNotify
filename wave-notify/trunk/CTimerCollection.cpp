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

CTimerCollection * CTimerCollection::m_lpInstance = NULL;

CTimerCollection::CTimerCollection(CWindowHandle * lpTargetWindow)
{
	ASSERT(m_lpInstance == NULL);

	m_lpInstance = this;

	m_lpTargetWindow = lpTargetWindow;
	m_nNextEventId = TIMER_MAX;
}

CTimerCollection::~CTimerCollection()
{
	ASSERT(m_vTimers.empty());

	m_lpInstance = NULL;
}

void CTimerCollection::Register(CTimer * lpTimer)
{
	UINT_PTR nEventId;

	if (m_vFreeEventIds.empty())
	{
		nEventId = m_nNextEventId++;
	}
	else
	{
		nEventId = m_vFreeEventIds.back();

		m_vFreeEventIds.pop_back();
	}

	lpTimer->m_nEventId = nEventId;

	m_vTimers[nEventId] = lpTimer;
}

void CTimerCollection::Unregister(CTimer * lpTimer)
{
	TTimerMapIter pos = m_vTimers.find(lpTimer->m_nEventId);

	ASSERT(pos != m_vTimers.end());

	m_vTimers.erase(pos);

	if (m_vTimers.empty())
	{
		m_vFreeEventIds.clear();
		m_nNextEventId = TIMER_MAX;
	}
}

BOOL CTimerCollection::Process(UINT_PTR nEventId)
{
	TTimerMapIter pos = m_vTimers.find(nEventId);

	if (pos == m_vTimers.end())
	{
		return FALSE;
	}
	else
	{
		pos->second->OnTick();

		return TRUE;
	}
}
