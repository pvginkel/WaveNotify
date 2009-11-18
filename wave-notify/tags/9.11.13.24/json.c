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

#define DEFAULT_BUFFER_LENGTH 1024

static void Json_Skip(LPJSON lpReader);
static BOOL Json_ReadSingle(LPJSON lpReader, INT nType);
static BOOL Json_ReadString(LPJSON lpReader, LPJSON_VALUE lpValue);
static BOOL Json_ReadBool(LPJSON lpReader, LPJSON_VALUE lpValue);
static BOOL Json_ReadNumber(LPJSON lpReader, LPJSON_VALUE lpValue);
static BOOL Json_ReadError(LPJSON lpReader);
static BOOL Json_ReadValue(LPJSON lpReader, LPJSON_VALUE lpValue);
static void Json_AppendBuffer(LPJSON lpReader, LPWSTR szSource, size_t cbSource);
static void Json_PushScope(LPJSON lpReader, INT nType);
static BOOL Json_PopScope(LPJSON lpReader, INT nType);
static LPWSTR Json_ParseEscape(LPWSTR szBuffer, LPWSTR szAppend);
static LPWSTR Json_ParseUnicodeEscape(LPWSTR szBuffer, LPWSTR szAppend);

LPJSON Json_CreateReader(LPWSTR szContent)
{
	LPJSON lpReader;

	lpReader = ALLOCA(JSON);

	memset(lpReader, 0, sizeof(JSON));

	lpReader->nType = JI_BOF;

	lpReader->State.szContent = szContent;
	lpReader->State.szOffset = lpReader->State.szContent;
	lpReader->State.fHadContent = FALSE;

	lpReader->State.cbBuffer = DEFAULT_BUFFER_LENGTH;
	lpReader->State.szBuffer = (LPWSTR)ALLOC(sizeof(WCHAR) * lpReader->State.cbBuffer);

	lpReader->State.cbKeyBuffer = DEFAULT_BUFFER_LENGTH;
	lpReader->State.szKeyBuffer = (LPWSTR)ALLOC(sizeof(WCHAR) * lpReader->State.cbKeyBuffer);

	return lpReader;
}

void Json_DestroyReader(LPJSON lpReader)
{
	while (lpReader->State.lpScope != NULL)
	{
		LPJSON_SCOPE lpDelete = lpReader->State.lpScope;

		lpReader->State.lpScope = lpDelete->lpParent;

		FREE(lpDelete);
	}

	FREE(lpReader->State.szBuffer);
	FREE(lpReader->State.szKeyBuffer);
	FREE(lpReader);
}

BOOL Json_Read(LPJSON lpReader)
{
	JSON_VALUE iValue;

	switch (lpReader->nType)
	{
	case JI_ERROR:
	case JI_EOF:
		return FALSE;
	}

	lpReader->Value.nType = JV_NONE;

	if (lpReader->State.fHadContent && lpReader->State.lpScope == NULL)
	{
		return Json_ReadSingle(lpReader, JI_EOF);
	}

	Json_Skip(lpReader);

	if (*lpReader->State.szOffset == L'\0')
	{
		return Json_ReadSingle(lpReader, JI_EOF);
	}
	else if (*lpReader->State.szOffset == L',')
	{
		lpReader->State.szOffset++;
	}

	Json_Skip(lpReader);

	switch (*lpReader->State.szOffset)
	{
	case L'\0':
		return Json_ReadError(lpReader);

	case L'{':
		Json_PushScope(lpReader, JS_DICT);

		return Json_ReadSingle(lpReader, JI_DICT_START);

	case L'}':
		if (!Json_PopScope(lpReader, JS_DICT))
		{
			return Json_ReadError(lpReader);
		}

		return Json_ReadSingle(lpReader, JI_DICT_END);

	case L'[':
		Json_PushScope(lpReader, JS_ARRAY);

		return Json_ReadSingle(lpReader, JI_ARRAY_START);

	case L']':
		if (!Json_PopScope(lpReader, JS_ARRAY))
		{
			return Json_ReadError(lpReader);
		}

		return Json_ReadSingle(lpReader, JI_ARRAY_END);

	case L';':
		return Json_ReadSingle(lpReader, JI_EOF);
	}

	lpReader->nType = JI_VALUE;

	if (!Json_ReadValue(lpReader, &iValue))
	{
		return FALSE;
	}

	Json_Skip(lpReader);

	if (*lpReader->State.szOffset == L':')
	{
		size_t cbBuffer;
		LPWSTR szBuffer;

		if (lpReader->State.lpScope == NULL || lpReader->State.lpScope->nType != JS_DICT)
		{
			DLOG("Found a colon outside of a dictionary");

			return Json_ReadError(lpReader);
		}

		if (iValue.nType != JV_STR)
		{
			return Json_ReadError(lpReader);
		}

		szBuffer = lpReader->State.szKeyBuffer;
		cbBuffer = lpReader->State.cbKeyBuffer;

		lpReader->State.szKeyBuffer = lpReader->State.szBuffer;
		lpReader->State.cbKeyBuffer = lpReader->State.cbBuffer;

		lpReader->State.szBuffer = szBuffer;
		lpReader->State.cbBuffer = cbBuffer;

		lpReader->nIndex++;
		lpReader->szKey = lpReader->State.szKeyBuffer;

		lpReader->State.szOffset++;

		Json_Skip(lpReader);

		switch (*lpReader->State.szOffset)
		{
		case L'{':
		case L'[':
			lpReader->Value.nType = JV_NESTED;
			return TRUE;
		}

		if (!Json_ReadValue(lpReader, &iValue))
		{
			return FALSE;
		}
	}
	else
	{
		if (lpReader->State.lpScope != NULL && lpReader->State.lpScope->nType == JS_DICT)
		{
			DLOG("Did not found a key-value pair in a dictionary");

			return Json_ReadError(lpReader);
		}

		lpReader->szKey = NULL;
	}

	memcpy(&lpReader->Value, &iValue, sizeof(iValue));

	lpReader->State.fHadContent = TRUE;

	return TRUE;
}

static void Json_Skip(LPJSON lpReader)
{
	while (*lpReader->State.szOffset != L'\0' && isspace(*lpReader->State.szOffset))
	{
		lpReader->State.szOffset++;
	}
}

static BOOL Json_ReadSingle(LPJSON lpReader, INT nType)
{
	lpReader->nType = nType;
	lpReader->State.szOffset++;

	lpReader->nIndex = -1;

	return TRUE;
}

static BOOL Json_ReadString(LPJSON lpReader, LPJSON_VALUE lpValue)
{
	WCHAR cQuot;

	*lpReader->State.szBuffer = L'\0';

	// Eat the opening quot

	cQuot = *lpReader->State.szOffset;
	lpReader->State.szOffset++;

	for (;;)
	{
		LPWSTR szBuffer = lpReader->State.szOffset;
		size_t cbLength;

		// Skip to the next recognizable character

		while (*szBuffer != L'\0' && *szBuffer != cQuot && *szBuffer != L'\\')
		{
			szBuffer++;
		}

		// Die when we encounter an \0

		if (*szBuffer == L'\0')
		{
			return Json_ReadError(lpReader);
		}

		// Process what we've eaten until the recognizable character

		cbLength = szBuffer - lpReader->State.szOffset;

		Json_AppendBuffer(lpReader, lpReader->State.szOffset, cbLength);

		// Process what we've recognized

		if (*szBuffer == cQuot)
		{
			// Skip over the detected quot
			lpReader->State.szOffset = szBuffer + 1;

			lpValue->nType = JV_STR;
			lpValue->szString = lpReader->State.szBuffer;

			return TRUE;
		}
		else // if (*szBuffer == L'\\')
		{
			WCHAR szAppend[2];

			szBuffer++;

			szBuffer = Json_ParseEscape(szBuffer, szAppend);

			if (szBuffer == NULL)
			{
				return Json_ReadError(lpReader);
			}
			else
			{
				lpReader->State.szOffset = szBuffer;

				Json_AppendBuffer(lpReader, szAppend, 1);
			}
		}
	}
}

static LPWSTR Json_ParseEscape(LPWSTR szBuffer, LPWSTR szAppend)
{
	szAppend[1] = L'\0';

	switch (*szBuffer)
	{
	case L'n':
		szAppend[0] = L'\n';
		break;

	case L'r':
		szAppend[0] = L'\r';
		break;

	case L't':
		szAppend[0] = L'\t';
		break;

	case L'\\':
		szAppend[0] = L'\\';
		break;

	case L'\'':
		szAppend[0] = L'\'';
		break;

	case L'\"':
		szAppend[0] = L'\"';
		break;

	case L'u':
		return Json_ParseUnicodeEscape(szBuffer, szAppend);

	default:
		return NULL;
	}

	szBuffer++;

	return szBuffer;
}

static LPWSTR Json_ParseUnicodeEscape(LPWSTR szBuffer, LPWSTR szAppend)
{
	INT i;
	INT nValue = 0;

	szBuffer++;

	for (i = 0; i < 4; i++)
	{
		nValue <<= 4;

		if (iswdigit(*szBuffer))
		{
			nValue += *szBuffer - L'0';
		}
		else if (*szBuffer >= L'a' && *szBuffer <= L'f')
		{
			nValue += (*szBuffer - L'a') + 10;
		}
		else if (*szBuffer >= L'A' && *szBuffer <= L'F')
		{
			nValue += (*szBuffer - L'A') + 10;
		}
		else
		{
			return NULL;
		}

		szBuffer++;
	}

	szAppend[0] = (WCHAR)nValue;
	szAppend[1] = L'\0';

	return szBuffer;
}

static void Json_AppendBuffer(LPJSON lpReader, LPWSTR szSource, size_t cbSource)
{
	size_t cbOffset = wcslen(lpReader->State.szBuffer);
	size_t cbBuffer = cbOffset + cbSource + 1;

	if (cbBuffer > lpReader->State.cbBuffer)
	{
		cbBuffer = (((cbBuffer - 1) >> 8) + 1) << 8;

		lpReader->State.szBuffer = (LPWSTR)REALLOC(lpReader->State.szBuffer, cbBuffer * sizeof(WCHAR));
		lpReader->State.cbBuffer = cbBuffer;
	}

	memcpy_s(
		lpReader->State.szBuffer + cbOffset,
		(lpReader->State.cbBuffer - cbOffset) * sizeof(WCHAR),
		szSource,
		cbSource * sizeof(WCHAR));

	lpReader->State.szBuffer[cbOffset + cbSource] = L'\0';
}

static BOOL Json_ReadBool(LPJSON lpReader, LPJSON_VALUE lpValue)
{
	if (_wcsicmp(lpReader->State.szOffset, L"true") == 0)
	{
		lpValue->fBool = TRUE;
		lpReader->State.szOffset += 4;
	}
	else if (_wcsicmp(lpReader->State.szOffset, L"false"))
	{
		lpValue->fBool = FALSE;
		lpReader->State.szOffset += 5;
	}
	else
	{
		lpValue->nType = JV_ERROR;

		return Json_ReadError(lpReader);
	}

	lpValue->nType = JV_BOOL;

	return TRUE;
}

static BOOL Json_ReadNumber(LPJSON lpReader, LPJSON_VALUE lpValue)
{
	BOOL fHadDot = FALSE;
	LPWSTR szOffset = lpReader->State.szOffset;

	// Skip over the already detected -, + or number

	lpReader->State.szOffset++;

	while (*lpReader->State.szOffset != L'\0')
	{
		if (*lpReader->State.szOffset == L'.')
		{
			fHadDot = TRUE;
		}
		else if (!isdigit(*lpReader->State.szOffset))
		{
			break;
		}

		lpReader->State.szOffset++;
	}

	if (fHadDot)
	{
		lpValue->nType = JV_FLOAT;
		lpValue->fNumber = _wtof(szOffset);
	}
	else
	{
		lpValue->nType = JV_NUM;
		lpValue->nNumber = _wtol(szOffset);
	}

	return TRUE;
}

static BOOL Json_ReadError(LPJSON lpReader)
{
	DEBUGBREAK;

	lpReader->nType = JI_ERROR;

	return FALSE;
}

static BOOL Json_ReadValue(LPJSON lpReader, LPJSON_VALUE lpValue)
{
	switch (*lpReader->State.szOffset)
	{
	case L'"':
		return Json_ReadString(lpReader, lpValue);

	case L't':
	case L'T':
	case L'f':
	case L'F':
		return Json_ReadBool(lpReader, lpValue);

	default:
		if (
			isdigit(*lpReader->State.szOffset) ||
			*lpReader->State.szOffset == L'+' ||
			*lpReader->State.szOffset == L'-'
		) {
			return Json_ReadNumber(lpReader, lpValue);
		}
		else
		{
			return Json_ReadError(lpReader);
		}
	}
}

size_t Json_GetProcessedContents(LPJSON lpReader)
{
	return lpReader->State.szOffset - lpReader->State.szContent;
}

static void Json_PushScope(LPJSON lpReader, INT nType)
{
	LPJSON_SCOPE lpScope = ALLOCA(JSON_SCOPE);

	lpScope->nType = nType;
	lpScope->lpParent = lpReader->State.lpScope;

	lpReader->State.lpScope = lpScope;
}

static BOOL Json_PopScope(LPJSON lpReader, INT nType)
{
	LPJSON_SCOPE lpScope = lpReader->State.lpScope;

	if (lpScope == NULL)
	{
		DLOG("There was no scope to pop");

		return FALSE;
	}
	else if (lpScope->nType != nType)
	{
		DLOG("Trying to pop incorrect scope");

		return FALSE;
	}

	lpReader->State.lpScope = lpScope->lpParent;

	FREE(lpScope);

	return TRUE;
}

INT Json_GetScopeDepth(LPJSON lpReader)
{
	LPJSON_SCOPE lpScope;
	INT nCount = 0;

	lpScope = lpReader->State.lpScope;

	while (lpScope != NULL)
	{
		nCount++;
		lpScope = lpScope->lpParent;
	}

	return nCount;
}

BOOL Json_SkipNestedValue(LPJSON lpReader)
{
	INT nExpectedEnd;
	INT nScopeDepth;

	if (lpReader->Value.nType != JV_NESTED)
	{
		DLOG("Value must be JV_NESTED to be able to skip a scope");
		return FALSE;
	}

	nScopeDepth = Json_GetScopeDepth(lpReader);

	if (!Json_Read(lpReader) || (lpReader->nType != JI_DICT_START && lpReader->nType != JI_ARRAY_START))
	{
		return FALSE;
	}

	nExpectedEnd = lpReader->nType == JI_DICT_START ? JI_DICT_END : JI_ARRAY_END;

	while (Json_Read(lpReader))
	{
		if (lpReader->nType == nExpectedEnd && Json_GetScopeDepth(lpReader) == nScopeDepth)
		{
			return TRUE;
		}
	}

	return FALSE;
}
