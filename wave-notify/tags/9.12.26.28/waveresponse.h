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

#ifndef _INC_WAVERESPONSE
#define _INC_WAVERESPONSE

#pragma once

class CWaveResponse
{
private:
	WAVE_MESSAGE_TYPE m_nType;
	wstring m_szRequestID;
	BOOL m_fIsFinal;

protected:
	CWaveResponse(WAVE_MESSAGE_TYPE nType) {
		m_nType = nType;
		m_fIsFinal = FALSE;
	}

public:
	virtual ~CWaveResponse() { }

	WAVE_MESSAGE_TYPE GetType() const { return m_nType; }
	wstring GetRequestID() const { return m_szRequestID; }
	BOOL GetIsFinal() const { return m_fIsFinal; }

	static CWaveResponse * Parse(Json::Value & vRoot);

protected:
	virtual void AssignJson(Json::Value & vRoot) = 0;

	void ReadStringArray(TStringVector & vStrings, Json::Value & vRoot);
};

class CWaveResponseGetAllContacts : public CWaveResponse
{
private:
	CWaveContactCollection * m_lpContacts;

public:
	CWaveResponseGetAllContacts() : CWaveResponse(WMT_GET_ALL_CONTACTS) {
		m_lpContacts = NULL;
	}
	~CWaveResponseGetAllContacts() {
		if (m_lpContacts != NULL) delete m_lpContacts;
	}

	CWaveContactCollection * GetContacts() const { return m_lpContacts; }

protected:
	void AssignJson(Json::Value & vRoot) {
		m_lpContacts = new CWaveContactCollection(vRoot[L"2"]);
	}
};

class  CWaveResponseStartListening : public CWaveResponse
{
private:
	CWaveCollection * m_lpWaves;
	TStringVector m_vRemovedWaves;

public:
	CWaveResponseStartListening() : CWaveResponse(WMT_START_LISTENING) {
		m_lpWaves = NULL;
	}
	~CWaveResponseStartListening() {
		if (m_lpWaves != NULL) delete m_lpWaves;
	}

	CWaveCollection * GetWaves() const { return m_lpWaves; }

	void GetRemovedWaves(TStringVector & vRemovedWaves) const {
		for (TStringVectorConstIter iter = m_vRemovedWaves.begin(); iter != m_vRemovedWaves.end(); iter++)
			vRemovedWaves.push_back(*iter);
	}

protected:
	void AssignJson(Json::Value & vRoot) {
		m_lpWaves = new CWaveCollection(vRoot[L"1"]);
		ReadStringArray(m_vRemovedWaves, vRoot[L"3"]);
	}
};

class CWaveResponseGetContactDetails : public CWaveResponse
{
private:
	CWaveContactCollection * m_lpContacts;

public:
	CWaveResponseGetContactDetails() : CWaveResponse(WMT_GET_CONTACT_DETAILS) {
		m_lpContacts = NULL;
	}
	~CWaveResponseGetContactDetails() {
		if (m_lpContacts != NULL) delete m_lpContacts;
	}

	CWaveContactCollection * GetContacts() const { return m_lpContacts; }

protected:
	void AssignJson(Json::Value & vRoot) {
		m_lpContacts = new CWaveContactCollection(vRoot[L"2"]);
	}
};

class CWaveResponseStopListening : public CWaveResponse
{
public:
	CWaveResponseStopListening() : CWaveResponse(WMT_STOP_LISTENING) { }

protected:
	void AssignJson(Json::Value & vRoot) {
		// Ignore any incoming details, no feedback is required
	}
};

#endif // _INC_WAVERESPONSE
