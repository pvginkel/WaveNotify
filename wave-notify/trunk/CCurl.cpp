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

static INT CCurl_DebugCallback(CURL * lpCurl, curl_infotype nInfoType, LPCSTR szMessage, size_t cbMessage, LPVOID lpParam);

CCurlProxySettings * CCurl::m_lpProxySettings = NULL;

CCurl::CCurl(wstring szUrl)
{
	m_nResult = CURLE_OK;

	/* Initialize CURL */

	m_lpCurl = curl_easy_init();

	curl_easy_setopt(m_lpCurl, CURLOPT_NOPROGRESS, 1);

	/* Set the URL */

	m_szUrl = _strdup(ConvertToMultiByte(szUrl).c_str());

	curl_easy_setopt(m_lpCurl, CURLOPT_URL, m_szUrl);

	/* Set the error buffer */

	curl_easy_setopt(m_lpCurl, CURLOPT_ERRORBUFFER, m_szError);

	/* Enable cookies */

	curl_easy_setopt(m_lpCurl, CURLOPT_COOKIEFILE, "");

	/* Set the writer function */

	curl_easy_setopt(m_lpCurl, CURLOPT_WRITEFUNCTION, CCurl_WriteData);
	curl_easy_setopt(m_lpCurl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(m_lpCurl, CURLOPT_HEADERFUNCTION, CCurl_WriteHeader);
	curl_easy_setopt(m_lpCurl, CURLOPT_HEADERDATA, this);

#if defined(_DEBUG) && defined(CURL_VERBOSE_LOG)
	curl_easy_setopt(m_lpCurl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(m_lpCurl, CURLOPT_DEBUGFUNCTION, CCurl_DebugCallback);
#endif

	m_szUserAgent = NULL;
	m_szPostData = NULL;
	m_lStatus = 0;
	strcpy(m_szError, "");
	m_lpReader = NULL;
	m_fPostAdded = FALSE;
	m_dwStartTime = GetTickCount();
	m_fIgnoreSSLErrors = FALSE;
	m_nTimeout = -1;
	m_fAutoRedirect = FALSE;
	m_nAutoRedirectIndex = 0;
	m_fDisableDataRead = FALSE;

	m_szProxyHost = NULL;
	m_fProxyAuthenticated = FALSE;
	m_szProxyUsername = NULL;

	if (m_lpProxySettings != NULL)
	{
		m_szProxyHost = _strdup(
			ConvertToMultiByte(
				Format(L"%s:%d", m_lpProxySettings->GetHost().c_str(), (int)m_lpProxySettings->GetPort())
			).c_str()
		);

		curl_easy_setopt(m_lpCurl, CURLOPT_PROXY, m_szProxyHost);

		m_fProxyAuthenticated = m_lpProxySettings->GetAuthenticated();

		if (m_fProxyAuthenticated)
		{
			m_szProxyUsername = _strdup(
				ConvertToMultiByte(
					m_lpProxySettings->GetUsername() + L":" + m_lpProxySettings->GetPassword()
				).c_str()
			);

			curl_easy_setopt(m_lpCurl, CURLOPT_PROXYAUTH, (long)CURLAUTH_BASIC);
			curl_easy_setopt(m_lpCurl, CURLOPT_PROXYUSERPWD, m_szProxyUsername);
		}
	}
}

CCurl::~CCurl()
{
#ifdef _DEBUG
	DWORD dwTime = GetTickCount() - m_dwStartTime;

	LOG2("%s - %.2f", m_szUrl, DOUBLE(dwTime) / 1000.0);
#endif

	curl_easy_cleanup(m_lpCurl);

	if (m_szUserAgent != NULL)
	{
		free(m_szUserAgent);
	}
	if (m_szPostData != NULL)
	{
		free(m_szPostData);
	}
	if (m_szProxyHost != NULL)
	{
		free(m_szProxyHost);
	}
	if (m_szProxyUsername != NULL)
	{
		free(m_szProxyUsername);
	}

	free(m_szUrl);
}

void CCurl::SetUserAgent(wstring szUserAgent)
{
	if (m_szUserAgent != NULL)
	{
		free(m_szUserAgent);
	}

	m_szUserAgent = _strdup(ConvertToMultiByte(szUserAgent).c_str());

	curl_easy_setopt(m_lpCurl, CURLOPT_USERAGENT, m_szUserAgent);
}

void CCurl::SetUrlEncodedPostData(wstring szPostData)
{
	if (m_szPostData != NULL)
	{
		free(m_szPostData);
	}

	m_szPostData = _strdup(ConvertToMultiByte(szPostData).c_str());

	curl_easy_setopt(m_lpCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_lpCurl, CURLOPT_POSTFIELDS, m_szPostData);
	curl_easy_setopt(m_lpCurl, CURLOPT_POSTFIELDSIZE, strlen(m_szPostData));
}

BOOL CCurl::Execute()
{
	/* Perform the request */

	m_nResult = curl_easy_perform(m_lpCurl);

	if (m_nResult != CURLE_OK)
	{
		LOG1("cURL failed: %s", m_szError);

		return FALSE;
	}

	curl_easy_getinfo(m_lpCurl, CURLINFO_RESPONSE_CODE, &m_lStatus);

	if (m_fAutoRedirect)
	{
		wstring szRedirectUrl;

		for (TStringStringMapIter iter = m_vHeaders.begin(); iter != m_vHeaders.end(); iter++)
		{
			if (_wcsicmp(iter->first.c_str(), L"Location") == 0)
			{
				szRedirectUrl = iter->second;
				break;
			}
		}
		
		if (!szRedirectUrl.empty())
		{
			if (m_nAutoRedirectIndex >= MAX_AUTO_REDIRECT)
			{
				return FALSE;
			}
			else
			{
				return ExecuteAutoRedirect(szRedirectUrl);
			}
		}
	}

	return TRUE;
}

BOOL CCurl::ExecuteAutoRedirect(wstring szRedirectUrl)
{
	CCurl * lpRequest = new CCurl(szRedirectUrl);

	// Initialize the new cURL request according to our own settings.

	lpRequest->SetReader(GetReader());
	lpRequest->SetUserAgent(GetUserAgent());
	lpRequest->SetAutoRedirect(TRUE);
	lpRequest->SetIgnoreSSLErrors(GetIgnoreSSLErrors());
	lpRequest->SetTimeout(GetTimeout());

	// Increment the auto redirect index to detect too many redirects.

	lpRequest->m_nAutoRedirectIndex = m_nAutoRedirectIndex + 1;

	// Execute the redirected request.

	BOOL fResult = lpRequest->Execute();

	// Copy the status of the new request to our own.

	m_vHeaders.clear();

	for (TStringStringMapIter iter = lpRequest->m_vHeaders.begin(); iter != lpRequest->m_vHeaders.end(); iter++)
	{
		m_vHeaders[iter->first] = iter->second;
	}

	m_lStatus = lpRequest->m_lStatus;
	strcpy(m_szError, lpRequest->m_szError);
	m_nResult = lpRequest->m_nResult;

	delete lpRequest;

	return fResult;
}

void CCurl::SetCookies(CCurlCookies * lpCookies)
{
	if (lpCookies != NULL)
	{
		curl_slist * lpCookie = lpCookies->GetCookies();

		while (lpCookie != NULL)
		{
			curl_easy_setopt(m_lpCurl, CURLOPT_COOKIELIST, lpCookie->data);
			lpCookie = lpCookie->next;
		}
	}
}

void CCurl::SetIgnoreSSLErrors(BOOL fIgnore)
{
	m_fIgnoreSSLErrors = fIgnore;

	curl_easy_setopt(m_lpCurl, CURLOPT_SSL_VERIFYHOST, (int)!fIgnore);
	curl_easy_setopt(m_lpCurl, CURLOPT_SSL_VERIFYPEER, (int)!fIgnore); 
}

void CCurl::SetTimeout(INT nTimeout)
{
	if (nTimeout > 0)
	{
		m_nTimeout = nTimeout;

		curl_easy_setopt(m_lpCurl, CURLOPT_TIMEOUT, nTimeout);
	}
}

CCurlCookies * CCurl::GetCookies() const
{
	CURLcode nResult;
	struct curl_slist * lpCookies = NULL;

	nResult = curl_easy_getinfo(m_lpCurl, CURLINFO_COOKIELIST, &lpCookies);
	
	if (nResult == CURLE_OK && lpCookies != NULL)
	{
		return new CCurlCookies(lpCookies);
	}

	return NULL;
}

size_t CCurl::WriteData(void * lpData, size_t dwSize, size_t dwBlocks)
{
	// Drop the incoming data when no reader is assigned.

	if (!m_fDisableDataRead && m_lpReader != NULL && dwSize * dwBlocks > 0)
	{
		if (!m_lpReader->Read((LPBYTE)lpData, (DWORD)(dwSize * dwBlocks)))
		{
			return 0;
		}
	}

	return dwSize * dwBlocks;
}

size_t CCurl::WriteHeader(void * lpData, size_t dwSize, size_t dwBlocks)
{
	string szData((char *)lpData, dwSize * dwBlocks);
	size_t nOffset = szData.find(L':');

	if (nOffset != string::npos)
	{
		wstring szName = Trim(ConvertToWideChar(szData.substr(0, nOffset)));
		wstring szValue = Trim(ConvertToWideChar(szData.substr(nOffset + 1)));
		
		if (!szName.empty() && !szValue.empty())
		{
			if (m_fAutoRedirect && _wcsicmp(szName.c_str(), L"Location") == 0)
			{
				m_fDisableDataRead = TRUE;
			}

			m_vHeaders[szName] = szValue;
		}
	}

	return dwSize * dwBlocks;
}

wstring CCurl::GetString(INT nCodePage) const
{
	return ConvertToWideChar(&m_vData, nCodePage < 0 ? CP_UTF8 : nCodePage);
}

string CCurl::GetAnsiString() const
{
	return string((LPCSTR)_VECTOR_DATA(m_vData), m_vData.size());
}

static size_t CCurl_WriteData(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream)
{
	return ((CCurl *)lpStream)->WriteData(lpData, dwSize, dwBlocks);
}

static size_t CCurl_WriteHeader(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream)
{
	return ((CCurl *)lpStream)->WriteHeader(lpData, dwSize, dwBlocks);
}

static INT CCurl_DebugCallback(CURL * lpCurl, curl_infotype nInfoType, LPCSTR szMessage, size_t cbMessage, LPVOID lpParam)
{
	LPSTR szBuffer = (LPSTR)malloc(cbMessage + 1);

	memcpy(szBuffer, szMessage, cbMessage);
	szBuffer[cbMessage] = '\0';

	Log_Append(L"curl-log.txt", szMessage);

	free(szBuffer);

	return 0;
}
