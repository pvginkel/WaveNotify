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

CWaveRequestStartListening::CWaveRequestStartListening(wstring szSearchString)
	: CWaveRequest(WMT_START_LISTENING)
{
	m_lpListener = CNotifierApp::Instance()->GetSession()->CreateListener(szSearchString);
}

void CWaveRequestStartListening::CreateRequest(Json::Value & vRoot)
{
	vRoot[L"1000"] = Json::Value(Json::arrayValue);
	vRoot[L"1000"][0u] = Json::Value(0);
	vRoot[L"1000"][1] = Json::Value(0);

	vRoot[L"2"] = Json::Value(m_lpListener->GetID());

	vRoot[L"3"] = Json::Value(m_lpListener->GetSearchString());

	vRoot[L"4"] = Json::Value(Json::objectValue);
	vRoot[L"4"][L"2"] = Json::Value(25);
	vRoot[L"4"][L"1"] = Json::Value(0);

	vRoot[L"6"] = Json::Value(Json::nullValue);
}

void CWaveRequestStartListening::RequestCompleted()
{
	CNotifierApp::Instance()->GetSession()->AddListener(m_lpListener);

	m_lpListener = NULL;
}
