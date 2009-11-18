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

#pragma once

#ifndef _INC_JSON
#define _INC_JSON

typedef enum {
	JS_ARRAY,
	JS_DICT
} JS_TYPE;

typedef enum {
	JI_DICT_START,
	JI_DICT_END,
	JI_ARRAY_START,
	JI_ARRAY_END,
	JI_VALUE,
	JI_EOF,
	JI_BOF,
	JI_ERROR
} JI_TYPE;

typedef enum {
	JV_NUM,
	JV_FLOAT,
	JV_STR,
	JV_BOOL,
	JV_NESTED,
	JV_ERROR,
	JV_NONE
} JV_TYPE;

typedef struct tagJSON_SCOPE
{
	JS_TYPE nType;
	struct tagJSON_SCOPE * lpParent;
} JSON_SCOPE, * LPJSON_SCOPE;

typedef struct tagJSON_VALUE
{
	JV_TYPE nType;
	LPWSTR szString;
	INT nNumber;
	DOUBLE fNumber;
	BOOL fBool;
} JSON_VALUE, * LPJSON_VALUE;

typedef struct tagJSON_STATE
{
	LPWSTR szContent;
	LPWSTR szOffset;
	LPWSTR szBuffer;
	size_t cbBuffer;
	LPWSTR szKeyBuffer;
	size_t cbKeyBuffer;
	BOOL fHadContent;
	LPJSON_SCOPE lpScope;
} JSON_STATE, * LPJSON_STATE;

typedef struct tagJSON
{
	JI_TYPE nType;
	LPWSTR szKey;
	INT nIndex;
	JSON_VALUE Value;
	JSON_STATE State;
} JSON, * LPJSON;

LPJSON Json_CreateReader(LPWSTR szContent);
void Json_DestroyReader(LPJSON lpReader);
BOOL Json_Read(LPJSON lpReader);
size_t Json_GetProcessedContents(LPJSON lpReader);
INT Json_GetScopeDepth(LPJSON lpReader);
BOOL Json_SkipNestedValue(LPJSON lpReader);

#endif // _INC_JSON
