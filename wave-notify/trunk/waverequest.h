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

#ifndef _INC_WAVEREQUEST
#define _INC_WAVEREQUEST

#pragma once

class CWaveRequest
{
private:
	WAVE_MESSAGE_TYPE m_nType;

protected:
	CWaveRequest(WAVE_MESSAGE_TYPE nType) {
		m_nType = nType;
	}

public:
	virtual ~CWaveRequest() { }

	WAVE_MESSAGE_TYPE GetType() const { return m_nType; }

	virtual void CreateRequest(Json::Value & vRoot) = 0;
	virtual void RequestCompleted() { }
};

class CWaveRequestGetAllContacts : public CWaveRequest
{
public:
	CWaveRequestGetAllContacts() : CWaveRequest(WMT_GET_ALL_CONTACTS) { }

	void CreateRequest(Json::Value & vRoot);
};

class CWaveRequestStartListening : public CWaveRequest
{
private:
	CWaveListener * m_lpListener;

public:
	CWaveRequestStartListening(wstring szSearchString);
	~CWaveRequestStartListening() {
		if (m_lpListener != NULL) delete m_lpListener;
	}

	void CreateRequest(Json::Value & vRoot);
	void RequestCompleted();
};

class CWaveRequestGetContactDetails : public CWaveRequest
{
private:
	TStringVector m_vEmailAddresses;

public:
	CWaveRequestGetContactDetails() : CWaveRequest(WMT_GET_CONTACT_DETAILS) { }

	void AddEmailAddress(wstring szEmailAddress) { m_vEmailAddresses.push_back(szEmailAddress); }
	void CreateRequest(Json::Value & vRoot);
};

class CWaveRequestStopListening : public CWaveRequest
{
private:
	wstring m_szID;

public:
	CWaveRequestStopListening(wstring szID) : CWaveRequest(WMT_STOP_LISTENING) {
		m_szID = szID;
	}

	void CreateRequest(Json::Value & vRoot);
	void RequestCompleted();
};

class CWaveRequestContactUpdates : public CWaveRequest
{
public:
	CWaveRequestContactUpdates() : CWaveRequest(WMT_CONTACT_UPDATES) { }

	void CreateRequest(Json::Value & vRoot);
};

#endif // _INC_WAVEREQUEST
