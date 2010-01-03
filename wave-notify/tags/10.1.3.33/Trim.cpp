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

wstring Trim(wstring szValue)
{
	wstring::const_iterator begin;
	wstring::const_iterator end;
	wstring::const_iterator iter;
	BOOL fFound = FALSE;

	for (iter = szValue.begin(); iter < szValue.end(); iter++)
	{
		if (!iswspace(*iter))
		{
			begin = iter;
			break;
		}
	}

	if (iter == szValue.end())
	{
		return L"";
	}

	for (iter = szValue.end() - 1; iter >= szValue.begin(); iter--)
	{
		if (!iswspace(*iter))
		{
			end = iter + 1;
			break;
		}
	}

	return wstring(begin, end);
}
