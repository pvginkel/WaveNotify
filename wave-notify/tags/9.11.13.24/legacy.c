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

errno_t compat_wcscpy_s(wchar_t * _Dst, rsize_t _SizeInWords, const wchar_t * _Src)
{
	if (StringCbCopy(_Dst, _SizeInWords * sizeof(wchar_t), _Src) == S_OK)
	{
		return 0;
	}
	else
	{
		if (_SizeInWords > 0)
		{
			*_Dst = L'\0';
		}
		return ERANGE;
	}
}

errno_t compat_wcsncpy_s(wchar_t * _Dst, rsize_t _SizeInWords, const wchar_t * _Src, rsize_t _MaxCount)
{
	if (StringCbCopyN(_Dst, _SizeInWords * sizeof(wchar_t), _Src, _MaxCount * sizeof(wchar_t)) == S_OK)
	{
		return 0;
	}
	else
	{
		if (_SizeInWords > 0)
		{
			*_Dst = L'\0';
		}
		return ERANGE;
	}
}

errno_t  compat_memcpy_s(void * _Dst, rsize_t _DstSize, const void * _Src, rsize_t _MaxCount)
{
	if (_DstSize < _MaxCount)
	{
		memset(_Dst, 0, _DstSize);

		return ERANGE;
	}
	else
	{
		memcpy(_Dst, _Src, _MaxCount);

		return 0;
	}
}

double compat__wtof(const wchar_t *_Str)
{
	double result = 0.0;
	double sign = 1.0;
	const wchar_t * offset;
	double exp = 1.0;
	int had_digit = 0;

	offset = _Str;

	switch (*offset)
	{
	case L'-':
		sign = -1.0;
	case L'+':
		offset++;
		break;
	}

	while (*offset != L'\0')
	{
		if (iswdigit(*offset))
		{
			if (had_digit)
			{
				exp *= 0.1;
				result += (double)(*offset - L'0') * exp;
			}
			else
			{
				result *= 10.0;
				result += (double)(*offset - L'0');
			}
		}
		else if (*offset == L'.')
		{
			if (had_digit)
			{
				goto __end;
			}

			had_digit = 1;
		}
		else
		{
			goto __end;
		}

		offset++;
	}

__end:
	result *= sign;

	return result;
}

errno_t compat_wcstombs_s(size_t * _PtNumOfCharConverted, char * _Dst, size_t _DstSizeInBytes, const wchar_t * _Src, size_t _MaxCountInBytes)
{
	/*
	 * A safe version could be implemented by getting the calculating
	 * the required size prior to performing the conversion. Since the
	 * usage in this project is in this manner, we skip this step for
	 * performance reasons, and this function is not safe when used
	 * improperly.
	 */

	*_PtNumOfCharConverted = wcstombs(_Dst, _Src, _MaxCountInBytes);

	return 0;
}

errno_t compat_mbstowcs_s(size_t * _PtNumOfCharConverted, wchar_t * _DstBuf, size_t _SizeInWords, const char * _SrcBuf, size_t _MaxCount )
{
	/*
	 * A safe version could be implemented by getting the calculating
	 * the required size prior to performing the conversion. Since the
	 * usage in this project is in this manner, we skip this step for
	 * performance reasons, and this function is not safe when used
	 * improperly.
	 */

	*_PtNumOfCharConverted = mbstowcs(_DstBuf, _SrcBuf, _MaxCount);

	return 0;
}
