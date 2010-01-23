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

#ifndef _INC_CCURL
#define _INC_CCURL

#pragma once

#define MAX_AUTO_REDIRECT	30

class CCurlCookies;
class CCurl;

typedef vector<CCurl *> TCurlVector;
typedef TCurlVector::iterator TCurlVectorIter;
typedef TCurlVector::const_iterator TCurlVectorConstIter;

typedef enum
{
	CR_COMPLETED
} CURL_RESPONSE;

class CCurlReader
{
public:
	virtual ~CCurlReader() { }

	virtual BOOL Read(LPBYTE lpData, DWORD cbData) = 0;
};

class CCurlProxySettings
{
private:
	wstring m_szHost;
	DWORD m_dwPort;
	BOOL m_fAuthenticated;
	wstring m_szUsername;
	wstring m_szPassword;

public:
	CCurlProxySettings(wstring szHost, DWORD dwPort, wstring szUsername = L"", wstring szPassword = L"") {
		ASSERT(!szHost.empty());
		ASSERT(dwPort > 0);

		m_szHost = szHost;
		m_dwPort = dwPort;
		m_fAuthenticated = !szUsername.empty();
		m_szUsername = m_fAuthenticated ? szUsername : L"";
		m_szPassword = m_fAuthenticated ? szPassword : L"";
	};

	virtual ~CCurlProxySettings() { }

	wstring GetHost() const { return m_szHost; }
	DWORD GetPort() const { return m_dwPort; }
	BOOL GetAuthenticated() const { return m_fAuthenticated; }
	wstring GetUsername() const { return m_szUsername; }
	wstring GetPassword() const { return m_szPassword; }
};

class CCurl
{
private:
	CURL * m_lpCurl;
	CWindowHandle * m_lpTargetWindow;
	TStringStringMap m_vHeaders;
	long m_lStatus;
	TByteVector m_vData;
	char * m_szUrl;
	char m_szError[CURL_ERROR_SIZE];
	char * m_szUserAgent;
	char * m_szPostData;
	CCurlReader * m_lpReader;
	char * m_szProxyHost;
	BOOL m_fProxyAuthenticated;
	char * m_szProxyUsername;
	CURLcode m_nResult;
	BOOL m_fPostAdded;
	BOOL m_fAutoRedirect;
	BOOL m_fIgnoreSSLErrors;
	INT m_nTimeout;

	static CCurlProxySettings * m_lpProxySettings;

public:
	CCurl(wstring szUrl, CWindowHandle * lpTargetWindow);
	virtual ~CCurl();

	wstring GetUrl() const { return ConvertToWideChar(m_szUrl); }
	wstring GetUrlEncodedPostData() const { return m_szPostData == NULL ? L"" : ConvertToWideChar(m_szPostData); }
	void SetUrlEncodedPostData(wstring szPostData);
	CCurlCookies * GetCookies() const;
	void SetCookies(CCurlCookies * lpCookies);
	wstring GetUserAgent() const { return m_szUserAgent == NULL ? L"" : ConvertToWideChar(m_szUserAgent); }
	void SetUserAgent(wstring szUserAgent);
	BOOL GetIgnoreSSLErrors() const { return m_fIgnoreSSLErrors; }
	void SetIgnoreSSLErrors(BOOL fIgnore);
	INT GetTimeout() const { return m_nTimeout; }
	void SetTimeout(INT nTimeout);
	CCurlReader * GetReader() const { return m_lpReader; }
	void SetReader(CCurlReader * lpReader) { m_lpReader = lpReader; }
	const TByteVector & GetData() const { return m_vData; }
	wstring GetString(INT nCodePage = -1) const;
	string GetAnsiString() const;
	const TStringStringMap & GetHeaders() const { return m_vHeaders; }
	CURLcode GetResult() const { return m_nResult; }
	CURL * GetHandle() const { return m_lpCurl; }
	BOOL GetAutoRedirect() const { return m_fAutoRedirect; }
	void SetAutoRedirect(BOOL fAutoRedirect);
	void SignalCompleted(CURLcode nCode, LONG lStatus);
	LONG GetStatus() const { return m_lStatus; }
	wstring GetHeader(wstring szHeader) const;

	static void SetProxySettings(CCurlProxySettings * lpProxySettings) {
		if (m_lpProxySettings != NULL) delete m_lpProxySettings;
		m_lpProxySettings = lpProxySettings;
	};
	static void Destroy(CCurl * lpCurl) {
		ASSERT(lpCurl != NULL);

		if (lpCurl->m_lpReader != NULL)
			delete lpCurl->m_lpReader;

		delete lpCurl;
	}

	static void GlobalInitialise() {
		CURLcode nResult = curl_global_init(CURL_GLOBAL_ALL);

		ASSERT(nResult == CURLE_OK);
	}
	static void GlobalCleanup() {
		curl_global_cleanup();
	}

private:
	size_t WriteData(void * lpData, size_t dwSize, size_t dwBlocks);
	size_t WriteHeader(void * lpData, size_t dwSize, size_t dwBlocks);

	static size_t WriteDataCallback(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream);
	static size_t WriteHeaderCallback(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream);
	static INT DebugCallback(CURL * lpCurl, curl_infotype nInfoType, LPCSTR szMessage, size_t cbMessage, LPVOID lpParam);
};

class CCurlCookies
{
private:
	curl_slist * m_lpCookies;

public:
	CCurlCookies(curl_slist * lpCookies) {
		ASSERT(lpCookies != NULL);

		m_lpCookies = lpCookies;
	}
	virtual ~CCurlCookies() { curl_slist_free_all(m_lpCookies); }

	curl_slist * GetCookies() const { return m_lpCookies; }
};

class CCurlUTF8StringReader : public CCurlReader
{
private:
	wstringstream m_szResult;
	CUTF8Converter m_vConverter;

public:
	BOOL Read(LPBYTE lpData, DWORD cbData) {
		ASSERT(lpData != NULL && cbData > 0);
		m_szResult << m_vConverter.Parse(lpData, cbData);
		return TRUE;
	}
	wstring GetString() const { return m_szResult.str(); }
};

class CCurlBinaryReader : public CCurlReader
{
private:
	TByteVector m_vResult;

public:
	BOOL Read(LPBYTE lpData, DWORD cbData) {
		ASSERT(lpData != NULL && cbData > 0);
		m_vResult.insert(m_vResult.end(), lpData, lpData + cbData);
		return TRUE;
	}
	const TByteVector & GetData() const { return m_vResult; }
};

class CCurlAnsiStringReader : public CCurlReader
{
private:
	wstringstream m_szResult;
	LPWSTR m_szBuffer;
	DWORD m_cbBuffer;

public:
	CCurlAnsiStringReader() : m_szBuffer(NULL), m_cbBuffer(0) { }
	virtual ~CCurlAnsiStringReader() { if (m_szBuffer != NULL) free(m_szBuffer); }

	BOOL Read(LPBYTE lpData, DWORD cbData);
	wstring GetString() const { return m_szResult.str(); }
};

class CCurlFileReader : public CCurlReader
{
private:
	HANDLE m_hFile;

public:
	CCurlFileReader(HANDLE hFile) {
		ASSERT(hFile != NULL);
		m_hFile = hFile;
	}

	BOOL Read(LPBYTE lpData, DWORD cbData) {
		ASSERT(lpData != NULL && cbData > 0);
		DWORD dwWritten;
		return WriteFile(m_hFile, lpData, cbData, &dwWritten, NULL) && dwWritten == cbData;
	}

	HANDLE GetHandle() const { return m_hFile; }
};

class CCurlMulti
{
	typedef map<SOCKET, INT> TSocketMap;
	typedef TSocketMap::iterator TSocketMapIter;
	typedef TSocketMap::const_iterator TSocketMapConstIter;

private:
	CURLM * m_lpMulti;
	INT m_nRunning;

public:
	CCurlMulti() {
		if (( m_lpMulti = curl_multi_init() ) == NULL)
			FAIL("Could not initialise curl_multi_init");
		m_nRunning = 0;
	}
	virtual ~CCurlMulti() { curl_multi_cleanup(m_lpMulti); }

	void Add(CCurl * lpCurl) {
		if (curl_multi_add_handle(m_lpMulti, lpCurl->GetHandle()) != CURLE_OK)
			FAIL("Could not curl_multi_add_handle");
	}
	void Remove(CCurl * lpCurl) {
		if (curl_multi_remove_handle(m_lpMulti, lpCurl->GetHandle()) != CURLE_OK)
			LOG("Could not curl_multi_remove_handle");
	}
	void Remove(CURL * lpCurl) {
		if (curl_multi_remove_handle(m_lpMulti, lpCurl) != CURLE_OK)
			LOG("Could not curl_multi_remove_handle");
	}
	CWSAEvent * GetEvent();
	void Perform();
	CURLMsg * GetNextMessage() {
		int nRemaining;
		return curl_multi_info_read(m_lpMulti, &nRemaining);
	}
	INT GetRunning() const { return m_nRunning; }
	
private:
	void AddSockets(TSocketMap & vSockets, fd_set & vSet, INT nEventType);
};

class CCurlCache
{
private:
	CURLSH * m_lpShare;

public:
	CCurlCache() {
		if (( m_lpShare = curl_share_init() ) == NULL)
			FAIL("Could not curl_share_init");
		curl_share_setopt(m_lpShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
	}
	virtual ~CCurlCache() { curl_share_cleanup(m_lpShare); }

	void Add(CCurl * lpCurl) {
		if (curl_easy_setopt(lpCurl->GetHandle(), CURLOPT_SHARE, m_lpShare) != CURLE_OK)
			FAIL("Could not curl_easy_setopt(CURLOPT_SHARE)");
	}
	void Remove(CCurl * lpCurl) {
		if (curl_easy_setopt(lpCurl->GetHandle(), CURLOPT_SHARE, NULL) != CURLE_OK)
			LOG("Could not curl_easy_setopt(CURLOPT_SHARE, 0)");
	}
};

class CCurlMonitor : private CThread
{
private:
	CLock m_vLock;
	CAutoResetEvent m_vEvent;
	BOOL m_fCancelled;
	TCurlVector m_vQueueRequests;
	TCurlVector m_vCancelRequests;
	TCurlVector m_vRequests;
	CWindowHandle * m_lpTargetWindow;
	CCurlCache m_vCache;
	CCurlMulti * m_lpMulti;

public:
	CCurlMonitor(CWindowHandle * lpTargetWindow);
	virtual ~CCurlMonitor();

	void Cancel() {
		m_fCancelled = TRUE;
		m_vEvent.Set();
	}

	void QueueRequest(CCurl * lpRequest) {
		ASSERT(lpRequest != NULL);

		m_vLock.Enter();

		ASSERT(find(m_vQueueRequests.begin(), m_vQueueRequests.end(), lpRequest) == m_vQueueRequests.end());
		m_vQueueRequests.push_back(lpRequest);

		m_vLock.Leave();

		m_vEvent.Set();
	}

	void CancelRequest(CCurl * lpRequest) {
		ASSERT(lpRequest != NULL);

		m_vLock.Enter();

		ASSERT(find(m_vCancelRequests.begin(), m_vCancelRequests.end(), lpRequest) == m_vCancelRequests.end());
		m_vCancelRequests.push_back(lpRequest);

		m_vLock.Leave();

		m_vEvent.Set();
	}

protected:
	DWORD ThreadProc();

private:
	void ProcessEvent();
	void ProcessMessages();
	void CancelAllRequests();
	void QueueRequests(TCurlVector & vRequests);
	void CancelRequests(TCurlVector & vRequests);
	void SignalCompleted(CCurl * lpCurl, CURLcode nCode, LONG lStatus);
};

#endif // _INC_CCURL
