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

#define DEBUGBREAK __asm { int 3 }

#else // !_DEBUG

#define DEBUGBREAK

#endif

#if defined(_DEBUG) && defined(BREAK_ON_LOG)

#define LOG(_Mesg)			{ DEBUGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ) ); }
#define LOG1(_Mesg, _1)			{ DEBUGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ) ); }
#define LOG2(_Mesg, _1, _2)		{ DEBUGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ) ); }
#define LOG3(_Mesg, _1, _2, _3)		{ DEBUGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ) ); }
#define LOG4(_Mesg, _1, _2, _3, _4)	{ DEBUGBREAK; Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ), ( _4 ) ); }

#else

#define LOG(_Mesg)			Log_WriteA( __FILE__, __LINE__, ( _Mesg ) )
#define LOG1(_Mesg, _1)			Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ) )
#define LOG2(_Mesg, _1, _2)		Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ) )
#define LOG3(_Mesg, _1, _2, _3)		Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ) )
#define LOG4(_Mesg, _1, _2, _3, _4)	Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ), ( _4 ) )

#endif

#ifdef _DEBUG

#define DLOG(_Mesg)			Log_WriteA( __FILE__, __LINE__, ( _Mesg ) )
#define DLOG1(_Mesg, _1)		Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ) )
#define DLOG2(_Mesg, _1, _2)		Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ) )
#define DLOG3(_Mesg, _1, _2, _3)	Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ) )
#define DLOG4(_Mesg, _1, _2, _3, _4)	Log_WriteA( __FILE__, __LINE__, ( _Mesg ), ( _1 ), ( _2 ), ( _3 ), ( _4 ) )

#else // !_DEBUG

#define DLOG(_Mesg)
#define DLOG1(_Mesg, _1)
#define DLOG2(_Mesg, _1, _2)
#define DLOG3(_Mesg, _1, _2, _3)
#define DLOG4(_Mesg, _1, _2, _3, _4)

#endif // _DEBUG

#ifdef _DEBUG

#define DASSERT(_Cond)			ASSERT(_Cond)
#define DFAIL(_Mesg)			FAIL(_Mesg)
#define ASSERT(_Cond)			do { if ( !(_Cond) ) { DEBUGBREAK; Log_AssertFailA( __FILE__, __LINE__, ( # _Cond ) ); } } while (0)
#define ASSERT_MSG(_Cond, _Mesg)	do { if ( !(_Cond) ) { DEBUGBREAK; Log_AssertFailA( __FILE__, __LINE__, ( _Mesg ) ); } } while (0)
#define FAIL(_Mesg)			do { DEBUGBREAK; Log_AssertFailA( __FILE__, __LINE__, ( _Mesg ) ); } while (0)

#else

#define DASSERT(_Cond)
#define DFAIL(_Mesg)
#define ASSERT(_Cond)			(void) ( ( !!(_Cond) ) || ( Log_AssertFailA( __FILE__, __LINE__, ( # _Cond ) ), 1 ) )
#define FAIL(_Mesg)			Log_AssertFailA( __FILE__, __LINE__, ( _Mesg ) )

#endif

void Log_WriteA(LPCSTR szFile, DWORD dwLine, LPCSTR szFormat, ...);
void __declspec(noreturn) Log_AssertFailA(LPCSTR szFile, DWORD dwLine, LPCSTR szCond);
void Log_Append(LPCWSTR szFileName, LPCSTR szLine);

#endif // _INC_LOG
