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

#ifndef _INC_TIMER
#define _INC_TIMER

#pragma once

class CTimer;

typedef map<UINT_PTR, CTimer *> TTimerMap;
typedef TTimerMap::iterator TTimerMapIter;
typedef TTimerMap::const_iterator TTimerMapConstIter;

class CTimerCollection
{
private:
	CWindowHandle * m_lpTargetWindow;
	TUintPtrVector m_vFreeEventIds;
	TTimerMap m_vTimers;
	UINT_PTR m_nNextEventId;

	static CTimerCollection * m_lpInstance;

public:
	CTimerCollection(CWindowHandle * lpTargetWindow);
	virtual ~CTimerCollection();

private:
	void Register(CTimer * lpTimer);
	void Unregister(CTimer * lpTimer);

public:
	CWindowHandle * GetTargetWindow() { return m_lpTargetWindow; }
	BOOL Process(UINT_PTR nEventId);

	static CTimerCollection * Instance() {
		ASSERT(m_lpInstance != NULL);
		return m_lpInstance;
	}

private:
	friend class CTimer;
};

class CTimer
{
private:
	UINT_PTR m_nEventId;
	BOOL m_fRunning;
	UINT m_uInterval;

public:
	CTimer(UINT uInterval = 1000, BOOL fStarted = FALSE);
	virtual ~CTimer();

	Event Tick;

	UINT GetInterval() const { return m_uInterval; }
	void SetInterval(UINT uInterval);
	BOOL GetRunning() const { return m_fRunning; }
	void SetRunning(BOOL fRunning);
	void OnTick() {
		if (Tick != NULL)
			Tick();
	}

private:
	friend class CTimerCollection;
};

#endif // _INC_TIMER
