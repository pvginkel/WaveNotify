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

#ifndef _INC_MUTEX
#define _INC_MUTEX

#pragma once

class CMutex
{
	HANDLE m_hHandle;

private:
	CMutex(HANDLE hHandle) {
		ASSERT(hHandle != NULL);
		m_hHandle = hHandle;
	}

public:
	~CMutex() { CloseHandle(m_hHandle); }

	HANDLE GetHandle() { return m_hHandle; }

	static CMutex * Create(BOOL fInitialOwner, wstring szName = L"") {
		HANDLE hHandle = CreateMutex(NULL, fInitialOwner, szName.empty() ? NULL : szName.c_str());
		return hHandle == NULL ? NULL : new CMutex(hHandle);
	}
};

#endif // _INC_MUTEX
