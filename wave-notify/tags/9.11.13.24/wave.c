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

typedef enum
{
	CONTENT_WAVES,
	CONTENT_CONTACTS
} WAVE_CONTENT;

#define WAVE_URL_CLIENTLOGIN 	L"https://www.google.com/accounts/ClientLogin"
#define WAVE_URL_AUTH 		L"https://wave.google.com/wave/?nouacheck&auth=%s"
#define WAVE_URL_LOGOUT 	L"https://wave.google.com/wave/logout"
#define WAVE_URL_WAVES 		L"https://wave.google.com/wave/?nouacheck"
#define WAVE_URL_INBOX 		L"https://wave.google.com/wave/?auth=%s"
#define WAVE_URL_WAVE		L"https://wave.google.com/wave/?auth=%s#restored:wave:%s.1"

static LPWSTR Wave_JumpToJson(LPWSTR szContent);
static BOOL Wave_ProcessJson(LPWAVE_RESPONSE lpState, LPJSON lpReader);
static BOOL Wave_ProcessWaves(LPWAVE_RESPONSE lpState, LPJSON lpReader);
static BOOL Wave_SkipJsonScope(LPJSON lpReader);
static BOOL Wave_ProcessWavesArray(LPWAVE_RESPONSE lpState, LPJSON lpReader);
static BOOL Wave_ProcessWave(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPLIST lpWaves);
static BOOL Wave_ProcessWaveSubject(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE lpWave);
static BOOL Wave_ProcessWaveMessages(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE lpWave);
static BOOL Wave_ProcessWaveMessage(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPLIST lpMessages);
static void Wave_DestroyWavesList(LPLIST lpWaves);
static void Wave_DestroyMessagesList(LPLIST lpMessages);
static void Wave_DestroyWave(LPWAVE lpWave);
static void Wave_DestroyMessage(LPWAVE_MSG lpMessage);
static LPWSTR Wave_GetAuthKeyFromResponse(LPRESPONSE lpResponse);
static LPWSTR Wave_CreateAuthUrl(LPWSTR szAuthKey);
static void Wave_DestroyContact(LPWAVE_CONTACT lpContact);
static BOOL Wave_ProcessWaveContacts(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE lpWave, LPLIST lpContacts);
static BOOL Wave_ProcessContacts(LPWAVE_RESPONSE lpState, LPJSON lpReader);
static void Wave_ProcessWaveFixupEmailAddresses(LPWAVE lpWave, LPLIST lpContacts);
static BOOL Wave_ProcessContactsArray(LPWAVE_RESPONSE lpState, LPJSON lpReader);
static BOOL Wave_ProcessContact(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPLIST lpContacts);
static void Wave_DestroyContactsList(LPLIST lpContacts);
static BOOL Wave_ProcessContactDetails(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE_CONTACT lpContact);
static LPWSTR Wave_GetEmailAddressFromResponse(LPWSTR szResponse);

#if defined(_DEBUG) && defined(WAVES_DEBUG_INPUT)
static LPWSTR Wave_ReadFromDebugFile();
#endif

LPWAVE_RESPONSE Wave_ParseResponse(LPWSTR szContent)
{
	LPWAVE_RESPONSE lpState;
	BOOL fResult;

	lpState = ALLOCA(WAVE_RESPONSE);

	memset(lpState, 0, sizeof(WAVE_RESPONSE));

	fResult = FALSE;

	for (;;)
	{
		LPJSON lpReader;

		szContent = Wave_JumpToJson(szContent);

		if (szContent == NULL)
		{
			break;
		}

		lpReader = Json_CreateReader(szContent);

		Wave_ProcessJson(lpState, lpReader);

		szContent += Json_GetProcessedContents(lpReader);

		Json_DestroyReader(lpReader);
	}

	return lpState;
}

static LPWSTR Wave_JumpToJson(LPWSTR szContent)
{
	for (;;)
	{
		LPWSTR szEnd;

		// Only consider json blocks inside a script block

		szContent = wcsstr(szContent, L"<script");

		if (szContent == NULL)
		{
			return NULL;
		}

		szEnd = wcsstr(szContent, L"</script>");

		if (szEnd == NULL)
		{
			DFAIL("Could not find end of script block");
			return NULL;
		}

		szContent = wcsstr(szContent, L"var json");

		if (szContent == NULL)
		{
			DFAIL("Could not find json block");
			return NULL;
		}

		if (szContent < szEnd)
		{
			break;
		}

		szContent = szEnd;
	}

	szContent = wcschr(szContent, L'{');

	if (szContent == NULL)
	{
		DFAIL("Could not find start of json code");
		return NULL;
	}

	return szContent;
}

static BOOL Wave_ProcessJson(LPWAVE_RESPONSE lpState, LPJSON lpReader)
{
	BOOL fHadKey = FALSE;
	WAVE_CONTENT nReading;

	if (!Json_Read(lpReader) || lpReader->nType != JI_DICT_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"r") == 0)
			{
				if (lpReader->Value.nType != JV_STR)
				{
					return FALSE;
				}
				if (wcscmp(lpReader->Value.szString, L"^d1") == 0)
				{
					nReading = CONTENT_WAVES;
				}
				else if (wcscmp(lpReader->Value.szString, L"^c1") == 0)
				{
					nReading = CONTENT_CONTACTS;
				}
				else
				{
					return FALSE;
				}

				fHadKey = TRUE;
			}
			else if (fHadKey && wcscmp(lpReader->szKey, L"p") == 0)
			{
				switch (nReading)
				{
				case CONTENT_WAVES:
					return Wave_ProcessWaves(lpState, lpReader);

				case CONTENT_CONTACTS:
					return Wave_ProcessContacts(lpState, lpReader);

				default:
					return FALSE;
				}
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			return FALSE;
		}
	}

	return FALSE;
}

static BOOL Wave_ProcessWaves(LPWAVE_RESPONSE lpState, LPJSON lpReader)
{
	if (!Json_Read(lpReader) || lpReader->nType != JI_DICT_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"1") == 0)
			{
				return Wave_ProcessWavesArray(lpState, lpReader);
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			DFAIL("Unexpected end");
			return FALSE;
		}
	}

	DFAIL("Unexpected end");
	return FALSE;
}

static BOOL Wave_ProcessWavesArray(LPWAVE_RESPONSE lpState, LPJSON lpReader)
{
	LPLIST lpWaves;

	if (!Json_Read(lpReader) || lpReader->nType != JI_ARRAY_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	lpWaves = List_Create();

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_DICT_START:
			if (!Wave_ProcessWave(lpState, lpReader, lpWaves))
			{
				Wave_DestroyWavesList(lpWaves);
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_ARRAY_END)
			{
				Wave_DestroyWavesList(lpWaves);
				DFAIL("Expected JI_ARRAY_END");
			}
			else
			{
				lpState->nWavesCount = List_GetCount(lpWaves);
				lpState->lpWaves = (LPWAVE *)List_ToArray(lpWaves);
				
				List_Destroy(lpWaves);
			}

			return lpReader->nType == JI_ARRAY_END;
		}
	}

	Wave_DestroyWavesList(lpWaves);
	DFAIL("Unexpected end");

	return FALSE;
}

static BOOL Wave_ProcessWave(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPLIST lpWaves)
{
	LPWAVE lpWave = ALLOCA(WAVE);
	LPLIST lpContacts = List_Create();

	List_Add(lpWaves, lpWave);

	memset(lpWave, 0, sizeof(WAVE));

	lpWave->nMessages = -1;
	lpWave->nUnreadMessages = -1;

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"1") == 0)
			{
				if (lpReader->Value.nType != JV_STR)
				{
					DFAIL("Expected JV_STR");
					return FALSE;
				}

				lpWave->szId = _wcsdup(lpReader->Value.szString);
			}
			else if (wcscmp(lpReader->szKey, L"4") == 0)
			{
				if (lpReader->Value.nType != JV_STR)
				{
					DFAIL("Expected JV_STR");
					return FALSE;
				}

				lpWave->szEmailAddress = _wcsdup(lpReader->Value.szString);
			}
			else if (wcscmp(lpReader->szKey, L"5") == 0)
			{
				if (lpReader->Value.nType != JV_NESTED)
				{
					DFAIL("Expected JV_NESTED");
					return FALSE;
				}

				if (!Wave_ProcessWaveContacts(lpState, lpReader, lpWave, lpContacts))
				{
					return FALSE;
				}
			}
			else if (wcscmp(lpReader->szKey, L"6") == 0)
			{
				if (lpReader->Value.nType != JV_NUM)
				{
					DFAIL("Expected JV_NUM");
					return FALSE;
				}

				lpWave->nMessages = lpReader->Value.nNumber;
			}
			else if (wcscmp(lpReader->szKey, L"7") == 0)
			{
				if (lpReader->Value.nType != JV_NUM)
				{
					DFAIL("Expected JV_NUM");
					return FALSE;
				}

				lpWave->nUnreadMessages = lpReader->Value.nNumber;
			}
			else if (wcscmp(lpReader->szKey, L"9") == 0)
			{
				if (lpReader->Value.nType != JV_NESTED)
				{
					DFAIL("Expected JV_NESTED");
					return FALSE;
				}

				if (!Wave_ProcessWaveSubject(lpState, lpReader, lpWave))
				{
					return FALSE;
				}
			}
			else if (wcscmp(lpReader->szKey, L"10") == 0)
			{
				if (lpReader->Value.nType != JV_NESTED)
				{
					DFAIL("Expected JV_NESTED");
					return FALSE;
				}

				if (!Wave_ProcessWaveMessages(lpState, lpReader, lpWave))
				{
					return FALSE;
				}
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_DICT_END)
			{
				DFAIL("Expected JI_DICT_END");
			}
			else
			{
				if (
					lpWave->lpMessages == NULL ||
					lpWave->szId == NULL ||
					lpWave->szSubject == NULL ||
					lpWave->nMessages < 0 ||
					lpWave->nUnreadMessages < 0
				) {
					DFAIL("Incomplete message");
					return FALSE;
				}
			}

			Wave_ProcessWaveFixupEmailAddresses(lpWave, lpContacts);

			List_CascadeDestroy(lpContacts);

			return lpReader->nType == JI_DICT_END;
		}
	}

	DFAIL("Unexpected end");
	return FALSE;
}

static void Wave_ProcessWaveFixupEmailAddresses(LPWAVE lpWave, LPLIST lpContacts)
{
	LPITER lpIter = List_Start(lpContacts);
	LPWSTR szEmailAddress;
	INT i = 0;
	INT j;

	while (szEmailAddress = (LPWSTR)List_Next(lpIter))
	{
		for (j = 0; j < lpWave->nCount; j++)
		{
			if (lpWave->lpMessages[j]->nContactId == i)
			{
				lpWave->lpMessages[j]->szEmailAddress = _wcsdup(szEmailAddress);
			}
		}

		i++;
	}

	List_End(lpIter);
}

static BOOL Wave_ProcessWaveSubject(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE lpWave)
{
	if (!Json_Read(lpReader) || lpReader->nType != JI_DICT_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"1") == 0)
			{
				if (lpReader->Value.nType != JV_STR)
				{
					DFAIL("Expected JV_STR");
					return FALSE;
				}

				lpWave->szSubject = _wcsdup(lpReader->Value.szString);
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_DICT_END)
			{
				DFAIL("Expected JI_DICT_END");
			}

			return lpReader->nType == JI_DICT_END;
		}
	}

	DFAIL("Unexpected end");
	return FALSE;
}

static BOOL Wave_ProcessWaveContacts(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE lpWave, LPLIST lpContacts)
{
	if (!Json_Read(lpReader) || lpReader->nType != JI_ARRAY_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (lpReader->Value.nType != JV_STR)
			{
				DFAIL("Expected JV_STR");
				return FALSE;
			}

			List_Add(lpContacts, _wcsdup(lpReader->Value.szString));
			break;

		default:
			if (lpReader->nType != JI_ARRAY_END)
			{
				DFAIL("Expected JI_ARRAY_END");
			}

			return lpReader->nType == JI_ARRAY_END;
		}
	}

	DFAIL("Unexpected end");

	return FALSE;
}

static BOOL Wave_ProcessWaveMessages(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE lpWave)
{
	LPLIST lpMessages;

	if (!Json_Read(lpReader) || lpReader->nType != JI_ARRAY_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	lpMessages = List_Create();

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_DICT_START:
			if (!Wave_ProcessWaveMessage(lpState, lpReader, lpMessages))
			{
				Wave_DestroyMessagesList(lpMessages);
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_ARRAY_END)
			{
				Wave_DestroyMessagesList(lpMessages);
				DFAIL("Expected JI_ARRAY_END");
			}
			else
			{
				lpWave->nCount = List_GetCount(lpMessages);
				lpWave->lpMessages = (LPWAVE_MSG *)List_ToArray(lpMessages);
				
				List_Destroy(lpMessages);
			}

			return lpReader->nType == JI_ARRAY_END;
		}
	}

	Wave_DestroyMessagesList(lpMessages);
	DFAIL("Unexpected end");

	return FALSE;
}

static BOOL Wave_ProcessWaveMessage(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPLIST lpMessages)
{
	LPWAVE_MSG lpMessage = ALLOCA(WAVE_MSG);

	List_Add(lpMessages, lpMessage);

	memset(lpMessage, 0, sizeof(WAVE_MSG));

	lpMessage->nOrder = List_GetCount(lpMessages);

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"1") == 0)
			{
				if (lpReader->Value.nType != JV_STR)
				{
					DFAIL("Expected JV_STR");
					return FALSE;
				}

				lpMessage->szText = _wcsdup(lpReader->Value.szString);
			}
			else if (wcscmp(lpReader->szKey, L"6") == 0)
			{
				if (lpReader->Value.nType != JV_NUM)
				{
					DFAIL("Expected JV_NUM");
					return FALSE;
				}

				lpMessage->nContactId = lpReader->Value.nNumber;
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_DICT_END)
			{
				DFAIL("Expected JI_DICT_END");
			}
			else
			{
				if (lpMessage->szText == NULL)
				{
					DFAIL("Incomplete message");
					return FALSE;
				}
			}

			return lpReader->nType == JI_DICT_END;
		}
	}

	DFAIL("Unexpected end");
	return FALSE;
}

void Wave_DestroyResponse(LPWAVE_RESPONSE lpRequest)
{
	INT i;

	for (i = 0; i < lpRequest->nWavesCount; i++)
	{
		Wave_DestroyWave(lpRequest->lpWaves[i]);
	}

	for (i = 0; i < lpRequest->nContactsCount; i++)
	{
		Wave_DestroyContact(lpRequest->lpContacts[i]);
	}

	FREE(lpRequest->lpContacts);
	FREE(lpRequest->lpWaves);
	FREE(lpRequest);
}

static void Wave_DestroyWavesList(LPLIST lpWaves)
{
	LPLIST_ITEM lpItem;

	for (lpItem = lpWaves->lpHead; lpItem != NULL; lpItem = lpItem->lpNext)
	{
		Wave_DestroyWave((LPWAVE)lpItem->lpValue);
	}

	List_Destroy(lpWaves);
}

static void Wave_DestroyMessagesList(LPLIST lpMessages)
{
	LPLIST_ITEM lpItem;

	for (lpItem = lpMessages->lpHead; lpItem != NULL; lpItem = lpItem->lpNext)
	{
		Wave_DestroyMessage((LPWAVE_MSG)lpItem->lpValue);
	}

	List_Destroy(lpMessages);
}

static void Wave_DestroyWave(LPWAVE lpWave)
{
	INT i;

	for (i = 0; i < lpWave->nCount; i++)
	{
		Wave_DestroyMessage(lpWave->lpMessages[i]);
	}

	if (lpWave->szId != NULL)
	{
		FREE(lpWave->szId);
	}

	if (lpWave->szSubject != NULL)
	{
		FREE(lpWave->szSubject);
	}

	if (lpWave->lpMessages != NULL)
	{
		FREE(lpWave->lpMessages);
	}

	if (lpWave->szEmailAddress != NULL)
	{
		FREE(lpWave->szEmailAddress);
	}

	FREE(lpWave);
}

static void Wave_DestroyContact(LPWAVE_CONTACT lpContact)
{
	FREE(lpContact->szName);
	FREE(lpContact->szEmailAddress);
	FREE(lpContact);
}

static void Wave_DestroyMessage(LPWAVE_MSG lpMessage)
{
	if (lpMessage->szText != NULL)
	{
		FREE(lpMessage->szText);
	}
	if (lpMessage->szEmailAddress != NULL)
	{
		FREE(lpMessage->szEmailAddress);
	}

	FREE(lpMessage);
}

LPWAVE_SESSION Wave_CreateSession(LPWSTR szUsername, LPWSTR szPassword)
{
	LPWAVE_SESSION lpSession = ALLOCA(WAVE_SESSION);

	memset(lpSession, 0, sizeof(WAVE_SESSION));

	lpSession->szUsername = _wcsdup(szUsername);
	lpSession->szPassword = _wcsdup(szPassword);

	return lpSession;
}

void Wave_DestroySession(LPWAVE_SESSION lpSession)
{
	FREE(lpSession->szUsername);
	FREE(lpSession->szPassword);
	
	if (lpSession->szAuthKey != NULL)
	{
		FREE(lpSession->szAuthKey);
	}

	if (lpSession->lpCookies != NULL)
	{
		Curl_DestroyCookies(lpSession->lpCookies);
	}

	if (lpSession->szEmailAddress != NULL)
	{
		FREE(lpSession->szEmailAddress);
	}

	FREE(lpSession);
}

BOOL Wave_Login(LPWAVE_SESSION lpSession)
{
#if defined(_DEBUG) && defined(WAVES_DEBUG_INPUT)
	return TRUE;
#else
	LPREQUEST lpRequest;
	LPRESPONSE lpResponse;
	BOOL fResult;
	LPWSTR szAuthUrl;

	lpRequest = Curl_Create(WAVE_URL_CLIENTLOGIN, USERAGENT);

	Curl_SetTimeout(lpRequest, CURL_TIMEOUT);
	Curl_IgnoreSSLErrors(lpRequest);

	Curl_AddPost(lpRequest, L"accountType", L"GOOGLE");
	Curl_AddPost(lpRequest, L"Email", lpSession->szUsername);
	Curl_AddPost(lpRequest, L"Passwd", lpSession->szPassword);
	Curl_AddPost(lpRequest, L"service", L"wave");
	Curl_AddPost(lpRequest, L"source", L"net.sf.wave-notify");

	lpResponse = Curl_Execute(lpRequest);

	fResult = lpResponse != NULL;

	if (!fResult)
	{
		goto __end;
	}

	if (lpSession->szAuthKey != NULL)
	{
		FREE(lpSession->szAuthKey);
	}

	lpSession->szAuthKey = Wave_GetAuthKeyFromResponse(lpResponse);

	fResult = lpSession->szAuthKey != NULL && wcslen(lpSession->szAuthKey) > 0;
	
	if (!fResult)
	{
		goto __end;
	}

	Curl_DestroyRequest(lpRequest);

	szAuthUrl = Wave_CreateAuthUrl(lpSession->szAuthKey);

	lpRequest = Curl_Create(szAuthUrl, USERAGENT);

	Curl_SetTimeout(lpRequest, CURL_TIMEOUT);
	Curl_IgnoreSSLErrors(lpRequest);

	FREE(szAuthUrl);

	lpResponse = Curl_Execute(lpRequest);

	fResult = lpResponse != NULL;

	if (!fResult)
	{
		goto __end;
	}

	lpSession->lpCookies = Curl_GetCookies(lpResponse);

	fResult = TRUE;

__end:
	if (lpRequest != NULL)
	{
		Curl_DestroyRequest(lpRequest);
	}

	return fResult;
#endif
}

static LPWSTR Wave_GetAuthKeyFromResponse(LPRESPONSE lpResponse)
{
	LPWSTR szResponse;
	LPWSTR szOffset;
	LPWSTR szKey = NULL;
	
	szResponse = duputf8towcsn((LPCSTR)lpResponse->lpData->lpData, lpResponse->lpData->dwSize);
	szOffset = szResponse;

	for (;;)
	{
		INT nKeyLength;
		INT nValueLength;
		LPWSTR szPos;
		LPWSTR szKeyOffset;
		LPWSTR szValueOffset;

		szKeyOffset = szOffset;
		szPos = wcschr(szOffset, L'=');

		if (szPos == NULL)
		{
			goto __end;
		}

		nKeyLength = szPos - szOffset;

		szOffset = szPos + 1;
		szValueOffset = szOffset;

		szPos = wcschr(szOffset, L'\n');

		if (szPos == NULL)
		{
			szPos = wcschr(szOffset, L'\0');
		}

		if (szPos == NULL)
		{
			goto __end;
		}

		// Jump to the last space

		szPos--;

		while (szPos > szOffset && iswspace(*szPos))
		{
			szPos--;
		}

		szPos++;

		nValueLength = szPos - szOffset;

		// Set offset after the last space

		szOffset = szPos;

		while (*szOffset != L'\0' && iswspace(*szOffset))
		{
			szOffset++;
		}

		if (_wcsnicmp(szKeyOffset, L"Auth", nKeyLength) == 0)
		{
			szKey = (LPWSTR)ALLOC((nValueLength + 1) * sizeof(WCHAR));

			wcsncpy_s(szKey, nValueLength + 1, szValueOffset, nValueLength);

			szKey[nValueLength] = L'\0';
			
			goto __end;
		}
	}

__end:
	FREE(szResponse);

	return szKey;
}

static LPWSTR Wave_CreateAuthUrl(LPWSTR szAuthKey)
{
	size_t uLength;
	LPWSTR szResult;
	LPWSTR szEncodedAuthKey;

	szEncodedAuthKey = Curl_UrlEncode(szAuthKey);

	uLength = wcslen(WAVE_URL_AUTH) + wcslen(szEncodedAuthKey);

	szResult = (LPWSTR)ALLOC((uLength + 1) * sizeof(WCHAR));

	StringCbPrintf(szResult, (uLength + 1) * sizeof(WCHAR), WAVE_URL_AUTH, szEncodedAuthKey);

	FREE(szEncodedAuthKey);

	return szResult;
}

static LPWSTR Wave_CreateInboxUrl(LPWSTR szAuthKey)
{
	size_t uLength;
	LPWSTR szResult;
	LPWSTR szEncodedAuthKey;

	szEncodedAuthKey = Curl_UrlEncode(szAuthKey);

	uLength = wcslen(WAVE_URL_INBOX) + wcslen(szEncodedAuthKey);

	szResult = (LPWSTR)ALLOC((uLength + 1) * sizeof(WCHAR));

	StringCbPrintf(szResult, (uLength + 1) * sizeof(WCHAR), WAVE_URL_INBOX, szEncodedAuthKey);

	FREE(szEncodedAuthKey);

	return szResult;
}

static LPWSTR Wave_CreateWaveUrl(LPWSTR szAuthKey, LPWSTR szWaveId)
{
	size_t uLength;
	LPWSTR szResult;
	LPWSTR szEncodedAuthKey;
	LPWSTR szEncodedId;
	LPWSTR szEncodedId2;

	szEncodedAuthKey = Curl_UrlEncode(szAuthKey);
	szEncodedId = Curl_UrlEncode(szWaveId);

	// For some reason WAVE requires the ID to be encoded twice
	szEncodedId2 = Curl_UrlEncode(szEncodedId);

	uLength = wcslen(WAVE_URL_WAVE) + wcslen(szEncodedAuthKey) + wcslen(szEncodedId2);

	szResult = (LPWSTR)ALLOC((uLength + 1) * sizeof(WCHAR));

	StringCbPrintf(szResult, (uLength + 1) * sizeof(WCHAR), WAVE_URL_WAVE, szEncodedAuthKey, szEncodedId2);

	FREE(szEncodedAuthKey);
	FREE(szEncodedId);
	FREE(szEncodedId2);

	return szResult;
}

void Wave_Logoff(LPWAVE_SESSION lpSession)
{
#if defined(_DEBUG) && defined(WAVES_DEBUG_INPUT)
	return;
#else
	LPREQUEST lpRequest;

	if (lpSession->lpCookies != NULL)
	{
		lpRequest = Curl_Create(WAVE_URL_LOGOUT, USERAGENT);

		Curl_SetTimeout(lpRequest, CURL_TIMEOUT);
		Curl_IgnoreSSLErrors(lpRequest);

		Curl_SetCookies(lpRequest, lpSession->lpCookies);

		Curl_Execute(lpRequest);

		Curl_DestroyCookies(lpSession->lpCookies);

		Curl_DestroyRequest(lpRequest);

		lpSession->lpCookies = NULL;
	}
#endif
}

LPWAVE_RESPONSE Wave_Query(LPWAVE_SESSION lpSession)
{
	LPWSTR szResponse;
	LPWAVE_RESPONSE lpWaves = NULL;

#if defined(_DEBUG) && defined(WAVES_DEBUG_INPUT)
	szResponse = Wave_ReadFromDebugFile();
#else
	LPREQUEST lpRequest;
	LPRESPONSE lpResponse;

	lpRequest = Curl_Create(WAVE_URL_WAVES, USERAGENT);

	Curl_SetTimeout(lpRequest, CURL_TIMEOUT);
	Curl_IgnoreSSLErrors(lpRequest);

	Curl_SetCookies(lpRequest, lpSession->lpCookies);

	lpResponse = Curl_Execute(lpRequest);

	if (lpResponse == NULL)
	{
		goto __end;
	}

	szResponse = duputf8towcsn((LPCSTR)lpResponse->lpData->lpData, lpResponse->lpData->dwSize);
#endif

	if (lpSession->szEmailAddress == NULL)
	{
		lpSession->szEmailAddress = Wave_GetEmailAddressFromResponse(szResponse);
	}

	lpWaves = Wave_ParseResponse(szResponse);

	FREE(szResponse);

#if !(defined(_DEBUG) && defined(WAVES_DEBUG_INPUT))
__end:

	Curl_DestroyRequest(lpRequest);
#endif

	return lpWaves;
}

static BOOL Wave_ProcessContacts(LPWAVE_RESPONSE lpState, LPJSON lpReader)
{
	if (!Json_Read(lpReader) || lpReader->nType != JI_DICT_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"2") == 0)
			{
				return Wave_ProcessContactsArray(lpState, lpReader);
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			DFAIL("Unexpected end");
			return FALSE;
		}
	}

	DFAIL("Unexpected end");
	return FALSE;
}

static BOOL Wave_ProcessContactsArray(LPWAVE_RESPONSE lpState, LPJSON lpReader)
{
	LPLIST lpContacts;

	if (!Json_Read(lpReader) || lpReader->nType != JI_ARRAY_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	lpContacts = List_Create();

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_DICT_START:
			if (!Wave_ProcessContact(lpState, lpReader, lpContacts))
			{
				Wave_DestroyContactsList(lpContacts);
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_ARRAY_END)
			{
				Wave_DestroyWavesList(lpContacts);
				DFAIL("Expected JI_ARRAY_END");
			}
			else
			{
				lpState->nContactsCount = List_GetCount(lpContacts);
				lpState->lpContacts = (LPWAVE_CONTACT *)List_ToArray(lpContacts);
				
				List_Destroy(lpContacts);
			}

			return lpReader->nType == JI_ARRAY_END;
		}
	}

	Wave_DestroyContactsList(lpContacts);
	DFAIL("Unexpected end");

	return FALSE;
}

static BOOL Wave_ProcessContact(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPLIST lpContacts)
{
	LPWAVE_CONTACT lpContact = ALLOCA(WAVE_CONTACT);

	List_Add(lpContacts, lpContact);

	memset(lpContact, 0, sizeof(WAVE_CONTACT));

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"6") == 0)
			{
				if (lpReader->Value.nType != JV_NESTED)
				{
					DFAIL("Expected JV_NESTED");
					return FALSE;
				}

				if (!Wave_ProcessContactDetails(lpState, lpReader, lpContact))
				{
					return FALSE;
				}
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_DICT_END)
			{
				DFAIL("Expected JI_DICT_END");
			}

			return lpReader->nType == JI_DICT_END;
		}
	}

	DFAIL("Unexpected end");
	return FALSE;
}

static BOOL Wave_ProcessContactDetails(LPWAVE_RESPONSE lpState, LPJSON lpReader, LPWAVE_CONTACT lpContact)
{
	if (!Json_Read(lpReader) || lpReader->nType != JI_DICT_START)
	{
		DFAIL("Unexpected end");
		return FALSE;
	}

	while (Json_Read(lpReader))
	{
		switch (lpReader->nType)
		{
		case JI_VALUE:
			if (wcscmp(lpReader->szKey, L"1") == 0)
			{
				if (lpReader->Value.nType != JV_STR)
				{
					DFAIL("Expected JV_STR");
					return FALSE;
				}

				lpContact->szName = _wcsdup(lpReader->Value.szString);
			}
			else if (wcscmp(lpReader->szKey, L"8") == 0)
			{
				if (lpReader->Value.nType != JV_STR)
				{
					DFAIL("Expected JV_STR");
					return FALSE;
				}

				lpContact->szEmailAddress = _wcsdup(lpReader->Value.szString);
			}

			if (lpReader->Value.nType == JV_NESTED && !Json_SkipNestedValue(lpReader))
			{
				DFAIL("Unexpected end");
				return FALSE;
			}
			break;

		default:
			if (lpReader->nType != JI_DICT_END)
			{
				DFAIL("Expected JI_DICT_END");
			}

			if (lpContact->szName == NULL)
			{
				lpContact->szName = _wcsdup(lpContact->szEmailAddress);
			}

			return lpReader->nType == JI_DICT_END;
		}
	}

	DFAIL("Unexpected end");
	return FALSE;
}

static void Wave_DestroyContactsList(LPLIST lpContacts)
{
	LPITER lpIter;
	LPWAVE lpWave;

	lpIter = List_Start(lpContacts);

	while (lpWave = (LPWAVE)List_Next(lpIter))
	{
		Wave_DestroyWave(lpWave);
	}

	List_End(lpIter);
	List_Destroy(lpContacts);
}

BOOL Wave_IsLoggedOn(LPWAVE_SESSION lpSession)
{
#if defined(_DEBUG) && defined(WAVES_DEBUG_INPUT)
	return TRUE;
#else
	return lpSession->lpCookies != NULL;
#endif
}

void Wave_OpenInbox(LPWAVE_SESSION lpSession)
{
	LPWSTR szInboxUrl;

	szInboxUrl = Wave_CreateInboxUrl(lpSession->szAuthKey);

	OpenUrl(szInboxUrl);

	FREE(szInboxUrl);
}

void Wave_OpenWave(LPWAVE_SESSION lpSession, LPWSTR szWaveId)
{
	LPWSTR szWaveUrl;

	szWaveUrl = Wave_CreateWaveUrl(lpSession->szAuthKey, szWaveId);

	OpenUrl(szWaveUrl);

	FREE(szWaveUrl);
}

#if defined(_DEBUG) && defined(WAVES_DEBUG_INPUT)

static LPWSTR Wave_ReadFromDebugFile()
{
	LPWSTR szContent;
	size_t cbRead;
	FILE * f;
	DWORD dwAllocated;

	fopen_s(&f, WAVES_DEBUG_INPUT, "rt,ccs=UTF-8");

	fseek(f, 0, SEEK_END);

	dwAllocated = ftell(f);

	fseek(f, 0, SEEK_SET);

	szContent = (LPWSTR)ALLOC(sizeof(WCHAR) * (dwAllocated + 1));
	
	cbRead = fread(szContent, sizeof(WCHAR), dwAllocated, f);

	szContent[cbRead] = L'\0';

	fclose(f);

	return szContent;
}

#endif

static LPWSTR Wave_GetEmailAddressFromResponse(LPWSTR szResponse)
{
	LPWSTR szStart;
	LPWSTR szEnd;
	LPWSTR szEmailAddress = NULL;

	szStart = wcsstr(szResponse, L"username:'");
	
	if (szStart != NULL)
	{
		szStart = wcschr(szStart, L'\'') + 1;
	}

	if (szStart != NULL)
	{
		szEnd = wcschr(szStart, L'\'');

		if (szEnd != NULL)
		{
			size_t uLength;

			uLength = szEnd - szStart;

			szEmailAddress = (LPWSTR)ALLOC(sizeof(WCHAR) * (uLength + 1));

			wcsncpy_s(szEmailAddress, uLength + 1, szStart, uLength);
			szEmailAddress[uLength] = L'\0';
		}
	}

	return szEmailAddress;
}
