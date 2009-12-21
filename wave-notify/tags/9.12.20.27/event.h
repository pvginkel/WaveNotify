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

#ifndef _INC_EVENT
#define _INC_EVENT

#pragma once

class CEvent
{
public:
	virtual ~CEvent() { }

	virtual HANDLE GetHandle() const = 0;
	virtual void Set() = 0;
	virtual void Reset() = 0;
};

class CWindowsEvent : public CEvent
{
private:
	HANDLE m_hEvent;

protected:
	CWindowsEvent(HANDLE hEvent) { m_hEvent = hEvent; }

public:
	~CWindowsEvent() { CloseHandle(m_hEvent); }

	HANDLE GetHandle() const { return m_hEvent; }
	void Set() { SetEvent(m_hEvent); }
	void Reset() { ResetEvent(m_hEvent); }
};

class CManualResetEvent : public CWindowsEvent
{
public:
	CManualResetEvent(BOOL fInitialState = FALSE) :
		CWindowsEvent(CreateEvent(NULL, TRUE, fInitialState, NULL)) { }
};

class CAutoResetEvent : public CWindowsEvent
{
public:
	CAutoResetEvent(BOOL fInitialState = FALSE) :
		CWindowsEvent(CreateEvent(NULL, FALSE, fInitialState, NULL)) { }
};

class CWSAEvent : public CEvent
{
	typedef vector<SOCKET> TSocketVector;
	typedef TSocketVector::iterator TSocketVectorIter;
	typedef TSocketVector::const_iterator TSocketVectorConstIter;

private:
	WSAEVENT m_hEvent;
	TSocketVector m_vSockets;

public:
	CWSAEvent() { m_hEvent = WSACreateEvent(); }
	~CWSAEvent() {
		for (TSocketVectorIter iter = m_vSockets.begin(); iter != m_vSockets.end(); iter++)
			WSAEventSelect(*iter, m_hEvent, 0);
		WSACloseEvent(m_hEvent);
	}

	HANDLE GetHandle() const { return (HANDLE)m_hEvent; }
	void Set() { WSASetEvent(m_hEvent); }
	void Reset() { WSAResetEvent(m_hEvent); }
	void EventSelect(SOCKET hSocket, INT nEventType) {
		m_vSockets.push_back(hSocket);
		WSAEventSelect(hSocket, m_hEvent, nEventType);
	}
};

#endif // _INC_EVENT
