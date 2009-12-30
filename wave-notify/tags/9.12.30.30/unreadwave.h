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

#ifndef _INC_UNREADWAVE
#define _INC_UNREADWAVE

#pragma once

typedef enum
{
	WCS_REFRESH,
	WCS_IGNORE,
	WCS_CHANGED
} WAVE_CHANGED_STATUS;

class CUnreadWave;

typedef map<wstring, CUnreadWave *> TUnreadWaveMap;
typedef TUnreadWaveMap::iterator TUnreadWaveMapIter;
typedef TUnreadWaveMap::const_iterator TUnreadWaveMapConstIter;
typedef vector<CUnreadWave *> TUnreadWaveVector;
typedef TUnreadWaveVector::iterator TUnreadWaveVectorIter;
typedef TUnreadWaveVector::const_iterator TUnreadWaveVectorConstIter;

class CUnreadWave
{
private:
	wstring m_szId;
	wstring m_szContact;
	CWaveContact * m_lpContact;
	wstring m_szSubject;
	wstring m_szBody;
	CDateTime m_dtTime;
	UINT m_uUnread;

public:
	CUnreadWave(CWave * lpWave, CWaveMessage * lpMessage);
	virtual ~CUnreadWave() { }

	wstring GetID() const { return m_szId; }
	wstring GetContact() const { return m_szContact; }
	wstring GetSubject() const { return m_szSubject; }
	wstring GetBody() const { return m_szBody; }
	CDateTime GetTime() const { return m_dtTime; }
	UINT GetUnread() const { return m_uUnread; }
	CWaveContact * GetWaveContact() const { return m_lpContact; }
	void SetWaveContact(CWaveContact * lpContact) { m_lpContact = lpContact; }

	INT Paint(CDC & dc, RECT & rcClient, BOOL fMouseOver, BOOL fExcluseCloseButton) const;
	INT CalculateHeight(RECT & rcClient) const;
};

class CUnreadWaveCollection
{
private:
	TUnreadWaveMap m_vUnreadsMap;
	TUnreadWaveVector m_vUnreadsVector;

private:
	CUnreadWaveCollection(CWaveCollection * lpLastReported, CWaveCollection * lpCurrent);

public:
	virtual ~CUnreadWaveCollection();

	const TUnreadWaveVector & GetChanges() const { return m_vUnreadsVector; }
	CUnreadWave * GetChange(wstring szId) const {
		TUnreadWaveMapConstIter pos = m_vUnreadsMap.find(szId);
		return pos == m_vUnreadsMap.end() ? NULL : pos->second;
	}
	CUnreadWave * GetChange(INT nId) const { return m_vUnreadsVector[nId]; }
	INT Find(CUnreadWave * lpUnread) const;
	void DetachAll() {
		m_vUnreadsMap.clear();
		m_vUnreadsVector.clear();
	}

	static CUnreadWaveCollection * CreateUnreadWaves(CWaveCollection * lpLastReported, CWaveCollection * lpCurrent) {
		return new CUnreadWaveCollection(lpLastReported, lpCurrent);
	}

private:
	void InsertAllWaves(CWaveCollection * lpCurrent);
	void InsertChangesOnly(CWaveCollection * lpLastReported, CWaveCollection * lpCurrent);
	void Insert(CUnreadWave * lpUnread);
	CUnreadWave * GetDifference(CWave * lpReportedWave, CWave * lpNewWave) const;
	BOOL WavesEqual(CWave * lpReportedWave, CWave * lpNewWave) const;
	WAVE_CHANGED_STATUS GetChangedStatus(CWave * lpReportedWave, CWave * lpNewWave) const;
};

#endif // _INC_UNREADWAVE
