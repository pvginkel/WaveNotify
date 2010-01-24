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
#include "wave.h"
#include "notifierapp.h"

CUnreadWaveCollection::CUnreadWaveCollection(CWaveCollection * lpLastReported, CWaveCollection * lpCurrent)
{
	CNotifierApp::Instance()->GetSession()->SuspendRequestFlush();

	if (lpLastReported == NULL)
	{
		InsertAllWaves(lpCurrent);
	}
	else
	{
		InsertChangesOnly(lpLastReported, lpCurrent);
	}

	CNotifierApp::Instance()->GetSession()->ReleaseRequestFlush();
}

CUnreadWaveCollection::~CUnreadWaveCollection()
{
	for (TUnreadWaveVectorIter iter = m_vUnreadsVector.begin(); iter != m_vUnreadsVector.end(); iter++)
	{
		delete *iter;
	}
}

void CUnreadWaveCollection::InsertAllWaves(CWaveCollection * lpCurrent)
{
	ASSERT(lpCurrent != NULL);

	const TWaveMap & vCurrent = lpCurrent->GetWaves();

	for (TWaveMapConstIter iter = vCurrent.begin(); iter != vCurrent.end(); iter++)
	{
		if (GetChangedStatus(NULL, iter->second) == WCHS_CHANGED)
		{
			CUnreadWave * lpUnread = GetDifference(NULL, iter->second);

			if (lpUnread != NULL)
			{
				Insert(lpUnread);
			}
		}
	}
}

void CUnreadWaveCollection::InsertChangesOnly(CWaveCollection * lpLastReported, CWaveCollection * lpCurrent)
{
	ASSERT(lpLastReported != NULL && lpCurrent != NULL);

	const TWaveMap & vCurrent = lpCurrent->GetWaves();
	const TWaveMap & vLastReported = lpLastReported->GetWaves();

	for (TWaveMapConstIter iter = vCurrent.begin(); iter != vCurrent.end(); iter++)
	{
		TWaveMapConstIter pos = vLastReported.find(iter->first);

		CWave * lpReportedWave = pos == vLastReported.end() ? NULL : pos->second;

		WAVE_CHANGED_STATUS nStatus = GetChangedStatus(lpReportedWave, iter->second);

		if (nStatus == WCHS_REFRESH)
		{
			lpLastReported->AddWave(new CWave(*iter->second));
		}
		else if (nStatus == WCHS_CHANGED)
		{
			CUnreadWave * lpUnread = GetDifference(lpReportedWave, iter->second);

			if (lpUnread != NULL)
			{
				Insert(lpUnread);
			}
		}
	}
}

void CUnreadWaveCollection::Insert(CUnreadWave * lpUnread)
{
	ASSERT(lpUnread != NULL);

	TUnreadWaveVectorIter pos = m_vUnreadsVector.end();

	for (TUnreadWaveVectorIter iter = m_vUnreadsVector.begin(); iter != m_vUnreadsVector.end(); iter++)
	{
		// TODO: Is this correct? This does not see to check the ID.

		if ((*iter)->GetTime() < lpUnread->GetTime())
		{
			pos = iter;
			break;
		}
	}

	m_vUnreadsVector.insert(pos, lpUnread);

	m_vUnreadsMap[lpUnread->GetID()] = lpUnread;
}

INT CUnreadWaveCollection::Find(CUnreadWave * lpUnread) const
{
	ASSERT(lpUnread != NULL);

	INT i = 0;

	for (TUnreadWaveVectorConstIter iter = m_vUnreadsVector.begin(); iter != m_vUnreadsVector.end(); iter++)
	{
		if ((*iter)->GetID() == lpUnread->GetID())
		{
			return i;
		}

		i++;
	}

	return -1;
}

CUnreadWave * CUnreadWaveCollection::GetDifference(CWave * lpReportedWave, CWave * lpNewWave) const
{
	ASSERT(lpNewWave != NULL);

	// So we're sure we want to create a popup for this Wave. The first message
	// that appears in the new Wave which does not appear in the old Wave, is
	// the message we're going to report.

	CWaveMessage * lpToReport = NULL;

	const TWaveMessageVector & vNewMessages = lpNewWave->GetMessages();

	if (lpReportedWave != NULL)
	{
		const TWaveMessageVector & vReportedMessages = lpReportedWave->GetMessages();

		for (TWaveMessageVectorConstIter iter_n = vNewMessages.begin(); iter_n != vNewMessages.end(); iter_n++)
		{
			BOOL fFound = FALSE;

			for (TWaveMessageVectorConstIter iter_r = vReportedMessages.begin(); iter_r != vReportedMessages.end(); iter_r++)
			{
				if (**iter_n == **iter_r)
				{
					fFound = TRUE;
					break;
				}
			}

			if (!fFound)
			{
				lpToReport = *iter_n;
				break;
			}
		}
	}

	// OK, hmmm. All messages that appeared in the new Wave were also present
	// in the reported Wave. Well, lets just pick the first message of the new Wave
	// to report then.

	if (lpToReport == NULL)
	{
		lpToReport = vNewMessages[0];
	}

	return new CUnreadWave(lpNewWave, lpToReport);
}

WAVE_CHANGED_STATUS CUnreadWaveCollection::GetChangedStatus(CWave * lpReportedWave, CWave * lpNewWave) const
{
	ASSERT(lpNewWave != NULL);

	if (WavesEqual(lpReportedWave, lpNewWave))
	{
		return WCHS_IGNORE;
	}

	if (lpNewWave->GetUnreadMessages() == 0)
	{
		return WCHS_REFRESH;
	}

	wstring szSelf = CNotifierApp::Instance()->GetSession()->GetEmailAddress();

	if (lpReportedWave == NULL)
	{
		// New waves with only messages of self (i.e. a new wave that is
		// currently being created) is set to refresh.

		const TWaveMessageVector & vMessages = lpNewWave->GetMessages();

		BOOL fHadOne = FALSE;
		BOOL fIsSelf = TRUE;
		BOOL fIsEmpty = lpNewWave->GetSubject().empty();

		for (TWaveMessageVectorConstIter iter = vMessages.begin(); iter != vMessages.end(); iter++)
		{
			fHadOne = TRUE;

			if ((*iter)->GetEmailAddress() != szSelf)
			{
				fIsSelf = FALSE;
			}

			if (!(*iter)->GetText().empty())
			{
				fIsEmpty = FALSE;
			}
		}

		return fHadOne && fIsSelf ? WCHS_REFRESH : ( fIsEmpty ? WCHS_IGNORE : WCHS_CHANGED );
	}

	INT nNewMessages = lpNewWave->GetUnreadMessages() - lpReportedWave->GetUnreadMessages();

	if (nNewMessages <= 0)
	{
		return WCHS_REFRESH;
	}

	// If the unread count is as much higher as new messages of the owner, we do
	// not consider the new wave. These messages will disappear when the owner
	// stops typing.

	TWaveMessageVector vReportedMessages(lpReportedWave->GetMessages());
	const TWaveMessageVector & vNewMessages = lpNewWave->GetMessages();

	for (TWaveMessageVectorConstIter iter_n = vNewMessages.begin(); iter_n != vNewMessages.end(); iter_n++)
	{
		BOOL fFound = FALSE;

		for (TWaveMessageVectorIter iter_r = vReportedMessages.begin(); iter_r != vReportedMessages.end(); iter_r++)
		{
			if (**iter_n == **iter_r)
			{
				fFound = TRUE;

				vReportedMessages.erase(iter_r);
				break;
			}
		}

		if (!fFound)
		{
			if (
				(*iter_n)->GetEmailAddress() == szSelf ||
				Trim((*iter_n)->GetText()).empty()
			) {
				nNewMessages--;
			}
			else
			{
				// We have found a message that is not of owner and has
				// not been reported before. Must consider.

				return WCHS_CHANGED;
			}
		}
	}

	// When all new messages were of the owner, we do not consider the new wave.

	return nNewMessages > 0 ? WCHS_CHANGED : WCHS_IGNORE;
}

BOOL CUnreadWaveCollection::WavesEqual(CWave * lpReportedWave, CWave * lpNewWave) const
{
	ASSERT(lpNewWave != NULL);

	if (lpReportedWave == NULL)
	{
		return FALSE;
	}

	if (
		lpReportedWave->GetTotalMessages() != lpNewWave->GetTotalMessages() ||
		lpReportedWave->GetUnreadMessages() != lpNewWave->GetUnreadMessages() ||
		lpReportedWave->GetSubject() != lpNewWave->GetSubject()
	) {
		return FALSE;
	}

	// We do not detect whether all messages are the same but rather whether
	// all messages in the new wave appear as such in the old wave. This is
	// equal for us.

	const TWaveMessageVector & vReportedMessages = lpReportedWave->GetMessages();
	const TWaveMessageVector & vNewMessages = lpNewWave->GetMessages();

	for (TWaveMessageVectorConstIter iter_n = vNewMessages.begin(); iter_n != vNewMessages.end(); iter_n++)
	{
		BOOL fFound = FALSE;

		for (TWaveMessageVectorConstIter iter_r = vReportedMessages.begin(); iter_r != vReportedMessages.end(); iter_r++)
		{
			if (**iter_n == **iter_r)
			{
				fFound = TRUE;
				break;
			}
		}

		if (!fFound)
		{
			return FALSE;
		}
	}

	return TRUE;
}
