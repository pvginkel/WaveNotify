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

CWaveView::CWaveView()
{
	m_lpContacts = new CWaveContactCollection();
	m_lpWaves = new CWaveCollection();
}

CWaveView::~CWaveView()
{
	delete m_lpContacts;
	delete m_lpWaves;
}

BOOL CWaveView::ProcessResponse(CWaveResponse * lpResponse)
{

	switch (lpResponse->GetType())
	{
	case WMT_GET_ALL_CONTACTS:
		ProcessContacts(((CWaveResponseGetAllContacts *)lpResponse)->GetContacts());
		break;

	case WMT_GET_CONTACT_DETAILS:
		ProcessContacts(((CWaveResponseGetContactDetails *)lpResponse)->GetContacts());
		break;

	case WMT_START_LISTENING:
		ProcessWaves((CWaveResponseStartListening *)lpResponse);
		break;

	case WMT_CONTACT_UPDATES:
		ProcessContactUpdates(((CWaveResponseContactUpdates *)lpResponse)->GetStatuses());

	default:
		return FALSE;
	}

	return TRUE;
}

void CWaveView::ProcessContacts(CWaveContactCollection * lpContacts)
{
	m_lpContacts->Merge(lpContacts);
}

void CWaveView::ProcessWaves(CWaveResponseStartListening * lpResponse)
{
	m_lpWaves->Merge(lpResponse->GetWaves());

	TStringVector vRemovedWaves;

	lpResponse->GetRemovedWaves(vRemovedWaves);

	m_lpWaves->RemoveWaves(vRemovedWaves);
}

void CWaveView::ProcessContactUpdates(CWaveContactStatusCollection * lpStatuses)
{
	m_lpContacts->Merge(lpStatuses);
}
