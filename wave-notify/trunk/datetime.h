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

#ifndef _INC_DATETIME
#define _INC_DATETIME

#pragma once

class CTimeSpan
{
private:
	LONGLONG m_nMilliseconds;

private:
	CTimeSpan(LONGLONG nMilliseconds) { m_nMilliseconds = nMilliseconds; }

public:
	static CTimeSpan FromMilliseconds(DOUBLE nMilliseconds) { return CTimeSpan((LONGLONG)nMilliseconds); }
	static CTimeSpan FromSeconds(DOUBLE nSeconds) { return CTimeSpan((LONGLONG)(nSeconds * 1000.0)); }
	static CTimeSpan FromMinutes(DOUBLE nMinutes) { return CTimeSpan((LONGLONG)(nMinutes * 60000.0)); }
	static CTimeSpan FromHours(DOUBLE nHours) { return CTimeSpan((LONGLONG)(nHours * 3600000.0)); }
	static CTimeSpan FromDays(DOUBLE nDays) { return CTimeSpan((LONGLONG)(nDays * 86400000.0)); }

	bool operator ==(const CTimeSpan & _Right) const { return m_nMilliseconds == _Right.m_nMilliseconds; }
	bool operator ==(LONGLONG _Right) const { return m_nMilliseconds == _Right; }
	bool operator !=(const CTimeSpan & _Right) const { return m_nMilliseconds != _Right.m_nMilliseconds; }
	bool operator !=(LONGLONG _Right) const { return m_nMilliseconds != _Right; }
	bool operator <(const CTimeSpan & _Right) const { return m_nMilliseconds < _Right.m_nMilliseconds; }
	bool operator <(LONGLONG _Right) const { return m_nMilliseconds < _Right; }
	bool operator <=(const CTimeSpan & _Right) const { return m_nMilliseconds <= _Right.m_nMilliseconds; }
	bool operator <=(LONGLONG _Right) const { return m_nMilliseconds <= _Right; }
	bool operator >(const CTimeSpan & _Right) const { return m_nMilliseconds > _Right.m_nMilliseconds; }
	bool operator >(LONGLONG _Right) const { return m_nMilliseconds > _Right; }
	bool operator >=(const CTimeSpan & _Right) const { return m_nMilliseconds >= _Right.m_nMilliseconds; }
	bool operator >=(LONGLONG _Right) const { return m_nMilliseconds >= _Right; }

	DOUBLE GetTotalMilliseconds() const { return DOUBLE(m_nMilliseconds); }
	DOUBLE GetTotalSeconds() const { return DOUBLE(m_nMilliseconds) / 1000.0; }
	DOUBLE GetTotalMinutes() const { return DOUBLE(m_nMilliseconds) / 60000.0; }
	DOUBLE GetTotalHours() const { return DOUBLE(m_nMilliseconds) / 3600000.0; }
	DOUBLE GetTotalDays() const { return DOUBLE(m_nMilliseconds) / 86400000.0; }
	DOUBLE GetMilliseconds() const { return DOUBLE(m_nMilliseconds % (LONGLONG)1000); }
	DOUBLE GetSeconds() const { return DOUBLE((m_nMilliseconds / (LONGLONG)1000) % (LONGLONG)60); }
	DOUBLE GetMinutes() const { return DOUBLE((m_nMilliseconds / (LONGLONG)60000) % (LONGLONG)60); }
	DOUBLE GetHours() const { return DOUBLE((m_nMilliseconds / (LONGLONG)3600000) % (LONGLONG)24); }
	DOUBLE GetDays() const { return DOUBLE(m_nMilliseconds / (LONGLONG)86400000); }

	CTimeSpan operator +(const CTimeSpan _Other) const { return CTimeSpan(m_nMilliseconds + _Other.m_nMilliseconds); }
	CTimeSpan operator -(const CTimeSpan _Other) const { return CTimeSpan(m_nMilliseconds - _Other.m_nMilliseconds); }

	friend class CDateTime;
};

class CDateTime
{
private:
	ULONGLONG m_uTime;
	SYSTEMTIME m_vSystemTime;

public:
	CDateTime() { Initialize((LONGLONG)0); }
	CDateTime(const ULONGLONG & vTime) { Initialize(vTime); }
	CDateTime(const FILETIME & vTime) { Initialize(*((ULONGLONG *)&vTime)); }
	CDateTime(const SYSTEMTIME & vSystemTime) { Initialize(vSystemTime); }
	CDateTime(const CDateTime & _Other) { Initialize(_Other.m_uTime); }
	CDateTime(WORD wYear, WORD wMonth, WORD wDay, WORD wHour = 0, WORD wMinute = 0, WORD wSecond = 0, WORD wMilliseconds = 0) {
		SYSTEMTIME vTime = { wYear, wMonth, 0, wDay, wHour, wMinute, wSecond, wMilliseconds };
		Initialize(vTime);
	}

	FILETIME GetFileTime() const {
		return *((FILETIME *)&m_uTime);
	}
	void GetFileTime(FILETIME & vFileTime) const {
		memcpy(&vFileTime, &m_uTime, sizeof(FILETIME));
	}

	SYSTEMTIME GetSystemTime() const {
		SYSTEMTIME vTmp;
		GetSystemTime(vTmp);
		return vTmp;
	}
	void GetSystemTime(SYSTEMTIME & vTime) const {
		FileTimeToSystemTime((LPFILETIME)&m_uTime, &vTime);
	}

	FILETIME GetLocalFileTime() const {
		FILETIME vTmp;
		GetLocalFileTime(vTmp);
		return vTmp;
	}
	void GetLocalFileTime(FILETIME & vTime) const {
		FileTimeToLocalFileTime((LPFILETIME)&m_uTime, &vTime);
	}

	SYSTEMTIME GetLocalSystemTime() const {
		SYSTEMTIME vTmp;
		GetLocalSystemTime(vTmp);
		return vTmp;
	}
	void GetLocalSystemTime(SYSTEMTIME & vTime) const {
		FILETIME vTmp;
		GetLocalFileTime(vTmp);
		FileTimeToSystemTime(&vTmp, &vTime);
	}

	static CTimeSpan Compare(const CDateTime & _Left, const CDateTime & _Right) {
		LONGLONG nDifference =
			_Left.m_uTime < _Right.m_uTime ?
			-LONGLONG(_Right.m_uTime - _Left.m_uTime) :
			LONGLONG(_Left.m_uTime - _Right.m_uTime);

		return CTimeSpan(nDifference / (LONGLONG)10000);
	}

	WORD GetYear() const { return m_vSystemTime.wYear; }
	WORD GetMonth() const { return m_vSystemTime.wMonth; }
	WORD GetDay() const { return m_vSystemTime.wDay; }
	WORD GetDayOfWeek() const { return m_vSystemTime.wDayOfWeek; }
	WORD GetHour() const { return m_vSystemTime.wHour; }
	WORD GetMinute() const { return m_vSystemTime.wMinute; }
	WORD GetSecond() const { return m_vSystemTime.wSecond; }
	WORD GetMilliseconds() const { return m_vSystemTime.wMilliseconds; }

	bool operator ==(const CDateTime & _Other) const { return Compare(*this, _Other) == (LONGLONG)0; }
	bool operator !=(const CDateTime & _Other) const { return Compare(*this, _Other) != (LONGLONG)0; }
	bool operator <(const CDateTime & _Other) const { return Compare(*this, _Other) < (LONGLONG)0; }
	bool operator >(const CDateTime & _Other) const { return Compare(*this, _Other) > (LONGLONG)0; }
	bool operator <=(const CDateTime & _Other) const { return Compare(*this, _Other) <= (LONGLONG)0; }
	bool operator >=(const CDateTime & _Other) const { return Compare(*this, _Other) >= (LONGLONG)0; }
	CDateTime operator+(const CTimeSpan & _Other) const { return CDateTime(m_uTime + _Other.m_nMilliseconds * (LONGLONG)10000); }
	CDateTime operator-(const CTimeSpan & _Other) const { return CDateTime(m_uTime - _Other.m_nMilliseconds * (LONGLONG)10000); }
	CTimeSpan operator-(const CDateTime & _Other) const { return Compare(*this, _Other); }

	static CDateTime Now() {
		SYSTEMTIME vTmp;
		::GetSystemTime(&vTmp);
		return CDateTime(vTmp);
	}

private:
	void Initialize(const ULONGLONG & vTime) {
		memcpy(&m_uTime, &vTime, sizeof(ULONGLONG));
		FileTimeToSystemTime((LPFILETIME)&m_uTime, &m_vSystemTime);
	}
	void Initialize(const SYSTEMTIME & vSystemTime) {
		memcpy(&m_vSystemTime, &vSystemTime, sizeof(SYSTEMTIME));
		BOOL fResult = SystemTimeToFileTime(&m_vSystemTime, (LPFILETIME)&m_uTime);
		if (!fResult)
			FAIL("Could not convert system time to filetime");
	}
};

#endif // _INC_DATETIME
