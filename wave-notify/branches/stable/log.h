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

#ifndef _INC_LOG
#define _INC_LOG

#pragma once

#ifdef _DEBUG

#define DEBUGBREAK	__asm { int 3 }

#ifdef BREAK_ON_LOG
#define LOGBREAK	DEBUGBREAK
#else
#define LOGBREAK
#endif // !BREAK_ON_LOG

#else // !_DEBUG

#define DEBUGBREAK
#define LOGBREAK

#endif

#define LOG(_Mesg)			do { LOGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ) ); } while (0)
#define LOG1(_Mesg, _1)			do { LOGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ) ); } } while (0)
#define LOG2(_Mesg, _1, _2)		do { LOGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ) ); } } while (0)
#define LOG3(_Mesg, _1, _2, _3)		do { LOGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ) ); } } while (0)
#define LOG4(_Mesg, _1, _2, _3, _4)	do { LOGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ), ( _4 ) ); } while (0)

#ifdef _DEBUG

#define ASSERT(_Cond)			do { if ( !(_Cond) ) { DEBUGBREAK; Log_AssertFailA( __FILE__, __LINE__, ( # _Cond ) ); } } while (0)
#define ASSERT_MSG(_Cond, _Mesg)	do { if ( !(_Cond) ) { DEBUGBREAK; Log_AssertFailA( __FILE__, __LINE__, ( _Mesg ) ); } } while (0)
#define FAIL(_Mesg)			do { DEBUGBREAK; Log_AssertFailA( __FILE__, __LINE__, ( _Mesg ) ); } while (0)

#else

#define ASSERT(_Cond)			(void) ( ( !!(_Cond) ) || ( Log_AssertFailA( __FILE__, __LINE__, ( # _Cond ) ), 1 ) )
#define ASSERT_MSG(_Cond, _Mesg)	(void) ( ( !!(_Cond) ) || ( Log_AssertFailA( __FILE__, __LINE__, ( # _Mesg ) ), 1 ) )
#define FAIL(_Mesg)			(void) ( Log_AssertFailA( __FILE__, __LINE__, ( _Mesg ) ) )

#endif

void Log_WriteA(LPCSTR szFile, DWORD dwLine, LPCSTR szFormat, ...);
void __declspec(noreturn) Log_AssertFailA(LPCSTR szFile, DWORD dwLine, LPCSTR szCond);
void Log_Append(LPCWSTR szFileName, LPCSTR szLine);
void Log_SetAppVersion(LPCSTR szAppVersion);

#define __CHECK_PREFIX			"CHECK: "

#define	CHECK(_Cond)			do { if ( !(_Cond) ) { LOG(__CHECK_PREFIX "Condition failed: " # _Cond ); } } while (0)
#define CHECK_PTR(_Ptr)			do { if ( IS_INTRESOURCE(_Ptr) ) { LOG(__CHECK_PREFIX "Illegal pointer or NULL: " # _Ptr ); } } while (0)
#define CHECK_PTR_NULL(_Ptr)		do { if ( ( _Ptr ) == NULL || IS_INTRESOURCE(_Ptr) { LOG(__CHECK_PREFIX "Illegal pointer: " # _Ptr ); } } while (0)
#define CHECK_ENUM(_Value, _Max)	do { if ( (_Value) < 0 || (_Value) >= (_Max) ) { LOG(__CHECK_PREFIX "Illegal enum value: " # _Value ); } } while (0)

#endif // _INC_LOG
