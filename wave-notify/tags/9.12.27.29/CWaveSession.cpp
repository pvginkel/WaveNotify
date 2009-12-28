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

CWaveSession::CWaveSession(wstring szUsername, wstring szPassword)
{
	m_szUsername = szUsername;
	m_szPassword = szPassword;
	m_lpCookies = NULL;
	m_szAuthKey = L"";
	m_szEmailAddress = L"";
	m_lpChannel = NULL;
	m_nLoginError = WLE_SUCCESS;
}

CWaveSession::~CWaveSession()
{
	if (m_lpCookies != NULL)
	{
		delete m_lpCookies;
	}
	
	StopListener();
}

BOOL CWaveSession::Login()
{
	m_vLock.Enter();

	BOOL fResult =
		GetAuthKey() &&
		GetAuthCookie() &&
		GetSessionDetails();

	m_vLock.Leave();

	return fResult;
}

BOOL CWaveSession::GetAuthKey()
{
	CCurl vRequest(WAVE_URL_CLIENTLOGIN);
	CCurlUTF8StringReader vReader;

	vRequest.SetUserAgent(USERAGENT);
	vRequest.SetTimeout(CURL_TIMEOUT);
	vRequest.SetIgnoreSSLErrors(TRUE);
	vRequest.SetReader(&vReader);

	wstringstream szPostData;

	szPostData
		<< L"accountType=GOOGLE&Email="
		<< UrlEncode(m_szUsername)
		<< L"&Passwd="
		<< UrlEncode(m_szPassword)
		<< L"&service=wave&source=net.sf.wave-notify";

	vRequest.SetUrlEncodedPostData(szPostData.str());

	CNotifierApp::Instance()->GetCurlCache()->Add(&vRequest);

	BOOL fResult = vRequest.Execute();

	CNotifierApp::Instance()->GetCurlCache()->Remove(&vRequest);

	if (!fResult)
	{
		m_nLoginError = WLE_NETWORK;

		return FALSE;
	}

	m_szAuthKey = GetAuthKeyFromRequest(vReader.GetString());

	m_nLoginError = m_szAuthKey.empty() ? WLE_AUTHENTICATE : WLE_SUCCESS;

	return m_nLoginError == WLE_SUCCESS;
}

wstring CWaveSession::GetAuthKeyFromRequest(wstring & szResponse) const
{
	TStringStringMap vMap;

	if (!ParseStringMap(szResponse, vMap))
	{
		return L"";
	}

	TStringStringMapIter pos = vMap.find(L"Auth");

	if (pos == vMap.end())
	{
		return L"";
	}
	else
	{
		return pos->second;
	}
}

BOOL CWaveSession::GetAuthCookie()
{
	CCurl vRequest(Format(WAVE_URL_AUTH, UrlEncode(m_szAuthKey).c_str()));

	vRequest.SetUserAgent(USERAGENT);
	vRequest.SetTimeout(CURL_TIMEOUT);
	vRequest.SetIgnoreSSLErrors(TRUE);

	CNotifierApp::Instance()->GetCurlCache()->Add(&vRequest);

	BOOL fResult = vRequest.Execute();

	CNotifierApp::Instance()->GetCurlCache()->Remove(&vRequest);

	if (!fResult)
	{
		m_nLoginError = WLE_NETWORK;

		return FALSE;
	}

	m_lpCookies = vRequest.GetCookies();

	m_nLoginError = m_lpCookies == NULL ? WLE_AUTHENTICATE : WLE_SUCCESS;

	return m_nLoginError == WLE_SUCCESS;
}

void CWaveSession::SignOut()
{
	m_vLock.Enter();

	if (m_lpCookies != NULL)
	{
		CCurl vRequest(WAVE_URL_LOGOUT);

		vRequest.SetUserAgent(USERAGENT);
		vRequest.SetTimeout(CURL_TIMEOUT);
		vRequest.SetIgnoreSSLErrors(TRUE);
		vRequest.SetCookies(m_lpCookies);
		vRequest.SetAutoRedirect(TRUE);

		CNotifierApp::Instance()->GetCurlCache()->Add(&vRequest);
	
		vRequest.Execute();

		CNotifierApp::Instance()->GetCurlCache()->Remove(&vRequest);

		delete m_lpCookies;
		m_lpCookies = NULL;
	}

	m_vLock.Leave();
}

BOOL CWaveSession::GetSessionDetails()
{
	CCurl vRequest(WAVE_URL_WAVES);
	CCurlUTF8StringReader vReader;

	vRequest.SetUserAgent(USERAGENT);
	vRequest.SetTimeout(CURL_TIMEOUT);
	vRequest.SetIgnoreSSLErrors(TRUE);
	vRequest.SetCookies(m_lpCookies);
	vRequest.SetReader(&vReader);

	CNotifierApp::Instance()->GetCurlCache()->Add(&vRequest);

	BOOL fResult = vRequest.Execute();

	CNotifierApp::Instance()->GetCurlCache()->Remove(&vRequest);

	if (!fResult)
	{
		m_nLoginError = WLE_NETWORK;

		return FALSE;
	}

	wstring szResponse(vReader.GetString());
	wstring szSessionData(GetSessionData(szResponse));

	if (szSessionData.empty())
	{
		m_nLoginError = WLE_AUTHENTICATE;

		return FALSE;
	}

	m_szEmailAddress = GetKeyFromSessionResponse(L"username", szSessionData);
	m_szSessionID = GetKeyFromSessionResponse(L"sessionid", szSessionData);
	m_szProfileID = GetKeyFromSessionResponse(L"id", szSessionData);

	if (
		!m_szEmailAddress.empty() &&
		!m_szSessionID.empty() &&
		!m_szProfileID.empty()
	) {
		m_nLoginError = WLE_SUCCESS;
	}
	else
	{
		m_nLoginError = WLE_AUTHENTICATE;
	}

	return m_nLoginError == WLE_SUCCESS;
}

wstring CWaveSession::GetSessionData(wstring & szResponse) const
{
	INT nBegin = 0;

	// Walk all script blocks

	for (;;)
	{
		// Find the start of the script block

		nBegin = szResponse.find(L"<script", nBegin);

		if (nBegin != wstring::npos)
		{
			nBegin = szResponse.find(L'>', nBegin);
		}

		if (nBegin == wstring::npos)
		{
			break;
		}

		nBegin++;

		// Find the end of the script block

		INT nEnd = szResponse.find(L"</script", nBegin);

		if (nEnd == wstring::npos)
		{
			break;
		}

		// Does the current script block contain the __session token?

		INT nSessionBegin = szResponse.find(L"__session", nBegin);

		if (nSessionBegin != wstring::npos && nSessionBegin < nEnd)
		{
			// The session block is always one single line

			INT nSessionEnd = szResponse.find(L'\n', nSessionBegin);

			// Return the session block

			return szResponse.substr(nSessionBegin, nSessionEnd - nSessionBegin);
		}

		// Skip over the end of the script block

		nBegin = nEnd;
	}

	return L"";
}

wstring CWaveSession::GetInboxUrl() const
{
	return Format(WAVE_URL_INBOX, UrlEncode(m_szAuthKey).c_str());
}

wstring CWaveSession::GetWaveUrl(wstring szWaveId) const
{
	return Format(WAVE_URL_WAVE, UrlEncode(m_szAuthKey).c_str(), UrlEncode(UrlEncode(szWaveId)).c_str());
}

wstring CWaveSession::GetKeyFromSessionResponse(wstring szKey, wstring & szResponse) const
{
	INT nOffset = 0;

	while (nOffset != string::npos)
	{
		nOffset = szResponse.find(szKey + L":'", nOffset);

		if (nOffset != string::npos)
		{
			CHAR cBefore = nOffset == 0 ? '\0' : szResponse[nOffset - 1];

			nOffset += szKey.length() + 1;

			if (!isalnum(cBefore) && cBefore != '_')
			{
				break;
			}
		}
	}

	if (nOffset != string::npos)
	{
		nOffset = szResponse.find(L'\'', nOffset) + 1;
	}

	if (nOffset != string::npos)
	{
		INT nEnd = szResponse.find(L'\'', nOffset);

		if (nEnd != string::npos)
		{
			return szResponse.substr(nOffset, nEnd - nOffset);
		}
	}

	return L"";
}

BOOL CWaveSession::IsLoggedIn() const
{
	return m_lpCookies != NULL;
}

void CWaveSession::StartListener()
{
	if (m_lpChannel == NULL)
	{
		m_lpChannel = new CWaveChannel(this, CNotifierApp::Instance()->GetAppWindow());
	}
}

void CWaveSession::StopListener()
{
	if (m_lpChannel != NULL)
	{
		m_lpChannel->Cancel();

		delete m_lpChannel;

		m_lpChannel = NULL;
	}
}

void CWaveSession::SetCookies(CCurlCookies * lpCookies)
{
	m_vLock.Enter();

	if (m_lpCookies != NULL)
	{
		delete m_lpCookies;
	}

	m_lpCookies = lpCookies;

	m_vLock.Leave();
}

BOOL CWaveSession::PostRequests(TWaveRequestVector & vRequests)
{
	// This function is responsible for deleting the requests because requests
	// may later be stored. Responses can be linked to a request and when this
	// becomes necessary, this function will take the requests for later retrieval.
	// Now, we just delete them.

	BOOL fResult = FALSE;

	if (m_lpChannel != NULL)
	{
		fResult = m_lpChannel->PostRequests(vRequests);
	}
	else
	{
		LOG("Could not post request; channel not running");
	}

	for (TWaveRequestVectorIter iter = vRequests.begin(); iter != vRequests.end(); iter++)
	{
		delete *iter;
	}

	vRequests.clear();

	return fResult;
}

void CWaveSession::AddListener(CWaveListener * lpListener)
{
	if (m_lpChannel != NULL)
	{
		m_lpChannel->AddListener(lpListener);
	}
	else
	{
		LOG("Could not add listener; channel not running");
		delete lpListener;
	}
}

CWaveListener * CWaveSession::CreateListener(wstring szSearchString)
{
	if (m_lpChannel != NULL)
	{
		return m_lpChannel->CreateListener(szSearchString);
	}
	else
	{
		LOG("Could not create listener; channel not running");
		return NULL;
	}
}

BOOL CWaveSession::RemoveListener(wstring szID)
{
	if (m_lpChannel != NULL)
	{
		return m_lpChannel->RemoveListener(szID);
	}
	else
	{
		LOG("Could not remove listener; channel not running");
		return FALSE;
	}
}
