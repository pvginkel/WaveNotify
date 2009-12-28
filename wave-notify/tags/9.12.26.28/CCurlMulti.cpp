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

CWSAEvent * CCurlMulti::GetEvent()
{
	fd_set rd, wr, exec;
	int max_fd = 0;

	FD_ZERO(&rd);
	FD_ZERO(&wr);
	FD_ZERO(&exec);

	if (curl_multi_fdset(m_lpMulti, &rd, &wr, &exec, &max_fd) != CURLE_OK)
		FAIL("Could not curl_multi_fdset");

	// Finding out what sockets must be set with what network event is done through
	// a map because we cannot to two subsequent calls to WSAEventSelect with
	// the two network events. This must be done in one call with the network
	// events or'red. The sockets and network events are gathered here and
	// added to the event lateron.

	TSocketMap vSockets;

	AddSockets(vSockets, rd, FD_READ);
	AddSockets(vSockets, wr, FD_WRITE);

	CWSAEvent * lpEvent = new CWSAEvent();

	// Exec is completely ignored?!? Verified and should be OK.

	for (TSocketMapIter iter = vSockets.begin(); iter != vSockets.end(); iter++)
	{
		lpEvent->EventSelect(iter->first, iter->second);
	}

	return lpEvent;
}

void CCurlMulti::AddSockets(TSocketMap & vSockets, fd_set & vSet, INT nEventType)
{
	for (DWORD i = 0; i < vSet.fd_count; i++)
	{
		if (vSockets.find(vSet.fd_array[i]) == vSockets.end())
		{
			vSockets[vSet.fd_array[i]] = 0;
		}

		vSockets[vSet.fd_array[i]] |= nEventType;
	}
}

BOOL CCurlMulti::Perform()
{
	CURLMcode nResult;

	while (( nResult = curl_multi_perform(m_lpMulti, &m_nRunning) ) == CURLM_CALL_MULTI_PERFORM)
		;

	if (nResult != CURLM_OK)
		FAIL("Wrong result from curl_multi_perform");

	return !Completed();
}

CURLcode CCurlMulti::GetLastResult()
{
	CURLMsg * lpMesg = curl_multi_info_read(m_lpMulti, &m_nRunning);

	return lpMesg->data.result;
}
