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

#ifndef _INC_WAVE
#define _INC_WAVE

typedef struct tagWAVE_SESSION
{
	LPWSTR szUsername;
	LPWSTR szPassword;
	LPCOOKIES lpCookies;
	LPWSTR szAuthKey;
	LPWSTR szEmailAddress;
} WAVE_SESSION, * LPWAVE_SESSION;

typedef struct tagWAVE_CONTACT
{
	LPWSTR szEmailAddress;
	LPWSTR szName;
} WAVE_CONTACT, * LPWAVE_CONTACT;

typedef struct tagWAVE_MSG
{
	INT nOrder;
	LPWSTR szText;
	LPWSTR szEmailAddress;
	INT nContactId;
} WAVE_MSG, * LPWAVE_MSG;

typedef struct tagWAVE
{
	LPWSTR szId;
	INT nMessages;
	INT nUnreadMessages;
	LPWSTR szSubject;
	INT nCount;
	LPWAVE_MSG * lpMessages;
	LPWSTR szEmailAddress;
} WAVE, * LPWAVE;

typedef struct tagWAVE_RESPONSE
{
	LPWAVE * lpWaves;
	INT nWavesCount;
	LPWAVE_CONTACT * lpContacts;
	INT nContactsCount;
} WAVE_RESPONSE, * LPWAVE_RESPONSE;

void Wave_DestroyResponse(LPWAVE_RESPONSE lpRequest);
LPWAVE_SESSION Wave_CreateSession(LPWSTR szUsername, LPWSTR szPassword);
void Wave_DestroySession(LPWAVE_SESSION lpSession);
BOOL Wave_Login(LPWAVE_SESSION lpSession);
void Wave_Logoff(LPWAVE_SESSION lpSession);
LPWAVE_RESPONSE Wave_Query(LPWAVE_SESSION lpSession);
LPWAVE_RESPONSE Wave_ParseResponse(LPWSTR szContent);
BOOL Wave_IsLoggedOn(LPWAVE_SESSION lpSession);
void Wave_OpenInbox(LPWAVE_SESSION lpSession);
void Wave_OpenWave(LPWAVE_SESSION lpSession, LPWSTR szWaveId);

#endif // _INC_WAVE
