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

static size_t Curl_WriteData(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream);
static size_t Curl_WriteFile(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream);
static size_t Curl_WriteHeader(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream);
static char * Curl_TrimA(char * szValue);

void Curl_Initialize()
{
	ASSERT( curl_global_init(CURL_GLOBAL_WIN32) == CURLE_OK );
}

void Curl_Destroy()
{
	curl_global_cleanup();
}

LPREQUEST Curl_Create(LPCWSTR szUrl, LPCWSTR szUserAgent)
{
	LPREQUEST lpRequest;

	/* Create the request */

	lpRequest = ALLOCA(REQUEST);

	lpRequest->lpResponse = ALLOCA(RESPONSE);

	lpRequest->lpResponse->lpHeaders = List_Create();

	lpRequest->lpResponse->lpRequest = lpRequest;

	/* Initialize CURL */

	lpRequest->lpCurl = curl_easy_init();

	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_NOPROGRESS, 1);

	/* Set the URL */

	lpRequest->szUrl = dupwcstombs(szUrl);

	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_URL, lpRequest->szUrl);

	/* Set the user agent */

	lpRequest->szUserAgent = dupwcstombs(szUserAgent);

	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_USERAGENT, lpRequest->szUserAgent);

	/* Set the error buffer */

	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_ERRORBUFFER, lpRequest->szError);

	/* Enable cookies */

	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_COOKIEFILE, "");

	return lpRequest;
}

void Curl_SetTimeout(LPREQUEST lpRequest, INT nTimeout)
{
	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_TIMEOUT, nTimeout);
}

void Curl_AddPost(LPREQUEST lpRequest, LPCWSTR szName, LPCWSTR szValue)
{
	LPSTR szNameA;
	LPSTR szValueA;
	CURLFORMcode lResult;

	szNameA = dupwcstombs(szName);
	szValueA = dupwcstombs(szValue);

	lResult = curl_formadd(
		&lpRequest->lpPost, &lpRequest->lpLastPost,
		CURLFORM_COPYNAME, szNameA,
		CURLFORM_CONTENTTYPE, "text/xml",
		CURLFORM_COPYCONTENTS, szValueA,
		CURLFORM_END);

	ASSERT(lResult == 0);

	FREE(szNameA);
	FREE(szValueA);
}

LPRESPONSE Curl_Execute(LPREQUEST lpRequest)
{
	CURLcode lResult;
	long lStatus;

	/* Set the post parameters */
	
	if (lpRequest->lpPost != NULL)
	{
		curl_easy_setopt(lpRequest->lpCurl, CURLOPT_HTTPPOST, lpRequest->lpPost);
	}

	/*
	 * Create the data buffer and set the correct write method
	 * if no file handle was set. Otherwise, link the file writer.
	 */

	if (lpRequest->lpResponse->hFile == NULL)
	{
		lpRequest->lpResponse->lpData = Buffer_Create();

		curl_easy_setopt(lpRequest->lpCurl, CURLOPT_WRITEFUNCTION, Curl_WriteData);
	}
	else
	{
		curl_easy_setopt(lpRequest->lpCurl, CURLOPT_WRITEFUNCTION, Curl_WriteFile);
	}

	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_HEADERFUNCTION, Curl_WriteHeader);
	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_HEADERDATA, lpRequest->lpResponse);

	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_WRITEDATA, lpRequest->lpResponse);

	/* Perform the request */

	lResult = curl_easy_perform(lpRequest->lpCurl);

	if (lResult != 0)
	{
		LOG1("cURL failed: %s", lpRequest->szError);

		return NULL;
	}

	curl_easy_getinfo(lpRequest->lpCurl, CURLINFO_RESPONSE_CODE, &lStatus);

	lpRequest->lpResponse->nStatus = lStatus;

	return lpRequest->lpResponse;
}

void Curl_DestroyRequest(LPREQUEST lpRequest)
{
	LPITER lpIter;
	LPRESPONSE_HEADER lpHeader;

	if (lpRequest->lpResponse->lpData != NULL)
	{
		Buffer_Destroy(lpRequest->lpResponse->lpData);
	}

	lpIter = List_Start(lpRequest->lpResponse->lpHeaders);

	while (( lpHeader = (LPRESPONSE_HEADER)List_Next(lpIter) ) != NULL)
	{
		FREE(lpHeader->szName);
		FREE(lpHeader->szValue);
		FREE(lpHeader);
	}

	List_End(lpIter);

	List_Destroy(lpRequest->lpResponse->lpHeaders);

	FREE(lpRequest->lpResponse);

	if (lpRequest->szUserAgent != NULL)
	{
		FREE(lpRequest->szUserAgent);
	}

	FREE(lpRequest->szUrl);

	if (lpRequest->lpPost != NULL)
	{
		curl_formfree(lpRequest->lpPost);
	}

	curl_easy_cleanup(lpRequest->lpCurl);

	FREE(lpRequest);
}

static size_t Curl_WriteData(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream)
{
	LPRESPONSE lpResponse = (LPRESPONSE)lpStream;

	Buffer_Append(lpResponse->lpData, (LPBYTE)lpData, dwSize * dwBlocks);

	return dwSize * dwBlocks;
}

static size_t Curl_WriteFile(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream)
{
	LPRESPONSE lpResponse = (LPRESPONSE)lpStream;
	DWORD dwWritten;
	BOOL fResult;

	fResult = WriteFile(
		lpResponse->hFile,
		lpData,
		dwSize * dwBlocks,
		&dwWritten,
		NULL);

	if (!fResult || dwWritten != dwSize * dwBlocks)
	{
		LOG("Could not write enough data to the target file");
	}

	if (!fResult)
	{
		return 0;
	}
	else
	{
		return dwWritten;
	}
}

static size_t Curl_WriteHeader(void * lpData, size_t dwSize, size_t dwBlocks, void * lpStream)
{
	char * szHeader;
	DWORD dwLength;
	char * szColonOffset;
	LPRESPONSE lpResponse = (LPRESPONSE)lpStream;
	LPRESPONSE_HEADER lpHeader;
	char * szName;
	char * szValue;

	dwLength = dwSize * dwBlocks;

	szHeader = (char *)ALLOC(dwLength + 1);

	memcpy(szHeader, lpData, dwLength);

	szHeader[dwLength] = '\0';

	szColonOffset = strchr(szHeader, ':');

	if (szColonOffset != NULL)
	{
		szName = szHeader;
		*szColonOffset = '\0';

		szValue = szColonOffset + 1;

		szName = Curl_TrimA(szName);
		szValue = Curl_TrimA(szValue);

		if (*szName && *szValue)
		{
			lpHeader = ALLOCA(RESPONSE_HEADER);

			lpHeader->szName = dupmbstowcs(szName);
			lpHeader->szValue = dupmbstowcs(szValue);

			List_Add(lpResponse->lpHeaders, lpHeader);
		}
	}

	FREE(szHeader);

	return dwLength;
}

void Curl_SetOutputFile(LPREQUEST lpRequest, HANDLE hFile)
{
	lpRequest->lpResponse->hFile = hFile;
}

LPWSTR Curl_GetHeader(LPRESPONSE lpResponse, LPCWSTR szHeaderName)
{
	LPITER lpIter;
	LPRESPONSE_HEADER lpHeader;
	LPWSTR szResult;

	lpIter = List_Start(lpResponse->lpHeaders);

	while (( lpHeader = (LPRESPONSE_HEADER)List_Next(lpIter) ) != NULL)
	{
		if (_wcsicmp(lpHeader->szName, szHeaderName) == 0)
		{
			break;
		}
	}

	List_End(lpIter);

	if (lpHeader != NULL)
	{
		size_t cbResult = (wcslen(lpHeader->szValue) + 1);

		szResult = (LPWSTR)ALLOC(cbResult * sizeof(WCHAR));

		wcscpy_s(szResult, cbResult, lpHeader->szValue);

		return szResult;
	}

	return NULL;
}

static char * Curl_TrimA(char * szValue)
{
	char * szEnd;

	while (*szValue && isspace(*szValue))
	{
		szValue++;
	}

	szEnd = strrchr(szValue, '\0');

	while (szEnd > szValue && isspace(*(szEnd - 1)))
	{
		*(szEnd - 1) = '\0';
		szEnd--;
	}

	return szValue;
}

void Curl_DestroyCookies(LPCOOKIES lpCookie)
{
	if (lpCookie->lpCookies != NULL)
	{
		curl_slist_free_all(lpCookie->lpCookies);
	}

	FREE(lpCookie);
}

LPCOOKIES Curl_GetCookies(LPRESPONSE lpResponse)
{
	LPCOOKIES lpResult = NULL;
	CURLcode nResult;
	struct curl_slist * lpCookies = NULL;

	nResult = curl_easy_getinfo(lpResponse->lpRequest->lpCurl, CURLINFO_COOKIELIST, &lpCookies);
	
	if (nResult == CURLE_OK && lpCookies != NULL)
	{
		lpResult = ALLOCA(COOKIES);

		lpResult->lpCookies = lpCookies;
	}

	return lpResult;
}

void Curl_SetCookies(LPREQUEST lpRequest, LPCOOKIES lpCookies)
{
	struct curl_slist * lpCookie;

	if (lpCookies != NULL)
	{
		lpCookie = lpCookies->lpCookies;

		while (lpCookie != NULL)
		{
			curl_easy_setopt(lpRequest->lpCurl, CURLOPT_COOKIELIST, lpCookie->data);
			lpCookie = lpCookie->next;
		}
	}
}

void Curl_IgnoreSSLErrors(LPREQUEST lpRequest)
{
	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(lpRequest->lpCurl, CURLOPT_SSL_VERIFYPEER, 0); 
}

LPWSTR Curl_UrlEncode(LPWSTR szSource)
{
	INT nLength = 0;
	LPSTR szSourceA;
	LPSTR szOffset;
	LPSTR szResultA;
	LPWSTR szResult;
	LPSTR szResultOffset;

	nLength = WideCharToMultiByte(CP_UTF8, 0, szSource, -1, NULL, 0, NULL, NULL);

	szSourceA = (LPSTR)ALLOC(nLength + 1);

	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, szSourceA, nLength + 1, NULL, NULL);
	szSourceA[nLength] = '\0';

	nLength = 0;

	szOffset = szSourceA;

	while (*szOffset != '\0')
	{
		if (
			*szOffset == '-' ||
			*szOffset == '_' ||
			*szOffset == '.' ||
			isalnum((unsigned char)*szOffset)
		) {
			nLength++;
		}
		else
		{
			nLength += 3;
		}

		szOffset++;
	}

	szResultA = (LPSTR)ALLOC(nLength + 1);

	szResultOffset = szResultA;
	szOffset = szSourceA;

	while (*szOffset != '\0')
	{
		if (
			*szOffset == '-' ||
			*szOffset == '_' ||
			*szOffset == '.' ||
			isalnum((unsigned char)*szOffset)
		) {
			*szResultOffset = *szOffset;

			szResultOffset++;
		}
		else
		{

			StringCbPrintfA(szResultOffset, (nLength + 1) - (szResultOffset - szResultA), "%%%2X", (int)(unsigned char)*szOffset);

			szResultOffset += 3;
		}

		szOffset++;
	}

	*szResultOffset = '\0';

	szResult = dupmbstowcs(szResultA);

	FREE(szSourceA);
	FREE(szResultA);

	return szResult;
}
