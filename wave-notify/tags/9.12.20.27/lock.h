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

#ifndef _INC_LOCK
#define _INC_LOCK

#pragma once

class CLock
{
private:
	CRITICAL_SECTION m_vLock;

public:
	CLock() { InitializeCriticalSection(&m_vLock); }
	void Enter() { EnterCriticalSection(&m_vLock); }
	void Leave() { LeaveCriticalSection(&m_vLock); }
};

#endif // _INC_LOCK
