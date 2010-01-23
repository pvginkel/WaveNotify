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

CWaveName::CWaveName() :
	m_nType(WNT_UNKNOWN)
{
}

CWaveName * CWaveName::CreateFromJson(Json::Value & vRoot)
{
	CWaveName * lpResult = new CWaveName();

	// (( scope ))
	{
		if (!vRoot.isObject())
		{
			goto __failure;
		}

		Json::Value & vName = vRoot[L"1"];
		Json::Value & vItem2 = vRoot[L"2"];

		if (
			!vName.isString() ||
			!vItem2.isObject()
		) {
			goto __failure;
		}

		Json::Value & vType = vItem2[L"1"];

		if (!vType.isIntegral())
		{
			goto __failure;
		}

		lpResult->m_szName = vName.asString();
		lpResult->m_nType = (WAVE_NAME_TYPE)vType.asUInt();

		return lpResult;
	}

__failure:
	LOG("Could not parse json");

	delete lpResult;

	return NULL;
}
