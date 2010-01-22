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

CWaveContactStatus::CWaveContactStatus(Json::Value & vRoot)
{
	m_szEmailAddress = vRoot[L"2"][L"1"].asString();
	
	Json::Value & vStatus = vRoot[L"3"];

	m_fOnline = vStatus[L"2"].asBool();
	m_szStatusMessage = vStatus[L"3"].asString();
}

CWaveContactStatus::~CWaveContactStatus()
{
}
