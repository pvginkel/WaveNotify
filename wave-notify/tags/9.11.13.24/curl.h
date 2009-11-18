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

#ifndef _INC_CURL
#define _INC_CURL

#pragma once

typedef struct tagCOOKIES {
	struct curl_slist * lpCookies;
} COOKIES, * LPCOOKIES;

struct tagREQUEST;

typedef struct tagRESPONSE_HEADER {
	LPWSTR szName;
	LPWSTR szValue;
} RESPONSE_HEADER, * LPRESPONSE_HEADER;

typedef struct tagRESPONSE {
	struct tagREQUEST * lpRequest;
	LPLIST lpHeaders;
	INT nStatus;
	PBUFFER lpData;
	HANDLE hFile;
} RESPONSE, * LPRESPONSE;

typedef struct tagREQUEST {
	LPSTR szUrl;
	CURL * lpCurl;
	struct curl_httppost * lpPost;
	struct curl_httppost * lpLastPost;
	LPRESPONSE lpResponse;
	CHAR szError[CURL_ERROR_SIZE];
	LPSTR szUserAgent;
} REQUEST, * LPREQUEST;

void Curl_Initialize();
void Curl_Destroy();
LPREQUEST Curl_Create(LPCWSTR szUrl, LPCWSTR szUserAgent);
void Curl_AddPost(LPREQUEST lpRequest, LPCWSTR szName, LPCWSTR szValue);
LPRESPONSE Curl_Execute(LPREQUEST lpRequest);
void Curl_DestroyRequest(LPREQUEST lpRequest);
void Curl_SetOutputFile(LPREQUEST lpRequest, HANDLE hFile);
LPWSTR Curl_GetHeader(LPRESPONSE lpResponse, LPCWSTR szHeaderName);
void Curl_DestroyCookies(LPCOOKIES lpCookie);
LPCOOKIES Curl_GetCookies(LPRESPONSE lpResponse);
void Curl_SetCookies(LPREQUEST lpRequest, LPCOOKIES lpCookies);
void Curl_IgnoreSSLErrors(LPREQUEST lpRequest);
LPWSTR Curl_UrlEncode(LPWSTR szSource);
void Curl_SetTimeout(LPREQUEST lpRequest, INT nTimeout);

#endif // _INC_CURL
