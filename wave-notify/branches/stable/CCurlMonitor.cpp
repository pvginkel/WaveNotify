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

CCurlMonitor::CCurlMonitor(CWindowHandle * lpTargetWindow) : CThread(TRUE)
{
	ASSERT(lpTargetWindow != NULL);

	m_lpTargetWindow = lpTargetWindow;
	m_fCancelled = FALSE;
	m_lpMulti = NULL;

	Resume();
}

CCurlMonitor::~CCurlMonitor()
{
	m_vLock.Enter();

	if (!m_fCancelled)
	{
		Cancel();
	}

	m_vLock.Leave();

	Join();
}

DWORD CCurlMonitor::ThreadProc()
{
	m_lpMulti = new CCurlMulti();

	while (!m_fCancelled)
	{
		m_lpMulti->Perform();

		ProcessMessages();

		DWORD dwResult;

		if (m_lpMulti->GetRunning() > 0)
		{
			CEvent * lpEvent = m_lpMulti->GetEvent();

			HANDLE vHandles[2] = { m_vEvent.GetHandle(), lpEvent->GetHandle() };

			dwResult = WaitForMultipleObjectsEx(
				_ARRAYSIZE(vHandles), vHandles, FALSE, CURL_WAIT_TIMEOUT, TRUE);

			delete lpEvent;
		}
		else
		{
			dwResult = WaitForSingleObject(m_vEvent.GetHandle(), CURL_WAIT_TIMEOUT);
		}

		if (dwResult == WAIT_FAILED)
		{
			LOG("Wait failed");
			m_fCancelled = TRUE;
		}

		if (m_fCancelled)
		{
			break;
		}

		if (dwResult == WAIT_OBJECT_0)
		{
			ProcessEvent();
		}
	}

	CancelAllRequests();

	delete m_lpMulti;

	m_lpMulti = NULL;

	return 0;
}

void CCurlMonitor::ProcessEvent()
{
	TCurlVector vQueueRequests;
	TCurlVector vCancelRequests;

	m_vLock.Enter();

	// Transport the m_vQueueRequests queue.

	for (TCurlVectorIter iter = m_vQueueRequests.begin(); iter != m_vQueueRequests.end(); iter++)
	{
		vQueueRequests.push_back(*iter);
	}

	m_vQueueRequests.clear();

	// Transport the m_vCancelRequests queue.

	for (TCurlVectorIter iter1 = m_vCancelRequests.begin(); iter1 != m_vCancelRequests.end(); iter1++)
	{
		// Verify whether we still have ownership of the requests.

		TCurlVectorIter pos = find(m_vRequests.begin(), m_vRequests.end(), *iter1);

		if (pos != m_vRequests.end())
		{
			vCancelRequests.push_back(*pos);

			m_vRequests.erase(pos);
		}
		else
		{
			// Check whether they're in our queue list.

			pos = find(vQueueRequests.begin(), vQueueRequests.end(), *iter1);

			if (pos != vQueueRequests.end())
			{
				// If we have a request to both queue and cancel, immediately
				// set it to completed.

				SignalCompleted(*pos, (CURLcode)-1, 0);

				vQueueRequests.erase(pos);
			}

			// If the cancel request was not in our queue list, and we do
			// not own it, we have already send a complete for it before the
			// main thread send the cancel request. Just ignore it.
		}
	}

	m_vCancelRequests.clear();

	m_vLock.Leave();

	// Process both queues.

	CancelRequests(vCancelRequests);

	QueueRequests(vQueueRequests);
}

void CCurlMonitor::CancelAllRequests()
{
	CancelRequests(m_vRequests);
}

void CCurlMonitor::ProcessMessages()
{
	CURLMsg * lpMessage;

	while (( lpMessage = m_lpMulti->GetNextMessage() ))
	{
		if (lpMessage->msg != CURLMSG_DONE)
		{
			LOG1("Could not understand message result %d", lpMessage->msg);
			continue;
		}

		// Copy the parameters.

		CURL * lpCurlHandle = lpMessage->easy_handle;
		CURLcode nCode = lpMessage->data.result;

		ASSERT(lpCurlHandle != NULL);

		// Remove the handle from the CCurlMulti.

		m_lpMulti->Remove(lpCurlHandle);

		// Find the curl object with the message.

		CCurl * lpCurl = NULL;

		for (TCurlVectorIter iter = m_vRequests.begin(); iter != m_vRequests.end(); iter++)
		{
			if ((*iter)->GetHandle() == lpCurlHandle)
			{
				lpCurl = *iter;
				m_vRequests.erase(iter);
				break;
			}
		}

		if (lpCurl == NULL)
		{
			LOG("Could not resolve easy_handle from message");
			continue;
		}

		// Get the status.

		long lStatus;

		nCode = curl_easy_getinfo(lpCurlHandle, CURLINFO_RESPONSE_CODE, &lStatus);

		CHECK(nCode == CURLE_OK);

		// Process the result from the message.

		SignalCompleted(lpCurl, nCode, lStatus);
	}
}

void CCurlMonitor::QueueRequests(TCurlVector & vRequests)
{
	for (TCurlVectorIter iter = vRequests.begin(); iter != vRequests.end(); iter++)
	{
		ASSERT(*iter != NULL);

		m_vCache.Add(*iter);

		m_lpMulti->Add(*iter);

		m_vRequests.push_back(*iter);
	}
}

void CCurlMonitor::CancelRequests(TCurlVector & vRequests)
{
	for (TCurlVectorIter iter = vRequests.begin(); iter != vRequests.end(); iter++)
	{
		ASSERT(*iter != NULL);

		m_lpMulti->Remove(*iter);

		SignalCompleted(*iter, (CURLcode)-1, 0);
	}

	vRequests.clear();
}

void CCurlMonitor::SignalCompleted(CCurl * lpCurl, CURLcode nCode, LONG lStatus)
{
	ASSERT(lpCurl != NULL);

	m_vCache.Remove(lpCurl);
	
	lpCurl->SignalCompleted(nCode, lStatus);
}
