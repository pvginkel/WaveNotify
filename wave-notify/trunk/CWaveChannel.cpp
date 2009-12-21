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

#define RECONNECT_DELAY	5000

class CWaveReader : public CCurlReader
{
private:
	wstringstream m_szBuffer;
	CUTF8Converter m_vConverter;
	CWaveChannel * m_lpChannel;

public:
	CWaveReader(CWaveChannel * lpChannel) { m_lpChannel = lpChannel; }
	BOOL Read(LPBYTE lpData, DWORD cbData);
	
private:
	BOOL PumpResponseBuffer();
	BOOL PumpMessage(BOOL & fSuccess);
};

////////////////////////////////////////////////////////////////////////////////
// CWaveChannel ////////////////////////////////////////////////////////////////

CWaveChannel::CWaveChannel(CWaveSession * lpSession, CWindowHandle * lpTargetWindow) : CThread(TRUE)
{
	m_lpSession = lpSession;
	m_lpTargetWindow = lpTargetWindow;
	
	ResetChannelParameters();

	Start();
}

CWaveChannel::~CWaveChannel()
{
	for (TWaveListenerMapIter iter = m_vListeners.begin(); iter != m_vListeners.end(); iter++)
	{
		delete iter->second;
	}
}

DWORD CWaveChannel::ThreadProc()
{
	BOOL fCancelled = FALSE;
	CCurlCache vCache;

	while (!fCancelled)
	{
		if (!m_lpSession->IsLoggedIn())
		{
			ReportStatus(LTS_RECONNECTING);

			m_lpSession->Login();
			ResetChannelParameters();

			if (!m_lpSession->IsLoggedIn())
			{
				DWORD dwResult = WaitForSingleObject(
					m_vCancelEvent.GetHandle(), RECONNECT_DELAY);

				if (dwResult == WAIT_OBJECT_0)
				{
					fCancelled = TRUE;
					break;
				}

				continue;
			}
		}

		if (m_szSID.empty())
		{
			if (!RetrieveSID())
			{
				m_lpSession->SignOut();
				continue;
			}

			ReportStatus(LTS_CONNECTED);
		}

		wstring szUrl = Format(WAVE_URL_CHANNEL, m_szSID.c_str(), m_nAID, BuildHash().c_str());

		CCurl vRequest(szUrl);
		CWaveReader vReader(this);

		vRequest.SetUserAgent(USERAGENT);
		vRequest.SetTimeout(600);
		vRequest.SetIgnoreSSLErrors(TRUE);
		vRequest.SetCookies(m_lpSession->GetCookies());
		vRequest.SetReader(&vReader);

		vCache.Add(&vRequest);

		CCurlMulti vMulti;

		vMulti.Add(&vRequest);

		while (vMulti.Perform())
		{
			CEvent * lpEvent = vMulti.GetEvent();

			HANDLE vHandles[2] = { m_vCancelEvent.GetHandle(), lpEvent->GetHandle() };

			DWORD dwResult = WaitForMultipleObjectsEx(
				_ARRAYSIZE(vHandles), vHandles, FALSE, CURL_WAIT_TIMEOUT, TRUE);

			delete lpEvent;

			if (dwResult == WAIT_OBJECT_0)
			{
				fCancelled = TRUE;
				break;
			}
		}

		vCache.Remove(&vRequest);

		if (!fCancelled)
		{
			CURLcode nResult = vMulti.GetLastResult();

			if (nResult != CURLE_OK && nResult != CURLE_OPERATION_TIMEOUTED)
			{
				LOG1("cURL failed: %d", nResult);

				m_lpSession->SignOut();
			}
			else
			{
				m_lpSession->SetCookies(vRequest.GetCookies());
			}
		}

		vMulti.Remove(&vRequest);
	}

	m_lpSession->SignOut();

	ResetChannelParameters();

	return 0;
}

void CWaveChannel::ResetChannelParameters()
{
	m_szSID = L"";
	m_nAID = 0;
	m_nRID = Rand(10000, 90000);
	m_nNextRequestID = 0;

	m_vLock.Enter();

	m_nNextListenerID = 0;

	m_vLock.Leave();

/*
	for (TWaveRequestMapIter iter = m_vRequests.begin(); iter != m_vRequests.end(); iter++)
	{
		delete iter->second;
	}

	m_vRequests.clear();
*/
}

BOOL CWaveChannel::RetrieveSID()
{
	wstring szUrl = Format(WAVE_URL_SESSIONID, m_nRID++, BuildHash().c_str());

	CCurl vRequest(szUrl);
	CCurlUTF8StringReader vReader;

	vRequest.SetUserAgent(USERAGENT);
	vRequest.SetTimeout(CURL_TIMEOUT);
	vRequest.SetIgnoreSSLErrors(TRUE);
	vRequest.SetCookies(m_lpSession->GetCookies());
	vRequest.SetReader(&vReader);

	vRequest.SetUrlEncodedPostData(L"count=0");

	if (!vRequest.Execute())
	{
		return FALSE;
	}

	m_lpSession->SetCookies(vRequest.GetCookies());

	wstring szResponse(vReader.GetString());

	if (szResponse.empty())
	{
		return FALSE;
	}

	DWORD dwPos = szResponse.find(L'\n');

	if (dwPos == wstring::npos)
	{
		return FALSE;
	}

	DWORD dwLength = _wtol(szResponse.substr(0, dwPos).c_str());

	if (szResponse.length() < dwPos + 1 + dwLength)
	{
		return FALSE;
	}

	return ParseResponse(szResponse.substr(dwPos + 1, dwLength));
}

wstring CWaveChannel::BuildHash()
{
	wstring szResult;

	szResult.reserve(12);

	for (INT i = 0; i < 12; i++)
	{
		szResult += *(WAVE_HASH_POOL + Rand(0, _ARRAYSIZE(WAVE_HASH_POOL) - 1));
	}

	return szResult;
}

BOOL CWaveChannel::ParseResponse(wstring szResponse)
{
	Json::Reader vReader;
	Json::Value vRoot;

	if (!vReader.parse(szResponse, vRoot))
	{
		return FALSE;
	}

	BOOL fSuccess = TRUE;

	for (DWORD i = 0; i < vRoot.size(); i++)
	{
		Json::Value vItem(vRoot[i]);
		INT nAID = vItem[0u].asInt();

		Json::Value vContent(vItem[1]);

		wstring szType = vContent[0u].asString();

		fSuccess = FALSE;

		if (szType == L"c")
		{
			if (vContent.size() > 1)
			{
				m_szSID = vContent[1].asString();

				fSuccess = TRUE;
			}
		}
		else if (szType == L"wfe")
		{
			if (vContent.size() > 1)
			{
				CWaveResponse * lpResponse = ParseWfeResponse(vContent[1].asString(), fSuccess);

				if (fSuccess && lpResponse != NULL)
				{
					ReportReceived(lpResponse);
				}
			}
		}
		else
		{
			// There are noop's being sent, but everything else is also
			// ignored.

			fSuccess = TRUE;
		}

		if (fSuccess)
		{
			m_nAID = nAID;
		}
		else
		{
			break;
		}
	}

	return fSuccess;
}

CWaveResponse * CWaveChannel::ParseWfeResponse(wstring szResponse, BOOL & fSuccess)
{
	Json::Reader vReader;
	Json::Value vRoot;

	if (!vReader.parse(szResponse, vRoot))
	{
		fSuccess = FALSE;
		return NULL;
	}

	fSuccess = TRUE;

	return CWaveResponse::Parse(vRoot);
}

BOOL CWaveChannel::PostRequests(const TWaveRequestVector & vRequests)
{
	wstringstream szPostData;

	szPostData << L"count=" << vRequests.size();

	INT nOffset = 0;

	for (TWaveRequestVectorConstIter iter = vRequests.begin(); iter != vRequests.end(); iter++)
	{
		szPostData << L"&req" << nOffset << L"_key=" << UrlEncode(SerializeRequest(*iter));

		nOffset++;
	}

	// Post the JSON to the channel.

	wstring szUrl = Format(WAVE_URL_CHANNEL_POST, m_szSID.c_str(), m_nRID++, BuildHash().c_str());

	CCurl vRequest(szUrl);

	vRequest.SetUserAgent(USERAGENT);
	vRequest.SetTimeout(CURL_TIMEOUT);
	vRequest.SetIgnoreSSLErrors(TRUE);
	vRequest.SetCookies(m_lpSession->GetCookies());

	vRequest.SetUrlEncodedPostData(szPostData.str());

	CNotifierApp::Instance()->GetCurlCache()->Add(&vRequest);

	BOOL fResult = vRequest.Execute();

	CNotifierApp::Instance()->GetCurlCache()->Remove(&vRequest);

	if (fResult)
	{
		for (TWaveRequestVectorConstIter iter = vRequests.begin(); iter != vRequests.end(); iter++)
		{
			(*iter)->RequestCompleted();
		}
	}

	return fResult;
}

wstring CWaveChannel::SerializeRequest(CWaveRequest * lpRequest)
{
	// Prepare the request basics.

	Json::Value vRoot(Json::objectValue);

	vRoot[L"a"] = Json::Value(m_lpSession->GetSessionID());
	vRoot[L"r"] = Json::Value(Format(L"%x", m_nNextRequestID++));
	vRoot[L"t"] = Json::Value(lpRequest->GetType());
	vRoot[L"p"] = Json::Value(Json::objectValue);

	// Let the request object write the parameters.

	lpRequest->CreateRequest(vRoot[L"p"]);

	// Get the encoded JSON data.

	Json::FastWriter vWriter;

	return vWriter.write(vRoot);
}

void CWaveChannel::AddListener(CWaveListener * lpListener)
{
	m_vLock.Enter();

	m_vListeners[lpListener->GetID()] = lpListener;
	
	m_vLock.Leave();
}

CWaveListener * CWaveChannel::CreateListener(wstring szSearchString)
{
	m_vLock.Enter();

	INT nListenerID = m_nNextListenerID++;

	m_vLock.Leave();

	wstring szListenerID = Format(L"%s%d", m_lpSession->GetSessionID().c_str(), nListenerID);

	return new CWaveListener(szListenerID, szSearchString);
}

BOOL CWaveChannel::RemoveListener(wstring szID)
{
	BOOL fResult;

	m_vLock.Enter();

	TWaveListenerMapIter pos = m_vListeners.find(szID);

	fResult = pos != m_vListeners.end();

	if (fResult)
	{
		m_vListeners.erase(pos);

		delete pos->second;
	}

	m_vLock.Leave();

	return fResult;
}

////////////////////////////////////////////////////////////////////////////////
// CWaveReader /////////////////////////////////////////////////////////////////

BOOL CWaveReader::Read(LPBYTE lpData, DWORD cbData)
{
	m_szBuffer << m_vConverter.Parse(lpData, cbData);

	return PumpResponseBuffer();
}

BOOL CWaveReader::PumpResponseBuffer()
{
	BOOL fSuccess;

	while (PumpMessage(fSuccess) && fSuccess)
		;

	return fSuccess;
}

BOOL CWaveReader::PumpMessage(BOOL & fSuccess)
{
	wstring szReceived = m_szBuffer.str();

	// Does the current content has a newline?

	DWORD dwPos = szReceived.find(L'\n');

	if (dwPos == wstring::npos)
	{
		// No full packet in yet.

		fSuccess = TRUE;
		return FALSE;
	}

	wstring szLength = szReceived.substr(0, dwPos);

	if (!isdigit(szLength))
	{
		// The first line was not a number; we cannot read a package.

		fSuccess = FALSE;
		return FALSE;
	}

	DWORD dwLength = _wtol(szLength.c_str());

	if (szReceived.length() < dwPos + 1 + dwLength)
	{
		// No full packet in yet.

		fSuccess = TRUE;
		return FALSE;
	}

	// We have received a full package; parse to a response and submit.

	fSuccess = m_lpChannel->ParseResponse(szReceived.substr(dwPos + 1, dwLength));

	m_szBuffer.str(L"");

	if (!fSuccess)
	{
		return FALSE;
	}

	if (szReceived.length() > dwPos + 1 + dwLength)
	{
		m_szBuffer << szReceived.substr(dwPos + 1 + dwLength);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
