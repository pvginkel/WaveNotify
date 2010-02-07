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

bool isword(char _Ch)
{
	return (_Ch == '_' || isalnum(_Ch));
}

bool iswword(wchar_t _Ch)
{
	return (_Ch == L'_' || iswalnum(_Ch));
}

bool iswalnum(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswalnum(*iter))
		{
			return false;
		}
	}

	return true;
}

bool isawlpha(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswalpha(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswcntrl(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswcntrl(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswdigit(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswdigit(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswgraph(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswgraph(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswlower(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswlower(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswprint(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswprint(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswpunct(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswpunct(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswspace(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswspace(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswupper(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswupper(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswword(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswword(*iter))
		{
			return false;
		}
	}

	return true;
}

bool iswxdigit(wstring _String)
{
	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		if (!iswxdigit(*iter))
		{
			return false;
		}
	}

	return true;
}

wstring towlower(wstring _String)
{
	wstring _Result;
	
	_Result.reserve(_String.length());

	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		_Result += towlower(*iter);
	}

	return _Result;
}

wstring towuppoer(wstring _String)
{
	wstring _Result;
	
	_Result.reserve(_String.length());

	for (wstring::const_iterator iter = _String.begin(); iter != _String.end(); iter++)
	{
		_Result += towupper(*iter);
	}

	return _Result;
}
