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
	m_lpTargetWindow = lpTargetWindow;
	m_fCancelled = FALSE;

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
	CCurlMulti vMulti;

	while (!m_fCancelled)
	{
		vMulti.Perform();

		ProcessMessages(vMulti);

		DWORD dwResult;

		if (vMulti.GetRunning() > 0)
		{
			CEvent * lpEvent = vMulti.GetEvent();

			HANDLE vHandles[2] = { m_vEvent.GetHandle(), lpEvent->GetHandle() };

			dwResult = WaitForMultipleObjectsEx(
				_ARRAYSIZE(vHandles), vHandles, FALSE, CURL_WAIT_TIMEOUT, TRUE);

			delete lpEvent;
		}
		else
		{
			dwResult = WaitForSingleObject(m_vEvent.GetHandle(), CURL_WAIT_TIMEOUT);
		}

		if (m_fCancelled)
		{
			break;
		}

		if (dwResult == WAIT_OBJECT_0)
		{
			ProcessEvent(vMulti);
		}
	}

	CancelAllRequests();

	return 0;
}

void CCurlMonitor::ProcessEvent(CCurlMulti & vMulti)
{
	TCurlVector vQueueRequests;
	TCurlVector vCancelRequests;

	m_vLock.Enter();

	for (TCurlVectorIter iter = m_vQueueRequests.begin(); iter != m_vQueueRequests.end(); iter++)
	{
		vQueueRequests.push_back(*iter);
	}

	m_vQueueRequests.clear();

	for (TCurlVectorIter iter1 = m_vCancelRequests.begin(); iter1 != m_vCancelRequests.end(); iter1++)
	{
		vCancelRequests.push_back(*iter1);
	}

	m_vCancelRequests.clear();

	m_vLock.Leave();

	for (TCurlVectorIter iter2 = vCancelRequests.begin(); iter2 != vCancelRequests.end(); iter2++)
	{
		vMulti.Remove(*iter2);
	}

	CancelRequests(vCancelRequests);

	QueueRequests(vMulti, vQueueRequests);
}

void CCurlMonitor::CancelAllRequests()
{
	CancelRequests(m_vRequests);
}

void CCurlMonitor::ProcessMessages(CCurlMulti & vMulti)
{
	CURLMsg * lpMessage;

	while (( lpMessage = vMulti.GetNextMessage() ))
	{
		if (lpMessage->msg != CURLMSG_DONE)
		{
			LOG1("Could not understand message result %d", lpMessage->msg);
			continue;
		}

		// Copy the parameters.

		CURL * lpCurlHandle = lpMessage->easy_handle;
		CURLcode nCode = lpMessage->data.result;

		// Remove the handle from the vMulti.

		vMulti.Remove(lpCurlHandle);

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

		curl_easy_getinfo(lpCurlHandle, CURLINFO_RESPONSE_CODE, &lStatus);

		// Process the result from the message.

		SignalCompleted(lpCurl, nCode, lStatus);
	}
}

void CCurlMonitor::QueueRequests(CCurlMulti & vMulti, TCurlVector & vRequests)
{
	for (TCurlVectorIter iter = vRequests.begin(); iter != vRequests.end(); iter++)
	{
		m_vCache.Add(*iter);

		vMulti.Add(*iter);

		m_vRequests.push_back(*iter);
	}
}

void CCurlMonitor::CancelRequests(TCurlVector & vRequests)
{
	TCurlVector vToRemove;

	for (TCurlVectorIter iter = vRequests.begin(); iter != vRequests.end(); iter++)
	{
		SignalCompleted(*iter, (CURLcode)-1, 0);

		vToRemove.push_back(*iter);
	}

	for (TCurlVectorIter iter1 = vToRemove.begin(); iter1 != vToRemove.end(); iter1++)
	{
		TCurlVectorIter pos = find(m_vRequests.begin(), m_vRequests.end(), *iter1);

		ASSERT(pos != m_vRequests.end());

		m_vRequests.erase(pos);
	}
}

void CCurlMonitor::SignalCompleted(CCurl * lpCurl, CURLcode nCode, LONG lStatus)
{
	m_vCache.Remove(lpCurl);
	
	lpCurl->SignalCompleted(nCode, lStatus);
}
