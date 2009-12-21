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

#ifndef _INC_WAVE
#define _INC_WAVE

#pragma once

#define WAVE_URL_CLIENTLOGIN 	L"https://www.google.com/accounts/ClientLogin"
#define WAVE_URL_AUTH 		L"https://wave.google.com/wave/?nouacheck&auth=%s"
#define WAVE_URL_LOGOUT 	L"https://wave.google.com/wave/logout"
#define WAVE_URL_WAVES 		L"https://wave.google.com/wave/?nouacheck"
#define WAVE_URL_INBOX 		L"https://wave.google.com/wave/?auth=%s"
#define WAVE_URL_WAVE		L"https://wave.google.com/wave/?auth=%s#restored:wave:%s.1"
#define WAVE_URL_SESSIONID	L"https://wave.google.com/wave/wfe/channel?VER=6&RID=%d&CVER=3&zx=%s&t=1"
#define WAVE_URL_CHANNEL	L"https://wave.google.com/wave/wfe/channel?VER=6&RID=rpc&SID=%s&CI=0&AID=%d&TYPE=xmlhttp&zx=%s&t=1"
#define WAVE_URL_CHANNEL_POST	L"https://wave.google.com/wave/wfe/channel?VER=6&SID=%s&RID=%d&zx=%s&t=1"

#define WAVE_HASH_POOL		L"abcdefghijklmnopqrstuvwxyz0123456789"

class CWave;
class CWaveName;
class CWaveContact;
class CWaveRequest;
class CWaveResponse;
class CWaveMessage;
class CWaveChannel;
class CWaveListener;
class CWaveResponseStartListening;

typedef vector<CWaveMessage *> TWaveMessageVector;
typedef TWaveMessageVector::iterator TWaveMessageVectorIter;
typedef TWaveMessageVector::const_iterator TWaveMessageVectorConstIter;
typedef vector<CWave *> TWaveVector;
typedef TWaveVector::iterator TWaveVectorIter;
typedef TWaveVector::const_iterator TWaveVectorConstIter;
typedef vector<CWaveContact *> TWaveContactVector;
typedef TWaveContactVector::iterator TWaveContactVectorIter;
typedef TWaveContactVector::const_iterator TWaveContactVectorConstIter;
typedef map<wstring, CWaveContact *> TWaveContactMap;
typedef TWaveContactMap::iterator TWaveContactMapIter;
typedef TWaveContactMap::const_iterator TWaveContactMapConstIter;
typedef vector<CWaveName *> TWaveNameVector;
typedef TWaveNameVector::iterator TWaveNameVectorIter;
typedef TWaveNameVector::const_iterator TWaveNameVectorConstIter;
typedef map<wstring, CWaveListener *> TWaveListenerMap;
typedef TWaveListenerMap::iterator TWaveListenerMapIter;
typedef TWaveListenerMap::const_iterator TWaveListenerMapConstIter;
typedef map<wstring, CWave *> TWaveMap;
typedef TWaveMap::iterator TWaveMapIter;
typedef TWaveMap::const_iterator TWaveMapConstIter;
typedef vector<CWaveRequest *> TWaveRequestVector;
typedef TWaveRequestVector::iterator TWaveRequestVectorIter;
typedef TWaveRequestVector::const_iterator TWaveRequestVectorConstIter;

typedef enum
{
	WMT_UNKNOWN = -1,
        WMT_GET_CONTACT_DETAILS = 2032,
        WMT_GET_ALL_CONTACTS = 2033,
        WMT_START_LISTENING = 2602,
        WMT_STOP_LISTENING = 2007
} WAVE_MESSAGE_TYPE;

typedef enum
{
	WLE_SUCCESS,
	WLE_AUTHENTICATE,
	WLE_NETWORK
} WAVE_LOGIN_ERROR;

class CWaveSession
{
private:
	wstring m_szUsername;
	wstring m_szPassword;
	CCurlCookies * m_lpCookies;
	wstring m_szAuthKey;
	wstring m_szEmailAddress;
	wstring m_szSessionID;
	wstring m_szProfileID;
	CWaveChannel * m_lpChannel;
	CLock m_vLock;
	WAVE_LOGIN_ERROR m_nLoginError;

public:
	CWaveSession(wstring szUsername, wstring szPassword);
	virtual ~CWaveSession();
	BOOL Login();
	BOOL GetAuthKey();
	BOOL GetAuthCookie();
	BOOL GetSessionDetails();
	void SignOut();
	BOOL IsLoggedIn() const;
	wstring GetInboxUrl() const;
	wstring GetWaveUrl(wstring szWaveId) const;
	wstring GetEmailAddress() const { return m_szEmailAddress; }
	wstring GetSessionID() const { return m_szSessionID; }
	wstring GetProfileID() const { return m_szProfileID; }
	void StartListener();
	void StopListener();
	CCurlCookies * GetCookies() const { return m_lpCookies; }
	void SetCookies(CCurlCookies * lpCookies);
	BOOL PostRequests(TWaveRequestVector & vRequests);
	void AddListener(CWaveListener * lpListener);
	CWaveListener * CreateListener(wstring szSearchString);
	BOOL RemoveListener(wstring szID);
	WAVE_LOGIN_ERROR GetLoginError() { return m_nLoginError; }

private:
	wstring GetAuthKeyFromRequest(wstring & szResponse) const;
	wstring GetKeyFromSessionResponse(wstring szKey, wstring & szResponse) const;
	wstring GetSessionData(wstring & szResponse) const;
};

class CWaveContact
{
private:
	wstring m_szEmailAddress;
	wstring m_szName;
	wstring m_szDisplayName;
	wstring m_szAvatarUrl;
	TWaveNameVector m_vNames;
	BOOL m_fIsSelf;

public:
	CWaveContact(Json::Value & vRoot);
	virtual ~CWaveContact();

	wstring GetEmailAddress() const { return m_szEmailAddress; }
	wstring GetName() const { return m_szName; }
	const TWaveNameVector & GetNames() const { return m_vNames; }
	wstring GetDisplayName() const { return m_szDisplayName; }
};

class CWaveContactCollection
{
private:
	TWaveContactMap m_vContacts;

public:
	CWaveContactCollection() { }
	CWaveContactCollection(Json::Value & vRoot);
	virtual ~CWaveContactCollection();

	const TWaveContactMap & GetContacts() const { return m_vContacts; }
	CWaveContact * GetContact(wstring szEmailAddress) const {
		TWaveContactMapConstIter pos = m_vContacts.find(szEmailAddress);
		return pos == m_vContacts.end() ? NULL : pos->second;
	}
	void Merge(CWaveContactCollection * lpContacts);
};

typedef enum
{
	WNT_FULL = 1,
	WNT_SHORT = 2,
	WNT_SELF = 3
} WAVE_NAME_TYPE;

class CWaveName
{
private:
	WAVE_NAME_TYPE m_nType;
	wstring m_szName;

public:
	CWaveName(Json::Value & vRoot);

	wstring GetName() const { return m_szName; }
	WAVE_NAME_TYPE GetType() const { return m_nType; }
};

class CWaveMessage
{
private:
	wstring m_szText;
	wstring m_szEmailAddress;
	UINT m_uContactId;
	UINT m_uOrder;

public:
	CWaveMessage(const CWaveMessage & _Other);
	CWaveMessage(Json::Value & vRoot, UINT uOrder);
	virtual ~CWaveMessage() { }

	void ResolveContact(CWave * lpWave);

	wstring GetText() const { return m_szText; }
	wstring GetEmailAddress() const { return m_szEmailAddress; }
	UINT GetContactId() const { return m_uContactId; }
	UINT GetOrder() const { return m_uOrder; }

	bool operator ==(const CWaveMessage & _Other) const {
		// Optimisation shortcut: only consider contactID's because these are unique
		// inside a wave.
		// return m_szText == _Other.m_szText && m_szEmailAddress == _Other.m_szEmailAddress;

		return m_szText == _Other.m_szText && m_uContactId == _Other.m_uContactId;
	}
	bool operator !=(const CWaveMessage & _Other) const {
		return !(*this == _Other);
	}
};

class CWave
{
private:
	wstring m_szID;
	UINT m_uMessages;
	UINT m_uUnreadMessages;
	wstring m_szSubject;
	TWaveMessageVector m_vMessages;
	TStringVector m_vContacts;
	wstring m_szEmailAddress;
	CDateTime m_dtTime;

public:
	CWave(const CWave & _Other);
	CWave(Json::Value & vRoot);
	virtual ~CWave();

	wstring GetID() const { return m_szID; }
	UINT GetTotalMessages() const { return m_uMessages; }
	UINT GetUnreadMessages() const { return m_uUnreadMessages; }
	wstring GetSubject() const { return m_szSubject; }
	const TStringVector & GetContacts() const { return m_vContacts; }
	const TWaveMessageVector & GetMessages() const { return m_vMessages; }
	wstring GetEmailAddress() const { return m_szEmailAddress; }
	CDateTime GetTime() const { return m_dtTime; }

	static CDateTime CreateDateTime(Json::Value & vRoot) {
		return CreateDateTime(vRoot[1].asInt(), vRoot[0u].asInt());
	}
	static CDateTime CreateDateTime(DWORD dwMajor, DWORD dwMinor) {
		SYSTEMTIME vOffsetSystemTime = { 1970, 1, 0, 1, 0 };
		ULONGLONG uTime;
		SystemTimeToFileTime(&vOffsetSystemTime, (LPFILETIME)&uTime);
		uTime += (ULONGLONG(dwMinor) | (ULONGLONG(dwMajor) << 32)) * (ULONGLONG)10000;
		return CDateTime(uTime);
	}

private:
	void AddContacts(Json::Value & vRoot);
	void AddMessages(Json::Value & vRoot);

};

class CWaveCollection
{
private:
	TWaveMap m_vWaves;

public:
	CWaveCollection() { }
	CWaveCollection(Json::Value & vRoot);
	~CWaveCollection();

	void Merge(CWaveCollection * lpWaves);
	void RemoveWaves(const TStringVector & vRemovedWaves);
	void AddWave(CWave * lpWave);

	const TWaveMap & GetWaves() const { return m_vWaves; }
};

class CWaveListener
{
private:
	wstring m_szID;
	wstring m_szSearchString;

public:
	CWaveListener(wstring szID, wstring szSearchString) {
		m_szID = szID;
		m_szSearchString = szSearchString;
	}
	CWaveListener(const CWaveListener & _Right) {
		m_szID = _Right.m_szID;
		m_szSearchString = _Right.m_szSearchString;
	}

	wstring GetID() const { return m_szID; }
	wstring GetSearchString() const { return m_szSearchString; }
};

class CWaveChannel : public CThread
{
private:
	// Unsynchronised variables, owned by the thread.

	CWaveSession * m_lpSession;
	CWindowHandle * m_lpTargetWindow;
	wstring m_szSID;
	INT m_nAID;
	INT m_nRID;
	INT m_nNextRequestID;
	CManualResetEvent m_vCancelEvent;

	// Synchronised variables.

	CLock m_vLock;
	TWaveListenerMap m_vListeners;
	INT m_nNextListenerID;

public:
	CWaveChannel(CWaveSession * lpSession, CWindowHandle * lpTargetWindow);
	~CWaveChannel();
	BOOL ParseResponse(wstring szResponse);
	void Cancel() {
		m_vCancelEvent.Set();
		Join();
	}
	BOOL PostRequests(const TWaveRequestVector & vRequests);
	void AddListener(CWaveListener * lpListener);
	CWaveListener * CreateListener(wstring szSearchString);
	BOOL RemoveListener(wstring szID);

protected:
	DWORD ThreadProc();

private:
	void ReportStatus(LISTENER_THREAD_STATUS nStatus) {
		m_lpTargetWindow->PostMessage(WM_LISTENER_STATUS, nStatus);
	}
	void ReportReceived(CWaveResponse * lpResponse) {
		m_lpTargetWindow->PostMessage(WM_LISTENER_STATUS, LTS_RECEIVED, (LPARAM)lpResponse);
	}
	CWaveResponse * ParseWfeResponse(wstring szResponse, BOOL & fSuccess);
	wstring BuildHash();
	void ResetChannelParameters();
	BOOL RetrieveSID();
	wstring SerializeRequest(CWaveRequest * lpRequest);
};

class CWaveView
{
private:
	CWaveContactCollection * m_lpContacts;
	CWaveCollection * m_lpWaves;

public:
	CWaveView();
	virtual ~CWaveView();

	BOOL ProcessResponse(CWaveResponse * lpResponse);

	CWaveContactCollection * GetContacts() const { return m_lpContacts; }
	CWaveCollection * GetWaves() const { return m_lpWaves; }

private:
	void ProcessContacts(CWaveContactCollection * lpContacts);
	void ProcessWaves(CWaveResponseStartListening * lpResponse);
};

#endif // _INC_WAVE
