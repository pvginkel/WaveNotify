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

CTimer::CTimer(UINT uInterval, BOOL fStarted)
{
	CTimerCollection::Instance()->Register(this);

	m_fRunning = fStarted;

	SetInterval(uInterval);
}

CTimer::~CTimer()
{
	SetRunning(FALSE);

	CTimerCollection::Instance()->Unregister(this);
}

void CTimer::SetInterval(UINT uInterval)
{
	m_uInterval = uInterval;

	if (m_fRunning)
	{
		SetTimer(
			CTimerCollection::Instance()->GetTargetWindow()->GetHandle(),
			m_nEventId,
			m_uInterval,
			NULL
		);
	}
}

void CTimer::SetRunning(BOOL fRunning)
{
	if (m_fRunning != fRunning)
	{
		m_fRunning = fRunning;
	
		if (m_fRunning)
		{
			SetInterval(m_uInterval);
		}
		else
		{
			KillTimer(
				CTimerCollection::Instance()->GetTargetWindow()->GetHandle(),
				m_nEventId
			);
		}
	}
}
